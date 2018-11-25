/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "queue.h"

#define TOPAGE(addr)	  (addr) >> 12
#define	TOPD(addr)	((addr) & 0xFFC00000) >> 22
#define	TOPT(addr)	((addr) & 0x003FF000) >> 12
#define	TOPG(addr)	(addr) & 0x00000FFF

#define	PAGESIZE 	4096 /* Taille d'une page */
#define	PAGENUMBER 	1024 /* Nombre de pages */
#define KERNELSIZE  PAGESIZE*PAGENUMBER*2 /* 2 pages de 4mo en identity mapping */

#define IDT_ADDR		0x00000000	/* adresse de la IDT */
#define GDT_ADDR		0x00000800	/* adresse de la GDT */
#define	KERNEL_PD_ADDR		0x00001000	/* adresse de la page directory */
#define	KERNEL_STACK_ADDR	0x0009FFFF	/* adresse de la pile du kernel */
#define	KERNEL_CODE_ADDR	0x00100000	/* adresse du code du noyau */
#define KERNEL_PAGES		0x00800000	/* adresse des pages */
#define KERNEL_HEAP		0x10000000	/* adresse du heap */
#define	USER_CODE 		0x40000000      /* adresse du code utilisateur */
#define	USER_STACK 		0xE0000000	/* adresse de la pile utilisateur */

/* limites de la mémoire 32 bits */
#define	MAXMEMSIZE	0x100000000
#define	MAXMEMPAGE	1024*1024
#define MAXHEAPSIZE	USER_CODE-KERNEL_HEAP
#define MAXPAGESSIZE	KERNEL_HEAP-KERNEL_PAGES

/* page directory */
#define PAGE_PRESENT	0b000000001/* page directory / table */
#define PAGE_WRITE	    0b000000010 /* page lecture ecriture */
#define PAGE_ALL		0b000000100 /* accessible user & supervisor */
#define PAGE_WTROUGH	0b000001000 /* write-through cache */
#define PAGE_NOCACHE	0b000010000 /* cache desactivé */
#define PAGE_ACCESS 	0b000100000 /* cache desactivé */
#define PAGE_4MB		0b010000000 /* page de 4mb au lieu de 4k (NECESSITE PSE)*/

/* page table */
#define PAGE_CACHE      0b000010000 /* page en cache */
#define PAGE_DIRTY      0b001000000 /* page écrite */
#define PAGE_GLOBAL     0b100000000 /* évite que le TLB mette à jour l'adresse dans le cache si CR4 est remis à zéro (NECESSITE CR4) */

#define MALLOC_MINIMUM		16

/* Malloc, pour l'attribution de mémoire en heap */
typedef struct tmalloc {
	u32 size:31;
	u32 used:1;
} __attribute__ ((packed)) tmalloc;

/* Page, pour la gestion de la mémoire virtuelle */
typedef struct page {
	u8 *vaddr;
	u8 *paddr;
	TAILQ_ENTRY(page) tailq;
} __attribute__ ((packed))  page;

typedef TAILQ_HEAD(page_s, page) page_t;

/* Page directory, pour la gestion de la mémoire virtuelle */
typedef struct pd {
	page *addr;
	page_t page_head;
} __attribute__ ((packed))  pd;

/* vaddrrange, pour la gestion des pages de la mémoire virtuelle */
typedef struct vrange {
	u8 *vaddrlow;	
	u8 *vaddrhigh;
	TAILQ_ENTRY(vrange) tailq;
} __attribute__ ((packed)) vrange;

typedef TAILQ_HEAD(vrange_s, vrange) vrange_t;

void panic(u8 *string);
void memset(void *dst, u8 val, u32 count,u32 size);
void memcpy(void *src, void *dst, u32 count, u32 size);
u32 memcmp(void *src, void *dst, u32 count, u32 size);
u64 getmemoryfree(void);
u64 physical_getmemorysize();
void physical_page_use(u32 page);
void physical_page_free(u32 page);
void physical_range_use(u64 addr,u64 len);
void physical_range_free(u64 addr,u64 len);
u8* physical_page_getfree(void);
void physical_init(void);
void initpaging(void);
void virtual_init(void);
tmalloc *mallocpage(u8 size);
void *vmalloc(u32 size);
void vfree(void *vaddr);
page *virtual_page_getfree(void);
pd *virtual_pd_create();

/*
Fonction à ajouter...pour gestion mémoire virtuelle
u8* virtual_to_physical(u8 *vaddr)



void virtual_pd_page_remove(pd *dst, u8* vaddr)

void virtual_pd_page_add(pd *dst, u8* vaddr, u8 * paddr, u32 flags)

void virtual_range_use(pd *dst, u8 vaddr, u8 paddr, u8 len)

void virtual_range_free(pd *dst, u8 vaddr, u8 len)

void virtual_range_new(pd *dst, u8 vaddr, u8 len)

page *virtual_page_getfree(void)

void virtual_page_free(pd *dst, u8* vaddr)

void virtual_page_use(pd *dst, u8* vaddr)

void virtual_init(void)




void virtual_range_use_kernel(u8 vaddr, u8 paddr, u8 len)

void virtual_range_free_kernel(u8 vaddr, u8 len)

void virtual_range_new_kernel(u8 vaddr, u8 len)

void virtual_range_use_current(u8 vaddr, u8 paddr, u8 len)

void virtual_range_free_current(u8 vaddr, u8 len)

void virtual_range_new_current(u8 vaddr, u8 len)
*/
