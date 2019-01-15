#include "memory.h"
#include "gdt.h"
.text
.code16

.global gotopmode
gotopmode:
	jmp	1f
1:
	xorl  %eax,%eax
	movl  %ds,%ax
	shl   $0x4,%eax
	add   %eax,(2f)
	movl	%cr0, %eax
	orb	$0x01, %al
	movl	%eax, %cr0
	.byte	0x66, 0xea
2:	.long	pmode
	.word	SEL_KERNEL_CODE

.code32
.section ".text32","ax"
.global pmode
pmode:
	movw $SEL_KERNEL_DATA, %ax
      movw %ax, %ds
      movw %ax, %es
      movw %ax, %fs
      movw %ax, %gs
      movl $KERNEL_CODE_ADDR, %ebx
      movw $SEL_KERNEL_STACK, %ax
      movw %ax, %ss
      movl $KERNEL_STACK_ADDR, %esp
	xor %eax,%eax
	xor %ecx,%ecx
	xor %edx,%edx
	xor %esi,%esi
	xor %edi,%edi
	xor %ebp,%ebp
	jmp *%ebx
