/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "vga.h"
#include "vesa.h"
#include "video.h"
#include "interrupts.h"
#include "timer.h"
#include "keyboard.h"
#include "mouse.h"
#include "asm.h"
#include "cpu.h"
#include "string.h"
#include "gdt.h"
#include "shell.h"
#include "syscall.h"
#include "memory.h"

static u8 warnmsg[] =
	"\033[150C\033[8D\033[37m\033[1m[ \033[36mNON\033[37m  ]\033[0m";
static u8 okmsg[] =
	"\033[150C\033[8D\033[37m\033[1m[  \033[32mOK\033[37m  ]\033[0m";
static u8 errormsg[] =
	"\033[150C\033[8D\033[37m\033[1m[\033[31mERREUR\033[37m]\033[0m";
static u8 key = 0;

extern wrapper_timer;
extern wrapper_interruption20;

void ok()
{
	print(okmsg);
	return;
}

void warning()
{
	print(warnmsg);
	return;
}

void error()
{
	print(errormsg);
	return;
}

int main(u8* info)
{
	cli();
	initdriver();
	registerdriver(&vgafonctions);
	registerdriver(&vesafonctions);
	apply_bestdriver();
	changemode(0x1);

	/*  Efface l'ecran   */
	print("\033[2J\r\n\000");
	logo();

	print("\033[37m\033[0m -Initilisation de la memoire virtuelle");
	initpaging();
	remap_memory(VESA_FBMEM);
	ok();

	print("\033[37m\033[0m -Initilisation des processus");
	inittr();
	initretry(&&retry);
	initprocesses();
	initsyscall();
	ok();

	print("\033[37m\033[0m -Initilisation des interruptions");
	initidt();
	initpic();
	setidt((u32) &wrapper_interruption20, SEL_KERNEL_CODE,
		 ENTRY_PRESENT | ENTRY_RING3 | TRAPGATE, 20);
	sti();
	ok();

	print(" -Installation de l'ordonnanceur et horloge systeme (IRQ 0)");
	setidt((u32) &wrapper_timer, SEL_KERNEL_CODE,
	       ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 32);
	enableirq(0);
	ok();

	print(" -Installation du pilote clavier (IRQ 1)");
	setidt((u32) &keyboard_handler, SEL_KERNEL_CODE,
	       ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 33);
	enableirq(1);
	ok();

	print(" -Installation du pilote souris (IRQ12+IRQ2)");
	setidt((u32) &mouse_handler, SEL_KERNEL_CODE,
	       ENTRY_PRESENT | ENTRY_RING0 | INTGATE, 100);
	enableirq(2);
	enableirq(12);
	if (initmouse() != 1)
		warning();
	else
		ok();
	printf(" -Installation du coprocesseur arithmetique");
	finit();
	ok();

      retry:
	sti();
	shell();
}
