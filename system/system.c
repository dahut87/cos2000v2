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
    static u8 field[]="                                                                                \000";
    static u8 item[]="                       \000";
    static u8 cmd_reboot[]="REBOOT\000";
    static u8 cmd_mode[]="MODE\000";
    static u8 cmd_clear[]="CLEAR\000";
    static u8 cmd_detectcpu[]="DETECTCPU\000";
    static u8 cmd_test2d[]="TEST2D\000";
	while (true) {
        print("\r\n# ");
        getstring(&field);
        if (strgetnbitems(&field,' ')<1) continue;
            strgetitem(&field, &item, ' ', 0);
        strtoupper(&item);
        if (strcmp(&item,&cmd_reboot)==0) reboot();
        if (strcmp(&item,&cmd_mode)==0) setvmode(0x84);
        if (strcmp(&item,&cmd_clear)==0) fill(0x00);
        if (strcmp(&item,&cmd_detectcpu)==0) detectcpu();
        if (strcmp(&item,&cmd_test2d)==0) test2d();
	}
}

void test2d() {
    setvmode(0x89);
    fill(0x00);
    struct vertex2d a,b,c;
    randomize();
    for(int i=0;i<3200;i++)
    {
        a.x=random(0, 800);
        a.y=random(0, 600);
        b.x=random(0, 800);
        b.y=random(0, 600);
        c.x=random(0, 800);
        c.y=random(0, 600);
        trianglefilled(&a,&b,&c,random(0, 16));
        triangle(&a,&b,&c,2);
    }
}

void detectcpu()
{
    cpuinfo cpu;
    u8 noproc[] = "\033[31mInconnu\033[0m\000";
    strcpy(&noproc, &cpu.detectedname);
	getcpuinfos(&cpu);
	printf("\r\nDetection du processeur\r\033[1m Revision \t:%d\r Modele \t:%d\r Famille \t:%d\r Nom cpuid\t:%s\rJeux d'instruction\t:%s\033[0m\r\n\000",cpu.stepping, cpu.models, cpu.family, &cpu.detectedname,&cpu.techs); 
}
