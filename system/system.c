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
#include "system.h"
#include "boot.h"
#include "cramfs.h"

static u8 warnmsg[] =
	"\033[150C\033[8D\033[37m\033[1m[ \033[36mNON\033[37m  ]\033[0m";
static u8 okmsg[] =
	"\033[150C\033[8D\033[37m\033[1m[  \033[32mOK\033[37m  ]\033[0m";
static u8 errormsg[] =
	"\033[150C\033[8D\033[37m\033[1m[\033[31mERREUR\033[37m]\033[0m";
static u8 key = 0;

extern wrapper_timer;
extern wrapper_interruption20;
extern u8* initrambloc;

bootparams* allparams;

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

void main(bootparams** params)
{
	cli();
	allparams=params;
	initdriver();
	registerdriver(&vgafonctions);
	registerdriver(&vesafonctions);
	apply_bestdriver();
	changemode(0x1);

	/*  Efface l'ecran   */
	print("\033[2J\r\n\000");
	logo();

	print("\033[37m\033[0m -Initialisation de la memoire virtuelle");
	initgdt(&&next);
next:
	initpaging();
	ok();

	print("\033[37m\033[0m -Remapping de l'arborescence de demarrage et du VESA");
	remap_initram();
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

	printf(" -Initialisation de l'arborescence");
	test_super(initrambloc ,allparams->ramdisksize);
	ok();

      retry:
	sti();
	shell();
}
