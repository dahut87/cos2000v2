/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "interrupts.h"
#include "types.h"
#include "asm.h"
#include "memory.h"
#include "video.h"
#include "gdt.h"
#include "system.h"
#include "debug.h"
#include "process.h"

#define IDT_SIZE		256	/* nombre de descripteurs */

 /* registre idt */
static struct idtr idtreg;

/* table de IDT */
static idtdes idt[IDT_SIZE];

static u32 retry_address;

/******************************************************************************/
/* Initialise la reprise après erreur */

void initretry(u32 address)
{
    retry_address=address;
}

/******************************************************************************/
/* Récupère l'adresse de reprise après erreur */

u32 getinitretry(void)
{
    return retry_address;
}

/******************************************************************************/
/* Initialise le controleur d'interruption 8259A */

void initpic(void)
{
	/* MASTER */
	/* Initialisation de ICW1 */
	outb(PIC1_CMD, ICW1_INIT + ICW1_ICW4);
	nop();
	/* Initialisation de ICW2 - vecteur de depart = 32 */
	outb(PIC1_DATA, 0x20);
	nop();
	/* Initialisation de ICW3 */
	outb(PIC1_DATA, 0x04);
	nop();
	/* Initialisation de ICW4 */
	outb(PIC1_DATA, ICW4_8086);
	nop();
	/* masquage des interruptions */
	outb(PIC1_DATA, 0xFF);
	nop();
	/* SLAVE */
	/* Initialisation de ICW1 */
	outb(PIC2_CMD, ICW1_INIT + ICW1_ICW4);
	nop();
	/* Initialisation de ICW2 - vecteur de depart = 96 */
	outb(PIC2_DATA, 0x60);
	nop();
	/* Initialisation de ICW3 */
	outb(PIC2_DATA, 0x02);
	nop();
	/* Initialisation de ICW4 */
	outb(PIC2_DATA, ICW4_8086);
	nop();
	/* masquage des interruptions */
	outb(PIC2_DATA, 0xFF);
	nop();
	/* Demasquage des irqs sauf clavier
	   outb(PIC1_DATA,0xFD);
	   nop();
	 */
}

/******************************************************************************/
/* Active une IRQ */

void enableirq(u8 irq)
{
	u16 port;
	cli();
	port = (((irq & 0x08) << 4) + PIC1_DATA);
	outb(port, inb(port) & ~(1 << (irq & 7)));
	sti();
}

/******************************************************************************/
/* Désactive une IRQ */

void disableirq(u8 irq)
{
	u16 port;
	cli();
	port = (((irq & 0x08) << 4) + PIC1_DATA);
	outb(port, inb(port) | (1 << (irq & 7)));
	sti();
}

/******************************************************************************/

/* Créé un descripteur pour l'IDT */

void makeidtdes(u32 offset, u16 select, u16 type, idtdes * desc)
{
	desc->offset0_15 = (offset & 0xffff);
	desc->select = select;
	desc->type = type;
	desc->offset16_31 = (offset & 0xffff0000) >> 16;
	return;
}

/******************************************************************************/
/* Change une entrée dans l'IDT */

void setidt(u32 offset, u16 select, u16 type, u16 index)
{
	cli();
	idtdes *desc;
	desc = idtreg.base;
	desc[index].offset0_15 = (offset & 0xffff);
	desc[index].select = select;
	desc[index].type = type;
	desc[index].offset16_31 = (offset & 0xffff0000) >> 16;
	sti();
}

/******************************************************************************/
/* Met une entrée dans l'IDT */

void putidt(u32 offset, u16 select, u16 type, u16 index)
{
	idtdes temp;
	makeidtdes(offset, select, type, &temp);
	idt[index] = temp;
}

/******************************************************************************/
/* Affiche une erreur CPU et fige l'ordinateur */

void cpuerror(const u8 * src, const regs *stack)
{
	printf("\033[31m*** ERREUR CPU : %s *** \r\n", src);
    if (stack!=NULL) show_cpu(stack);
	print("<Appuyer une touche pour continuer>\033[0m\r\n");
    sti();
    waitascii();
    initselectors(retry_address);
	/*while (true) {
		nop();
	}*/
}

/******************************************************************************/
/* Déclenché lors de l'appel d'une interruption */

