/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "vga.h"
#include "video.h"
#include "2d.h"
#include "math.h"

/******************************************************************************/
/* Affiche une ligne entre les points spécifiés */

void linev(vertex2d * A, vertex2d * B, u32 color)
{
	line(A->x, A->y, B->x, B->y, color);
}

void line(u32 x1, u32 y1, u32 x2, u32 y2, u32 color)
{
	s32 dx, dy, sdx, sdy;
	u32 i, dxabs, dyabs, x, y, px, py;

	dx = x2 - x1;		/*  distance horizontale de la line */
	dy = y2 - y1;		/* distance verticale de la line * */
	dxabs = abs(dx);
	dyabs = abs(dy);
	sdx = sgn(dx);
	sdy = sgn(dy);
	x = dyabs >> 1;
	y = dxabs >> 1;
	px = x1;
	py = y1;

	writepxl(px, py, color);

	if (dxabs >= dyabs) {	/* la ligne est plus horizontale que verticale */
		for (i = 0; i < dxabs; i++) {
			y += dyabs;
			if (y >= dxabs) {
				y -= dxabs;
				py += sdy;
			}
			px += sdx;
			writepxl(px, py, color);
		}
	} else {		/* la ligne est plus verticale que horizontale */

		for (i = 0; i < dyabs; i++) {
			x += dxabs;
			if (x >= dyabs) {
				x -= dyabs;
				px += sdx;
			}
			py += sdy;
			writepxl(px, py, color);
		}
	}
}

/******************************************************************************/
/* Affiche un triangle rempli entre les points spécifiés */

void trianglefilled(vertex2d * AA, vertex2d * BB, vertex2d * CC, u32 color)
{
	vertex2d *A, *B, *C, *TEMP;
	u32 a, b, y, last;
	int dx1, dx2, dx3, dy1, dy2, dy3, sa, sb;
	A = AA;
	B = BB;
	C = CC;
	while (A->y > B->y || B->y > C->y || A->y == C->y) {
		if (A->y > B->y)
			swapvertex(A, B);
		if (B->y > C->y)
			swapvertex(B, C);
		if (A->y > C->y)
			swapvertex(A, C);
	}
	if (A->y == C->y) {	//meme ligne
		a = b = A->x;
		if (B->x < a)
			a = B->x;
		else if (B->x > b)
			b = B->x;
		if (C->x < a)
			a = C->x;
		else if (C->x > b)
			b = C->x;
		hline(a, b, A->y, color);
		return;
	}
	dx1 = B->x - A->x;
	dy1 = B->y - A->y;
	dx2 = C->x - A->x;
	dy2 = C->y - A->y;
	dx3 = C->x - B->x;
	dy3 = C->y - B->y;
	sa = 0;
	sb = 0;

	if (B->y == C->y)
		last = B->y;
	else
		last = B->y - 1;

	for (y = A->y; y <= last; y++) {
		a = A->x + sa / dy1;
		b = A->x + sb / dy2;
		sa += dx1;
		sb += dx2;
		hline(a, b, y, color);
	}

	sa = dx3 * (y - B->y);
	sb = dx2 * (y - A->y);
	for (; y <= C->y; y++) {
		a = B->x + sa / dy3;
		b = A->x + sb / dy2;
		sa += dx3;
		sb += dx2;
		hline(a, b, y, color);
	}
}

void swapvertex(vertex2d * A, vertex2d * B)
{
	vertex2d temp = *A;
	*A = *B;
	*B = temp;
}

/******************************************************************************/
/* Affiche un triangle  entre les points spécifiés */

void triangle(vertex2d * AA, vertex2d * BB, vertex2d * CC, u32 color)
{
	line(AA->x, AA->y, BB->x, BB->y, color);
	line(BB->x, BB->y, CC->x, CC->y, color);
	line(CC->x, CC->y, AA->x, AA->y, color);
}


