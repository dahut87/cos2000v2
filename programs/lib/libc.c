/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "libc.h";
#include "syscall.h";
#include "types.h";

u32 test_api(void)
{
    syscall0(0x0);
}
