#include "types.h"
#define sti() __asm__ ("sti"::)
#define cli() __asm__ ("cli"::)
#define nop() __asm__ ("nop"::)
#define iret() __asm__ ("iret"::)
#define pause() __asm__ ("iret"::)

#define outbp(port,value) \
	asm volatile ("outb %%al,%%dx; jmp 1f; 1:"::"d" (port), "a" (value));

#define outb(port,value) \
	asm volatile ("outb %%al,%%dx"::"d" (port), "a" (value));

#define outw(port,value) \
	asm volatile ("outw %%ax,%%dx"::"d" (port), "a" (value));

#define outd(port,value) \
	asm volatile ("outd %%eax,%%dx"::"d" (port), "a" (value));

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
 	asm volatile ("ind %%dx,%%eax" : "=a" (_v) : "d"(port));	\
	_v;	\
}

#define rolb(input,rotate) ({	\
	u32 _v;	\
        asm volatile ("roll %1,%0" : "=g" (_v) : "cI" (rotate), "0" (input));  \
	_v;	\
}


