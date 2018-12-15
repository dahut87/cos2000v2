/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Horde Nicolas             */
/*                                                                             */

#include "libsys.h";
#include "syscall.h";
#include "types.h";

u32 print(u8* string)
{
	return syscall1(2,(u32) string);
}


