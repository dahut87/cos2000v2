/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "memory.h"
#include "multiboot2.h"

static u32 *pd0 = (u32 *) KERNEL_PGD_ADDR;	  /* page directory */
static u8 *pg0 = (u8 *) 0;		              /* page 0 */
static u8 *pg1 = (u8 *) (PAGESIZE*PAGENUMBER); /* page 1 */
static u8 bitmap[MAXMEMPAGE / 8];

/*******************************************************************************/ 
/* Retourne la taille de la mémoire (selon grub) */ 

u64 getmemorysize()
{
    u64 maxaddr=0;
    struct multiboot_tag_mmap *tag=getgrubinfo_mem();
    multiboot_memory_map_t *mmap;       
    for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;(u8 *) mmap < (u8 *) tag + tag->size; mmap = (multiboot_memory_map_t *)
                        ((unsigned long) mmap + ((struct multiboot_tag_mmap *) tag)->entry_size))
        if (mmap->addr+mmap->len>maxaddr)
                maxaddr=mmap->addr+mmap->len;
    return maxaddr;
}

/*******************************************************************************/ 
/* Retourne que la page actuelle est occupée */ 

void bitmap_page_use(page)
{
	bitmap[((u32) page)/8] |= (1 << (((u32) page)%8));
}

/*******************************************************************************/ 
/* Retourne que la page actuelle est libre */ 

void bitmap_page_free(page)
{
	bitmap[((u32) page)/8] &= ~(1 << (((u32) page)%8));
}

/*******************************************************************************/ 
/* Reserve un espace mémoire dans le bitmap */ 

void bitmap_page_setused(u64 addr,u64 len)
{
    u32 nbpage=TOPAGE(len);
    u32 pagesrc=TOPAGE(addr);
    if (len & 0b1111111111 > 0)
        nbpage++;
    for(u32 page=pagesrc;page<pagesrc+nbpage;page++)
        bitmap_page_use(page);
}

/*******************************************************************************/ 
/* Indique un espace mémoire libre dans le bitmap */ 

void bitmap_page_setfree(u64 addr,u64 len)
{
    u32 nbpage=TOPAGE(len);
    u32 pagesrc=TOPAGE(addr);
    if (len & 0b1111111111 > 0)
        nbpage++;
    for(u32 page=pagesrc;page<pagesrc+nbpage;page++)
        bitmap_page_free(page);
}

/*******************************************************************************/ 
/* Retourne une page libre */ 

u8* bitmap_page_getonefree(void)
{
	u8 byte, bit;
	u32 page = 0;
	for (byte = 0; byte < sizeof(bitmap); byte++)
		if (bitmap[byte] != 0xFF)
			for (bit = 0; bit < 8; bit++)
				if (!(bitmap[byte] & (1 << bit))) {
					page = 8 * byte + bit;
					bitmap_page_use(page);
					return (u8 *) (page * PAGESIZE);
				}
	return NULL;
}

/*******************************************************************************/ 
/* Retourne l'espace libre */ 

u64 getmemoryfree(void)
{
	u32 byte, bit;
	u64 free = 0;
	for (byte = 0; byte < sizeof(bitmap); byte++)
		if (bitmap[byte] != 0xFF)
			for (bit = 0; bit < 8; bit++)
				if (!(bitmap[byte] & (1 << bit)))
					free+=PAGESIZE;
	return free;
}

/*******************************************************************************/ 
/* Initialisation du bitmap */
 
void bitmap_init()
{
    u64 page;
	for (page=0; page < sizeof(bitmap); page++)
		bitmap[page] = 0xFF;
    struct multiboot_tag_mmap *tag=getgrubinfo_mem();
    multiboot_memory_map_t *mmap;       
    for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;(u8 *) mmap < (u8 *) tag + tag->size; mmap = (multiboot_memory_map_t *)
                        ((unsigned long) mmap + ((struct multiboot_tag_mmap *) tag)->entry_size))
        if (mmap->type==1) 
            bitmap_page_setfree(mmap->addr,mmap->len);
        else
            bitmap_page_setused(mmap->addr,mmap->len);
    bitmap_page_setused(0x0,KERNELSIZE);
}

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
