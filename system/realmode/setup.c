//*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"                                                             
#include "asm.h"
#include "setup.h"
#include "memory.h"

struct params {
	entrye820 e820_table[E820_MAX_ENTRIES];
	u32	e820_numbers;
	u8 kbflag;
} params;

 /* registre gdt */
static struct gdtr gdtreg;

/* table de GDT */
static gdtdes gdt[GDT_SIZE];

u8 kernel_version[] = "COS2000 Version " MACRO(VERSION) "- compiled " __DATE__ ;

u8 initmemory(void)
{
	u32 count = 0;
	miniregs reg;
	cleanreg(&reg);
	entrye820 *desc = params.e820_table;
	static struct entrye820 buf; 
	do {
		reg.ax  = 0xe820;
		reg.cx  = sizeof(buf);
		reg.edx = SMAP;
		reg.di  = &buf;
		interrupt(0x15, reg);
		if (reg.eflags & EFLAGS_CF)
			break;
		if (reg.eax != SMAP) 
		{
			count = 0;
			break;
		}
		*desc++ = buf;
		count++;
	} while (reg.ebx && count < ARRAY_SIZE(params.e820_table));
	return params.e820_numbers= count;
}

void showstr(u8 *str)
{
	while (*str!='\000')
		showchar(*str++);
}

void cleanreg(miniregs *reg)
{
	for (u8 i;i<sizeof(miniregs);i++)
		*((u8 *)reg+i)=0;
}

void showchar(u8 achar)
{
	miniregs reg;
	cleanreg(&reg);
	reg.bx = 0x0007;
	reg.cx = 0x0001;
	reg.ah = 0x0e;
	reg.al = achar;
	interrupt(0x10, reg); /* INT 10 - VIDEO - TELETYPE OUTPUT */
}

u8 gettime(void)
{
	miniregs reg;
	cleanreg(&reg);
	reg.ah = 0x02;
	interrupt(0x1a, reg); /* TIME - GET REAL-TIME CLOCK TIME (AT,XT286,PS) */
	return reg.dh;
}

u8 waitchar(void)
{
	miniregs reg;
	cleanreg(&reg);
	reg.ah = 0x00;
	interrupt(0x16, reg); /* INT 16 - KEYBOARD - GET KEYSTROKE */
	return reg.al;
}

void initkeyboard(void)
{
	miniregs reg;
	cleanreg(&reg);
	reg.ah = 0x02;
	interrupt(0x16, reg); /* INT 16 - KEYBOARD - GET SHIFT FLAGS */
	params.kbflag=reg.al;
	reg.ah = 0x03;
	reg.al = 0x05;
	reg.bx = 0x00;
	interrupt(0x16, reg); /* INT 16 - KEYBOARD - SET TYPEMATIC RATE AND DELAY */
}

u8 empty8042(void)
{
	u8 status;
	u32 loops = LOOPS_8042;
	u8 ffs   = FF_8042;
	while (loops--) {
		iodelay();
		status = inb(0x64);
		if (status == 0xff) {
			if (!--ffs)
				return NULL; 
		}
		if (status & 1) {
			iodelay();
			(void)inb(0x60);
		} else if (!(status & 2)) {
			return 1;
		}
	}
	return NULL;
}

void iodelay(void)
{
	//asm("outb %%al,$0x80" ::);
	for(u32 loop=LOOPS_DELAY;loop>0;loop--) nop();
}


u8 testA20(void)
{
	asm("xorw  %%cx, %%cx\n\
	    decw  %%cx\n\
	    movw  %%cx, %%gs\n\
	    movw  $0x4000, %%cx\n\
	    movw  %%fs:(0x200), %%ax\n\
	    pushw %%ax\n\
	1:\n\
	    incw  %%ax\n\
	    movw  %%ax, %%fs:(0x200)\n\
	    call  iodelay\n\
	    cmpw  %%gs:(0x210), %%ax\n\
	    loope 1b\n\
	    popw  %%fs:(0x200)\n\
	    xor   %%ax,%%ax\n\
	    je   2f\n\
	    mov   $0x1,%%ax\n\
	2:":::"ax","cx");
}

