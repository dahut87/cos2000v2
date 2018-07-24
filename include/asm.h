#include "types.h"

/******************************************************************************/

#define sti() asm("sti"::)

#define cli() asm("cli"::)

#define nop() asm("nop"::)

#define iret() asm("addl  $0x0C, %esp; \
                          iret;")

#define irqendmaster() asm("movb $0x20,%al; \
                       outb %al,$0x20;")

#define irqendslave() asm("movb $0x20,%al; \
                       outb %al,$0xA0;")

#define lidt(dtr) asm ("lidtl %0"::"m" (*dtr))

/******************************************************************************/

#define outb(port,value) \
	asm volatile ("outb %%al,%%dx"::"d" (port), "a" (value));

#define outw(port,value) \
	asm volatile ("outw %%ax,%%dx"::"d" (port), "a" (value));

#define outd(port,value) \
	asm volatile ("outl %%eax,%%dx"::"d" (port), "a" (value));

/******************************************************************************/

#define inb(port) ({	\
	u8 _v;	\
	asm volatile ("inb %%dx,%%al" : "=a" (_v) : "d" (port)); \
	_v;	\
})

#define inw(port) ({	\
	u16 _v;	\
 	asm volatile ("inw %%dx,%%ax" : "=a" (_v) : "d"(port));	\
	_v;	\
}


#define ind(port) ({	\
	u32 _v;	\
 	asm volatile ("inl %%dx,%%eax" : "=a" (_v) : "d"(port));	\
	_v;	\
}

/******************************************************************************/

/* pas terminé */

#define rolb(input,rotate) ({	\
	u32 _v;	\
        asm volatile ("roll %1,%0" : "=g" (_v) : "cI" (rotate), "0" (input));  \
	_v;	\
}





