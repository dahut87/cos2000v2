/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"

typedef struct vertex2d{
	u16 x;	
	u16 y;
} vertex2d __attribute__ ((packed));

void linev(vertex2d *A, vertex2d *B, u32 color);
void trianglefilled(vertex2d *A, vertex2d *B, vertex2d *C, u32 color);
void triangle(vertex2d *A, vertex2d *B, vertex2d *C, u32 color);
