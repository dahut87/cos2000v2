/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "memory.h"
#include "multiboot2.h"
#include "queue.h"
#include "asm.h"

static u8 *kernelcurrentheap=NULL; 		/* pointeur vers le heap noyau */
static u8 bitmap[MAXMEMPAGE / 8]; 	/* bitmap */
static vrange_t vrange_head;

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
	if ((kernelcurrentheap - KERNEL_HEAP + realsize) > MAXHEAPSIZE)
		panic("Plus de memoire noyau heap disponible a allouer !\n");
	chunk = (tmalloc *) kernelcurrentheap;
 	virtual_range_new_kernel(kernelcurrentheap, realsize);
    kernelcurrentheap += realsize;
	chunk->size = realsize;
	chunk->used = 0;
	return chunk;
}

/*******************************************************************************/ 
/* Retourne la mémoire virtuelle utilisée de façon dynamique (heap) */ 

u32 getmallocused(void)
{
    u32 realsize=0;	
	tmalloc *chunk, *new;
	chunk = KERNEL_HEAP;
	while (chunk < (tmalloc *) kernelcurrentheap) {
        if (chunk->used)
            realsize+=chunk->size;
		chunk = chunk + chunk->size;
    }
    return realsize;
}

/*******************************************************************************/ 
/* Retourne la mémoire virtuelle libre de façon dynamique (heap) */ 

u32 getmallocfree(void)
{
    u32 realsize=0;	
	tmalloc *chunk, *new;
	chunk = KERNEL_HEAP;
	while (chunk < (tmalloc *) kernelcurrentheap) {
        if (!chunk->used)
            realsize+=chunk->size;
		chunk = chunk + chunk->size;
    }
    return realsize;
}

/*******************************************************************************/ 
/* Retourne la mémoire virtuelle non allouée de façon dynamique (heap) */ 

