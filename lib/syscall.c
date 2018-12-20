/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Horde Nicolas             */
/*                                                                             */
#include <types.h>
#include <gdt.h>
#include <asm.h>
#include <memory.h>
#include <interrupts.h>
#include <syscall.h>
#include <process.h>

 /* 32bit SYSENTER instruction entry.
  *
  * Arguments:
  * %eax System call number.
  * %ebx Arg1
  * %esi Arg2
  * %edi Arg3*/

extern wrapper_sysenter;

/*******************************************************************************/
/* Initialise les appels système par SYSENTER/SYSEXIT */

void initsyscall(void)
{
	wrmsr(0x174, SEL_KERNEL_CODE, 0x0);
	wrmsr(0x175, NULL, 0x0);
	wrmsr(0x176, &wrapper_sysenter, 0x0);
	return;
}

/*******************************************************************************/
/* Fonction permettant de tester le fonctionnement de SYSENTER */
/* SYSCALL 
{
"ID":0,
"LIBRARY":"libsys",
"NAME":"testapi",
"INTERNALNAME":"testapi",
"DESCRIPTION":"Simple function to test if SYSCALL API is correctly running",
"ARGS": [
{"TYPE":"u32","NAME":"arg1","DESCRIPTION":"first argument of your choice"},
{"TYPE":"u32","NAME":"arg2","DESCRIPTION":"second argument of your choice"},
{"TYPE":"u32","NAME":"arg3","DESCRIPTION":"third argument of your choice"}
],
"RETURN":"u32",
"DUMP":"yes"
}
END */

u32 testapi(u32 arg1, u32 arg2, u32 arg3, regs* dump)
{
	printf("Appel syscall %u depuis %Y:%Y avec arguments => ARG1:%Y ARG2:%Y ARG3:%Y\r\n", dump->eax, (u32) dump->cs, dump->eip, arg1, arg2, arg3);	
	return;
}

/*******************************************************************************/
/* Entrée pour les appels système SYSENTER */
__attribute__ ((noreturn)) void sysenter_handler(regs *dump)
{
	sti();
	switch (dump->eax)
	{
     		case 4:
			dump->eax=(u32) gettimer();
			break;
     		case 2:
			dump->eax=(u32) print(dump->ebx);
			break;
     		case 0:
			dump->eax=(u32) testapi(dump->ebx, dump->esi, dump->edi, dump);
			break;
     		case 1:
			dump->eax=(u32) waitascii();
			break;
     		case 5:
			dump->eax=(u32) processexit(dump->ebx);
			break;

		default:
			printf("Appel syscall vers fonction inexistante en %Y:%Y", dump->cs, dump->eip);
			break;
	}
      //dump->eflags &= ~(1 << 6);
      dump->eflags |= (1 << 6);
	setESP(dump);
	restcpu_user();
	sysexit();
}

/*******************************************************************************/

