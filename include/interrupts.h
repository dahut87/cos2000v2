/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "asm.h"

#ifndef _INTERRUPTS
#   define _INTERRUPTS

#   define PIC1_CMD    0x20	/*PIC 8259A Commandes n°1 */
#   define PIC1_DATA   0x21	/*PIC 8259A Données n°1 */
#   define PIC2_CMD    0xa0	/*PIC 8259A Commandes n°2 */
#   define PIC2_DATA   0xa1	/*PIC 8259A Données n°1 */

#   define ICW1_ICW4       0x01	/* ICW4 ou pas */
#   define ICW1_SINGLE     0x02	/* mode seul ou cascadé */
#   define ICW1_INTERVAL4  0x04	/* adresses appel d'interval 4 ou 8 */
#   define ICW1_LEVEL      0x08	/* déclenchement sur niveau ou sur front */
#   define ICW1_INIT       0x10	/* Initialization */

#   define ICW4_8086       0x01	/* 8086/88 (MCS-80/85) mode */
#   define ICW4_AUTO       0x02	/* Auto EOI ou normal */
#   define ICW4_BUF_SLAVE  0x08	/* mode/slave avec tampon */
#   define ICW4_BUF_MASTER 0x0C	/* mode/master avec tampon */
#   define ICW4_SFNM       0x10	/* Complètement lié ou non */

#   define INTGATE     0x0E00	/* utilise pour gerer les interruptions */
#   define TRAPGATE    0x0F00	/* utilise pour faire des appels systemes */
#   define TASKGATE    0x0500	/* utilise pour commuter des taches */
#   define CALLGATE    0x0C00	/* utilise pour appeler du code */
#   define LDTDES      0x0200	/* utilise pour pointer une LDT */

#   define ENTRY_PRESENT     0b1000000000000000	/* Segment défini (obligatoire) */

#   define ENTRY_STORAGE     0b0001000000000000	/* Segment défini (obligatoire) */

#   define ENTRY_RING0       0b0000000000000000	/* Segment anneau 0 */
#   define ENTRY_RING1       0b0010000000000000	/* Segment anneau 1 */
#   define ENTRY_RING2       0b0100000000000000	/* Segment anneau 2 */
#   define ENTRY_RING3       0b0110000000000000	/* Segment anneau 3 */

/* 00-11-010-0 : Compteur 0 - LSB puis MSB - generateur taux - binaire */
#   define TIMER0         0x40	/* port E/S pour le timer canal 0 */
#   define TIMER_MODE     0x43	/* port E/S pour le mode controle du timer */
#   define RATE_GENERATOR 0x34	/* générateur de fréquence */
#   define SQUARE_WAVE    0x36	/* générateur d'onde carrée */
#   define TIMER_FREQ     1193180
				/* fréquence pour timer dans un PC ou AT */
#   define HZ             100	/* Fréquence d'horloge (ajutste logiciellement sur IBM-PC) */

#   define setESP(mem) ({ \
        asm volatile ("movl %[frommem],%%esp;":[frommem] "=m" (mem):); \
})

#   define savecpu(dump,caller,oldesp) ({\
	getEBP(oldesp);\
	dumpcpu();\
	getESP(dump);\
	caller = (exception_stack *) (oldesp + 1);\
	dump->ebp = *oldesp;\
	dump->eip = caller->eip;\
	dump->cs = caller->cs;\
	if (caller->cs==SEL_KERNEL_CODE)\
		dump->esp = (u32) oldesp + sizeof(exception_stack);\
	else\
	{\
		dump->esp = (u32) ((exception_stack_user*) caller)->esp;\
		dump->ss = (u32) ((exception_stack_user*) caller)->ss;\
	}\
})

#   define savecpu_noerror(dump,caller,oldesp) ({\
	getEBP(oldesp);\
	dumpcpu();\
	getESP(dump);\
	caller = (exception_stack_noerror *) (oldesp + 1);\
	dump->ebp = *oldesp;\
	dump->eip = caller->eip;\
	dump->cs = caller->cs;\
	if (caller->cs==SEL_KERNEL_CODE)\
		dump->esp = (u32) oldesp + sizeof(exception_stack_noerror);\
	else\
	{\
		dump->esp = (u32) ((exception_stack_noerror_user*) caller)->esp;\
		dump->ss = (u32) ((exception_stack_noerror_user*) caller)->ss;\
	}\
})

#   define createdump(dump) ({ \
        push(dump.ss);\
        push(dump.esp);\
        push(dump.eflags);\
        push(dump.cs);\
        push(dump.eip);\
        push(dump.ds);\
        push(dump.es);\
        push(dump.fs);\
        push(dump.gs);\
        push(dump.eax);\
        push(dump.ebx);\
        push(dump.ecx);\
        push(dump.edx);\
        push(dump.esi);\
        push(dump.edi);\
        push(dump.ebp);\
        push(dump.cr0);\
        push(dump.cr2);\
        push(dump.cr3);\
        push(dump.cr4);\
        push(dump.dr0);\
        push(dump.dr1);\
        push(dump.dr2);\
        push(dump.dr3);\
        push(dump.dr6);\
        push(dump.dr7);\
        u32 eferlow=(u32) dump.efer & 0xFFFF;\
        u32 eferhigh=(u32) dump.efer >> 32;\
        push(eferlow);\
        push(eferhigh);\
})

