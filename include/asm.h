#include "types.h"
#define sti() __asm__ ("sti"::)
#define cli() __asm__ ("cli"::)
#define nop() __asm__ ("nop"::)
#define iret() __asm__ ("iret"::)

#define outb(port,value) \
	asm volatile ("outb %%al,%%dx"::"d" (port), "a" (value));

#define outw(port,value) \
	asm volatile ("outw %%ax,%%dx"::"d" (port), "a" (value));

#define inb(port) ({	\
	u8 _v;	\
	asm volatile ("inb %%dx,%%al" : "=a" (_v) : "d" (port)); \
	_v;	\
})

#define inw(port) ({	\
	u16 _v;	\
 	asm volatile ("inw %%dx,%%ax" : "=a" (_v) : "d"(port));	\
	_v;	\
})