#include "vga.h"
#include "video.h"
#include "interrupts.h"
#include "timer.h"
#include "keyboard.h"
#include "mouse.h"
#include "asm.h"
#include "cpu.h"
#include "string.h"
#include "2d.h"
#include "ansi.c"
#include "gdt.h"
#include "shell.h"
#include "syscall.h"
#include "multiboot2.h"

static u8 warnmsg[] =
    "\033[99C\033[8D\033[37m\033[1m[ \033[36mNON\033[37m  ]\033[0m\000";
static u8 okmsg[] =
    "\033[99C\033[8D\033[37m\033[1m[  \033[32mOK\033[37m  ]\033[0m\000";
static u8 errormsg[] =
    "\033[99C\033[8D\033[37m\033[1m[\033[31mERREUR\033[37m]\033[0m\000";
static u8 key = 0;

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

int main(unsigned long magic, unsigned long addr)
{
	cli();
	setvmode(0x02);
	/*  Efface l'ecran   */
    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
         {
           printf ("Nombre magic inconnu: 0x%x\n", (u32) magic);
           return;
         }
	print("\033[2J\000");
	printf(ansilogo);

	print("\033[37m\033[0m -Chargement noyaux\000");
    ok();

	print("\033[37m\033[0m -Initilisation de la memoire (GDT)\000");
    initgdt(&&next);
next:
	ok();

	print("\033[37m\033[0m -Initilisation des taches (TSR)\000");
    inittr();
	ok();

	print("\033[37m\033[0m -Initilisation des interruptions (IDT/PIC)\000");
	initidt();
	initpic();
	sti();
	ok();

	print(" -Installation du handler timer (IRQ 0)\000");
	setidt((u32) timer, SEL_KERNEL_CODE, INTGATE, 32);
	enableirq(0);
	ok();

	print(" -Installation du handler clavier (IRQ 1)\000");
	setidt((u32) keyboard, SEL_KERNEL_CODE, INTGATE, 33);
	enableirq(1);
	ok();

	print(" -Installation du handler souris (IRQ12+Cascade IRQ2)\000");
	setidt((u32) mouse, SEL_KERNEL_CODE, INTGATE, 100);
	enableirq(2);
	enableirq(12);
	if (initmouse() != 1)
		warning();
	else
		ok();

	print(" -Installation des appels systemes utilisateur\000");
	initsyscall();
	ok();

    shell();
}
