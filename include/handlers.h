/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
__attribute__ ((noreturn)) void exception_handler(regs *dump);
__attribute__((interrupt)) void interruption(exception_stack_noerror *caller);
void exception0();
void exception1();
void exception2();
void exception3();
void exception4();
void exception5();
void exception6();
void exception7();
void exception8();
void exception9();
void exception10();
void exception11();
void exception12();
void exception13();
void exception14();
void exception15();
void exception16();
void exception17();
void exception18();
__attribute__((interrupt)) void irq0(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq1(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq2(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq3(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq4(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq5(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq6(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq7(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq8(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq9(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq10(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq11(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq12(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq13(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq14(exception_stack_noerror *caller);
__attribute__((interrupt)) void irq15(exception_stack_noerror *caller);
