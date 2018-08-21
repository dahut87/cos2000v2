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
	static u8 field[]="                                                                                \000";
    static u8 item[]="                       \000";
    static u8 cmd_reboot[]="REBOOT\000";
	while (true) {
        print("\r\n# ");
        getstring(&field);
        if (strgetnbitems(&field,' ')<1) continue;
            strgetitem(&field, &item, ' ', 0);
        strtoupper(&item);
        if (strcmp(&item,&cmd_reboot)==0) reboot();
	}

}
