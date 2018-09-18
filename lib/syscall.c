#include <types.h>
#include <gdt.h>  
#include <asm.h>  

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