u32 getmallocnonallocated(void)
{
    return VESA_FBMEM-((u32) kernelcurrentheap);
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
			panic(sprintf("Element du heap %x defectueux avec une taille nulle (heap %x) !",chunk, kernelcurrentheap));
		chunk = chunk + chunk->size;
		if (chunk == (tmalloc *) kernelcurrentheap)
			mallocpage((realsize / PAGESIZE) + 1);
		else if (chunk > (tmalloc *) kernelcurrentheap)
			panic (sprintf("Element du heap %x depassant la limite %x !",chunk, kernelcurrentheap));
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

void vfree(void *vaddr)
{
	tmalloc *chunk, *new;
	chunk = (tmalloc *) (vaddr - sizeof(tmalloc));
	chunk->used = 0;
	while ((new = (tmalloc *) chunk + chunk->size) && new < (tmalloc *) kernelcurrentheap && new->used == 0)
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
        if ((mmap->addr+mmap->len>maxaddr) && mmap->type==1)
                maxaddr=mmap->addr+mmap->len;
    if (maxaddr>=MAXMEMSIZE)
         maxaddr=MAXMEMSIZE-1;
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
	u32 byte;
	u8 bit;
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
    physical_range_use(0x0,KERNELSIZE);
}

/*******************************************************************************/ 
/* Retourne une page virtuelle de mémoire */

page *virtual_page_getfree(void)
{
	page *pg;
	vrange *vpages;
	u8 *vaddr, *paddr;
	paddr = physical_page_getfree();
	if (paddr == NULL) 
		panic ("Plus de memoire physique disponible !\n");
	if (TAILQ_EMPTY(&vrange_head))
		panic("Plus de place disponible dans la reserve de page !\n");
	vpages = TAILQ_FIRST(&vrange_head);
	vaddr = vpages->vaddrlow;
	vpages->vaddrlow += PAGESIZE;
	if (vpages->vaddrlow == vpages->vaddrhigh) {
		TAILQ_REMOVE(&vrange_head, vpages, tailq);
		vfree(vpages);
	}
	virtual_pd_page_add(NULL,vaddr,paddr, 0);
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
	u32 *pdir,*pd0;
	u32 i;
	new = (pd *) vmalloc(sizeof(pd));
	new->addr = virtual_page_getfree();
	pdir = (u32 *) new->addr->vaddr;
	pd0 = (u32 *) KERNEL_PD_ADDR;
	for (i = 0; i < 256; i++)
		pdir[i] = pd0[i];
	for (i = 256; i < 1023; i++)
		pdir[i] = 0;
	pdir[1023] = ((u32) new->addr->paddr | (PAGE_PRESENT | PAGE_WRITE));
	TAILQ_INIT(&new->page_head);
	return new;
}

/*******************************************************************************/ 
/* Attache une page virtuelle de la mémoire dans le directory spécifié */

void virtual_pd_page_add(pd *dst, u8* vaddr, u8 * paddr, u32 flags)
{
	u32 *pdir;	
	u32 *ptable;
	u32 *pt;
	page *pg;
	int i;
	if (dst==NULL)
		if (vaddr > (u8 *) USER_CODE) {
		print("ERREUR: Adresse %X n'est pas dans l'espace noyau !\n", vaddr);
		return ;
	}
	pdir = (u32 *) (0xFFFFF000 | (((u32) vaddr & 0xFFC00000) >> 20));
	if ((*pdir & PAGE_PRESENT) == 0) {
		if (dst==NULL)
			panic(sprintf("Page table introuvable pour l'adresse %x !\r\n", vaddr));
		pg = virtual_page_getfree();
		pt = (u32 *) pg->vaddr;
		for (i = 1; i < 1024; i++)
			pt[i] = 0;
		*pdir = (u32) pg->paddr | (PAGE_PRESENT | PAGE_WRITE | flags);
		if (dst) 
 			TAILQ_INSERT_TAIL(&dst->page_head, pg, tailq);
	}
	ptable = (u32 *) (0xFFC00000 | (((u32) vaddr & 0xFFFFF000) >> 10));
	*ptable = ((u32) paddr) | (PAGE_PRESENT | PAGE_WRITE | flags);
	return;
}

/*******************************************************************************/ 
/* Retire une page virtuelle de la mémoire dans le directory spécifié */

void virtual_pd_page_remove(u8* vaddr)
{
	u32 *ptable;
	if (virtual_to_physical(vaddr)) {
		ptable = (u32 *) (0xFFC00000 | (((u32) vaddr & 0xFFFFF000) >> 10));
		*ptable = (*ptable & (~PAGE_PRESENT));
		asm("invlpg %0"::"m"(vaddr));
	}
	return;
}

/*******************************************************************************/ 
/* Renvoie l'adresse physique de la page virtuel */

u8* virtual_to_physical(u8 *vaddr)
{
	u32 *pdir;	
	u32 *ptable;

	pdir = (u32 *) (0xFFFFF000 | (((u32) vaddr & 0xFFC00000) >> 20));
	if ((*pdir & PAGE_PRESENT)) {
		ptable = (u32 *) (0xFFC00000 | (((u32) vaddr & 0xFFFFF000) >> 10));
		if ((*ptable & PAGE_PRESENT))
			return (u8 *) ((*ptable & 0xFFFFF000) + (TOPG((u32) vaddr)));
	}
	return 0;
}

/*******************************************************************************/ 
/* Détermine une plage virtuelle de mémoire comme étant mappé aux adresses physiques spécifiées GENERIQUE*/

void virtual_range_use(pd *dst, u8 *vaddr, u8 *paddr, u64 len, u32 flags)
{
	u64 i;
	u32 realen=len/PAGESIZE;
	page *pg;
	if (len%PAGESIZE!=0) realen++;
	for(i=0;i<realen;i++)
	{
		pg = (page *) vmalloc(sizeof(page));
		pg->paddr = paddr+i*PAGESIZE; 
		pg->vaddr = vaddr+i*PAGESIZE;
		if (dst!=NULL)
            TAILQ_INSERT_TAIL(&dst->page_head, pg, tailq);
        else
            vfree(pg);
		virtual_pd_page_add(dst, pg->vaddr, pg->paddr, flags);
	}
}

/*******************************************************************************/ 
/* Supprime une plage virtuelle de mémoire GENERIQUE */

void virtual_range_free(pd *dst, u8 *vaddr, u64 len)
{
	u64 i;
	u32 realen=len/PAGESIZE;
	if (len%PAGESIZE!=0) realen++;
	for(i=0;i<realen;i++)
	{
		virtual_pd_page_remove(vaddr+i*PAGESIZE);
		virtual_page_free(vaddr);
	}
}

/*******************************************************************************/ 
/* Détermine une plage virtuelle de mémoire en attribuant de la mémoire physique GENERIQUE */

void virtual_range_new(pd *dst, u8 *vaddr, u64 len, u32 flags)
{
	u64 i;
	u32 realen=len/PAGESIZE;
	page *pg;
	if (len%PAGESIZE!=0) realen++;
	for(i=0;i<realen;i++)
	{
		pg = (page *) vmalloc(sizeof(page));
		pg->paddr = physical_page_getfree(); 
		pg->vaddr = (u8 *) (vaddr+i*PAGESIZE);
		if (dst!=NULL)
            TAILQ_INSERT_TAIL(&dst->page_head, pg, tailq);
        else
            vfree(pg);
		virtual_pd_page_add(dst, pg->vaddr, pg->paddr, flags);
	}
}

/*******************************************************************************/ 
/* Détermine une plage virtuelle de mémoire comme étant mappé aux adresses physiques spécifiées pour le noyau*/

void virtual_range_use_kernel(u8 *vaddr, u8 *paddr, u64 *len, u32 flags)
{
	virtual_range_use(NULL, vaddr, paddr, len, flags);
}

/*******************************************************************************/ 
/* Supprime une plage virtuelle de mémoire pour le noyau */

void virtual_range_free_kernel(u8 *vaddr, u64 len)
{
	virtual_range_free(NULL, vaddr, len);
}

/*******************************************************************************/ 
/* Détermine une plage virtuelle de mémoire en attribuant de la mémoire physique pour le noyau */

void virtual_range_new_kernel(u8 *vaddr, u64 len, u32 flags)
{
	virtual_range_new(NULL, vaddr, len, flags);
}

/*******************************************************************************/ 
/* Libère une page virtuelle de la mémoire */

void virtual_page_free(u8 *vaddr)
{
	vrange *next, *prev, *new;
	u8 *paddr;
	paddr = virtual_to_physical(vaddr);
	if (paddr)
		physical_page_free(TOPAGE((u32) paddr));
	else {
		printf("Aucune page associee a l'adresse virtuelle %x\n", vaddr);
		return;
	}
	virtual_pd_page_remove(vaddr);
	TAILQ_FOREACH(next, &vrange_head, tailq) {
		if (next->vaddrlow > vaddr)
			break;
	}
	prev = TAILQ_PREV(next, vrange_s, tailq);                
	if (prev->vaddrhigh == vaddr) {
		prev->vaddrhigh += PAGESIZE;
		if (prev->vaddrhigh == next->vaddrlow) {
			prev->vaddrhigh = next->vaddrhigh;
			TAILQ_REMOVE(&vrange_head, next, tailq);
			vfree(next);
		}
	}
	else if (next->vaddrlow == vaddr + PAGESIZE) {
		next->vaddrlow = vaddr;
	}
	else if (next->vaddrlow > vaddr + PAGESIZE) {
		new = (vrange*) vmalloc(sizeof(vrange));
		new->vaddrlow = vaddr;
		new->vaddrhigh = vaddr + PAGESIZE;
		TAILQ_INSERT_BEFORE(prev, new, tailq);
	}
	else
		panic("Liste chainee corrompue !\n");
	return 0;
}

/*******************************************************************************/ 
/* Destruction d'un directory pour la gestion virtuelle de la mémoire */

void virtual_pd_destroy(pd *dst)
{
	page *pg;
	TAILQ_FOREACH(pg, &dst->page_head, tailq) {
		virtual_page_free(pg->vaddr);
		TAILQ_REMOVE(&dst->page_head, pg, tailq);
		vfree(pg);
	}
	virtual_page_free(dst->addr->vaddr);
	vfree(dst);
	return 0;
}

/*******************************************************************************/ 
/* Initialise une pages virtuelles (size) pour le heap du noyau */ 

void malloc_init(void)
{
	tmalloc *chunk;
	chunk = (tmalloc *) KERNEL_HEAP;
	virtual_pd_page_add(NULL, KERNEL_HEAP, physical_page_getfree(), PAGE_NOFLAG);
	kernelcurrentheap=KERNEL_HEAP+PAGESIZE;
	chunk->size = PAGESIZE;
	chunk->used = 0;
 	//virtual_range_new_kernel(kernelcurrentheap, chunk->size, PAGE_NOFLAG);
}


/*******************************************************************************/ 
/* Initialisation d'une STAILQ pour la gestion virtuelle de la mémoire */
 
void virtual_init(void)
{
	vrange *vpages = (vrange*) vmalloc(sizeof(vrange));
	vpages->vaddrlow = (u8 *) KERNEL_HEAP+PAGESIZE;
	vpages->vaddrhigh = (u8 *) KERNEL_HEAP+MAXHEAPSIZE;
	TAILQ_INIT(&vrange_head);
        TAILQ_INSERT_TAIL(&vrange_head, vpages, tailq);
}

/*******************************************************************************/ 
/* Initialisation des 8 premiers MB de la mémoire en identity mapping */
 
void identity_init(void)
{
	u32 i;
		u32 *pd0 = KERNEL_PD_ADDR;
		u8 *pg0 = (u8 *) 0;
		u8 *pg1 = (u8 *) (PAGESIZE*PAGENUMBER);
		pd0[0] = ((u32) pg0 | (PAGE_PRESENT | PAGE_WRITE | PAGE_4MB));
		pd0[1] = ((u32) pg1 | (PAGE_PRESENT | PAGE_WRITE | PAGE_4MB));
		for (i = 2; i < 1023; i++)
			pd0[i] =
		    	((u32) pg1 + PAGESIZE * i) | (PAGE_PRESENT | PAGE_WRITE);
		pd0[1023] = ((u32) pd0 | (PAGE_PRESENT | PAGE_WRITE));
}

/*******************************************************************************/ 
/* Initialisation des registres CR0, CR3, CR4 */
 
void registry_init(void)
{
asm("mov %[directory_addr], %%eax \n \
        mov %%eax, %%cr3 \n \
        mov %%cr4, %%eax \n \
		or $0x00000010, %%eax \n \
        mov %%eax, %%cr4 \n \
        mov %%cr0, %%eax \n \
		or $0x80000001, %%eax \n \
        mov %%eax, %%cr0"::[directory_addr]"i"(KERNEL_PD_ADDR));
}

/*******************************************************************************/ 
/* Initialisation de la mémoire paginée */ 

void initpaging(void) 
{
	identity_init();
	registry_init();
	physical_init();
	malloc_init();
	virtual_init();
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
