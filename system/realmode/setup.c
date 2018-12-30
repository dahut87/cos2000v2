/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"                                                             
#include "asm.h"
#include "setup.h"

struct params {
	entrye820 *e820_table;
	u32	e820_numbers;
	u8 kbflag;
} params;

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
			io_delay();
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

void main(void)
{
	showstr("Chargement de COS2000 - mode reel");
	initparams();
	initheap();
	initmemory();
	initkeyboard();
	initvideo();
	initpmode();
}
