/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "memory.h"
#include "multiboot2.h"
#include "queue.h"
#include "asm.h"

static pd *kerneldirectory=NULL;	/* pointeur vers le page directory noyau */
static u8 *kernelheap=NULL; 		/* pointeur vers le heap noyau */
static u8 bitmap[MAXMEMPAGE / 8]; 	/* bitmap */
static vrange_t freepages;

/*******************************************************************************/ 
/* Erreur fatale */ 
void panic(u8 *string)
{
	printf("KERNEL PANIC: %s\r\nSysteme arrete...\n");
	halt();
}

/*******************************************************************************/ 
/* Alloue plusieurs pages virtuelles (size) pour le heap du noyau */ 

tmalloc *mallocpage(u8 size)
{
	tmalloc *chunk;
	u8 *paddr;
	u32 realsize=size * PAGESIZE;
	if ((kernelheap - KERNEL_HEAP + realsize) > MAXHEAPSIZE)
		panic("Plus de memoire noyau heap disponible a allouer !\n");
	chunk = (tmalloc *) kernelheap;
 	virtual_range_new_kernel(kernelheap, realsize);
	chunk->size = realsize;
	chunk->used = 0;
	return chunk;
}

/*******************************************************************************/ 
/* Alloue de la mémoire virtuelle au noyau de façon dynamique (heap) */ 

void *vmalloc(u32 size)
{
	u32 realsize;	
	tmalloc *chunk, *new;
	realsize = sizeof(tmalloc) + size;
	if (realsize < MALLOC_MINIMUM)
		realsize = MALLOC_MINIMUM;
	chunk = KERNEL_HEAP;
	while (chunk->used || chunk->size < realsize) {
		if (chunk->size == 0)
			panic(sprintf("Element du heap %x defectueux avec une taille nulle (heap %x) !",chunk, kernelheap));
		chunk = chunk + chunk->size;
		if (chunk == (tmalloc *) kernelheap)
			mallocpage((realsize / PAGESIZE) + 1);
		else if (chunk > (tmalloc *) kernelheap)
			panic (sprintf("Element du heap %x depassant la limite %x !",chunk, kernelheap));
	}
	if (chunk->size - realsize < MALLOC_MINIMUM)
		chunk->used = 1;
	else {
		new = chunk + realsize;
		new->size = chunk->size - realsize;
		new->used = 0;
		chunk->size = realsize;
		chunk->used = 1;
	}
	return (u8 *) chunk + sizeof(tmalloc);
}

/*******************************************************************************/ 
/* Libère de la mémoire virtuelle depuis le heap noyau */ 

void vmfree(void *vaddr)
{
	tmalloc *chunk, *new;
	chunk = (tmalloc *) (vaddr - sizeof(tmalloc));
	chunk->used = 0;
	while ((new = (tmalloc *) chunk + chunk->size) && new < (tmalloc *) kernelheap && new->used == 0)
		chunk->size += new->size;
}

/*******************************************************************************/ 
/* Retourne la taille de la mémoire physique (selon grub) */ 

u64 physical_getmemorysize()
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
/* Retourne que la page physique actuelle est occupée */ 

void physical_page_use(u32 page)
{
	bitmap[(page/8)] |= (1 << (page%8));
}

/*******************************************************************************/ 
/* Retourne que la page physique actuelle est libre */ 

void physical_page_free(u32 page)
{
	bitmap[(page/8)] &= ~(1 << (page%8));
}

/*******************************************************************************/ 
/* Reserve un espace mémoire physique dans le bitmap */ 

void physical_range_use(u64 addr,u64 len)
{
    u32 nbpage=TOPAGE(len);
    u32 pagesrc=TOPAGE(addr);
    if (len & 0b1111111111 > 0)
        nbpage++;
    if (addr>=MAXMEMSIZE)
	return;
    if (addr+len>=MAXMEMSIZE)
	len=MAXMEMSIZE-addr-1;
    for(u32 page=pagesrc;page<pagesrc+nbpage;page++)
        physical_page_use(page);
}

/*******************************************************************************/ 
/* Libère un espace mémoire physique dans le bitmap */ 

void physical_range_free(u64 addr,u64 len)
{
    u32 nbpage=TOPAGE(len);
    u32 pagesrc=TOPAGE(addr);
    if (len & 0b1111111111 > 0)
        nbpage++; 
    if (addr>=MAXMEMSIZE)
	return;
    if (addr+len>=MAXMEMSIZE)
	len=MAXMEMSIZE-addr-1;
    for(u32 page=pagesrc;page<pagesrc+nbpage;page++)
        physical_page_free(page);
}

