/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "libc.h";
#include "syscall.h";
#include "types.h";

u32 libc_testapi(void)
{
	syscall3(0x0, 0x1980, 0x2505, 0x4444);
}

u32 libc_exit(u32 errorcode)
{
	syscall1(0x1, errorcode);
}