void interruption()
{
	cli();
	pushad();
	print("Appel d'une interruption\r\n");
	popad();
	sti();
	iret();
}

/******************************************************************************/
/* Les expections */

void exception0()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#DE Divide error",dump);
}

void exception1()
{
    cli();
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
    changevc(6);
    clearscreen();
    show_lightcpu(dump);
    printf("\r\n\033[7m[P]\033[0m PAS A PAS \033[7m D \033[0m PAS A PAS DETAILLE \033[7m C \033[0m CONTINUER \033[7m S \033[0m STOPPER \033[7m V \033[0m VOIR \033[7m S \033[0m SCINDER");
	sti();
    u8 ascii=waitascii();
    cli(); 
    if (ascii=='P' || ascii=='p')
          setdebugreg(0,current->eip+disasm(current->eip, NULL, false), DBG_EXEC);
    else if (ascii=='D' || ascii=='d')
          setdebugreg(0,0, DBG_CLEAR);  
    else if (ascii=='C' || ascii=='c')
          setdebugreg(0,0, DBG_CLEAR);   
    else if (ascii=='S' || ascii=='s')
          {
          changevc(0);
          sti();
          initselectors(retry_address);            
    }
    changevc(0);
    restdebugcpu();
    iret();
}

void exception2()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("NMI Non-maskable hardware interrupt",dump);
}

void exception3()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#BP INT3 instruction",dump);
}

void exception4()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#OF INTO instruction detected overflow",dump);
}

void exception5()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#BR BOUND instruction detected overrange",dump);
}

void exception6()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#UD Invalid instruction opcode",dump);
}

void exception7()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#NM No coprocessor",dump);
}

void exception8()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#DF Double fault",dump);
}

void exception9()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("Coprocessor segment overrun",dump);
}

void exception10()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#TS Invalid task state segment (TSS)",dump);
}

void exception11()
{
    regs *dump;
    exception_stack *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#NP Segment not present",dump);
}

void exception12()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#SS Stack fault",dump);
}

void exception13()
{
    regs *dump;
    exception_stack *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#GP General protection fault (GPF)",dump);
}

static u8 ex14_errors1[]="Supervisory process tried to read a non-present page entry";
static u8 ex14_errors2[]="Supervisory process tried to read a page and caused a protection fault";
static u8 ex14_errors3[]="Supervisory process tried to write to a non-present page entry";
static u8 ex14_errors4[]="Supervisory process tried to write a page and caused a protection fault";
static u8 ex14_errors5[]="User process tried to read a non-present page entry";
static u8 ex14_errors6[]="User process tried to read a page and caused a protection fault";
static u8 ex14_errors7[]="User process tried to write to a non-present page entry";
static u8 ex14_errors8[]="User process tried to write a page and caused a protection fault";
static u8 *ex14_errors[]={&ex14_errors1,&ex14_errors2,&ex14_errors3,&ex14_errors4,&ex14_errors5,&ex14_errors6,&ex14_errors7,&ex14_errors8};

void exception14()
{
    regs *dump;
    exception_stack *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
    if (dump->cr2 >= USER_CODE && dump->cr2 < USER_STACK) 
    {
        virtual_range_new(getcurrentprocess()->pdd, (u8 *) (dump->cr2 & 0xFFFFF000), PAGESIZE, PAGE_ALL);
	} 
else {		
        printf("Page fault - %s at adress %Y cs:eip - %Y:%Y\r\n",ex14_errors[current->error_code & 0xF],dump->cr2,dump->cs,dump->eip);
        cpuerror("#PGF Page fault",dump);
	}
    restdebugcpu();
    iret();
}

void exception15()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("(reserved)",dump);
}

void exception16()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#MF Coprocessor error",dump);
}

void exception17()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#AC Alignment check",dump);
}

void exception18()
{
    regs *dump;
    exception_stack_noerror *current;
    u32 *oldesp;
    getEBP(oldesp);
    dumpcpu();
    getESP(dump);
    current=(exception_stack *) (oldesp+1);
    dump->ebp=*oldesp;
    dump->esp=(u32) oldesp+sizeof(exception_stack);
    dump->eip=current->eip;
	cpuerror("#MC Machine check",dump);
}

/******************************************************************************/
/* Les IRQ par défaut */

