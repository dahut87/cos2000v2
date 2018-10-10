/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"

/******************************************************************************/

#define sti() asm("sti"::)

#define cli() asm("cli"::)

#define nop() asm("nop"::)

#define pushad() asm("pushal"::)

#define popad() asm("popal"::)

#define pushf() asm("pushf"::)

#define popf() asm("popf"::)

#define getESP() ({ \
        u32 _v; \
        asm volatile ("movl %%esp,%%eax;": "=a" (_v)); \
        _v; \
})
   u32 ss;
   u32 gs;
   u32 fs;
   u32 es;
   u32 ds;
   u32 eip;
   u32 cs;




#define dumpcpu() asm("\
        mov $0x6666666, %%eax  \n \
        mov $0x8888888, %%ebx  \n \
        pushal \n \
        pushf \n \
        pushl %%cs\n \
        pushl $0x0\n \
        pushl %%ds\n \
        pushl %%es\n \
        pushl %%fs\n \
        pushl %%gs\n \
        pushl %%ss\n \
        pushl $0x0\n \
        mov %%cr0, %%eax \n \
        pushl %%eax\n \
        mov %%cr2, %%eax \n \
        pushl %%eax\n \
        mov %%cr3, %%eax \n \
        pushl %%eax\n \
        mov %%cr4, %%eax \n \
        pushl %%eax \n \
        mov %%dr0, %%eax \n \
        pushl %%eax\n \
        mov %%dr1, %%eax \n \
        pushl %%eax\n \
        mov %%dr2, %%eax \n \
        pushl %%eax\n \
        mov %%dr3, %%eax \n \
        pushl %%eax\n \
        mov %%dr4, %%eax \n \
        pushl %%eax\n \
        mov %%dr5, %%eax \n \
        pushl %%eax\n \
        mov %%dr6, %%eax \n \
        pushl %%eax\n \
        mov %%dr7, %%eax \n \
        pushl %%eax\n \
        mov $0xC0000080, %%ecx \n \
        rdmsr \n \
        pushl %%edx \n \
        pushl %%eax":::)

#define restcpu() asm("\
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%ss\n \
        popl %%gs\n \
        popl %%fs\n \
        popl %%es\n \
        popl %%ds\n \
        popl %%eax \n \
        popl %%eax \n \
        popf \n \
        popal":::)

#define iret() asm("iret"::)

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

/******************************************************************************/

