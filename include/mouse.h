/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "interrupts.h"

bool    initmouse(void);
__attribute__((interrupt)) void mouse_handler(exception_stack_noerror *caller);
void    outmseack(u8 value);
void    outmsecmd(u8 command);
