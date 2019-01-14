/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"                                                             
#include "asm.h"
#include "setup.h"
#include "gdt.h"
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

#define STRINGIFY(x) #x
#define MACRO(x)     STRINGIFY(x)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))
#define __must_be_array(a) BUILD_BUG_ON_ZERO(__same_type((a), &(a)[0]))
#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:(-!!(e)); }))

u8 kernel_version[] = "COS2000 Version " MACRO(VERSION) "- compiled " __DATE__ ;

#define EFLAGS_CF			0x00000001
#define LOOPS_8042		100000
#define FF_8042			32
#define LOOPS_A20_ENABLE 	255
#define LOOPS_DELAY 		32768
#define SMAP	0x534d4150

#define interrupt(num, regs) ({\
	asm("pushal\n\
          mov %[eflagsregs],%%eax\n\
	    pushl %%eax\n\
	    popfl\n\
	    mov %[eaxregs],%%eax\n\
	    mov %[ebxregs],%%ebx\n\
	    mov %[ecxregs],%%ecx\n\
	    mov %[edxregs],%%edx\n\
	    mov %[esiregs],%%esi\n\
	    mov %[ediregs],%%edi\n\
	    mov %[ebpregs],%%ebp\n\
          int %[numregs]\n\
	    pushfl\n\
          popl %%eax\n\
          mov %%eax,%[eflagsregs]\n\
	    mov %%eax,%[eaxregs]\n\
	    mov %%ebx,%[ebxregs]\n\
	    mov %%ecx,%[ecxregs]\n\
	    mov %%edx,%[edxregs]\n\
	    mov %%esi,%[esiregs]\n\
	    mov %%edi,%[ediregs]\n\
	    mov %%ebp,%[ebpregs]\n\
          popal":[eaxregs] "+m" (regs.eax),[ebxregs] "+m" (regs.ebx),[ecxregs] "+m" (regs.ecx),[edxregs] "+m" (regs.edx),[esiregs] "+m" (regs.esi),[ediregs] "+m" (regs.edi),[ebpregs] "+m" (regs.ebp),[eflagsregs] "+m" (regs.eflags):[numregs] "i" (num):);})


u8 initmemory(void)
{
	u32 count = 0;
	miniregs reg;
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

void showchar(u8 achar)
{
	miniregs reg;
	reg.bx = 0x0007;
	reg.cx = 0x0001;
	reg.ah = 0x0e;
	reg.al = achar;
	interrupt(0x10, reg); /* INT 10 - VIDEO - TELETYPE OUTPUT */
}

void showstr(u8 *str)
{
	while (*str!='\000')
		showchar(*str++);
}

u8 gettime(void)
{
	miniregs reg;
	reg.ah = 0x02;
	interrupt(0x1a, reg); /* TIME - GET REAL-TIME CLOCK TIME (AT,XT286,PS) */
	return reg.dh;
}

u8 waitchar(void)
{
	miniregs reg;
	reg.ah = 0x00;
	interrupt(0x16, reg); /* INT 16 - KEYBOARD - GET KEYSTROKE */
	return reg.al;
}

void initkeyboard(void)
{
	miniregs reg;
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
	       if (testA20())
		       return 1;
	       kbcerr = empty8042();
	       if (testA20())
		       return 1;
	       if (!kbcerr) {
		       enableA20kbc();
		       if (testA20())
			       return 1;
	       }
	       enableA20fast();
	       if (testA20())
		       return NULL;
       }
       return 1;
}

/*******************************************************************************/
/* Copie un octet une ou plusieurs fois en mémoire */

void memset(void *dst, u8 val, u32 count, u32 size)
{
	u8     *d = (u8 *) dst;
	if (size > 0)
		size--;
	for (; count != 0; count--)
	{
		*(d++) = val;
		d += size;
	}
}

/*******************************************************************************/
/* Copie une portion de mémoire vers une autre */

void memcpy(void *src, void *dst, u32 count, u32 size)
{
	u8     *s = (u8 *) src;
	u8     *d = (u8 *) dst;
	if (size > 0)
		size--;
	for (; count != 0; count--)
	{
		*(d++) = *(s++);
		d += size;
	}
}


/*******************************************************************************/
/* Initialise les selecteurs avec la GDT */

void initselectors(u32 executingoffset)
{
	asm(" movl	%%cr0, %%eax \n \
		orb	$0x00000001, %%eax \n \
		movl	%%eax, %%cr0 \n \
            ljmp %[code], $raz\n\
		raz:\n \
	.code32\n\
		movw %[data], %%ax	\n \
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
	makegdtdes(0x0, 0xFFFFF, SEG_PRESENT | SEG_NORMAL | SEG_CODE | SEG_RING3 | SEG_CONFORMING | SEG_READ | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[3]);	/* code -> SEL_USER_CODE */
	makegdtdes(0x0, 0x00000, SEG_PRESENT | SEG_NORMAL | SEG_DATA | SEG_RING3 | SEG_EXPAND_DOWN | SEG_READ_WRITE | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[4]);	/* pile -> SEL_USER_STACK */
	makegdtdes(0x0, 0xFFFFF, SEG_PRESENT | SEG_NORMAL | SEG_DATA | SEG_RING0 | SEG_READ_WRITE | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[5]);	/* data -> SEL_KERNEL_DATA */
	makegdtdes(0x0, 0xFFFFF, SEG_PRESENT | SEG_NORMAL | SEG_DATA | SEG_RING3 | SEG_READ_WRITE | SEG_ACCESSED, GRANULARITY_4K | OPSIZE_32B | SYS_AVAILABLE, &gdt[6]);	/* data -> SEL_USER_DATA */
	makegdtdes(0x0, 0x67, SEG_PRESENT | SEG_CODE | SEG_RING3 | SEG_ACCESSED, 0x00, &gdt[7]);	/* descripteur de tss */
	/* initialise le registre gdt */
	gdtreg.limite = GDT_SIZE * sizeof(gdtdes);
	gdtreg.base = GDT_ADDR;
	/* recopie de la GDT a son adresse */
	memcpy(&gdt, (u8 *) gdtreg.base, gdtreg.limite, 1);
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

void initpmode(u32 offset)
{
	if (enableA20()) {
		showstr("impossible d'ouvrir la ligne A20...\n");
		halt();
	}
	maskinterrupts();
	initgdt();
	initselectors(offset);
}

void main(void)
{
	showstr("Chargement de COS2000 - mode reel");
waitchar();
	/* initparams(); */
	initmemory();
	initkeyboard();
	/* initvideo(); */
	initcoprocessor();
	initpmode(0x10000);
}