void enableA20kbc(void)
{
	empty8042();
	outb(0xd1, 0x64);
	empty8042();
	outb(0xdf, 0x60);
	empty8042();
	outb(0xff, 0x64);
	empty8042();
}

void enableA20fast(void)
{
	u8 port;
	port = inb(0x92);
	port |=  0x02;
	port &= ~0x01;
	outb(port, 0x92);
}

u8 enableA20(void)
{
       int loops = LOOPS_A20_ENABLE;
       int kbcerr;
       while (loops--) {
	       if (!testA20())
		       return NULL;
	       kbcerr = empty8042();
	       if (!testA20())
		       return NULL;
	       if (!kbcerr) {
		       enableA20kbc();
		       if (!testA20())
			       return NULL;
	       }
	       enableA20fast();
	       if (!testA20())
		       return NULL;
       }
       return 1;
}

/*******************************************************************************/
/* Copie une portion de mémoire vers une autre */

void memcpyto(void *src, void *dst, u32 count)
{
	asm("push %%es\n\
           cld\n\
           xor %%eax,%%eax\n\
           mov %%ax,%%es\n\
           rep movsb\n\
           pop %%es"::"S" (src), "D" (dst), "c" (count):);
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

/*******************************************************************************/
/* Initialise la GDT */

void initgdt()
{
	makegdtdes(0x0, 0x00000, 0x00, 0x00, &gdt[0]);	/* descripteur nul         */
	makegdtdes(0x0, 0xFFFFF, SEG_PRESENT | SEG_NORMAL | SEG_CODE | SEG_RING0 | SEG_READ | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[1]);	/* code -> SEL_KERNEL_CODE */
	makegdtdes(0x0, 0x00000, SEG_PRESENT | SEG_NORMAL | SEG_DATA | SEG_RING0 | SEG_EXPAND_DOWN | SEG_READ_WRITE | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[2]);	/* pile -> SEL_KERNEL_STACK */
	makegdtdes(0x0, 0x00000, 0x00, 0x00, &gdt[3]);	/* LIBRE */
	makegdtdes(0x0, 0x00000, 0x00, 0x00, &gdt[4]);	/* LIBRE */
	makegdtdes(0x0, 0xFFFFF, SEG_PRESENT | SEG_NORMAL | SEG_DATA | SEG_RING0 | SEG_READ_WRITE | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[5]);	/* data -> SEL_KERNEL_DATA */
	makegdtdes(0x0, 0x00000, 0x00, 0x00, &gdt[6]);	/* LIBRE */
	makegdtdes(0x0, 0x00000, 0x00, 0x00, &gdt[7]);	/* LIBRE */
	/* initialise le registre gdt */
	gdtreg.limite = GDT_SIZE * sizeof(gdtdes);
	gdtreg.base = GDT_ADDR;
	/* recopie de la GDT a son adresse */
	memcpyto(&gdt, (u8 *) gdtreg.base, gdtreg.limite);
	/* chargement du registre GDT */
	lgdt(gdtreg);
}

void maskinterrupts(void)
{
	cli();
	outb(0x80, 0x70); /* Disable NMI */
	iodelay();
	outb(0xff, 0xa1);	/* Mask all interrupts on the secondary PIC */
	iodelay();
	outb(0xfb, 0x21);	/* Mask all but cascade on the primary PIC */
	iodelay();
}

void initcoprocessor(void)
{
	outb(0, 0xf0);
	iodelay();
	outb(0, 0xf1);
	iodelay();
}

void initpmode()
{
	if (enableA20()) {
		showstr("impossible d'ouvrir la ligne A20...\n");
		halt();
	}
	maskinterrupts();
	initgdt();
	gotopmode();		
}

void main(void)
{
	showstr("*** Chargement de COS2000 - mode reel ***\r\n");
	/* initparams(); */
	showstr(" -Initialisation de la memoire\r\n");
	initmemory();
	showstr(" -Initialisation du clavier\r\n");
	initkeyboard();
	/* initvideo(); */
	showstr(" -Initialisation du coprocesseur\r\n");
	initcoprocessor();
	showstr(" -Passage en mode protege\r\n");
	initpmode();
}
