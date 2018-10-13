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
    u8 *d = (u8 *) dst;
    if (size>0) size--;
    for (; count != 0; count--) {	
        *(d++) = val;
        d+=size;	
    }
}

/*******************************************************************************/   
/* Copie une portion de mémoire vers une autre */ 

void memcpy(void *src, void *dst, u32 count, u32 size)
{
    u8 *s = (u8 *) src;
    u8 *d = (u8 *) dst;
    if (size>0) size--;
    for (; count != 0; count--) {	
        *(d++) = *(s++);
        d+=size;
    }
}

/*******************************************************************************/   
/* Compare 2 portions de mémoire */ 
    
u32 memcmp(void *src, void *dst, u32 count, u32 size) 
{
    u8 *s = (u8 *) src;
    u8 *d = (u8 *) dst;
    if (size>0) size--;
    for (; count != 0; count--) {
        if (*(s++) != *(d++))	
            return *d - *s;
        s+= size;
        d+= size;
}
}
 
/*******************************************************************************/ 