void irq0()
{
	cli();
	pushf();
	pushad();
	print("irq 0");
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq1()
{
	cli();
	pushf();
	pushad();
	print("irq 1");
	while ((inb(0x64) & 1) == 0) ;
	inb(0x60);
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq2()
{
	cli();
	pushf();
	pushad();
	print("irq 2");
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq3()
{
	cli();
	pushf();
	pushad();
	print("irq 3");
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq4()
{
	cli();
	pushf();
	pushad();
	print("irq 4");
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq5()
{
	cli();
	pushf();
	pushad();
	print("irq 5");
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq6()
{
	cli();
	pushf();
	pushad();
	print("irq 6");
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq7()
{
	cli();
	pushf();
	pushad();
	print("irq 7");
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq8()
{
	cli();
	pushf();
	pushad();
	print("irq 8");
	irqendslave();
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq9()
{
	cli();
	pushf();
	pushad();
	print("irq 9");
	irqendslave();
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq10()
{
	cli();
	pushf();
	pushad();
	print("irq 10");
	irqendslave();
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq11()
{
	cli();
	pushf();
	pushad();
	print("irq 11");
	irqendslave();
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq12()
{
	cli();
	pushf();
	pushad();
	print("irq 12");
	while ((inb(0x64) & 1) == 0) ;
	inb(0x60);
	irqendslave();
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq13()
{
	cli();
	pushf();
	pushad();
	print("irq 13");
	irqendslave();
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq14()
{
	cli();
	pushf();
	pushad();
	print("irq 14");
	irqendslave();
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

void irq15()
{
	cli();
	print("irq 15");
	irqendslave();
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

/******************************************************************************/
/* Initialise une IDT */

void initidt(void)
{
	u16 i;
	putidt((u32) exception0, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 0);
	putidt((u32) exception1, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 1);
	putidt((u32) exception2, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 2);
	putidt((u32) exception3, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 3);
	putidt((u32) exception4, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 4);
	putidt((u32) exception5, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 5);
	putidt((u32) exception6, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 6);
	putidt((u32) exception7, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 7);
	putidt((u32) exception8, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 8);
	putidt((u32) exception9, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 9);
	putidt((u32) exception10, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 10);
	putidt((u32) exception11, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 11);
	putidt((u32) exception12, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 12);
	putidt((u32) exception13, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 13);
	putidt((u32) exception14, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 14);
	putidt((u32) exception15, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 15);
	putidt((u32) exception16, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 16);
	putidt((u32) exception17, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 17);
	putidt((u32) exception18, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 18);
	for (i = 19; i < 32; i++) {
		putidt((u32) interruption, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING3 | TRAPGATE, i);
	}
	putidt((u32) irq0, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 32);
	putidt((u32) irq1, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 33);
	putidt((u32) irq2, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 34);
	putidt((u32) irq3, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 35);
	putidt((u32) irq4, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 36);
	putidt((u32) irq5, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 37);
	putidt((u32) irq6, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 38);
	putidt((u32) irq7, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 39);
	for (i = 40; i < 96; i++) {
		putidt((u32) interruption, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING3 | TRAPGATE, i);
	}
	putidt((u32) irq8, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 96);
	putidt((u32) irq9, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 97);
	putidt((u32) irq10, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 98);
	putidt((u32) irq11, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 99);
	putidt((u32) irq12, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 100);
	putidt((u32) irq13, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 101);
	putidt((u32) irq14, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 102);
	putidt((u32) irq15, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 103);
	for (i = 104; i < IDT_SIZE; i++) {
		putidt((u32) interruption, SEL_KERNEL_CODE, ENTRY_PRESENT | ENTRY_RING0 | TRAPGATE, i);
	}
	/* initialise le registre idt */
	idtreg.limite = IDT_SIZE * sizeof(idtdes);
	idtreg.base = IDT_ADDR;
	/* recopie de la IDT a son adresse */
	memcpy(&idt, (u8 *) idtreg.base, idtreg.limite, 1);
	/* chargement du registre IDTR */
	lidt(&idtreg);
}

/******************************************************************************/
/* 8253/8254 PIT (Programmable Interval Timer) Timer ajustable */

void inittimer(void)
{
    u32 divisor = TIMER_FREQ / HZ;
	outb(TIMER_MODE, RATE_GENERATOR);
	outb(TIMER0, (u8) divisor);
	outb(TIMER0, (u8) (divisor >> 8));
}
/*******************************************************************************/
