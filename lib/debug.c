/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "video.h"
#include "debug.h"

u8 *r8[] = {"al","cl","dl","bl","ah","ch","dh","bh"};
u8 *r16[] = {"ax","cx","dx","bx","sp","bp","si","di"};
u8 *r32[] = {"eax","ecx","edx","ebx","esp","ebp","esi","edi"};
u8 *rseg[] = {"ds","es","fs","gs","ss","cs","ip"};

/*******************************************************************************/
/* Efface un breakpoint sur une adresse */
void cleardebugreg(u8* address)
{
    for(u8 i=0;i<4;i++)
        if (getdebugreg(i)==address)
            setdebugreg(i,0x0, DBG_CLEAR);
}

/*******************************************************************************/
/* Permet de manipuler dr0-dr7 pour mettre en place des breakpoints */

u8* getdebugreg(u8 number)
{
    u8* address;
    if (number==0) asm("mov %%dr0,%%eax; mov %%eax,%[address]":[address] "=m" (address)::);    
    else if (number==1) asm("mov %%dr1,%%eax; movl %%eax,%[address]":[address] "=m" (address)::);
    else if (number==2) asm("mov %%dr2,%%eax; movl %%eax,%[address]":[address] "=m" (address)::); 
    else if (number==3) asm("mov %%dr0,%%eax; movl %%eax,%[address]":[address] "=m" (address)::); 
    return address;
}

/*******************************************************************************/
/* Permet de manipuler dr0-dr7 pour mettre en place des breakpoints */

void setdebugreg(u8 number,u8 *address, u8 type)
{
    u32 dr7=0;   
    asm("movl %%dr7,%%eax; mov %%eax,%[dr7]":[dr7] "=m" (dr7)::);    
    if (type==DBG_WRITE || type==DBG_READWRITE)
        dr7|=0b11<<8;
    if (type!=DBG_CLEAR)
        dr7|=0b11<<(number<<1);
    else
        dr7&=~(0b11<<(number<<1));  
    dr7&=~(0b11<<(16+(number<<2)));      
    dr7|=type<<(16+(number<<2));
    dr7&=~(0b11<<(16+((number<<2)+2)));
    asm("movl %[dr7],%%eax; mov %%eax,%%dr7"::[dr7] "m" (dr7):);    
    if (number==0) asm("movl %[address],%%eax; mov %%eax,%%dr0"::[address] "m" (address):);    
    else if (number==1) asm("movl %[address],%%eax; mov %%eax,%%dr1"::[address] "m" (address):);
    else if (number==2) asm("movl %[address],%%eax; mov %%eax,%%dr2"::[address] "m" (address):); 
    else if (number==3) asm("movl %[address],%%eax; mov %%eax,%%dr3"::[address] "m" (address):); 
    else
        return;
}

/*******************************************************************************/
/* Fonctions de déboguage */

