/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "gdt.h"
#include "asm.h"
#include "types.h"
#include "memory.h"

/* TSS */
static struct tss tss0;

/*******************************************************************************/
/* Initialise les selecteurs avec la GDT */

void initselectors(u32 executingoffset)
{
	asm("	movw %[data], %%ax	\n \
            movw %%ax, %%ds	\n \
            movw %%ax, %%es	\n \
            movw %%ax, %%fs	\n \
            movw %%ax, %%gs   \n \
            movl %[offset], %%ebx \n \
            movw %[stack], %%ax \n \
            movw %%ax, %%ss \n \
            movl %[stackoff], %%esp \n \
		xor %%eax,%%eax\n\
		xor %%ebx,%%ebx\n\
		xor %%ecx,%%ecx\n\
		xor %%edx,%%edx\n\
		xor %%esi,%%esi\n\
		xor %%edi,%%edi\n\
		xor %%ebp,%%ebp\n\
		jmp %%ebx"::[data] "i"(SEL_KERNEL_DATA),[code] "i"(SEL_KERNEL_CODE),[stack] "i"(SEL_KERNEL_STACK),[stackoff] "i"(KERNEL_STACK_ADDR),[offset] "m"(executingoffset));
}

/*******************************************************************************/
/* Change le TSS courant */

void setTSS(u32 ss, u32 sp)
{

	tss0.trapflag = 0x00;
	tss0.iomap = 0x00;
	tss0.ss0 = SEL_TSS;
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
