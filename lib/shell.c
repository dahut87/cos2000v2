#include "vga.h"
#include "video.h"
#include "interrupts.h"
#include "asm.h"
#include "cpu.h"
#include "string.h"
#include "2d.h"
#include "gdt.h"
#include "shell.h"
#include "multiboot2.h"

static command commands[] = {
	{"REBOOT", "", &rebootnow},
	{"CLEAR", "", &clear},
	{"MODE", "", &mode},
	{"DETECTCPU", "", &detectcpu},
	{"TEST2D", "", &test2d},
	{"REGS", "", &dump_regs},
	{"GDT", "", &readgdt},
	{"IDT", "", &readidt},
	{"INFO", "", &info}
};

/*******************************************************************************/

/* Shell, traite les commandes */

void shell()
{
	static u8 field[] =
	    "                                                                                \000";
	static u8 item[] = "                       \000";
	int i;
	bool found;
	while (true) {
		print("\r\n# ");
		getstring(&field);
		print("\r\n");
		if (strgetnbitems(&field, ' ') < 1)
			continue;
		strgetitem(&field, &item, ' ', 0);
		strtoupper(&item);
		found = false;
		for (i = 0; i < sizeof(commands); i++) {
			if (strcmp(&item, &commands[i].name) == 0) {
				(*commands[i].function) ();
				found = true;
				break;
			}
		}
		if (!found)
			printf("Commande inconnue !\r\n\000");
	}
}

/*******************************************************************************/

/* Information sur le démarrage */

int info()
{
    getbootinfo();
    return 0;
}

/*******************************************************************************/

/* Affiche les registres */

int regs()
{
	dump_regs();
	return 0;
}

/*******************************************************************************/

/* Change le mode */

int mode()
{
	setvmode(0x84);
	return 0;
}

/*******************************************************************************/

/* Efface l'écran */

int clear()
{
	fill(0x00);
	gotoscr(0, 0);
	return 0;
}

/*******************************************************************************/

/* Redemarre */

int rebootnow()
{
	print("<Appuyer sur une touche pour redemarrer>");
	waitascii();
	reboot();
	return 0;
}

/*******************************************************************************/

/* Test les fonctionnalité 2D graphiques */

int test2d()
{
	setvmode(0x89);
	fill(0x00);
	struct vertex2d a, b, c;
	randomize();
	for (int i = 0; i < 3200; i++) {
		a.x = random(0, 800);
		a.y = random(0, 600);
		b.x = random(0, 800);
		b.y = random(0, 600);
		c.x = random(0, 800);
		c.y = random(0, 600);
		trianglefilled(&a, &b, &c, random(0, 16));
		triangle(&a, &b, &c, 2);
	}
	return 0;
}

/*******************************************************************************/

/* Lit l'IDT et l'affiche */

int readidt()
{
	u32 index, i = 0;
	idtdes *desc;
	struct idtr idtreg;
	sidt(&idtreg);
	printf("Information sur l'IDT\r\nAdresse:%X Limite:%hX", idtreg.base,
	       (u32) idtreg.limite);
	desc = idtreg.base;
	for (index = 0; index < idtreg.limite / sizeof(idtdes); index++) {
		u32 select = desc[index].select;
		u32 offset =
		    desc[index].offset0_15 + (desc[index].offset16_31 << 16);
		u32 type = desc[index].type;
		printf("\r\nInterruption % d Selecteur %hX: offset:%X type:",
		       i++, select, offset, type);
		if (type == INTGATE)
			print("INTGATE");
		else if (type == TRAPGATE)
			print("TRAPGATE");
		else if (type == TASKGATE)
			print("TASKGATE");
		else if (type == CALLGATE)
			print("CALLGATE");
		else if (type == LDTDES)
			print("LDTDES");
		else
			print("inconnu");
		if (i % 32 == 0) {
			print("\r\n<Appuyez sur une touche>\r\n");
			waitascii();
		}
	}
	return 0;
}

/*******************************************************************************/

/* Lit les descripteurs GDT et les affiche */

int readgdt()
{
	u32 index;
	gdtdes *desc;
	struct gdtr gdtreg;
	sgdt(&gdtreg);
	printf("Information sur la LGDT\r\nAdresse:%X Limite:%hX", gdtreg.base,
	       (u32) gdtreg.limite);
	desc = gdtreg.base;
	for (index = 0; index < gdtreg.limite / sizeof(gdtdes); index++) {
		u32 acces = desc[index].acces;
		if (acces >> 7 == 1) {
			u32 flags = desc[index].flags;
			u32 limit =
			    desc[index].lim0_15 + (desc[index].lim16_19 << 16);
			u32 base =
			    desc[index].base0_15 +
			    (desc[index].base16_23 << 16) +
			    (desc[index].base24_31 << 24);
			printf
			    ("\r\nSelecteur %hX: base:%X limit:%X access:%hX flags:%hX\r\n  -> ",
			     index * sizeof(gdtdes), base, limit, acces, flags);
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
			} else {
				print("Data.");
				if ((acces >> 3) & 1 == 1)
					print("Down ");
				else
					print("up ");
				if ((acces >> 1) & 1 == 1)
					print("writeable ");
			}
			if (flags & 1 == 1)
				print("Dispo ");
			if ((flags >> 2) & 1 == 1)
				print("32 bits ");
			else
				print("16 bits ");
			if ((flags >> 3) & 1 == 1)
				print("4k ");
			else
				print("1b ");
			u8 dpl = (acces >> 5) & 0b11;
			printf("DPL:%d", dpl);
		}
	}
	return 0;
}

/*******************************************************************************/

/* Detecte et affiche les information sur le CPU */

int detectcpu()
{
	cpuinfo cpu;
	u8 noproc[] = "\033[31mInconnu\033[0m\000";
	strcpy(&noproc, &cpu.detectedname);
	getcpuinfos(&cpu);
	printf
	    ("\r\nDetection du processeur\r\033[1m Revision \t:%d\r Modele \t:%d\r Famille \t:%d\r Nom cpuid\t:%s\rJeux d'instruction\t:%s\033[0m\r\n\000",
	     cpu.stepping, cpu.models, cpu.family, &cpu.detectedname,
	     &cpu.techs);
	return 0;
}
