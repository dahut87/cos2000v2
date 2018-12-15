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

/******************************************************************************/
/* Déclenché lors de l'appel d'une interruption */

__attribute__((interrupt)) void interruption(exception_stack_noerror *caller)
{
	print("Appel d'une interruption\r\n");
}

/******************************************************************************/
/* Les expections */

void exception0()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("#DE Divide error", dump, false);
}

void exception1()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	changevc(6);
	clearscreen();
	show_lightcpu(dump);
	printf("\r\n\033[7m[P]\033[0m PAS A PAS \033[7m D \033[0m PAS A PAS DETAILLE \033[7m C \033[0m CONTINUER \033[7m S \033[0m STOPPER \033[7m V \033[0m VOIR \033[7m S \033[0m SCINDER");
	sti();
	u8      ascii = waitascii();
	cli();
	if (ascii == 'P' || ascii == 'p')
		setdebugreg(0,
			    caller->eip + disasm(caller->eip, NULL, false),
			    DBG_EXEC);
	else if (ascii == 'D' || ascii == 'd')
		setdebugreg(0, 0, DBG_CLEAR);
	else if (ascii == 'C' || ascii == 'c')
		setdebugreg(0, 0, DBG_CLEAR);
	else if (ascii == 'S' || ascii == 's')
	{
		changevc(0);
		sti();
		initselectors(getinitretry());
	}
	changevc(0);
	restdebugcpu();
	iret();
}

void exception2()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("NMI Non-maskable hardware interrupt", dump, false);
}

void exception3()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("#BP INT3 instruction", dump, true);
	iret();
}

void exception4()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("#OF INTO instruction detected overflow", dump, false);
}

void exception5()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("#BR BOUND instruction detected overrange", dump, false);
}

void exception6()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("#UD Invalid instruction opcode", dump, false);
}

void exception7()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("#NM No coprocessor", dump, false);
}

void exception8()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("#DF Double fault", dump, false);
}

void exception9()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("Coprocessor segment overrun", dump, false);
}

void exception10()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("#TS Invalid task state segment (TSS)", dump, false);
}

void exception11()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("#NP Segment not present", dump, false);
}

void exception12()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("#SS Stack fault", dump, false);
}

void exception13()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu(dump, caller, oldesp);
	cpuerror("#GP General protection fault (GPF)", dump, false);
}

static u8 ex14_errors1[] =
	"Supervisory process tried to read a non-present page entry";
static u8 ex14_errors2[] =
	"Supervisory process tried to read a page and caused a protection fault";
static u8 ex14_errors3[] =
	"Supervisory process tried to write to a non-present page entry";
static u8 ex14_errors4[] =
	"Supervisory process tried to write a page and caused a protection fault";
static u8 ex14_errors5[] =
	"User process tried to read a non-present page entry";
static u8 ex14_errors6[] =
	"User process tried to read a page and caused a protection fault";
static u8 ex14_errors7[] =
	"User process tried to write to a non-present page entry";
static u8 ex14_errors8[] =
	"User process tried to write a page and caused a protection fault";
static u8 *ex14_errors[] =
	{ &ex14_errors1, &ex14_errors2, &ex14_errors3, &ex14_errors4,
	&ex14_errors5, &ex14_errors6, &ex14_errors7, &ex14_errors8
};

void exception14()
{
	regs   *dump;
	exception_stack *caller;
	u32    *oldesp;
	getEBP(oldesp);
	dumpcpu();
	getESP(dump);
	dump->ebp = *oldesp;
	dump->eip = caller->eip;
	dump->cs = caller->cs;
	if (caller->cs == SEL_KERNEL_CODE)
		dump->esp = (u32) oldesp + sizeof(exception_stack);
	else
	{
		dump->esp = (u32) ((exception_stack_user *) caller)->esp;
		dump->ss = (u32) ((exception_stack_user *) caller)->ss;
	}
	if (dump->cr2 >= USER_CODE && dump->cr2 < USER_STACK)
	{
		virtual_range_new(getcurrentprocess()->pdd,
				  (u8 *) (dump->cr2 & 0xFFFFF000),
				  PAGESIZE, PAGE_ALL);
	}
	else
	{
		printf("Page fault - %s at adress %Y cs:eip - %Y:%Y\r\n",
		       ex14_errors[caller->error_code & 0xF], dump->cr2,
		       dump->cs, dump->eip);
		cpuerror("#PGF Page fault", dump, false);
	}
	restdebugcpu();
	iret();
}

void exception15()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("(reserved)", dump, false);
}

void exception16()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("#MF Coprocessor error", dump, false);
}

void exception17()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("#AC Alignment check", dump, false);
}

void exception18()
{
	regs   *dump;
	exception_stack_noerror *caller;
	u32    *oldesp;
	savecpu_noerror(dump, caller, oldesp);
	cpuerror("#MC Machine check", dump, false);
}

/******************************************************************************/
/* Les IRQ par défaut */

__attribute__((interrupt)) void irq0(exception_stack_noerror *caller)
{	print("irq 0");
	irqendmaster();
}

__attribute__((interrupt)) void irq1(exception_stack_noerror *caller)
{
	print("irq 1");
	while ((inb(0x64) & 1) == 0);
	inb(0x60);
	irqendmaster();
}

__attribute__((interrupt)) void irq2(exception_stack_noerror *caller)
{	print("irq 2");
	irqendmaster();
}

__attribute__((interrupt)) void irq3(exception_stack_noerror *caller)
{
	print("irq 3");
	irqendmaster();
}

__attribute__((interrupt)) void irq4(exception_stack_noerror *caller)
{
	print("irq 4");
	irqendmaster();
}

__attribute__((interrupt)) void irq5(exception_stack_noerror *caller)
{
	print("irq 5");
	irqendmaster();
}

__attribute__((interrupt)) void irq6(exception_stack_noerror *caller)
{
	print("irq 6");
	irqendmaster();
}

__attribute__((interrupt)) void irq7(exception_stack_noerror *caller)
{
	print("irq 7");
	irqendmaster();
}

__attribute__((interrupt)) void irq8(exception_stack_noerror *caller)
{
	print("irq 8");
	irqendslave();
	irqendmaster();
}

__attribute__((interrupt)) void irq9(exception_stack_noerror *caller)
{
	print("irq 9");
	irqendslave();
	irqendmaster();
}

__attribute__((interrupt)) void irq10(exception_stack_noerror *caller)
{
	print("irq 10");
	irqendslave();
	irqendmaster();
}

__attribute__((interrupt)) void irq11(exception_stack_noerror *caller)
{
	print("irq 11");
	irqendslave();
	irqendmaster();
}

__attribute__((interrupt)) void irq12(exception_stack_noerror *caller)
{
	print("irq 12");
	while ((inb(0x64) & 1) == 0);
	inb(0x60);
	irqendslave();
	irqendmaster();
}

__attribute__((interrupt)) void irq13(exception_stack_noerror *caller)
{
	print("irq 13");
	irqendslave();
	irqendmaster();
	popad();
	popf();
}

__attribute__((interrupt)) void irq14(exception_stack_noerror *caller)
{
	print("irq 14");
	irqendslave();
	irqendmaster();
}

__attribute__((interrupt)) void irq15(exception_stack_noerror *caller)
{
	print("irq 15");
	irqendslave();
	irqendmaster();
}
