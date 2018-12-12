/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
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
	makegdtdes(0x0, 0xFFFFF, SEG_PRESENT | SEG_NORMAL | SEG_CODE | SEG_RING0 | SEG_READ | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[1]);	/* code -> SEL_KERNEL_CODE */
	makegdtdes(0x0, 0x00000, SEG_PRESENT | SEG_NORMAL | SEG_DATA | SEG_RING0 | SEG_EXPAND_DOWN | SEG_READ_WRITE | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[2]);	/* pile -> SEL_KERNEL_STACK */
	makegdtdes(0x0, 0xFFFFF, SEG_PRESENT | SEG_NORMAL | SEG_CODE | SEG_RING3 | SEG_CONFORMING | SEG_READ | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[3]);	/* code -> SEL_USER_CODE */
	makegdtdes(0x0, 0x00000, SEG_PRESENT | SEG_NORMAL | SEG_DATA | SEG_RING3 | SEG_EXPAND_DOWN | SEG_READ_WRITE | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[4]);	/* pile -> SEL_USER_STACK */
	makegdtdes(0x0, 0xFFFFF, SEG_PRESENT | SEG_NORMAL | SEG_DATA | SEG_RING0 | SEG_READ_WRITE | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[5]);	/* data -> SEL_KERNEL_DATA */
	makegdtdes(0x0, 0xFFFFF, SEG_PRESENT | SEG_NORMAL | SEG_DATA | SEG_RING3 | SEG_READ_WRITE | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[6]);	/* data -> SEL_USER_DATA */

	tss0.trapflag = 0x00;
	tss0.iomap = 0x00;
	tss0.esp0 = 0x00;
	tss0.ss0 = SEL_TSS;

	makegdtdes(&tss0, 0x67, SEG_PRESENT | SEG_CODE | SEG_RING3 | SEG_ACCESSED, 0x00, &gdt[7]);	/* descripteur de tss */

	/* initialise le registre gdt */
	gdtreg.limite = GDT_SIZE * sizeof(gdtdes);
	gdtreg.base = GDT_ADDR;
	/* recopie de la GDT a son adresse */
	memcpy(&gdt, (u8 *) gdtreg.base, gdtreg.limite, 1);
	/* chargement du registre GDT */
	lgdt(&gdtreg);
	/* initialisation des segments */
	initselectors(offset);
}

/*******************************************************************************/
/* Change le TSS courant */

void setTSS(u32 ss, u32 sp)
{
	tss0.esp0 = sp;
	tss0.ss0 = ss;
}

/*******************************************************************************/
/* Initialise le registre de tâche (TSR) */

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
/* récupère la base d'un descripteur GDT */

u32 getdesbase(u16 sel)
{
	gdtdes *entry = GDT_ADDR;
	u8      index = sel / sizeof(gdtdes);
	return (entry[index].base0_15 + (entry[index].base16_23 << 16) +
		(entry[index].base24_31 << 24));
}

/*******************************************************************************/
/* récupère la limite d'un descripteur GDT */

u32 getdeslimit(u16 sel)
{
	gdtdes *entry = GDT_ADDR;
	u8      index = sel / sizeof(gdtdes);
	return (entry[index].lim0_15 + (entry[index].lim16_19 << 16));
}

/*******************************************************************************/
/* récupère la limite d'un descripteur GDT */

u32 getdesdpl(u16 sel)
{
	gdtdes *entry = GDT_ADDR;
	u8      index = sel / sizeof(gdtdes);
	return (entry[index].acces >> 5 & 0x03);
}

/*******************************************************************************/
/* récupère le type d'un descripteur GDT */

u8 getdestype(u16 sel)
{
	gdtdes *entry = GDT_ADDR;
	u8      index = sel / sizeof(gdtdes);
	if (((entry[index].acces & 0x14) == 0)
	    && ((entry[index].acces & 0x08) > 0)
	    && ((entry[index].acces & 0x01) > 0)
	    && ((entry[index].flags & 0x06) == 0))
		return 'T';
	else
		return (((entry[index].acces & 0x08) > 0) ? 'C' : 'D');
}

/*******************************************************************************/
/* récupère l'info 1 d'un descripteur GDT */

u8 getdesbit1(u16 sel)
{
	gdtdes *entry = GDT_ADDR;
	u8      index = sel / sizeof(gdtdes);
	if (((entry[index].acces & 0x14) == 0)
	    && ((entry[index].acces & 0x08) > 0)
	    && ((entry[index].acces & 0x01) > 0)
	    && ((entry[index].flags & 0x06) == 0))
		return (((entry[index].acces & 0x04) > 0) ? 'B' : '-');
	else
		return (((entry[index].acces & 0x01) > 0) ? 'A' : '-');
}

/*******************************************************************************/
/* récupère l'info 2 d'un descripteur GDT */

u8 getdesbit2(u16 sel)
{
	gdtdes *entry = GDT_ADDR;
	u8      index = sel / sizeof(gdtdes);
	if (((entry[index].acces & 0x20) == 0)
	    && ((entry[index].acces & 0x08) > 0)
	    && ((entry[index].acces & 0x01) > 0)
	    && ((entry[index].flags & 0x06) == 0))
		return (((entry[index].flags & 0x01) > 0) ? 'U' : '-');
	else if ((entry[index].acces & 0x8) > 0)
		return (((entry[index].acces & 0x02) > 0) ? 'R' : '-');
	else
		return (((entry[index].acces & 0x02) > 0) ? 'W' : 'R');
}

/*******************************************************************************/
/* récupère l'info 3 d'un descripteur GDT */

u8 getdesbit3(u16 sel)
{
	gdtdes *entry = GDT_ADDR;
	u8      index = sel / sizeof(gdtdes);
	if ((entry[index].acces & 0x08) > 0)
		return (((entry[index].acces & 0x04) > 0) ? 'C' : '-');
	else
		return (((entry[index].acces & 0x04) > 0) ? 'D' : 'U');
}

/*******************************************************************************/
/* récupère l'alignement d'un descripteur GDT */

u16 getdesalign(u16 sel)
{
	gdtdes *entry = GDT_ADDR;
	u8      index = sel / sizeof(gdtdes);
	return (((entry[index].flags & 0x08) > 0) ? 4096 : 1);
}

/*******************************************************************************/
/* récupère si descripteur GDT est valide */

bool isdesvalid(u16 sel)
{
	gdtdes *entry = GDT_ADDR;
	u8      index = sel / sizeof(gdtdes);
	return ((entry[index].acces & 0x80) > 0);
}

/*******************************************************************************/
/* récupère la dimension d'un descripteur GDT */

u32 getdessize(u16 sel)
{
	gdtdes *entry = GDT_ADDR;
	u8      index = sel / sizeof(gdtdes);
	if (((entry[index].acces & 0x14) == 0)
	    && ((entry[index].acces & 0x08) > 0)
	    && ((entry[index].acces & 0x01) > 0)
	    && ((entry[index].flags & 0x06) == 0))
		return 32;
	else
		return (((entry[index].flags & 0x08) > 0) ? 32 : 16);
}
/*******************************************************************************/
/* Créé un descripteur GDT */

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