/*******************************************************************************/ 
/* Retourne une page physique libre */ 

u8* physical_page_getfree(void)
{
	u8 byte, bit;
	u32 page = 0;
	for (byte = 0; byte < sizeof(bitmap); byte++)
		if (bitmap[byte] != 0xFF)
			for (bit = 0; bit < 8; bit++)
				if (!(bitmap[byte] & (1 << bit))) {
					page = 8 * byte + bit;
					physical_page_use(page);
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
/* Initialisation du bitmap pour la gestion physique de la mémoire */
 
void physical_init(void)
{
    u64 page;
	for (page=0; page < sizeof(bitmap); page++)
		bitmap[page] = 0xFF;
    struct multiboot_tag_mmap *tag=getgrubinfo_mem();
    multiboot_memory_map_t *mmap;       
    for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;(u8 *) mmap < (u8 *) tag + tag->size; mmap = (multiboot_memory_map_t *)
                        ((unsigned long) mmap + ((struct multiboot_tag_mmap *) tag)->entry_size))
        if (mmap->type==1) 
            physical_range_free(mmap->addr,mmap->len);
        else
            physical_range_use(mmap->addr,mmap->len);
    //physical_range_use(0x0,KERNELSIZE);
}
/*******************************************************************************/ 
/* Allocation de page virtuelle de mémoire */
page *virtual_page_getfree(void)
{
	page *pg;
	vrange *vpages;
	u8 *vaddr, *paddr;
	paddr = physical_page_getfree();
	if (paddr == NULL) 
		panic ("Plus de memoire physique disponible !\n");
	if (TAILQ_EMPTY(&freepages)
		panic ("Plus de place disponible dans la reserve de page !\n");
	vpages = TAILQ_FIRST(&freepages);
	vaddr = vpages->vaddrlow;
	vpages->vaddrlow += PAGESIZE;
	if (pages->vaddrlow == pages->vaddrhigh) {
		TAILQ_REMOVE(&freepages, vpages, tailq);
		vfree(vpages);
	}
	pd0_add_page(v_addr, p_addr, 0);*/
	virtual_pd_page_add(pd,vaddr,paddr, 0)
	pg = (page*) vmalloc(sizeof(page));
	pg->vaddr = vaddr;
	pg->paddr = paddr;
	return pg;
}

/*******************************************************************************/ 
/* Création d'un directory pour la gestion virtuelle de la mémoire */
 
pd *virtual_pd_create()
{
	pd *new;
	u32 *pdir,pd0;
	u32 i;
	pd = (pd *) vmalloc(sizeof(pd));
	pd->addr = virtual_page_getfree();
	if (kerneldirectory!=NULL)
	{
		pdir = (u32 *) pd->base->vaddr;
		pd0 = (u32 *) kerneldirectory->base->vaddr;
		for (i = 0; i < 256; i++)
			pdir[i] = pd0[i];
		for (i = 256; i < 1023; i++)
			pdir[i] = 0;
		pdir[1023] = ((u32) pd->base->p_addr | (PG_PRESENT | PG_WRITE));
	}	
	TAILQ_INIT(&pd->addr);
	return pd;
}
}

/*******************************************************************************/ 
/* Initialisation d'une STAILQ pour la gestion virtuelle de la mémoire */
 
void virtual_init(void)
{
	kernelheap = (u8 *) KERNEL_HEAP;
	vrange *vpages = (vrange*) vmalloc(sizeof(vrange));
	vpages->vaddrlow = (u8 *) KERNEL_HEAP;
	vpages->vaddrhigh = (u8 *) KERNEL_HEAP+MAXHEAPSIZE;
	TAILQ_INIT(&freepages);
        TAILQ_INSERT_TAIL(&freepages, vpages, tailq);
	kerneldirectory=virtual_pd_create();
 	virtual_range_use_kernel(0x00000000, 0x00000000, KERNELSIZE);
}

/*******************************************************************************/ 
/* Initialisation de la mémoire paginée */ 

void initpaging(void) 
{
	physical_init();
	virtual_init();
    asm("mov %[directory_addr], %%eax \n \
        mov %%eax, %%cr3 \n \
        mov %%cr4, %%eax \n \
		or $0x00000010, %%eax \n \
        mov %%eax, %%cr4 \n \
        mov %%cr0, %%eax \n \
		or $0x80000001, %%eax \n \
        mov %%eax, %%cr0"::[directory_addr]"m"(kerneldirectory->addr));
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
