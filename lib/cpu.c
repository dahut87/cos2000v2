#include "types.h"
#include "cpu.h"
#include "cpuid.h"
#include "memory.h"
#include "string.h"



static u8 *msg[] =
{
  "mmx",
  "mmx2",
  "sse",
  "sse2",
  "sse3",
  "fpu",  
  "3dNow!",
  "3dNow Extended!",
  "HyperThreading",
  "apic",
};

static u8 space[]=" ";

bool cansetflag (u32 flag)
{
u32 r1, r2;
asm("pushfl\n"
"popl %0\n"
"movl %0, %1\n"
"xorl %2, %0\n"
"pushl %0\n"
"popfl\n"
"pushfl\n"
"popl %0\n"
"pushl %1\n"
"popfl\n"
: "=&r" (r1), "=&r" (r2)
: "ir" (flag)
);
return ((r1 ^ r2) & flag) != 0;
}

void cpuid(u32 op, u32 *eax, u32 *ebx,u32 *ecx, u32 *edx)
{
	asm("cpuid" : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)	: "a" (op) : "cc");
}

u8 getcpuinfos(cpuinfo *proc)
{
u32 i,maxfunction,maxextended,unused,regeax,regebx,regecx,regedx;
bool *boolean;
      
if (!cansetflag (0x00040000)) return 1; /*386 processor - no cpuid*/
if (!cansetflag (0x00200000)) return 2; /*486 processor with no cpuid*/

    cpuid(0, &maxfunction, &unused, &unused, &unused);
    maxfunction &= 0xffff;
    cpuid(0x80000000, &maxextended, &unused, &unused, &unused);
    maxextended &= 0xffff;
    if (maxfunction >= 1)
    {
         cpuid(1, &regeax, &regebx, &regecx, &regedx);
         proc->stepping = (regeax & 0x0000000F); 
         proc->models = ((regeax>>4) & 0x0000000F); 
         proc->family = ((regeax>>8) & 0x0000000F); 
         proc->types = ((regeax>>12) & 0x00000003); 
         proc->emodels = ((regeax>>16) & 0x0000000F); 
         proc->efamily = ((regeax>>20) & 0x000000FF); 
                
         proc->brandid = (regeax & 0xF); 
         proc->linesize = ((regeax>>8) & 0xF); 
         proc->count = ((regeax>>16) & 0xF); 
         proc->apicid = ((regeax>>24) & 0xF); 
                
         proc->mmx=((regedx>>23) & 0x00000001);
         proc->sse=((regedx>>25) & 0x00000001);
         proc->sse2=((regedx>>26) & 0x00000001);
         proc->sse3=(regecx & 0x00000001);
         proc->fpu=(regedx & 0x00000001);
         proc->htt=((regedx>>28) & 0x00000001);

    }  
    if (maxextended >= 1)
    {
         cpuid(0x80000001, &regeax, &regebx, &regecx, &regedx);    
         proc->mmx2=((regedx>>22) & 0x00000001);        
         proc->apic=((regedx>>9) & 0x00000001);         
         proc->now3d=((regedx>>30) & 0x00000001);
         proc->now3d2=((regedx>>31) & 0x00000001);         
     } 
     if (maxextended >= 4)
     {
         int i;
         for(i=0;i<3;i++)
         {
            cpuid(0x80000002+i, &regeax, &regebx, &regecx, &regedx);    
            memcpy(&regeax,&proc->detectedname[0+i*16],4,1); 
            memcpy(&regebx,&proc->detectedname[4+i*16],4,1); 
            memcpy(&regecx,&proc->detectedname[8+i*16],4,1); 
            memcpy(&regedx,&proc->detectedname[12+i*16],4,1);   
         }
    }    
    boolean=&proc->mmx;
    i=0;

    for(i=0;i<10;i++)
        if (*(boolean++)==1) 
        {
            strcat(msg[i],&proc->techs); 
            strcat(space,&proc->techs); 
        }
    return 0;
}

u32 viewstack(u32 number)
{
 u32 stack = 0;
  asm (
         "movl %[a1],%%esi;"
         "addl %%esp,%%esi;"
         "movl (%%esi), %[a1] ;"
         :[result] "=r" (stack)
         :[a1] "r" (number)
         :"%esi"
         );
    return stack;
}

void dump_regs()
{
  u32 eax = 0;
  u32 ebx = 0;
  u32 ecx = 0;
  u32 edx = 0;
  u32 esi = 0;
  u32 edi = 0;
  u32 ebp = 0;
  u32 esp = 0;
  u16 cs = 0;
  u16 ds = 0;
  u16 es = 0;
  u16 fs = 0;
  u16 gs = 0;
  u16 ss = 0;
  u32 flags = 0;
  asm (
         "movl %%eax, %[a1] ;"
         "movl %%ebx, %[b1] ;"
         "movl %%ecx, %[c1] ;"
         "movl %%edx, %[d1] ;"
         "movl %%esi, %[e1] ;"
         "movl %%edi, %[f1] ;"
         "movl %%esp, %[g1] ;"
         "movl %%ebp, %[h1] ;"
         "movw %%cs, %[i1] ;"
         "movw %%ds, %[j1] ;"
         "movw %%es, %[k1] ;"
         "movw %%fs, %[l1] ;"
         "movw %%gs, %[m1] ;"
         "movw %%ss, %[n1] ;"
         :
         [a1] "=m" (eax), [b1] "=m" (ebx), [c1] "=m" (ecx), [d1] "=m" (edx), [e1] "=m" (esi), [f1] "=m" (edi), 
         [g1] "=m" (ebp), [h1] "=m" (esp), [i1] "=m" (cs), [j1] "=m" (ds), [k1] "=m" (es), [l1] "=m" (fs), [m1] "=m" (gs), [n1] "=m" (ss)
         );

    printf("eax=%x      ebx=%x         ecx=%x        eax=%x\r\n", eax,ebx,ecx,edx);
    printf("esi=%x      edi=%x\r\n", esi,edi);
    printf("cs=%x       ds=%x          es=%x         fs=%x       gs=%x\r\n", cs,ds,es,fs,gs);
    printf("ss=%x       esp=%x         ebp=%x\r\n",ss,esp,ebp);

    asm (
         "pushf      ;"
         "pop %[f1] ;"
         :
         [f1] "=m" (flags)
         );

    printf("FLAGS");

    if(flags & (1 << 0)) // Carry
        printf(" (C1"); 
    else
        printf(" (C0");

    if(flags & (1 << 2)) // Parity
        printf(" P1");
    else
        printf(" P0");

    if(flags & (1 << 4)) // Adjust
        printf(" A1");
    else
        printf(" A0");

    if(flags & (1 << 6)) // Zero
        printf(" Z1");
    else
        printf(" Z0");

    if(flags & (1 << 7)) // Sign
        printf(" S1");
    else
        printf(" S0");

    if(flags & (1 << 11)) // Overflow
        printf(" O1)\n");
    else
        printf(" O0)\n");

  printf("STACK\r\n");
  for(u8 i=0;i<25;i++)
        printf("+%d\t\t%x\r\n",i,viewstack(i*4));


}

