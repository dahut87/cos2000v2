#include "types.h"

typedef struct vertex2d{
	u32 x;	
	u32 y;
} vertex2d __attribute__ ((packed));


void line(u32 x1, u32 y1, u32 x2, u32 y2, u8 color);
void linev(vertex2d *A, vertex2d *B, u8 color);
void hline(u32 x1, u32 x2, u32 y, u8 color);
void trianglefilled(vertex2d *A, vertex2d *B, vertex2d *C, u8 color);
void triangle(vertex2d *A, vertex2d *B, vertex2d *C, u8 color);
