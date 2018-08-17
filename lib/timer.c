#include "interrupts.h"
#include "types.h"
#include "asm.h"
#include "memory.h"
#include "timer.h"
#include "vga.h"

static u8 curs[4] = { "-\\|/" };

static u8 curspos = 0;

/******************************************************************************/

/* Handler d'interruption de la souris IRQ 0 */

void timer()
{
	cli();
	pushf();
	pushad();
	showchar(0, 0, curs[curspos], 7);
	curspos = (curspos + 1) & 0x3;
	irqendmaster();
	popad();
	popf();
	sti();
	asm("addl  $0x0C, %esp;");
	iret();
}
