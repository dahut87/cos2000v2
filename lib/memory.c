/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "memory.h"

u32 *pd0 = (u32 *) KERNEL_PGD_ADDR;	  /* page directory */
u8 *pg0 = (u8 *) 0;		              /* page 0 */
u8 *pg1 = (u8 *) (PAGESIZE*PAGENUMBER); /* page 1 */

/*******************************************************************************/ 
/* Initialisation de la mémoire paginée */ 

void initpaging(void) 
{
    u16 i;
    pd0[0] = ((u32) pg0 | (PAGE_PRESENT | PAGE_WRITE | PAGE_4MB));
	pd0[1] = ((u32) pg1 | (PAGE_PRESENT | PAGE_WRITE | PAGE_4MB));
	for (i = 2; i < 1023; i++)
		pd0[i] = ((u32) pg1 + PAGESIZE * i) | (PAGE_PRESENT | PAGE_WRITE);

	pd0[1023] = ((u32) pd0 | (PAGE_PRESENT | PAGE_WRITE));

	asm("mov %[pd0_addr], %%eax \n \
        mov %%eax, %%cr3 \n \
        mov %%cr4, %%eax \n \
		or $0x00000010, %%eax \n \
        mov %%eax, %%cr4 \n \
        mov %%cr0, %%eax \n \
		or $0x80000001, %%eax \n \
        mov %%eax, %%cr0"::[pd0_addr]"m"(pd0));
}

/*******************************************************************************/ 
/* Copie un octet une ou plusieurs fois en mémoire */ 

void memset(void *dst, u8 val, u32 count, u32 size) 
{
u8 * temp;
for (temp = (u8 *) dst; count != 0; count--) {	
temp += size;	
*temp = val;
}
}

/*******************************************************************************/   
/* Copie une portion de mémoire vers une autre */ 

void memcpy(void *src, void *dst, u32 count, u32 size)
{
char *s, *d;
u32 i;
s = (u8 *) src;
d = (u8 *) dst;
for (i = 0; i < count; i++) {	
*(d + i * size) = *(s + i);
}
}

/*******************************************************************************/   
/* Compare 2 portions de mémoire */ 
    
u32 memcmp(void *src, void *dst, u32 count, u32 size) 
{
const u8 *mem1 = (const u8 *)src;
const u8 *mem2 = (const u8 *)dst;
for (; count != 0; count--) {
if (*mem1 != *mem2)	
return *mem1 - *mem2;
mem1 += size;
mem2 += size;
}
}
 
/*******************************************************************************/ 
