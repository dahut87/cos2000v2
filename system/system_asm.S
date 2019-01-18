/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "memory.h"
#include "gdt.h"

.code32
.section ".inittext"
.global start
.extern main
start:
	movw $SEL_KERNEL_DATA, %ax
      movw %ax, %ds
      movw %ax, %es
      movw %ax, %fs
      movw %ax, %gs
      movw $SEL_KERNEL_STACK, %ax
      movw %ax, %ss
      movl %cr3, %eax
	mov $KERNEL_STACK_ADDR,%esp
	pushl %eax
	xor %eax,%eax
	xor %ebx,%ebx
	xor %ecx,%ecx
	xor %edx,%edx
	xor %esi,%esi
	xor %edi,%edi
	xor %ebp,%ebp
	calll main
