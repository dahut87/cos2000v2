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

	print("\033[37m\033[0m -Chargement noyaux\000");
    ok();

	print("\033[37m\033[0m -Initilisation de la memoire (GDT)\000");
    initgdt();
	ok();

	print("\033[37m\033[0m -Initilisation des interruptions (IDT/PIC)\000");
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
    static u8 cmd_regs[]="REGS\000";
    static u8 cmd_gdt[]="GDT\000";
    static u8 cmd_idt[]="IDT\000";
	while (true) {
        print("\r\n# ");
        getstring(&field);
        print("\r\n");
        if (strgetnbitems(&field,' ')<1) continue;
            strgetitem(&field, &item, ' ', 0);
        strtoupper(&item);

        if (strcmp(&item,&cmd_reboot)==0) reboot();
        else if (strcmp(&item,&cmd_mode)==0) setvmode(0x84);
        else if (strcmp(&item,&cmd_clear)==0) fill(0x00);
        else if (strcmp(&item,&cmd_detectcpu)==0) detectcpu();
        else if (strcmp(&item,&cmd_test2d)==0) test2d();
        else if (strcmp(&item,&cmd_regs)==0) dump_regs();
        else if (strcmp(&item,&cmd_gdt)==0) readgdt();
        else if (strcmp(&item,&cmd_idt)==0) readidt();
        else
            printf("Commande inconnue !\r\n\000");
	}
}

/*******************************************************************************/

/* Test les fonctionnalité 2D graphiques */

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
/*******************************************************************************/

/* Lit l'IDT et l'affiche */

void readidt()
{
    u32 index,i=0;
    idtdes* desc;
    struct idtr idtreg;
    sidt(&idtreg);
    printf("Information sur l'IDT\r\nAdresse:%X Limite:%hX",idtreg.base,(u32)idtreg.limite);
    desc=idtreg.base;
    for(index=0;index<idtreg.limite/sizeof(idtdes);index++)
    {
        u32 select=desc[index].select;
        u32 offset=desc[index].offset0_15+(desc[index].offset16_31 << 16);
        u32 type=desc[index].type;
        printf("\r\nInterruption % d Selecteur %hX: offset:%X type:",i++,select,offset,type);
        if (type==INTGATE) print("INTGATE");
        else if (type==TRAPGATE) print("TRAPGATE");
        else if (type==TASKGATE) print("TASKGATE");
        else if (type==CALLGATE) print("CALLGATE");
        else if (type==LDTDES) print("LDTDES");
        else print("inconnu");
       if (i%32==0) waitascii();
    }

}
/*******************************************************************************/

/* Lit les descripteurs GDT et les affiche */

void readgdt()
{
    u32 index;
    gdtdes* desc;
    struct gdtr gdtreg;
    sgdt(&gdtreg);
    printf("Information sur la LGDT\r\nAdresse:%X Limite:%hX",gdtreg.base,(u32)gdtreg.limite);
    desc=gdtreg.base;
    for(index=0;index<=gdtreg.limite/sizeof(gdtdes);index++)
    {
        u32 acces=desc[index].acces;
        if (acces >> 7 == 1) {
            u32 flags=desc[index].flags; 
            u32 limit=desc[index].lim0_15+(desc[index].lim16_19 << 16);
            u32 base=desc[index].base0_15+(desc[index].base16_23 << 16)+(desc[index].base24_31 << 24) ;
            printf("\r\nSelecteur %hX: base:%X limit:%X access:%hX flags:%hX\r\n  -> ",index*sizeof(gdtdes),base,limit,acces,flags);
                if ((acces >> 4) & 1 == 1) 
                    print("Systeme ");
                else {
                    if (acces & 1 == 1)
                        print("Acces ");
                }
                if ((acces >> 3) & 1 == 1) {
                    print("Code.");
                    if ((acces >> 1) & 1 == 1)
                        print("Readable ");
                    if ((acces >> 2) & 1 == 1)
                        print("Conforming ");
                    else
                        print("Normal ");
                }
                else {
                    print("Data.");
                    if ((acces >> 3) & 1 == 1) 
                        print("Down ");
                    else
                        print("up ");
                    if ((acces >> 1) & 1 == 1)
                        print("writeable ");
                }   
                if (flags & 1 == 1)
                    print("Dispo");
                if ((flags >> 2) & 1 == 1)
                    print("32 bits ");
                else
                    print("16 bits "); 
                if ((flags >> 3) & 1 == 1)
                    print("4k ");
                else
                    print("1b ");
                u8 dpl=(acces>>5) & 0b11;  
                printf("DPL:%d",dpl);            
        }           
    }
}

/*******************************************************************************/

/* Detecte et affiche les information sur le CPU */

void detectcpu()
{
    cpuinfo cpu;
    u8 noproc[] = "\033[31mInconnu\033[0m\000";
    strcpy(&noproc, &cpu.detectedname);
	getcpuinfos(&cpu);
	printf("\r\nDetection du processeur\r\033[1m Revision \t:%d\r Modele \t:%d\r Famille \t:%d\r Nom cpuid\t:%s\rJeux d'instruction\t:%s\033[0m\r\n\000",cpu.stepping, cpu.models, cpu.family, &cpu.detectedname,&cpu.techs); 
}
