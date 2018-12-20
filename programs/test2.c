/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "libsys.h";
#include "libvideo.h";
#include "types.h";

void main(void)
{
	while (true)
	{
		if (getticks()%100000==0)
			print("[TEST]\r\n");
	}
	exit(1);
}
