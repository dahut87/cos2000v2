/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "interrupts.h"
#include "types.h"
#include "asm.h"
#include "memory.h"
#include "timer.h"
#include "vga.h"
#include "gdt.h"
#include "process.h"

static u8 curs[4] = { "-\\|/" };

static u8 curspos = 0;

static u32 time = 0;

/******************************************************************************/
/* Récupère la valeur du timer */
/* SYSCALL 
{
"ID":4, 
"NAME":"getticks",
"LIBRARY":"libsys",
"INTERNALNAME":"gettimer",
"DESCRIPTION":"Return the internal value of the timer",
"ARGS": [],
"RETURN":"u32"
}
END */

u32 gettimer(void)
{
	return time;
}

/******************************************************************************/
/* Handler d'interruption du timer IRQ 0 */

__attribute__ ((noreturn)) void timer_handler(regs *dump)
{
	exception_stack_noerror *caller = (exception_stack_noerror*) ((u32*)dump->esp+1);
	bool noerror,user;
	if ((caller->cs & 0xFFF8)==SEL_KERNEL_CODE || (caller->cs & 0xFFF8)==SEL_USER_CODE)
	{
		noerror=true;
		dump->eip = caller->eip;
		dump->cs = caller->cs;
		dump->eflags = caller->eflags;
		if ((dump->cs & 0xFFF8)==SEL_KERNEL_CODE)
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
		if ((dump->cs & 0xFFF8)==SEL_KERNEL_CODE)
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
	irqendmaster();
	showchar(0, 0, curs[curspos], 7);
	curspos = (curspos + 1) & 0x3;
	time++;
	task *new=getschedule();
	if (new!=NULL)
	{
		task *old=findcurrenttask();
		memcpy(dump, &old->dump, sizeof(regs), 0);
		switchtask(new->tid);
	}
	if ((dump->cs & 0xFFF8)==SEL_KERNEL_CODE)
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


/*******************************************************************************/