u16 decodeModSM(bool show, u8 *a, u8 *op, u16 order, u32 Gsz, u32 Esz){
    u32 len = 0;
    u32 reg = 0;
    u32 scale = 0;
    u8 *b = a;
    u8 *E, *G, *indx, *base, *disp = "\0"; 
    u8 ebuf[32] = {0};
    
    setG();
    
    if(Esz){
        switch(*b&0xc7){
            case 0x00:
                E = "[bx+si]";
                break;
            case 0x01:
                E = "[bx+di]";
                break;
            case 0x02:
                E = "[bp+si]";
                break;
            case 0x03:
                E = "[bp+di]";
                break;
            case 0x04:
                E = "[si]";
                break;
            case 0x05:
                E = "[di]";
                break;
            case 0x06:
                snprintf(ebuf, sizeof(ebuf), "ds:%x", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x07:
                E = "[bx]";
                break;
            case 0x40:
                snprintf(ebuf, sizeof(ebuf), "[bx+si+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x41:
                snprintf(ebuf, sizeof(ebuf), "[bx+di+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x42:
                snprintf(ebuf, sizeof(ebuf), "[bp+si+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x43:
                snprintf(ebuf, sizeof(ebuf), "[bp+di+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x44:
                snprintf(ebuf, sizeof(ebuf), "[si+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x45:
                snprintf(ebuf, sizeof(ebuf), "[di+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x46:
                snprintf(ebuf, sizeof(ebuf), "[bp+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x47:
                snprintf(ebuf, sizeof(ebuf), "[bx+%x]", *(u8 *)++b); 
                E = ebuf;
            case 0x80:
                snprintf(ebuf, sizeof(ebuf), "[bx+si+%x]", *(u8 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x81:
                snprintf(ebuf, sizeof(ebuf), "[bx+di+%x]", *(u8 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x82:
                snprintf(ebuf, sizeof(ebuf), "[bp+si+%x]", *(u8 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x83:
                snprintf(ebuf, sizeof(ebuf), "[bp+di+%x]", *(u8 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x84:
                snprintf(ebuf, sizeof(ebuf), "[si+%x]", *(u8 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x85:
                snprintf(ebuf, sizeof(ebuf), "[di+%x]", *(u8 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x86:
                snprintf(ebuf, sizeof(ebuf), "[bp+%x]", *(u8 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x87:
                snprintf(ebuf, sizeof(ebuf), "[bx+%x]", *(u8 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0xc0:
                E = "al";
                break;
            case 0xc1:
                E = "cl";
                break;
            case 0xc2:
                E = "dl";
                break;
            case 0xc3:
                E = "bl";
                break;
            case 0xc4:
                E = "ah";
                break;
            case 0xc5:
                E = "ch";
                break;
            case 0xc6:
                E = "dh";
                break;
            case 0xc7:
                E = "bh";
                break;
            default:
                if (show) print("Invalid Mod R/M byte.");
                return 1000;
        }   
    } else{ 
        switch(*b & 0xc7){
            case 0x00:
                E = "[eax]";
                break;
            case 0x01:
                E = "[ecx]";
                break;
            case 0x02:
                E = "[edx]";
                break;
            case 0x03:
                E = "[ebx]";
                break;
            case 0x04:
                ++b;
                decodeSIB();
                if(!indx){
                    snprintf(ebuf, sizeof(ebuf), "[%s]", base);
                } else if(!base){
                    if(!scale){
                        snprintf(ebuf, sizeof(ebuf), "[%s]", indx);
                    } else{
                        snprintf(ebuf, sizeof(ebuf), "[%s*%i]", indx, scale);
                    }
                } else if(!scale){
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s]", base, indx);
                } else{
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s*%i]", base, indx, scale);
                }
                E = ebuf;
                break;
            case 0x05:
                snprintf(ebuf, sizeof(ebuf), "ds:%x", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x06:
                E = "[esi]";
                break;
            case 0x07:
                E = "[edi]";
                break;
             case 0x40:
                snprintf(ebuf, sizeof(ebuf), "[eax+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x41:
                snprintf(ebuf, sizeof(ebuf), "[ecx+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x42:
                snprintf(ebuf, sizeof(ebuf), "[edx+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x43:
                snprintf(ebuf, sizeof(ebuf), "[ebx+%x]", *(u8 *)++b); 
                E = ebuf;
            case 0x44:
                ++b;
                decodeSIB(); 
                if(!indx){
                    snprintf(ebuf, sizeof(ebuf), "[%s", base);
                } else if(!base){
                    if(!scale){
                        snprintf(ebuf, sizeof(ebuf), "[%s", indx);
                    } else{
                        snprintf(ebuf, sizeof(ebuf), "[%s*%i", indx, scale);
                    }
                } else if(!scale){
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s", base, indx);
                } else{
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s*%i", base, indx, scale);
                }
                len = strlen(ebuf);
                snprintf(ebuf+len, sizeof(ebuf)-len, "+%x]", *(u8 *)++b);
                E = ebuf;
                break;
            case 0x45:
                snprintf(ebuf, sizeof(ebuf), "[ebp+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x46:
                snprintf(ebuf, sizeof(ebuf), "[esi+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x47:
                snprintf(ebuf, sizeof(ebuf), "[edi+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x80:
                snprintf(ebuf, sizeof(ebuf), "[eax+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x81:
                snprintf(ebuf, sizeof(ebuf), "[ecx+%x]", *(u32 *)++b); 
                E = ebuf;
                break;
            case 0x82:
                snprintf(ebuf, sizeof(ebuf), "[edx+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x83:
                snprintf(ebuf, sizeof(ebuf), "[ebx+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
             case 0x84:
                ++b;
                decodeSIB(); 
                if(!indx){
                    snprintf(ebuf, sizeof(ebuf), "[%s", base);
                } else if(!base){
                    if(!scale){
                        snprintf(ebuf, sizeof(ebuf), "[%s", indx);
                    } else{
                        snprintf(ebuf, sizeof(ebuf), "[%s*%i", indx, scale);
                    }
                } else if(!scale){
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s", base, indx);
                } else{
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s*%i", base, indx, scale);
                }
                len = strlen(ebuf);
                snprintf(ebuf+len, sizeof(ebuf)-len, "+%x]", *(u32 *)++b);
                b += 3;
                E = ebuf;
                break;
           case 0x85:
                snprintf(ebuf, sizeof(ebuf), "[ebp+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x86:
                snprintf(ebuf, sizeof(ebuf), "[esi+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x87:
                snprintf(ebuf, sizeof(ebuf), "[edi+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
           case 0xc0:
                E = Esz == 1 ? "ax": "eax";
                break;
            case 0xc1:
                E = Esz == 1 ? "cx": "ecx";
                break;
            case 0xc2:
                E = Esz == 1 ? "dx": "edx";
                break;
            case 0xc3:
                E = Esz == 1 ? "bx": "ebx";
                break;
            case 0xc4:
                E = Esz == 1 ? "sp": "esp";
                break;
            case 0xc5:
                E = Esz == 1 ? "bp": "ebp";
                break;
            case 0xc6:
                E = Esz == 1 ? "si": "esi";
                break;
            case 0xc7:
                E = Esz == 1 ? "di": "edi";
                break;
            default:
                if (show) print("Invalid Mod R/M byte.");
                return 1000;
        }
    }

    if(order){
        snprintf(op, opBufSz, "%s", E);
        len = strlen(op);
        if(Gsz){
            snprintf(op+len, opBufSz-len, ", %s", G);
        }
    } else{
        snprintf(op, opBufSz, "%s, ", G);
        len = strlen(op);
        snprintf(op+len, opBufSz-len, "%s", E);
    }
    return b-a;
}

u32 decodeModSM_float(bool show, u8 *a, u8 *op, u32 order, u32 Gsz, u32 Esz){
    u32 len = 0;
    u32 reg = 0;
    u32 scale = 0;
    u8 *b = a;
    u8 *E, *G, *indx, *base, *disp = "\0"; 
    u8 ebuf[32] = {0};
    
    setG();
    
    if(Esz){ 
        switch(*b&0xc7){
            case 0x00:
                E = "[bx+si]";
                break;
            case 0x01:
                E = "[bx+di]";
                break;
            case 0x02:
                E = "[bp+si]";
                break;
            case 0x03:
                E = "[bp+di]";
                break;
            case 0x04:
                E = "[si]";
                break;
            case 0x05:
                E = "[di]";
                break;
            case 0x06:
                snprintf(ebuf, sizeof(ebuf), "ds:%x", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x07:
                E = "[bx]";
                break;
            case 0x40:
                snprintf(ebuf, sizeof(ebuf), "[bx+si+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x41:
                snprintf(ebuf, sizeof(ebuf), "[bx+di+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x42:
                snprintf(ebuf, sizeof(ebuf), "[bp+si+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x43:
                snprintf(ebuf, sizeof(ebuf), "[bp+di+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x44:
                snprintf(ebuf, sizeof(ebuf), "[si+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x45:
                snprintf(ebuf, sizeof(ebuf), "[di+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x46:
                snprintf(ebuf, sizeof(ebuf), "[bp+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x47:
                snprintf(ebuf, sizeof(ebuf), "[bx+%x]", *(u8 *)++b); 
                E = ebuf;
            case 0x80:
                snprintf(ebuf, sizeof(ebuf), "[bx+si+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x81:
                snprintf(ebuf, sizeof(ebuf), "[bx+di+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x82:
                snprintf(ebuf, sizeof(ebuf), "[bp+si+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x83:
                snprintf(ebuf, sizeof(ebuf), "[bp+di+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x84:
                snprintf(ebuf, sizeof(ebuf), "[si+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x85:
                snprintf(ebuf, sizeof(ebuf), "[di+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x86:
                snprintf(ebuf, sizeof(ebuf), "[bp+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x87:
                snprintf(ebuf, sizeof(ebuf), "[bx+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0xc0:
                E = "st(0)";
                break;
            case 0xc1:
                E = "st(1)";
                break;
            case 0xc2:
                E = "st(2)";
                break;
            case 0xc3:
                E = "st(3)";
                break;
            case 0xc4:
                E = "st(4)";
                break;
            case 0xc5:
                E = "st(5)";
                break;
            case 0xc6:
                E = "st(6)";
                break;
            case 0xc7:
                E = "st(7)";
                break;
            default:
                if (show) print("Invalid Mod R/M byte.");
                return 1000;
        }   
    } else{
        switch(*b & 0xc7){
            case 0x00:
                E = "[eax]";
                break;
            case 0x01:
                E = "[ecx]";
                break;
            case 0x02:
                E = "[edx]";
                break;
            case 0x03:
                E = "[ebx]";
                break;
            case 0x04:
                ++b;
                decodeSIB();
                if(!indx){
                    snprintf(ebuf, sizeof(ebuf), "[%s]", base);
                } else if(!base){
                    if(!scale){
                        snprintf(ebuf, sizeof(ebuf), "[%s]", indx);
                    } else{
                        snprintf(ebuf, sizeof(ebuf), "[%s*%i]", indx, scale);
                    }
                } else if(!scale){
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s]", base, indx);
                } else{
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s*%i]", base, indx, scale);
                }
                E = ebuf;
                break;
            case 0x05:
                snprintf(ebuf, sizeof(ebuf), "ds:%x", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x06:
                E = "[esi]";
                break;
            case 0x07:
                E = "[edi]";
                break;
             case 0x40:
                snprintf(ebuf, sizeof(ebuf), "[eax+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x41:
                snprintf(ebuf, sizeof(ebuf), "[ecx+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x42:
                snprintf(ebuf, sizeof(ebuf), "[edx+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x43:
                snprintf(ebuf, sizeof(ebuf), "[ebx+%x]", *(u8 *)++b); 
                E = ebuf;
            case 0x44:
                ++b;
                decodeSIB(); 
                if(!indx){
                    snprintf(ebuf, sizeof(ebuf), "[%s", base);
                } else if(!base){
                    if(!scale){
                        snprintf(ebuf, sizeof(ebuf), "[%s", indx);
                    } else{
                        snprintf(ebuf, sizeof(ebuf), "[%s*%i", indx, scale);
                    }
                } else if(!scale){
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s", base, indx);
                } else{
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s*%i", base, indx, scale);
                }
                len = strlen(ebuf);
                snprintf(ebuf+len, sizeof(ebuf)-len, "+%x]", *(u8 *)++b);
                E = ebuf;
                break;
            case 0x45:
                snprintf(ebuf, sizeof(ebuf), "[ebp+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x46:
                snprintf(ebuf, sizeof(ebuf), "[esi+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x47:
                snprintf(ebuf, sizeof(ebuf), "[edi+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x80:
                snprintf(ebuf, sizeof(ebuf), "[eax+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x81:
                snprintf(ebuf, sizeof(ebuf), "[ecx+%x]", *(u32 *)++b); 
                E = ebuf;
                break;
            case 0x82:
                snprintf(ebuf, sizeof(ebuf), "[edx+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x83:
                snprintf(ebuf, sizeof(ebuf), "[ebx+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
             case 0x84:
                ++b;
                decodeSIB(); 
                if(!indx){
                    snprintf(ebuf, sizeof(ebuf), "[%s", base);
                } else if(!base){
                    if(!scale){
                        snprintf(ebuf, sizeof(ebuf), "[%s", indx);
                    } else{
                        snprintf(ebuf, sizeof(ebuf), "[%s*%i", indx, scale);
                    }
                } else if(!scale){
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s", base, indx);
                } else{
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s*%i", base, indx, scale);
                }
                len = strlen(ebuf);
                snprintf(ebuf+len, sizeof(ebuf)-len, "+%x]", *(u32 *)++b);
                b += 3;
                E = ebuf;
                break;
           case 0x85:
                snprintf(ebuf, sizeof(ebuf), "[ebp+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x86:
                snprintf(ebuf, sizeof(ebuf), "[esi+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x87:
                snprintf(ebuf, sizeof(ebuf), "[edi+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
           case 0xc0:
                E = "st(0)";
                break;
            case 0xc1:
                E = "st(1)";
                break;
            case 0xc2:
                E = "st(2)";
                break;
            case 0xc3:
                E = "st(3)";
                break;
            case 0xc4:
                E = "st(4)";
                break;
            case 0xc5:
                E = "st(5)";
                break;
            case 0xc6:
                E = "st(6)";
                break;
            case 0xc7:
                E = "st(7)";
                break;
            default:
                if (show) print("Invalid Mod R/M byte.");
                return 1000;
        }
    }

    if(order){
        snprintf(op, opBufSz, "%s", E);
        len = strlen(op);
        if(Gsz){
            snprintf(op+len, opBufSz-len, ", %s", G);
        }
    } else{
        snprintf(op, opBufSz, "%s, ", G);
        len = strlen(op);
        snprintf(op+len, opBufSz-len, "%s", E);
    }
    return b-a;
}

u32 decodeModSM_memonly(bool show, u8 *a, u8 *op, u32 order, u32 Gsz, u32 Esz){
    u32 len = 0;
    u32 reg = 0;
    u32 scale = 0;
    u8 *b = a;
    u8 *E, *G, *indx, *base, *disp = "\0"; 
    u8 ebuf[32] = {0};
    
    setG();
    
    if(Esz){ 
        switch(*b&0xc7){
            case 0x00:
                E = "[bx+si]";
                break;
            case 0x01:
                E = "[bx+di]";
                break;
            case 0x02:
                E = "[bp+si]";
                break;
            case 0x03:
                E = "[bp+di]";
                break;
            case 0x04:
                E = "[si]";
                break;
            case 0x05:
                E = "[di]";
                break;
            case 0x06:
                snprintf(ebuf, sizeof(ebuf), "ds:%x", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x07:
                E = "[bx]";
                break;
            case 0x40:
                snprintf(ebuf, sizeof(ebuf), "[bx+si+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x41:
                snprintf(ebuf, sizeof(ebuf), "[bx+di+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x42:
                snprintf(ebuf, sizeof(ebuf), "[bp+si+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x43:
                snprintf(ebuf, sizeof(ebuf), "[bp+di+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x44:
                snprintf(ebuf, sizeof(ebuf), "[si+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x45:
                snprintf(ebuf, sizeof(ebuf), "[di+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x46:
                snprintf(ebuf, sizeof(ebuf), "[bp+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x47:
                snprintf(ebuf, sizeof(ebuf), "[bx+%x]", *(u8 *)++b); 
                E = ebuf;
            case 0x80:
                snprintf(ebuf, sizeof(ebuf), "[bx+si+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x81:
                snprintf(ebuf, sizeof(ebuf), "[bx+di+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x82:
                snprintf(ebuf, sizeof(ebuf), "[bp+si+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x83:
                snprintf(ebuf, sizeof(ebuf), "[bp+di+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x84:
                snprintf(ebuf, sizeof(ebuf), "[si+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x85:
                snprintf(ebuf, sizeof(ebuf), "[di+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x86:
                snprintf(ebuf, sizeof(ebuf), "[bp+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            case 0x87:
                snprintf(ebuf, sizeof(ebuf), "[bx+%x]", *(u16 *)++b); 
                E = ebuf;
                ++b;
                break;
            default:
                if (show) print("Invalid Mod R/M byte.");
                return 1000;
        }   
    } else{ 
        switch(*b&0xc7){
            case 0x00:
                E = "[eax]";
                break;
            case 0x01:
                E = "[ecx]";
                break;
            case 0x02:
                E = "[edx]";
                break;
            case 0x03:
                E = "[ebx]";
                break;
            case 0x04:
                ++b;
                decodeSIB();
                if(!indx){
                    snprintf(ebuf, sizeof(ebuf), "[%s]", base);
                } else if(!base){
                    if(!scale){
                        snprintf(ebuf, sizeof(ebuf), "[%s]", indx);
                    } else{
                        snprintf(ebuf, sizeof(ebuf), "[%s*%i]", indx, scale);
                    }
                } else if(!scale){
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s]", base, indx);
                } else{
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s*%i]", base, indx, scale);
                }
                E = ebuf;
                break;
            case 0x05:
                snprintf(ebuf, sizeof(ebuf), "ds:%x", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x06:
                E = "[esi]";
                break;
            case 0x07:
                E = "[edi]";
                break;
             case 0x40:
                snprintf(ebuf, sizeof(ebuf), "[eax+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x41:
                snprintf(ebuf, sizeof(ebuf), "[ecx+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x42:
                snprintf(ebuf, sizeof(ebuf), "[edx+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x43:
                snprintf(ebuf, sizeof(ebuf), "[ebx+%x]", *(u8 *)++b); 
                E = ebuf;
            case 0x44:
                ++b;
                decodeSIB(); 
                if(!indx){
                    snprintf(ebuf, sizeof(ebuf), "[%s", base);
                } else if(!base){
                    if(!scale){
                        snprintf(ebuf, sizeof(ebuf), "[%s", indx);
                    } else{
                        snprintf(ebuf, sizeof(ebuf), "[%s*%i", indx, scale);
                    }
                } else if(!scale){
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s", base, indx);
                } else{
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s*%i", base, indx, scale);
                }
                len = strlen(ebuf);
                snprintf(ebuf+len, sizeof(ebuf)-len, "+%x]", *(u8 *)++b);
                E = ebuf;
                break;
            case 0x45:
                snprintf(ebuf, sizeof(ebuf), "[ebp+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x46:
                snprintf(ebuf, sizeof(ebuf), "[esi+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x47:
                snprintf(ebuf, sizeof(ebuf), "[edi+%x]", *(u8 *)++b); 
                E = ebuf;
                break;
            case 0x80:
                snprintf(ebuf, sizeof(ebuf), "[eax+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x81:
                snprintf(ebuf, sizeof(ebuf), "[ecx+%x]", *(u32 *)++b); 
                E = ebuf;
                break;
            case 0x82:
                snprintf(ebuf, sizeof(ebuf), "[edx+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x83:
                snprintf(ebuf, sizeof(ebuf), "[ebx+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
             case 0x84:
                ++b;
                decodeSIB(); 
                if(!indx){
                    snprintf(ebuf, sizeof(ebuf), "[%s", base);
                } else if(!base){
                    if(!scale){
                        snprintf(ebuf, sizeof(ebuf), "[%s", indx);
                    } else{
                        snprintf(ebuf, sizeof(ebuf), "[%s*%i", indx, scale);
                    }
                } else if(!scale){
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s", base, indx);
                } else{
                    snprintf(ebuf, sizeof(ebuf), "[%s+%s*%i", base, indx, scale);
                }
                len = strlen(ebuf);
                snprintf(ebuf+len, sizeof(ebuf)-len, "+%x]", *(u32 *)++b);
                b += 3;
                E = ebuf;
                break;
           case 0x85:
                snprintf(ebuf, sizeof(ebuf), "[ebp+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x86:
                snprintf(ebuf, sizeof(ebuf), "[esi+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            case 0x87:
                snprintf(ebuf, sizeof(ebuf), "[edi+%x]", *(u32 *)++b); 
                E = ebuf;
                b += 3;
                break;
            default:
                if (show) print("Invalid Mod R/M byte.");
                return 1000;
        }
    }

    if(order){
        snprintf(op, opBufSz, "%s", E);
        len = strlen(op);
        if(Gsz){
            snprintf(op+len, opBufSz-len, ", %s", G);
        }
    } else{
        snprintf(op, opBufSz, "%s, ", G);
        len = strlen(op);
        snprintf(op+len, opBufSz-len, "%s", E);
    }
    return b-a;
}

/*******************************************************************************/
/* Décode une adresse mémoire au format intel */

u32 disasm(u8 *a, u8 *string, bool show){
    u8 *f_entry;
    u16 entry;
    u8 *b = a;
    u32 len = 0;
    u32 flip_addr_sz = 0;
    u32 flip_imm_sz = 0;
    u32 EG = 1; 
    u32 B  = 1;
    u8 *s, *prefix, *seg_oride = "\0";
    u8 op1[opBufSz] = {0};
    
    if(*b == 0xf3){
        prefix = "rep";
        ++b;
    } else if(*b == 0xf2){
        prefix = "repnz";
        ++b;
    } else if(*b == 0xf0){
        prefix = "lock";
        ++b;
    } 

    if(*b == 0x67){
        flip_addr_sz = 1;
        ++b;
    }
    if (*b == 0x66){
        flip_imm_sz = 1;
        ++b;
    }

    switch(*b){
        case 0x2e:
            seg_oride = "cs";
            ++b;
            break;
        case 0x36:
            seg_oride = "ss";
            ++b;
            break;
        case 0x3e:
            seg_oride = "ds";
            ++b;
            break;
        case 0x26:
            seg_oride = "es";
            ++b;
            break;
        case 0x64:
            seg_oride = "fs";
            ++b;
            break;
        case 0x65:
            seg_oride = "gs";
            ++b;
    }

    if(*b == 0x0f){ 

        if (show) print("Extended opcodes not implimented.\r\n");
        return 1000;
    
    } else{
        
        switch(*b){
            case 0x00:
                s = "add";
                b += decodeModSM(show, ++b, op1, 1, 1, flip_addr_sz);
                break;
            case 0x01:
                s = "add";
                b += decodeModSM(show, ++b, op1, 1, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x02:
                s = "add";
                b += decodeModSM(show, ++b, op1, 0, 1, flip_addr_sz);
                break;
            case 0x03:
                s = "add";
                b += decodeModSM(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x04:
                s = "add";
                ++b;
                snprintf(op1, sizeof(op1), "al, %x", (void *)*b);
                break;
            case 0x05:
                s = "add";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "ax, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "eax, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0x06:
                s = "push    es";
                break;
            case 0x07:
                s = "pop     es";
                break;
            case 0x08:
                s = "or";
                b += decodeModSM(show, ++b, op1, 1, 1, flip_addr_sz);
                break;
            case 0x09:
                s = "or";
                b += decodeModSM(show, ++b, op1, 1, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x0a:
                s = "or";
                b += decodeModSM(show, ++b, op1, 0, 1, flip_addr_sz);
                break;
            case 0x0b:
                s = "or";
                b += decodeModSM(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x0c:
                s = "or";
                ++b;
                snprintf(op1, sizeof(op1), "al, %x", (void *)*b);
                break;
            case 0x0d:
                s = "or";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "ax, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "eax, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0x0e:
                s = "push    cs";
                break;
            case 0x10:
                s = "adc";
                b += decodeModSM(show, ++b, op1, 1, 1, flip_addr_sz);
                break;
            case 0x11:
                s = "adc";
                b += decodeModSM(show, ++b, op1, 1, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x12:
                s = "adc";
                b += decodeModSM(show, ++b, op1, 0, 1, flip_addr_sz);
                break;
            case 0x13:
                s = "adc";
                b += decodeModSM(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x14:
                s = "adc";
                ++b;
                snprintf(op1, sizeof(op1), "al, %x", (void *)*b);
                break;
            case 0x15:
                s = "adc";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "ax, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "eax, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0x16:
                s = "push    ss";
                break;
            case 0x17:
                s = "pop     ss";
                break;
            case 0x18:
                s = "sbb";
                b += decodeModSM(show, ++b, op1, 1, 1, flip_addr_sz);
                break;
            case 0x19:
                s = "sbb";
                b += decodeModSM(show, ++b, op1, 1, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x1a:
                s = "sbb";
                b += decodeModSM(show, ++b, op1, 0, 1, flip_addr_sz);
                break;
            case 0x1b:
                s = "sbb";
                b += decodeModSM(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x1c:
                s = "sbb";
                ++b;
                snprintf(op1, sizeof(op1), "al, %x", (void *)*b);
                break;
            case 0x1d:
                s = "sbb";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "ax, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "eax, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0x1e:
                s = "push    ds";
                break;
            case 0x1f:
                s = "pop     ds";
                break;
            case 0x20:
                s = "and";
                b += decodeModSM(show, ++b, op1, 1, 1, flip_addr_sz);
                break;
            case 0x21:
                s = "and";
                b += decodeModSM(show, ++b, op1, 1, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x22:
                s = "and";
                b += decodeModSM(show, ++b, op1, 0, 1, flip_addr_sz);
                break;
            case 0x23:
                s = "and";
                b += decodeModSM(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x24:
                s = "and";
                ++b;
                snprintf(op1, sizeof(op1), "al, %x", (void *)*b);
                break;
            case 0x25:
                s = "and";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "ax, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "eax, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0x27:
                s = "daa";
                break;
            case 0x28:
                s = "sub";
                b += decodeModSM(show, ++b, op1, 1, 1, flip_addr_sz);
                break;
            case 0x29:
                s = "sub";
                b += decodeModSM(show, ++b, op1, 1, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x2a:
                s = "sub";
                b += decodeModSM(show, ++b, op1, 0, 1, flip_addr_sz);
                break;
            case 0x2b:
                s = "sub";
                b += decodeModSM(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x2c:
                s = "sub";
                ++b;
                snprintf(op1, sizeof(op1), "al, %x", (void *)*b);
                break;
            case 0x2d:
                s = "sub";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "ax, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "eax, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0x2f:
                s = "das";
                break;
            case 0x30:
                s = "xor";
                b += decodeModSM(show, ++b, op1, 1, 1, flip_addr_sz);
                break;
            case 0x31:
                s = "xor";
                b += decodeModSM(show, ++b, op1, 1, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x32:
                s = "xor";
                b += decodeModSM(show, ++b, op1, 0, 1, flip_addr_sz);
                break;
            case 0x33:
                s = "xor";
                b += decodeModSM(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x34:
                s = "xor";
                ++b;
                snprintf(op1, sizeof(op1), "al, %x", (void *)*b);
                break;
            case 0x35:
                s = "xor";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "ax, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "eax, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0x37:
                s = "aaa";
                break;
            case 0x38:
                s = "cmp";
                b += decodeModSM(show, ++b, op1, 1, 1, flip_addr_sz);
                break;
            case 0x39:
                s = "cmp";
                b += decodeModSM(show, ++b, op1, 1, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x3a:
                s = "cmp";
                b += decodeModSM(show, ++b, op1, 0, 1, flip_addr_sz);
                break;
            case 0x3b:
                s = "cmp";
                b += decodeModSM(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x3c:
                s = "cmp";
                ++b;
                snprintf(op1, sizeof(op1), "al, %x", (void *)*b);
                break;
            case 0x3d:
                s = "cmp";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "ax, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "eax, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0x3f:
                s = "aas";
                break;
            case 0x40:
                s = flip_imm_sz ? "inc     ax": "inc     eax";
                break;
            case 0x41:
                s = flip_imm_sz ? "inc     cx": "inc     ecx";
                break;
            case 0x42:
                s = flip_imm_sz ? "inc     dx": "inc     edx";
                break;
            case 0x43:
                s = flip_imm_sz ? "inc     bx": "inc     ebx";
                break;
            case 0x44:
                s = flip_imm_sz ? "inc     sp": "inc     esp";
                break;
            case 0x45:
                s = flip_imm_sz ? "inc     bp": "inc     ebp";
                break;
            case 0x46:
                s = flip_imm_sz ? "inc     si": "inc     esi";
                break;
            case 0x47:
                s = flip_imm_sz ? "inc     di": "inc     edi";
                break;
            case 0x48:
                s = flip_imm_sz ? "dec     ax": "dec     eax";
                break;
            case 0x49:
                s = flip_imm_sz ? "dec     cx": "dec     ecx";
                break;
            case 0x4a:
                s = flip_imm_sz ? "dec     dx": "dec     edx";
                break;
            case 0x4b:
                s = flip_imm_sz ? "dec     bx": "dec     ebx";
                break;
            case 0x4c:
                s = flip_imm_sz ? "dec     sp": "dec     esp";
                break;
            case 0x4d:
                s = flip_imm_sz ? "dec     bp": "dec     ebp";
                break;
            case 0x4e:
                s = flip_imm_sz ? "dec     si": "dec     esi";
                break;
            case 0x4f:
                s = flip_imm_sz ? "dec     di": "dec     edi";
                break;
            case 0x50:
                s = flip_imm_sz ? "push    ax": "push    eax";
                break;
            case 0x51:
                s = flip_imm_sz ? "push    cx": "push    ecx";
                break;
            case 0x52:
                s = flip_imm_sz ? "push    dx": "push    edx";
                break;
            case 0x53:
                s = flip_imm_sz ? "push    bx": "push    ebx";
                break;
            case 0x54:
                s = flip_imm_sz ? "push    sp": "push    esp";
                break;
            case 0x55:
                s = flip_imm_sz ? "push    bp": "push    ebp";
                break;
            case 0x56:
                s = flip_imm_sz ? "push    si": "push    esi";
                break;
            case 0x57:
                s = flip_imm_sz ? "push    di": "push    edi";
                break;
            case 0x58:
                s = flip_imm_sz ? "pop     ax": "pop     eax";
                break;
            case 0x59:
                s = flip_imm_sz ? "pop     cx": "pop     ecx";
                break;
            case 0x5a:
                s = flip_imm_sz ? "pop     dx": "pop     edx";
                break;
            case 0x5b:
                s = flip_imm_sz ? "pop     bx": "pop     ebx";
                break;
            case 0x5c:
                s = flip_imm_sz ? "pop     sp": "pop     esp";
                break;
            case 0x5d:
                s = flip_imm_sz ? "pop     bp": "pop     ebp";
                break;
            case 0x5e:
                s = flip_imm_sz ? "pop     si": "pop     esi";
                break;
            case 0x5f:
                s = flip_imm_sz ? "pop     di": "pop     edi";
                break;
            case 0x60:
                s = flip_imm_sz ? "pushaw": "pusha";
                break;
            case 0x61:
                s = flip_imm_sz ? "popaw": "popa";
                break;
            case 0x63:
                s = "arpl";
                b += decodeModSM(show, ++b, op1, 1, 1, flip_addr_sz);
                break;
            case 0x68:
                s = "push";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "%x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "%x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0x69:
                s = "imul";
                b += decodeModSM(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                len = strlen(op1);
                if(flip_imm_sz){
                    snprintf(op1+len, sizeof(op1)-len, ", %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1+len, sizeof(op1)-len, ", %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0x6a:
                s = "push";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)*b);
            case 0x6b:
                s = "imul";
                b += decodeModSM(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                len = strlen(op1);
                snprintf(op1+len, sizeof(op1)-len, ", %x", *(u8 *)++b);
               break;
            case 0x6c:
                s = flip_addr_sz ? "ins     bytes ptr es:[di], dx": "insb";
                break;
            case 0x6d:
                if(flip_addr_sz){
                    s = flip_imm_sz ? "ins     word ptr es:[di], dx": "ins     dword ptr es:[di], dx";
                } else{
                    s = flip_imm_sz ? "insw": "insd";
                }
                break;
            case 0x6e:
                s = flip_addr_sz ? "outs    dx, byte ptr [si]": "outsb";
                break;
            case 0x6f:
                if(flip_addr_sz){
                    s = flip_imm_sz ? "outs    dx, word ptr [si]": "outs    dx, dword ptr [si]";
                } else{
                    s = flip_imm_sz ? "outsw": "outsd";
                }
                break;
            case 0x70:
                s = "jo";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x71:
                s = "jno";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x72:
                s = "jb";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x73:
                s = "jnb";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x74:
                s = "jz";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x75:
                s = "jnz";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x76:
                s = "jbe";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x77:
                s = "ja";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x78:
                s = "js";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x79:
                s = "jns";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x7a:
                s = "jp";
                ++b;
                break;
            case 0x7b:
                s = "jnp";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x7c:
                s = "jl";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x7d:
                s = "jnl";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x7e:
                s = "jle";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x7f:
                s = "jnle";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0x80:
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                switch((*b&0x38)/8){
                    case 0:
                        s = "add";
                        break;
                    case 1:
                        s = "or";
                        break;
                    case 2:
                        s = "adc";
                        break;
                    case 3:
                        s = "sbb";
                        break;
                    case 4:
                        s = "and";
                        break;
                    case 5:
                        s = "sub";
                        break;
                    case 6:
                        s = "xor";
                        break;
                    case 7:
                        s = "cmp";
                        break;
                    default:
                        if (show) print("Invalid Mod R/M byte.");
                        return 1000;
                }
                len = strlen(op1);
                snprintf(op1+len, sizeof(op1)-len, ", %x", *(u8 *)++b);
                break;
             case 0x81:
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                 switch((*b&0x38)/8){
                    case 0:
                        s = "add";
                        break;
                    case 1:
                        s = "or";
                        break;
                    case 2:
                        s = "adc";
                        break;
                    case 3:
                        s = "sbb";
                        break;
                    case 4:
                        s = "and";
                        break;
                    case 5:
                        s = "sub";
                        break;
                    case 6:
                        s = "xor";
                        break;
                    case 7:
                        s = "cmp";
                        break;
                    default:
                        if (show) print("Invalid Mod R/M byte.");
                        return 1000;
                }
                len = strlen(op1);
                if(flip_imm_sz){
                    snprintf(op1+len, sizeof(op1)-len, ", %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1+len, sizeof(op1)-len, ", %x", *(u32 *)++b);
                    b += 3;
                }
                break;
           case 0x82:
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                switch((*b&0x38)/8){
                    case 0:
                        s = "add";
                        break;
                    case 1:
                        s = "or";
                        break;
                    case 2:
                        s = "adc";
                        break;
                    case 3:
                        s = "sbb";
                        break;
                    case 4:
                        s = "and";
                        break;
                    case 5:
                        s = "sub";
                        break;
                    case 6:
                        s = "xor";
                        break;
                    case 7:
                        s = "cmp";
                        break;
                    default:
                        if (show) print("Invalid Mod R/M byte.");
                        return 1000;
                }
               len = strlen(op1);
                snprintf(op1+len, sizeof(op1)-len, ", %x", *(u8 *)++b);
                break;
           case 0x83:
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                switch((*b&0x38)/8){
                    case 0:
                        s = "add";
                        break;
                    case 1:
                        s = "or";
                        break;
                    case 2:
                        s = "adc";
                        break;
                    case 3:
                        s = "sbb";
                        break;
                    case 4:
                        s = "and";
                        break;
                    case 5:
                        s = "sub";
                        break;
                    case 6:
                        s = "xor";
                        break;
                    case 7:
                        s = "cmp";
                        break;
                    default:
                        if (show) print("Invalid Mod R/M byte.");
                        return 1000;
                }
                len = strlen(op1);
                snprintf(op1+len, sizeof(op1)-len, ", %x", *(u8 *)++b);
                break;
            case 0x84:
                s = "test";
                b += decodeModSM(show, ++b, op1, 1, 1, flip_addr_sz);
                break;
            case 0x85:
                s = "test";
                b += decodeModSM(show, ++b, op1, 1, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x86:
                s = "xchg";
                b += decodeModSM(show, ++b, op1, 1, 1, flip_addr_sz);
                break;
            case 0x87:
                s = "xchg";
                b += decodeModSM(show, ++b, op1, 1, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x88:
                s = "mov";
                b += decodeModSM(show, ++b, op1, 1, 1, flip_addr_sz);
                break;
            case 0x89:
                s = "mov";
                b += decodeModSM(show, ++b, op1, 1, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x8a:
                s = "mov";
                b += decodeModSM(show, ++b, op1, 0, 1, flip_addr_sz);
                break;
            case 0x8b:
                s = "mov";
                b += decodeModSM(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x8c:
                s = "mov";
                b += decodeModSM(show, ++b, op1, 1, 4, flip_addr_sz);
                break;
            case 0x8d: 
                s = "lea";
                b += decodeModSM_memonly(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0x8e:
                s = "mov";
                b += decodeModSM(show, ++b, op1, 0, 4, flip_addr_sz);
                break;
            case 0x8f:
                s = "pop";
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                break;
            case 0x90:
                s = flip_imm_sz ? "xchg    ax, ax": "nop";
                break;
            case 0x91:
                s = flip_imm_sz ? "xchg    ax, cx": "xchg    eax, ecx";
                break;
            case 0x92:
                s = flip_imm_sz ? "xchg    ax, dx": "xchg    eax, edx";
                break;
            case 0x93:
                s = flip_imm_sz ? "xchg    ax, bx": "xchg    eax, ebx";
                break;
            case 0x94:
                s = flip_imm_sz ? "xchg    ax, sp": "xchg    eax, esp";
                break;
            case 0x95:
                s = flip_imm_sz ? "xchg    ax, bp": "xchg    eax, ebp";
                break;
            case 0x96:
                s = flip_imm_sz ? "xchg    ax, si": "xchg    eax, esi";
                break;
            case 0x97:
                s = flip_imm_sz ? "xchg    ax, di": "xchg    eax, edi";
                break;
            case 0x98:
                s = flip_imm_sz ? "cbw": "cwde";
                break;
            case 0x99:
                s = flip_imm_sz ? "cwd": "cdq";
                break;
            case 0x9a:
                s = "call";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "%x:%x", *(u16 *)(b+2), *(u16 *)++b);
                    b += 3;                
                } else{
                    snprintf(op1, sizeof(op1), "far ptr %x:%x", *(u16 *)(b+4), *(u32 *)++b);
                    b += 5;
                }
                break;
            case 0x9b:
                s = "wait";
                break;
            case 0x9c:
                s = flip_imm_sz ? "pushfw": "pushf";
                break;
            case 0x9d:
                s = flip_imm_sz ? "popfw": "popf";
                break;
            case 0x9e:
                s = "sahf";
                break;
            case 0x9f:
                s = "lahf";
                break;
            case 0xa0:
                s = "mov";
                snprintf(op1, sizeof(op1), "al, ds:%x", *(u32 *)++b); 
                b += 3;
                break;
            case 0xa1:
                s = "mov";
                snprintf(op1, sizeof(op1), flip_imm_sz ? "ax, ds:%x": "eax, ds:%x", *(u32 *)++b); 
                b += 3;
                break;
            case 0xa2:
                s = "mov";
                snprintf(op1, sizeof(op1), "ds:%x, al", *(u32 *)++b); 
                b += 3;
                break;
            case 0xa3:
                s = "mov";
                snprintf(op1, sizeof(op1), flip_imm_sz ? "ds:%x, ax": "ds:%x, eax", *(u32 *)++b); 
                b += 3;
                break;
            case 0xa4:
                s = flip_addr_sz ? "movs    byte ptr es:[di], byte ptr [si]": "movsb";
                break;
            case 0xa5:
                if(flip_addr_sz){
                    s = flip_imm_sz ? "movs    word ptr es:[di], word ptr [si]": "movs    dword ptr es:[di], dword ptr [si]";
                } else{
                    s = flip_imm_sz ? "movsw": "movsd";
                }
                break;
            case 0xa6:
                s = flip_addr_sz ? "cmps    byte ptr [si], byte ptr es:[di]": "cmpsb";
                break;
            case 0xa7:
                if(flip_addr_sz){
                    s = flip_imm_sz ? "movs    word ptr [si], word ptr es:[di]": "movs    dword ptr [si], dword ptr es:[di]";
                } else{
                    s = flip_imm_sz ? "cmpsw": "cmpsd";
                }
                break;
            case 0xa8:
                s = "test";
                ++b;
                snprintf(op1, sizeof(op1), "al, %x", (void *)*b);
                break;
            case 0xa9:
                s = "test";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "eax, %x", *(u32 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "ax, %x", *(u16 *)++b);
                    b += 3;
                }
                break;
            case 0xaa:
                s = flip_addr_sz ? "stos    byte ptr es:[di]": "stosb";
                break;
            case 0xab:
                if(flip_addr_sz){
                    s = flip_imm_sz ? "stos    word ptr es:[di]": "stos    dword ptr es:[di]";
                } else{
                    s = flip_imm_sz ? "stosw": "stosd";
                }
                break;
            case 0xac:
                s = flip_addr_sz ? "lods    byte ptr [si]": "lodsb";
                break;
            case 0xad:
                if(flip_addr_sz){
                    s = flip_imm_sz ? "lods    word ptr [si]": "lods    dword ptr [si]";
                } else{
                    s = flip_imm_sz ? "lodsw": "lodsd";
                }
                break;
            case 0xae:
                s = flip_addr_sz ? "scas    byte ptr es:[di]": "scasb";
                break;
            case 0xaf:
                if(flip_addr_sz){
                    s = flip_imm_sz ? "scas    word ptr es:[di]": "scas    dword ptr es:[di]";
                } else{
                    s = flip_imm_sz ? "scasw": "scasd";
                }
                break;
            case 0xb0:
                s = "mov";
                ++b;
                snprintf(op1, sizeof(op1), "al, %x", (void *)*b);
                break;
            case 0xb1:
                s = "mov";
                ++b;
                snprintf(op1, sizeof(op1), "cl, %x", (void *)*b);
                break;
            case 0xb2:
                s = "mov";
                ++b;
                snprintf(op1, sizeof(op1), "dl, %x", (void *)*b);
                break;
            case 0xb3:
                s = "mov";
                ++b;
                snprintf(op1, sizeof(op1), "al, %x", (void *)*b);
                break;
            case 0xb4:
                s = "mov";
                ++b;
                snprintf(op1, sizeof(op1), "ah, %x", (void *)*b);
                break;
            case 0xb5:
                s = "mov";
                ++b;
                snprintf(op1, sizeof(op1), "ch, %x", (void *)*b);
                break;
            case 0xb6:
                s = "mov";
                ++b;
                snprintf(op1, sizeof(op1), "dh, %x", (void *)*b);
                break;
            case 0xb7:
                s = "mov";
                ++b;
                snprintf(op1, sizeof(op1), "bh, %x", (void *)*b);
                break;
            case 0xb8:
                s = "mov";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "ax, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "eax, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0xb9:
                s = "mov";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "cx, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "ecx, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0xba:
                s = "mov";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "dx, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "edx, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0xbb:
                s = "mov";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "bx, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "ebx, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0xbc:
                s = "mov";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "sp, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "esp, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0xbd:
                s = "mov";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "bp, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "ebp, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0xbe:
                s = "mov";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "si, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "esi, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0xbf:
                s = "mov";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "di, %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "edi, %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0xc0:
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                switch((*b&0x38)/8){
                    case 0:
                        s = "rol";
                        break;
                    case 1:
                        s = "ror";
                        break;
                    case 2:
                        s = "rcl";
                        break;
                    case 3:
                        s = "rcr";
                        break;
                    case 4:
                        s = "shl";
                        break;
                    case 5:
                        s = "shr";
                        break;
                    case 6:
                        s = "shl";
                        break;
                    case 7:
                        s = "sar";
                        break;
                    default:
                        if (show) print("Invalid Mod R/M byte.");
                        return 1000;
                }
                len = strlen(op1);
                snprintf(op1+len, sizeof(op1)-len, ", %x", *(u8 *)++b);
                break;
            case 0xc1:
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                switch((*b&0x38)/8){
                    case 0:
                        s = "rol";
                        break;
                    case 1:
                        s = "ror";
                        break;
                    case 2:
                        s = "rcl";
                        break;
                    case 3:
                        s = "rcr";
                        break;
                    case 4:
                        s = "shl";
                        break;
                    case 5:
                        s = "shr";
                        break;
                    case 6:
                        s = "shl";
                        break;
                    case 7:
                        s = "sar";
                        break;
                    default:
                        if (show) print("Invalid Mod R/M byte.");
                        return 1000;
                }
                len = strlen(op1);
                snprintf(op1+len, sizeof(op1)-len, ", %x", *(u8 *)++b);
                break;
           case 0xc2:
                s = "retn";
                snprintf(op1, sizeof(op1), "%x", *(u16 *)++b);
                ++b;
                break;
            case 0xc3:
                s = "retn";
                break;
            case 0xc4:
                s = "les";
                b += decodeModSM_memonly(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0xc5: 
                s = "lds";
                b += decodeModSM_memonly(show, ++b, op1, 0, flip_imm_sz ? 2: 3, flip_addr_sz);
                break;
            case 0xc6:
                s = "mov";
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                len = strlen(op1);
                snprintf(op1+len, sizeof(op1)-len, ", %x", *(u8 *)++b);
                break;
            case 0xc7:
                s = "mov";
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                len = strlen(op1);
                if(flip_imm_sz){
                    snprintf(op1+len, sizeof(op1)-len, ", %x", *(u16 *)++b);
                    ++b;
                } else{
                    snprintf(op1+len, sizeof(op1)-len, ", %x", *(u32 *)++b);
                    b += 3;
                }
                break;
            case 0xc8:
                s = "enter";
                snprintf(op1, sizeof(op1), "%x, %x", *(u16 *)(b+1), *(u8 *)(b+3));
                b += 3;
                break;
            case 0xc9:
                s = "leave";
                break;
            case 0xca:
                s = "retf";
                snprintf(op1, sizeof(op1), "%x", *(u16 *)++b);
                ++b;
                break;
            case 0xcb:
                s = "retf";
                break;
            case 0xcc:
                s = "int     3";
                break;
            case 0xcd:
                s = "int";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)*b);
                break;
            case 0xce:
                s = "into";
                break;
            case 0xcf:
                s = "iret";
                break;
            case 0xd0:
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                switch((*b&0x38)/8){
                    case 0:
                        s = "rol";
                        break;
                    case 1:
                        s = "ror";
                        break;
                    case 2:
                        s = "rcl";
                        break;
                    case 3:
                        s = "rcr";
                        break;
                    case 4:
                        s = "shl";
                        break;
                    case 5:
                        s = "shr";
                        break;
                    case 6:
                        s = "shl";
                        break;
                    case 7:
                        s = "sar";
                        break;
                    default:
                        if (show) print("Invalid Mod R/M byte.");
                        return 1000;
                }
                len = strlen(op1);
                snprintf(op1+len, sizeof(op1)-len, ", 1");
                break;
            case 0xd1:
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                switch((*b&0x38)/8){
                    case 0:
                        s = "rol";
                        break;
                    case 1:
                        s = "ror";
                        break;
                    case 2:
                        s = "rcl";
                        break;
                    case 3:
                        s = "rcr";
                        break;
                    case 4:
                        s = "shl";
                        break;
                    case 5:
                        s = "shr";
                        break;
                    case 6:
                        s = "shl";
                        break;
                    case 7:
                        s = "sar";
                        break;
                    default:
                        if (show) print("Invalid Mod R/M byte.");
                        return 1000;
                }
                len = strlen(op1);
                snprintf(op1+len, sizeof(op1)-len, ", 1");
                break;
            case 0xd2:
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                switch((*b&0x38)/8){
                    case 0:
                        s = "rol";
                        break;
                    case 1:
                        s = "ror";
                        break;
                    case 2:
                        s = "rcl";
                        break;
                    case 3:
                        s = "rcr";
                        break;
                    case 4:
                        s = "shl";
                        break;
                    case 5:
                        s = "shr";
                        break;
                    case 6:
                        s = "shl";
                        break;
                    case 7:
                        s = "sar";
                        break;
                    default:
                        if (show) print("Invalid Mod R/M byte.");
                        return 1000;
                }
                len = strlen(op1);
                snprintf(op1+len, sizeof(op1)-len, ", cl");
                break;
            case 0xd3:
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                switch((*b&0x38)/8){
                    case 0:
                        s = "rol";
                        break;
                    case 1:
                        s = "ror";
                        break;
                    case 2:
                        s = "rcl";
                        break;
                    case 3:
                        s = "rcr";
                        break;
                    case 4:
                        s = "shl";
                        break;
                    case 5:
                        s = "shr";
                        break;
                    case 6:
                        s = "shl";
                        break;
                    case 7:
                        s = "sar";
                        break;
                    default:
                        if (show) print("Invalid Mod R/M byte.");
                        return 1000;
                }
                len = strlen(op1);
                snprintf(op1+len, sizeof(op1)-len, ", cl");
                break;
           case 0xd4:
                s = "aam";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)*b);
                break;
            case 0xd5:
                s = "aad";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)*b);
                break;
            case 0xd6:
                s = "salc";
                break;
            case 0xd7:
                s = "xlat";
                break;
            case 0xd8:
                if(*++b == 0xd1){
                   s = "fcom    st(1)";
                } else if(*b == 0xd9){
                    s = "fcomp   st(1)";
                } else{
                    switch((*b&0x38)>>3){
                        case 0:
                            s = "fadd";
                            b += decodeModSM_float(show, b, op1, 1, 0, flip_addr_sz);
                            break;
                        case 1:
                            s = "fmul";
                            b += decodeModSM_float(show, b, op1, 1, 0, flip_addr_sz);
                            break;
                        case 2:
                            s = "fcom";
                            b += decodeModSM_float(show, b, op1, 1, 0, flip_addr_sz);
                            break;
                        case 3:
                            s = "fcomp";
                            b += decodeModSM_float(show, b, op1, 1, 0, flip_addr_sz);
                            break;
                        case 4:
                            s = "fsub";
                            b += decodeModSM_float(show, b, op1, 1, 0, flip_addr_sz);
                            break;
                        case 5:
                            s = "fsubr";
                            b += decodeModSM_float(show, b, op1, 1, 0, flip_addr_sz);
                            break;
                        case 6:
                            s = "fdiv";
                            b += decodeModSM_float(show, b, op1, 1, 0, flip_addr_sz);
                            break;
                        case 7:
                            s = "fdivr";
                            b += decodeModSM_float(show, b, op1, 1, 0, flip_addr_sz);
                            break;
                        default:
                            if (show) print("Invalid Mod R/M byte.");
                            return 1000;
                    }
                }
                break;
           case 0xe0:
                s = "loopnz";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0xe1:
                s = "loopz";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0xe2:
                s = "loop";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0xe3:
                s = "jcxz";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0xe4:
                s = "in";
                ++b;
                snprintf(op1, sizeof(op1), "al, %x", (void *)*b);
                break;
            case 0xe5:
                s = "in";
                ++b;
                snprintf(op1, sizeof(op1), flip_imm_sz ? "ax, %x": "eax, %x", (void *)*b);
                break;
            case 0xe6:
                s = "out";
                ++b;
                snprintf(op1, sizeof(op1), "%x, al", (void *)*b);
                break;
            case 0xe7:
                s = "in";
                ++b;
                snprintf(op1, sizeof(op1), flip_imm_sz ? "%x, ax": "%x, eax", (void *)*b);
                break;
            case 0xe8:
                s = "call";
                if(flip_imm_sz){
                snprintf(op1, sizeof(op1), "%x", entry+(u32)b-(u32)f_entry+*(u16 *)++b+3);
                    b += 2;
                } else{
                    snprintf(op1, sizeof(op1), "%x",(void *)entry+(u32)b-(u32)f_entry+*(u32 *)++b+4);
                    b += 3;
                }
                break;
            case 0xe9:
                s = "jmp";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "near ptr %x", entry+(u32)b-(u32)f_entry+*(u16 *)++b+3);
                    ++b;
                } else{
                    snprintf(op1, sizeof(op1), "near ptr %x", (void *)entry+(u32)b-(u32)f_entry+*(u32 *)++b+4);
                    b += 3;
                }
                break;
            case 0xea:
                s = "jmp";
                if(flip_imm_sz){
                    snprintf(op1, sizeof(op1), "%x:%x", *(u16 *)(b+2), *(u16 *)++b);
                    b += 3;
                } else{
                    snprintf(op1, sizeof(op1), "%x:%x", *(u16 *)(b+4), *(u32 *)++b);
                    b += 5;
                }
                break;
            case 0xeb:
                s = "jmp";
                ++b;
                snprintf(op1, sizeof(op1), "%x", (void *)entry+(u32)b-(u32)f_entry+*b+1);
                break;
            case 0xec:
                s = "in      al, dx";
                break;
            case 0xed:
                s = flip_imm_sz ? "in      ax, dx": "in      eax, dx";
                break;
            case 0xee:
                s= "out     dx, al";
                break;
            case 0xef:
                s = flip_imm_sz ? "out     dx, ax": "out     dx, eax";
                break;
            case 0xf1:
                s = "int     1";
                break;
            case 0xf4:
                s = "hlt";
                break;
            case 0xf5:
                s = "cmc";
                break;
            case 0xf6:
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                switch((*b&0x38)/8){
                    case 0:
                        s = "test";
                        len = strlen(op1);
                        snprintf(op1+len, sizeof(op1)-len, "%x", *(u8 *)++b);
                        break;
                    case 1:
                        s = "test";
                        len = strlen(op1);
                        snprintf(op1+len, sizeof(op1)-len, "%x", *(u8 *)++b);
                        break;
                    case 2:
                        s = "not";
                        break;
                    case 3:
                        s = "neg";
                        break;
                    case 4:
                        s = "mul     ax, al, ";
                        break;
                    case 5:
                        s = "imul    ax, al, ";
                        break;
                    case 6:
                        s = "div     al, ah, ax, ";  
                        break;
                    case 7:
                        s = "idiv    al, ah, ax, ";
                        break;
                    default:
                        if (show) print("Invalid Mod R/M byte.");
                        return 1000;
                }
                break;
            case 0xf7:
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                switch((*b&0x38)/8){
                    case 0:
                        s = "test";
                        len = strlen(op1);
                        if(flip_imm_sz){
                            snprintf(op1+len, sizeof(op1)-len, "%x", *(u16 *)++b);
                            ++b;
                        } else{
                            snprintf(op1+len, sizeof(op1)-len, "%x", *(u32 *)++b);
                            b += 3;
                        }
                        break;
                    case 1:
                        s = "test";
                        len = strlen(op1);
                        if(flip_imm_sz){
                            snprintf(op1+len, sizeof(op1)-len, "%x", *(u16 *)++b);
                            ++b;
                        } else{
                            snprintf(op1+len, sizeof(op1)-len, "%x", *(u32 *)++b);
                            b += 3;
                        }
                        break;
                    case 2:
                        s = "not";
                        break;
                    case 3:
                        s = "neg";
                        break;
                    case 4:
                        s = "mul     rdx, rax, ";
                        break;
                    case 5:
                        s = "imul    rdx, rax, ";
                        break;
                    case 6:
                        s = "div     rdx, rax, ";
                        break;
                    case 7:
                        s = "idiv    rdx, rax, ";
                        break;
                    default:
                        if (show) print("Invalid Mod R/M byte.");
                        return 1000;
                }
                break;
           case 0xf8:
                s = "clc";
                break;
            case 0xf9:
                s = "stc";
                break;
            case 0xfa:
                s = "cli";
                break;
            case 0xfb:
                s = "sti";
                break;
            case 0xfc:
                s = "cld";
                break;
            case 0xfd:
                s = "std";
                break;
            case 0xfe:
                b += decodeModSM(show, ++b, op1, 1, 0, flip_addr_sz);
                if(!((*b&0x38)/8)){
                    s = "inc";
                } else if((*b&0x38)/8 == 1){
                    s = "dec";
                } else{
                    if (show) print("Invalid Mod R/M byte.");
                    return 1000;
                }
                break;
            case 0xff:
                switch((*++b&0x38)>>3){
                    case 0:
                        s = "inc";
                        b += decodeModSM(show, b, op1, 1, 0, flip_addr_sz);
                        break;
                    case 1:
                        s = "dec";
                        b += decodeModSM(show, b, op1, 1, 0, flip_addr_sz);
                        break;
                    case 2:
                        s = "call";
                        b += decodeModSM(show, b, op1, 1, 0, flip_addr_sz);
                        break;
                    case 3:  
                        s = "callf";
                        b += decodeModSM_memonly(show, b, op1, 1, 0, flip_addr_sz);
                        break;
                    case 4:
                        s = "jmp";
                        b += decodeModSM(show, b, op1, 1, 0, flip_addr_sz);
                        break;
                    case 5:  
                        s = "jmpf";
                        b += decodeModSM_memonly(show, b, op1, 1, 0, flip_addr_sz);
                        break;
                    case 6:
                        s = "push";
                        b += decodeModSM(show, b, op1, 1, 0, flip_addr_sz);
                        break;
                    default:
                        if (show) print("Invalid Mod R/M byte.");
                        return 1000;
                }
                break;
            default:
                if (show) print("invalid opcode\r\n");
                return 1000;
        }
            strcompressdelimiter(s,' ');
            if (show)
                printf(" %X:  %s %s\r\n", (u32)a, s, op1);             
            else
                if (string!=NULL) sprintf(string, " %X:  %s %s\r\n", (u32)a, s, op1);
            
        ++b;

    }

    return b-a;
}

