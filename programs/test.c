/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "libc.h";
#include "types.h";

void main(void)
{
	u32     result = libc_testapi();
	libc_exit(result);
}
