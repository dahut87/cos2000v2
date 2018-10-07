/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
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
	{"REBOOT"    , "", &rebootnow},
	{"CLEAR"     , "", &clear},
	{"MODE"      , "", &mode},
	{"DETECTCPU" , "", &detectcpu},
	{"TEST2D"    , "", &test2d},
	{"REGS"      , "", &regs},
	{"GDT"       , "", &readgdt},
	{"IDT"       , "", &readidt},
	{"INFO"      , "", &info},
    {"PGFAULTW" , "", &pgfaultw},
    {"PGFAULTR" , "", &pgfaultr},
    {"DIVZERR" , "", &divzerr},
    {"INVALIDOP","", &invalidop},
    {"INT3" , "", &int3},
    {"GENERALFAULT" , "", &generalfault},
    {"SEGFAULT","", &segfault},
    {"BREAKPOINT","", &breakpoint},
    {"TESTING","", &testing}
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

void testing(void)
{
    print("Fonction de test !\r\n");
}

/*******************************************************************************/
/* Génère un breakpoint */
int breakpoint()
{
	print("Creation d'un breakpoint !\r\n");
    asm("movl %[address],%%dr0 \n \
         movl $0x00000003, %%eax\n \
         movl %%eax, %%dr7"::[address] "a" (&testing):);
}

/*******************************************************************************/
/* Génère une interruption 3 */
int int3()
{
	print("Creation d'une erreur interruption 3 !\r\n");
    asm("int $0x3");
}

/*******************************************************************************/
/* Génère une erreur general fault */
int generalfault()
{
	print("Creation d'une erreur general fault !\r\n");
    asm("mov $0x666, %ax; ltr %ax");
}

/*******************************************************************************/
/* Génère une erreur double fault */
int segfault()
{
	print("Creation d'une erreur double fault !\r\n");
	setidt(&segfault, SEL_KERNEL_CODE, INTGATE, 104); 
    asm("int $0x68");
}


/*******************************************************************************/
/* Génère une erreur d'opcode invalide */
int invalidop()
{
	print("Creation d'une erreur d'opcode invalide !\r\n");
    asm("mov %cr7, %eax");
}

/*******************************************************************************/
/* Génère une erreur de division par 0 */
int divzerr()
{
	print("Creation d'une erreur de division par 0 !\r\n");
    asm("movl $0x0,%ecx; divl %ecx");
}

/*******************************************************************************/
/* Génère une erreur de page à l'adresse 0xE0000000 */
int pgfaultw()
{
	print("Creation d'une erreur de page !\r\n");
    asm("movl $0x66666666,(0xE0000000)");
}

/*******************************************************************************/
/* Génère une erreur de page à l'adresse 0xD0000000 */
int pgfaultr()
{
	print("Creation d'une erreur de page !\r\n");
    asm("movl (0xD0000000),%eax");
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
    save_stack dump;
    dump_cpu(&dump);
    show_cpu(&dump);
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
	printf("Information sur l'IDT\r\nAdresse:%X Limite:%hX\r\n", idtreg.base,
	       (u32) idtreg.limite);
	desc = idtreg.base;
	for (index = 0; index < idtreg.limite / sizeof(idtdes); index++) {
		u32 select = desc[index].select;
		u32 offset =
		    desc[index].offset0_15 + (desc[index].offset16_31 << 16);
		u32 type = desc[index].type & 0x0F00;
        u8 *typestr1, *typestr2;
        if (i>=32 & i<=39)
            typestr1="IRQ master";
        else if (i>=96 & i<=103)
            typestr1="IRQ slave ";
        else if (i<19)
            typestr1="EXCEPTION ";    
        else
            typestr1="INTERRUPT ";
		if (type == INTGATE)
			typestr2="INTGATE";
		else if (type == TRAPGATE)
			typestr2="TRAPGATE";
		else if (type == TASKGATE)
			typestr2="TASKGATE";
		else if (type == CALLGATE)
			typestr2="CALLGATE";
		else if (type == LDTDES)
			typestr2="LDTDES";
		else
			print("inconnu");
		printf("%s % hu %hY:%Y - %s\r\n", typestr1, i++, select, offset, typestr2);

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
	struct gdtr gdtreg;
	sgdt(&gdtreg);
	printf("Information sur la GDT\r\nAdresse:%X Limite:%hX\r\n", gdtreg.base, gdtreg.limite);
	for (index = 0; index < gdtreg.limite ; index+=sizeof(gdtdes)) {
        if (!isdesvalid(index))
            printf("\033[31m");
        printf("SEL =%hY %Y %Y DPL=%d %cS%d [%c%c%c] %h ub\033[0m\r\n",index,getdesbase(index),getdeslimit(index),getdesdpl(index),getdestype(index),getdessize(index),getdesbit3(index),getdesbit2(index),getdesbit1(index),getdesalign(index));
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

/*******************************************************************************/
