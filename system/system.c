#include "vga.h"
#include "video.h"
#include "interrupts.h"
#include "timer.h"
#include "keyboard.h"
#include "mouse.h"
#include "asm.h"
#include "cpu.h"
#include "string.h"

#include "ansi.c"

static cpuinfo cpu;
static u8 noproc[] = "\033[31mInconnu\033[0m\000";
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

int main(void)
{

	cli();
	setvmode(0x02);
	/*  Efface l'ecran   */
	print("\033[2J\000");
	printf(ansilogo);

	print("\033[37m\033[0m -Initilisation des interruptions\000");
	initidt();
	initpic();
	sti();
	ok();

	print(" -Installation du handler timer (IRQ 0)\000");
	setidt((u32) timer, 0x20, INTGATE, 32);
	enableirq(0);
	ok();

	print(" -Installation du handler clavier (IRQ 1)\000");
	setidt((u32) keyboard, 0x20, INTGATE, 33);
	enableirq(1);
	ok();

	print(" -Installation du handler souris (IRQ12+Cascade IRQ2)\000");
	setidt((u32) mouse, 0x20, INTGATE, 100);
	enableirq(2);
	enableirq(12);
	if (initmouse() != 1)
		warning();
	else
		ok();

	strcpy(&noproc, &cpu.detectedname);
	getcpuinfos(&cpu);


	printf
	    (" -Detection du processeur\r\033[1m Revision \t:%d\r Modele \t:%d\r Famille \t:%d\r Nom cpuid\t:%s\rJeux d'instruction\t:%s\033[0m\000",
	     cpu.stepping, cpu.models, cpu.family, &cpu.detectedname,

	     &cpu.techs);
	ok();
u8 test[]="0101011101b\000";
u8 test2[]="12106567h\000";
u8 test3[]="11A1baA7d\000";
u8 test4[]="11454589d\000";
u8 test5[]="0x11A1b7\000";
u8 test6[]="0x11A1B7h\000";
u8 test7[]="129220\000";
    u8 dest[]="                                                                                       \000";
    u8 src[]="Ceci est un  test pour voir si cela fonctionne correctement\000                                                                                ";
    printf(&src);
    strtoint(&test);
    strtoint(&test2);
    strtoint(&test3);
    strtoint(&test4);
    strtoint(&test5);
    strtoint(&test6);
    strtoint(&test7);
	while (1) {
		key = waitascii();
		putchar(key);
	}

}
