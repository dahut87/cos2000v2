/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"

/******************************************************************************/
#define halt() asm("hlt"::)

#define sti() asm("sti"::)

#define cli() asm("cli"::)

#define nop() asm("nop"::)

#define pushad() asm("pushal"::)

#define popad() asm("popal"::)

#define pushf() asm("pushf"::)

#define popf() asm("popf"::)

#define iret() asm("iret"::)

#define leave() asm("leave"::)

#define irqendmaster() asm("movb $0x20,%al; \
                       outb %al,$0x20;")

#define irqendslave() asm("movb $0x20,%al; \
                       outb %al,$0xA0;")

#define lidt(idtr) asm ("lidtl %0"::"m" (*idtr))

#define lgdt(gdtr) asm ("lgdtl %0"::"m" (*gdtr))

#define lldt(ldtr) asm ("lldtl %0"::"m" (*ldtr))

#define ltr(tss) asm volatile ("ltr %%ax":: "a" (tss))

#define sidt(idtr) asm ("sidtl %0"::"m" (*idtr))

#define sgdt(gdtr) asm ("sgdtl %0"::"m" (*gdtr))

#define sldt(ldtr) asm ("sldtl %0"::"m" (*ldtr))

#define str(tss) asm volatile ("str %%ax;\
                                mov %%ax,%0":: "m" (tss))

#define wrmsr(reg,low,high) asm volatile ("wrmsr" :: "c" (reg), "a" (low), "d" (high))

#define rdmsr(reg,low,high) asm volatile ("rdmsr" :: "=a" (low), "=d" (high) : "c" (reg) )

#define movsb(src,dst,count) \
	asm volatile ("cld;rep movsb"::"S" (src), "D" (dst), "c" (count));

#define movsw(src,dst,count) \
	asm volatile ("cld;rep movsw"::"S" (src), "D" (dst), "c" (count));

#define movsd(src,dst,count) \
	asm volatile ("cld;rep movsl"::"S" (src), "D" (dst), "c" (count));

#define stosb(pattern,dst,count) \
	asm volatile ("cld;rep stosb"::"c" (count), "D" (dst), "a" (pattern));

#define stosw(pattern,dst,count) \
	asm volatile ("cld;rep stosw"::"c" (count), "D" (dst), "a" (pattern));

#define stosd(pattern,dst,count) \
	asm volatile ("cld;rep stosl"::"c" (count), "D" (dst), "a" (pattern));

#define rol(addr) \
	asm volatile ("rolb $0x1,%0":"=m" (addr):);

#define ror(addr) \
	asm volatile ("rorb $0x1,%0":"=m" (addr):);

#define finit() \
    asm volatile ("finit"::);
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
})


#define ind(port) ({	\
	u32 _v;	\
 	asm volatile ("inl %%dx,%%eax" : "=a" (_v) : "d"(port));	\
	_v;	\
})

/******************************************************************************/

/* pas terminé */

#define rolb(input,rotate) ({	\
	u32 _v;	\
        asm volatile ("roll %1,%0" : "=g" (_v) : "cI" (rotate), "0" (input));  \
	_v;	\
}

/******************************************************************************/

