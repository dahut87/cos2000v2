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

void cpuerror(const u8 * src, const exception_stack *stack)
{
	printf("\033[31m*** ERREUR CPU : %s *** \r\n", src);
	dump_regs(stack);
	print("<Appuyer une touche pour continuer>\r\n");
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
	pushf();
	pushad();
	print("Appel d'une interruption\r\n");
	popad();
	popf();
	sti();
	iret();
}

/******************************************************************************/
/* Les expections */

void exception0()
{
	cpuerror("divide error",0x0);
}

void exception1()
{
	cpuerror("debug exception",0x0);
}

void exception2()
{
	cpuerror("non-maskable hardware interrupt",0x0);
}

void exception3()
{
	cpuerror("INT3 instruction",0x0);
}

void exception4()
{
	cpuerror("INTO instruction detected overflow",0x0);
}

void exception5()
{
	cpuerror("BOUND instruction detected overrange",0x0);
}

void exception6()
{
	cpuerror("invalid instruction opcode",0x0);
}

void exception7()
{
	cpuerror("no coprocessor",0x0);
}

void exception8()
{
	cpuerror("double fault",0x0);
}

void exception9()
{
	cpuerror("coprocessor segment overrun",0x0);
}

void exception10()
{
	cpuerror("invalid task state segment (TSS)",0x0);
}

void exception11()
{
	cpuerror("segment not present",0x0);
}

void exception12()
{
	cpuerror("stack fault",0x0);
}

void exception13()
{
	cpuerror("general protection fault (GPF)",0x0);
}

void exception14()
{
    exception_stack *stack = getESP()+0x30;
    u8 *errorstring;
    switch (stack->error_code) {
        case 0:
            errorstring="page fault - Supervisory process tried to read a non-present page entry";
            break;
        case 1:
            errorstring="Page fault - Supervisory process tried to read a page and caused a protection fault";
            break;
        case 2:
            errorstring="Page fault - Supervisory process tried to write to a non-present page entry";
            break;
        case 3:
            errorstring="Page fault - Supervisory process tried to write a page and caused a protection fault";
            break;
        case 4:
            errorstring="Page fault - User process tried to read a non-present page entry";
            break;
        case 5:
            errorstring="Page fault - User process tried to read a page and caused a protection fault";
            break;
        case 6:
            errorstring="Page fault - User process tried to write to a non-present page entry";
            break;      
        case 7:
            errorstring="Page fault - User process tried to write a page and caused a protection fault";
            break;   
    }
	cpuerror(errorstring,stack);
}

void exception15()
{
	cpuerror("(reserved)",0x0);
}

void exception16()
{
	cpuerror("coprocessor error",0x0);
}

void exception17()
{
	cpuerror("alignment check",0x0);
}

void exception18()
{
	cpuerror("machine check",0x0);
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
	asm("addl  $0x0C, %esp;");
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
	asm("addl  $0x01C, %esp;");
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
	asm("addl  $0x0C, %esp;");
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
	asm("addl  $0x0C, %esp;");
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
	asm("addl  $0x0C, %esp;");
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
	asm("addl  $0x0C, %esp;");
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
	asm("addl  $0x0C, %esp;");
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
	asm("addl  $0x0C, %esp;");
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
	asm("addl  $0x0C, %esp;");
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
	asm("addl  $0x0C, %esp;");
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
	asm("addl  $0x0C, %esp;");
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
	asm("addl  $0x0C, %esp;");
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
	asm("addl  $0x1C, %esp;");
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
	asm("addl  $0x0C, %esp;");
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
	asm("addl  $0x0C, %esp;");
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
	asm("addl  $0x0C, %esp;");
	iret();
}

/******************************************************************************/
/* Initialise une IDT */

void initidt(void)
{
	u16 i;
	putidt((u32) exception0, SEL_KERNEL_CODE, INTGATE, 0);
	putidt((u32) exception1, SEL_KERNEL_CODE, INTGATE, 1);
	putidt((u32) exception2, SEL_KERNEL_CODE, INTGATE, 2);
	putidt((u32) exception3, SEL_KERNEL_CODE, INTGATE, 3);
	putidt((u32) exception4, SEL_KERNEL_CODE, INTGATE, 4);
	putidt((u32) exception5, SEL_KERNEL_CODE, INTGATE, 5);
	putidt((u32) exception6, SEL_KERNEL_CODE, INTGATE, 6);
	putidt((u32) exception7, SEL_KERNEL_CODE, INTGATE, 7);
	putidt((u32) exception8, SEL_KERNEL_CODE, INTGATE, 8);
	putidt((u32) exception9, SEL_KERNEL_CODE, INTGATE, 9);
	putidt((u32) exception10, SEL_KERNEL_CODE, INTGATE, 10);
	putidt((u32) exception11, SEL_KERNEL_CODE, INTGATE, 11);
	putidt((u32) exception12, SEL_KERNEL_CODE, INTGATE, 12);
	putidt((u32) exception13, SEL_KERNEL_CODE, INTGATE, 13);
	putidt((u32) exception14, SEL_KERNEL_CODE, INTGATE, 14);
	putidt((u32) exception15, SEL_KERNEL_CODE, INTGATE, 15);
	putidt((u32) exception16, SEL_KERNEL_CODE, INTGATE, 16);
	putidt((u32) exception17, SEL_KERNEL_CODE, INTGATE, 17);
	putidt((u32) exception18, SEL_KERNEL_CODE, INTGATE, 18);
	for (i = 19; i < 32; i++) {
		putidt((u32) interruption, 0x20, TRAPGATE, i);
	}
	putidt((u32) irq0, SEL_KERNEL_CODE, INTGATE, 32);
	putidt((u32) irq1, SEL_KERNEL_CODE, INTGATE, 33);
	putidt((u32) irq2, SEL_KERNEL_CODE, INTGATE, 34);
	putidt((u32) irq3, SEL_KERNEL_CODE, INTGATE, 35);
	putidt((u32) irq4, SEL_KERNEL_CODE, INTGATE, 36);
	putidt((u32) irq5, SEL_KERNEL_CODE, INTGATE, 37);
	putidt((u32) irq6, SEL_KERNEL_CODE, INTGATE, 38);
	putidt((u32) irq7, SEL_KERNEL_CODE, INTGATE, 39);
	for (i = 40; i < 96; i++) {
		putidt((u32) interruption, 0x20, TRAPGATE, i);
	}
	putidt((u32) irq8, SEL_KERNEL_CODE, INTGATE, 96);
	putidt((u32) irq9, SEL_KERNEL_CODE, INTGATE, 97);
	putidt((u32) irq10, SEL_KERNEL_CODE, INTGATE, 98);
	putidt((u32) irq11, SEL_KERNEL_CODE, INTGATE, 99);
	putidt((u32) irq12, SEL_KERNEL_CODE, INTGATE, 100);
	putidt((u32) irq13, SEL_KERNEL_CODE, INTGATE, 101);
	putidt((u32) irq14, SEL_KERNEL_CODE, INTGATE, 102);
	putidt((u32) irq15, SEL_KERNEL_CODE, INTGATE, 103);
	for (i = 104; i < IDT_SIZE; i++) {
		putidt((u32) interruption, 0x20, TRAPGATE, i);
	}
	/* initialise le registre idt */
	idtreg.limite = IDT_SIZE * 8;
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
	outb(TIMER_MODE, RATE_GENERATOR);
	outb(TIMER0, (u8) (TIMER_FREQ / HZ));
	outb(TIMER0, (u8) ((TIMER_FREQ / HZ) >> 8));
}
/*******************************************************************************/
