/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Horde Nicolas             */
/*                                                                             */

#include "libsys.h";
#include "syscall.h";
#include "types.h";

u32 testapi(u32 arg1, u32 arg2, u32 arg3)
{
	return syscall3(0,(u32) arg1,(u32) arg2,(u32) arg3);
}

u32 getticks(void)
{
	return syscall0(4);
}

u8 waitkey(void)
{
	return syscall0(1);
}

void exit(u32 resultcode)
{
	syscall1(5,(u32) resultcode);
	return;
}