#   define dumpcpu()  ({ \
	asm("\
        pushl %%ss\n \
        pushl %%esp\n \
        pushf \n \
        pushl %%cs\n \
        pushl $0x0\n \
    	pushl %%ds\n \
        pushl %%es\n \
        pushl %%fs\n \
        pushl %%gs\n \
        pushl %%eax\n \
        pushl %%ebx\n \
        pushl %%ecx\n \
        pushl %%edx\n \
        pushl %%esi\n \
        pushl %%edi\n \
        pushl %%ebp\n \
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
        mov %%dr6, %%eax \n \
        pushl %%eax\n \
        mov %%dr7, %%eax \n \
        pushl %%eax\n \
        mov $0xC0000080, %%ecx \n \
        rdmsr \n \
        pushl %%edx \n \
        pushl %%eax":::);\
})

#   define restcpu_kernel() ({\
	asm("\
        popl %%eax \n \
        popl %%edx \n \
        mov $0xC0000080, %%ecx \n \
	wrmsr\n \
        popl %%eax\n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
	mov %%eax,%%cr3 \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%ebp\n \
        popl %%edi\n \
        popl %%esi\n \
        popl %%edx\n \
        popl %%ecx\n \
        mov 36(%%esp),%%eax\n \
        mov 32(%%esp),%%ebx\n \
        mov %%ebx,-4(%%eax)\n \
        mov 28(%%esp),%%ebx\n \
        mov %%ebx,-8(%%eax)\n \
        mov 24(%%esp),%%ebx\n \
        mov %%ebx,-12(%%eax)\n \
        popl %%ebx\n \
        popl %%eax\n \
        popl %%gs\n \
        popl %%fs\n \
        popl %%es\n \
        popl %%ds\n \
        add $12,%%esp\n \
  	  popl %%esp\n \
        sub $12,%%esp\n \
        iret ":::);\
})

#   define restcpu_user() ({\
	asm("\
        popl %%eax \n \
        popl %%edx \n \
        mov $0xC0000080, %%ecx \n \
	wrmsr\n \
        popl %%eax\n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%eax \n \
	mov %%eax,%%cr3 \n \
        popl %%eax \n \
        popl %%eax \n \
        popl %%ebp\n \
        popl %%edi\n \
        popl %%esi\n \
        popl %%edx\n \
        popl %%ecx\n \
        popl %%ebx\n \
        popl %%eax\n \
        popl %%gs\n \
        popl %%fs\n \
        popl %%es\n \
  	popl %%ds":::);\
})


/*
lors d'un iret en mode user:
	pushl %%ss\n \
        pushl %%esp\n \
lors d'un iret en mode kernel:
        pushf \n \
        pushl $cs\n \
        pushl $0x0\
*/


/* save pile */
typedef struct regs
{
	union {
		struct {
			u64     efer;
			u32     dr7;
			u32     dr6;
			u32     dr3;
			u32     dr2;
			u32     dr1;
			u32     dr0;
			u32     cr4;
			u32     cr3;
			u32     cr2;
			u32     cr0;
			u32     ebp;
			u32     edi;
			u32     esi;
			u32     edx;
			u32     ecx;
			u32     ebx;
			u32     eax;
			u32     gs;
			u32     fs;
			u32     es;
			u32     ds;
			u32     eip;
			u32     cs;
			u32     eflags;
			u32     esp;
			u32     ss;
		};
		struct {
			u32     efer_low, efer_high;
			u32     dummy[13*sizeof(u32)];
			u16     dx,hdx;
			u16     cx,hcx;
			u16     bx,hbx;
			u16     ax,hax;
			u16     gsl,dummy_gs;
			u16     fsl,dummy_fs;
			u16     esl,dummy_es;
			u16     dsl,dummy_ds;
			u16     ip,hip;
			u16     csl,dummy_cs;
			u16     flags,hflags;
			u16     sp,hsp;
			u16     ssl,dummy_ss;
		};
		struct {
			u64    dummy2;
			u32    dummy3[13*sizeof(u32)];
			u8     dl,dh,dx2,hdx2;
			u8     cl,ch,cx2,hcx2;
			u8     bl,bh,bx2,hbx2;
			u8     al,ah,ax2,hax2;
		};
	}
} regs __attribute__ ((packed));

/* exception pile depuis code kernel*/
typedef struct exception_stack
{
	u32     error_code;
	u32     eip;
	u32     cs;
	u32     eflags;
} exception_stack __attribute__ ((packed));

/* sans code erreur depuis code kernel*/
typedef struct exception_stack_noerror
{
	u32     eip;
	u32     cs;
	u32     eflags;
} exception_stack_noerror __attribute__ ((packed));

/* exception pile depuis code user */
typedef struct exception_stack_user
{
	u32     error_code;
	u32     eip;
	u32     cs;
	u32     eflags;
	u32     esp;
	u32     ss;
} exception_stack_user __attribute__ ((packed));

/* sans code erreu depuis code user */
typedef struct exception_stack_noerror_user
{
	u32     eip;
	u32     cs;
	u32     eflags;
	u32     esp;
	u32     ss;
} exception_stack_noerror_user __attribute__ ((packed));

/* descripteur de segment */
typedef struct idtdes
{
	u16     offset0_15;
	u16     select;
	u16     type;
	u16     offset16_31;
} idtdes __attribute__ ((packed));


struct idtr
{
	u16     limite;
	u32     base;
} __attribute__ ((packed));

void    initretry(u32 address);
void    initidt(void);
u32     getinitretry(void);
void    setidt(u32 offset, u16 select, u16 type, u16 index);
void    makeidtdes(u32 offset, u16 select, u16 type, idtdes * desc);
void    initpic(void);
void    enableirq(u8 irq);
void    disableirq(u8 irq);
void    cpuerror(const u8 * src, const regs * stack, bool returnto);
#endif
