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

__attribute__ ((noreturn)) void interruption_handler(regs *dump)
{
	u32 interruption=dump->eip;
	exception_stack_noerror *caller = (exception_stack_noerror*) ((u32*)dump->esp+1);
	bool noerror,user;
	if (caller->cs==SEL_KERNEL_CODE || caller->cs==SEL_USER_CODE)
	{
		noerror=true;
		dump->eip = caller->eip;
		dump->cs = caller->cs;
		dump->eflags = caller->eflags;
		if (dump->cs==SEL_KERNEL_CODE)
		{
			dump->esp = (u32) caller + sizeof(exception_stack_noerror);
			user=false;
		}
		else
		{
			dump->esp = (u32) ((exception_stack_noerror_user*) caller)->esp;
			dump->ss = (u32) ((exception_stack_noerror_user*) caller)->ss;
			user=true;
		}
	}
	else
	{
		noerror=false;
		dump->eip = ((exception_stack*)caller)->eip;
		dump->cs = ((exception_stack*)caller)->cs;
		if (dump->cs==SEL_KERNEL_CODE)
		{
			dump->esp = (u32) caller + sizeof(exception_stack);
			user=false;
		}
		else
		{
			dump->esp = (u32) ((exception_stack_user*) caller)->esp;
			dump->ss = (u32) ((exception_stack_user*) caller)->ss;
			user=true;
		}
	}
	switch (interruption)
	{
		case 20:
			show_lightcpu(dump);
			break;
		default:
			print("Appel d'une interruption\r\n");
	}
	if (dump->cs==SEL_KERNEL_CODE)
		{
			setESP(dump);
			restcpu_kernel();
		}
		else
		{
			setESP(dump);
			restcpu_user();
			iret();
		}
}

/******************************************************************************/
/* Les expections */

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

__attribute__ ((noreturn)) void exception_handler(regs *dump)
{
	u32 exception=dump->eip;
	exception_stack_noerror *caller = (exception_stack_noerror*) ((u32*)dump->esp+1);
	bool noerror,user;
	if (caller->cs==SEL_KERNEL_CODE || caller->cs==SEL_USER_CODE)
	{
		noerror=true;
		dump->eip = caller->eip;
		dump->cs = caller->cs;
		dump->eflags = caller->eflags;
		if (dump->cs==SEL_KERNEL_CODE)
		{
			dump->esp = (u32) caller + sizeof(exception_stack_noerror);
			user=false;
		}
		else
		{
			dump->esp = (u32) ((exception_stack_noerror_user*) caller)->esp;
			dump->ss = (u32) ((exception_stack_noerror_user*) caller)->ss;
			user=true;
		}
	}
	else
	{
		noerror=false;
		dump->eip = ((exception_stack*)caller)->eip;
		dump->cs = ((exception_stack*)caller)->cs;
		if (dump->cs==SEL_KERNEL_CODE)
		{
			dump->esp = (u32) caller + sizeof(exception_stack);
			user=false;
		}
		else
		{
			dump->esp = (u32) ((exception_stack_user*) caller)->esp;
			dump->ss = (u32) ((exception_stack_user*) caller)->ss;
			user=true;
		}
	}
	switch (exception)
	{
		case 0:
			cpuerror("#DE Divide error", dump, false);
		case 1:
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
			goto endofexception;
		case 2:
			cpuerror("NMI Non-maskable hardware interrupt", dump, false);
		case 3:
			cpuerror("#BP INT3 instruction", dump, true);
			iret();
		case 4:
			cpuerror("#OF INTO instruction detected overflow", dump, false);
		case 5:
			cpuerror("#BR BOUND instruction detected overrange", dump, false);
		case 6:
			cpuerror("#UD Invalid instruction opcode", dump, false);
		case 7:
			cpuerror("#NM No coprocessor", dump, false);
		case 8:
			cpuerror("#DF Double fault", dump, false);
		case 9:
			cpuerror("Coprocessor segment overrun", dump, false);	
		case 10:
			cpuerror("#TS Invalid task state segment (TSS)", dump, false);
		case 11:
			cpuerror("#NP Segment not present", dump, false);
		case 12:
			cpuerror("#SS Stack fault", dump, false);
		case 13:
			cpuerror("#GP General protection fault (GPF)", dump, false);
		case 14:
			if (dump->cr2 >= USER_CODE && dump->cr2 < USER_STACK)
			{
				virtual_range_new(findcurrentprocess()->pdd,
						  (u8 *) (dump->cr2 & 0xFFFFF000),
						  PAGESIZE, PAGE_ALL);
			}
			else
			{
				printf("Page fault - %s at adress %Y cs:eip - %Y:%Y\r\n",
					 ex14_errors[((exception_stack*) caller)->error_code & 0xF], dump->cr2,
					 dump->cs, dump->eip);
				cpuerror("#PGF Page fault", dump, false);
			}
			goto endofexception;
		case 15:
			cpuerror("(reserved)", dump, false);
		case 16:
			cpuerror("#MF Coprocessor error", dump, false);
		case 17:
			cpuerror("#AC Alignment check", dump, false);
		case 18: 
			cpuerror("#MC Machine check", dump, false);
	}
	endofexception:
		if (dump->cs==SEL_KERNEL_CODE)
		{
			setESP(dump);
			restcpu_kernel();
		}
		else
		{
			setESP(dump);
			restcpu_user();
			iret();
		}
}

/******************************************************************************/
/* Les IRQ par défaut */

__attribute__((interrupt)) void irq0(exception_stack_noerror *caller)
{	
	print("irq 0");
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
