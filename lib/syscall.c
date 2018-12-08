/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include <types.h>
#include <gdt.h>
#include <asm.h>
#include <memory.h>
#include <interrupts.h>
#include <syscall.h>

 /* 32bit SYSENTER instruction entry.
  *
  * Arguments:
  * %eax System call number.
  * %ebx Arg1
  * %ecx Arg2
  * %edx Arg3
  * %esi Arg4
  * %edi Arg5
  * %ebp user stack
  * 0(%ebp) Arg6*/

/*******************************************************************************/

/* Entrée pour les appels système SYSENTER */

void sysenter_handler(void)
{ 
    cli(); 
    regs *dump;
    dumpcpu();
    getESP(dump);
    sti(); 
	switch (dump->eax)
	{
		case 0:
    			printf("Test de fonctionnement syscall\r\n -arguments 1:%Y 2:%Y 3:%Y\r\n", dump->ebx,dump->esi,dump->edi);
			dump->eax=0x6666666;
			break;
		default:
	    		printf("Appel syscall vers fonction inexistante en %Y:%Y", dump->cs,dump->eip);
			break;
		
	}
    restdebugcpu();
    sysexit();
}

/*******************************************************************************/

/* Initialise les appels système par SYSENTER/SYSEXIT */

void initsyscall(void)
{
	wrmsr(0x174, SEL_KERNEL_CODE, 0x0);
	wrmsr(0x175, 0x60000, 0x0);
	wrmsr(0x176, &sysenter_handler+6, 0x0);
}

/*******************************************************************************/
