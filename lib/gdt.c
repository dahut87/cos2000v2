/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hord� Nicolas             */
/*                                                                             */
#include "gdt.h"
#include "asm.h"
#include "types.h"
#include "memory.h"

 /* registre gdt */
static struct gdtr gdtreg;

/* table de GDT */
static gdtdes gdt[GDT_SIZE];

/* TSS */
static struct tss tss0;

/*******************************************************************************/
/* Initialise la GDT */

void initgdt(u32 offset)
{
	makegdtdes(0x0, 0x00000, 0x00, 0x00, &gdt[0]);	/* descripteur nul         */
	makegdtdes(0x0, 0xFFFFF, SEG_PRESENT | SEG_NORMAL | SEG_CODE | SEG_RING0 | SEG_READ | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[1]);	                        /* code -> SEL_KERNEL_CODE */
	makegdtdes(0x0, 0x00000, SEG_PRESENT | SEG_NORMAL | SEG_DATA | SEG_RING0 | SEG_EXPAND_DOWN | SEG_READ_WRITE | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[2]);	/* pile -> SEL_KERNEL_STACK */
	makegdtdes(0x0, 0xFFFFF, SEG_PRESENT | SEG_NORMAL | SEG_CODE | SEG_RING3 | SEG_CONFORMING | SEG_READ | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[3]);	        /* code -> SEL_USER_CODE */
	makegdtdes(0x0, 0x00000, SEG_PRESENT | SEG_NORMAL | SEG_DATA | SEG_RING3 | SEG_EXPAND_DOWN | SEG_READ_WRITE | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[4]);	/* pile -> SEL_USER_STACK */
	makegdtdes(0x0, 0xFFFFF, SEG_PRESENT | SEG_NORMAL | SEG_DATA | SEG_RING0 | SEG_READ_WRITE | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[5]);	                /* data -> SEL_KERNEL_DATA */
	makegdtdes(0x0, 0xFFFFF, SEG_PRESENT | SEG_NORMAL | SEG_DATA | SEG_RING3 | SEG_READ_WRITE | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[6]);	                /* data -> SEL_USER_DATA */

	tss0.trapflag = 0x00;
	tss0.iomap = 0x00;
	tss0.esp0 = 0x1FFF0;
	tss0.ss0 = SEL_TSS;

	makegdtdes(&tss0, 0x67, SEG_PRESENT | SEG_CODE | SEG_RING3 | SEG_ACCESSED , 0x00, &gdt[7]);	/* descripteur de tss */

	/* initialise le registre gdt */
	gdtreg.limite = GDT_SIZE * 8;
	gdtreg.base = GDT_ADDR;
	/* recopie de la GDT a son adresse */
	memcpy(&gdt, (u8 *) gdtreg.base, gdtreg.limite, 1);
	/* chargement du registre GDT */
	lgdt(&gdtreg);
	/* initialisation des segments */
	initselectors(offset);
}

/*******************************************************************************/
/* Initialise le registre de t�che (TSR) */

void inittr(void)
{
	ltr(SEL_TSS);
}

/*******************************************************************************/
/* Initialise les selecteurs avec la GDT */

void initselectors(u32 executingoffset)
{
	asm("   movw %[data], %%ax	\n \
            movw %%ax, %%ds	\n \
            movw %%ax, %%es	\n \
            movw %%ax, %%fs	\n \
            movw %%ax, %%gs   \n \
            movl %[offset], %%ebx \n \
            movw %[stack], %%ax \n \
            movw %%ax, %%ss \n \
            movl %[stackoff], %%esp \n \
            ljmp %[code], $raz	\n \
            raz:		\n \
            pushl %%ebx \n \
            ret\n"::[data] "i"(SEL_KERNEL_DATA),[code] "i"(SEL_KERNEL_CODE),[stack] "i"(SEL_KERNEL_STACK),[stackoff] "i"(KERNEL_STACK_ADDR),[offset] "m"(executingoffset));
}

/*******************************************************************************/
/* Cr�� un descripteur GDT */

void makegdtdes(u32 base, u32 limite, u8 acces, u8 flags, gdtdes * desc)
{
	desc->lim0_15 = (limite & 0xffff);
	desc->base0_15 = (base & 0xffff);
	desc->base16_23 = (base & 0xff0000) >> 16;
	desc->acces = acces;
	desc->lim16_19 = (limite & 0xf0000) >> 16;
	desc->flags = (flags & 0xf);
	desc->base24_31 = (base & 0xff000000) >> 24;
	return;
}
