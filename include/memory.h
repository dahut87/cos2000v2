/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"

#define TOPAGE(addr)	  (addr) >> 12


#define	PAGESIZE 	4096 /* Taille d'une page */
#define	PAGENUMBER 	1024 /* Nombre de pages */
#define KERNELSIZE  PAGESIZE*PAGENUMBER*2 /* 2 pages de 4mo en identity mapping */

#define IDT_ADDR		  0x00000000	/* adresse de la IDT */
#define GDT_ADDR		  0x00000800	/* adresse de la GDT */
#define	KERNEL_PGD_ADDR	  0x00001000    /* adresse de la page directory */
#define	KERNEL_STACK_ADDR 0x0009FFFF    /* adresse de la pile du kernel */
#define	KERNEL_CODE_ADDR  0x00100000

/* limites de la mémoire 32 bits */
#define	MAXMEMSIZE	0x100000000
#define	MAXMEMPAGE	1024*1024

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

void memset(void *dst, u8 val, u32 count,u32 size);
void memcpy(void *src, void *dst, u32 count, u32 size);
u32 memcmp(void *src, void *dst, u32 count, u32 size);

