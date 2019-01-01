/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#define _ASSEMBLY

#include "memory.h"
#include "gdt.h"

.code32
.section ".text"
.global startup_32
startup_32:
	ljmp $SEL_KERNEL_CODE,$suite
suite:
	movw $SEL_KERNEL_DATA, %ax
      movw %ax, %ds
      movw %ax, %es
      movw %ax, %fs
      movw %ax, %gs
      movw $SEL_KERNEL_STACK, %ax
      movw %ax, %ss
      movl $KERNEL_STACK_ADDR, %esp
	xor %eax,%eax
	xor %ebx,%ebx
	xor %ecx,%ecx
	xor %edx,%edx
	xor %esi,%esi
	xor %edi,%edi
	xor %ebp,%ebp
