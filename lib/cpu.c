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

void strcpuinfos(cpuinfo *proc,u8 *string)
{

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
          cpuid(0x80000002+i, &regeax, &regebx, &regecx, &regedx);    
          memcpy(&regeax,&proc->detectedname[0+i*16],4,1); 
         memcpy(&regebx,&proc->detectedname[4+i*16],4,1); 
          memcpy(&regecx,&proc->detectedname[8+i*16],4,1); 
           memcpy(&regedx,&proc->detectedname[12+i*16],4,1);   
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

