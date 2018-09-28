/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include <types.h>
#include <gdt.h>
#include <asm.h>

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

}

/*******************************************************************************/

/* Initialise les appels système par SYSENTER/SYSEXIT */

void initsyscall(void)
{
	wrmsr(0x174, SEL_KERNEL_CODE, 0x0);
	wrmsr(0x175, STACK_OFFSET, 0x0);
	wrmsr(0x176, &sysenter_handler, 0x0);
}

/*******************************************************************************/
