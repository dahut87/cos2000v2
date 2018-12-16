/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

.section .text

.macro  exception num
.global wrapper_exception\num
wrapper_exception\num:
pushl %ss
pushl %esp
pushf 
pushl %cs
pushl $\num
jmp dumpcpu
.endm

exception 0
exception 1
exception 2
exception 3
exception 4
exception 5
exception 6
exception 7
exception 8
exception 9
exception 10
exception 11
exception 12
exception 13
exception 14
exception 15
exception 16
exception 17
exception 18

.global wrapper_sysenter
wrapper_sysenter:
pushl %ss
pushl %esp
pushf 
pushl %cs
pushl $0x00
pushl %ds
pushl %es
pushl %fs
pushl %gs
pushl %eax
pushl %ebx
pushl %ecx
pushl %edx
pushl %esi
pushl %edi
pushl %ebp
mov %cr0, %eax 
pushl %eax
mov %cr2, %eax 
pushl %eax
mov %cr3, %eax 
pushl %eax
mov %cr4, %eax 
pushl %eax 
mov %dr0, %eax 
pushl %eax
mov %dr1, %eax 
pushl %eax
mov %dr2, %eax 
pushl %eax
mov %dr3, %eax 
pushl %eax
mov %dr6, %eax 
pushl %eax
mov %dr7, %eax 
pushl %eax
mov $0xC0000080, %ecx 
rdmsr 
pushl %edx 
pushl %eax
pushl %esp
call sysenter_handler

dumpcpu:
pushl %ds
pushl %es
pushl %fs
pushl %gs
pushl %eax
pushl %ebx
pushl %ecx
pushl %edx
pushl %esi
pushl %edi
pushl %ebp
mov %cr0, %eax 
pushl %eax
mov %cr2, %eax 
pushl %eax
mov %cr3, %eax 
pushl %eax
mov %cr4, %eax 
pushl %eax 
mov %dr0, %eax 
pushl %eax
mov %dr1, %eax 
pushl %eax
mov %dr2, %eax 
pushl %eax
mov %dr3, %eax 
pushl %eax
mov %dr6, %eax 
pushl %eax
mov %dr7, %eax 
pushl %eax
mov $0xC0000080, %ecx 
rdmsr 
pushl %edx 
pushl %eax
pushl %esp
call exception_handler
