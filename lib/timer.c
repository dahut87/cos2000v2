/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "interrupts.h"
#include "types.h"
#include "asm.h"
#include "memory.h"
#include "timer.h"
#include "vga.h"

static u8 curs[4] = { "-\\|/" };

static u8 curspos = 0;

static u32 time = 0;

/******************************************************************************/
/* Récupère la valeur du timer */
/* SYSCALL 
{
"ID":4, 
"NAME":"getticks",
"LIBRARY":"libsys",
"INTERNALNAME":"gettimer",
"DESCRIPTION":"Return the internal value of the timer",
"ARGS": [],
"RETURN":"u32"
}
END */

u32 gettimer(void)
{
	return time;
}

/******************************************************************************/
/* Handler d'interruption de la souris IRQ 0 */

void timer(void)
{
	cli();
	pushf();
	pushad();
	showchar(0, 0, curs[curspos], 7);
	curspos = (curspos + 1) & 0x3;
	time++;
	irqendmaster();
	popad();
	popf();
	sti();
	leave();
	iret();
}

/*******************************************************************************/
