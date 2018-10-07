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
#include "math.h"

static command commands[] = {
	{"reboot"    , "", &rebootnow},
	{"clear"     , "", &clear},
	{"mode"      , "", &mode},
	{"detectcpu" , "", &detectcpu},
	{"test2d"    , "", &test2d},
	{"regs"      , "", &regs},
	{"gdt"       , "", &readgdt},
	{"idt"       , "", &readidt},
	{"info"      , "", &info},
	{"err"      , "", &err},
	{"test"      , "", &test},
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
		strtolower(&item);
		found = false;
		for (i = 0; i < sizeof(commands); i++) {
			if (strcmp(&item, &commands[i].name) == 0) {
				(*commands[i].function) (&field);
				found = true;
				break;
			}
		}
		if (!found)
			printf("Commande inconnue !\r\n\000");
	}
}

int test(void)
{
    print("Fonction de test !\r\n");
}

/*******************************************************************************/
/* Génère des exceptions */

int err(u8* commandline)
{
	u8 arg[] = "       \000";
    u32 argint;
	if (strgetnbitems(commandline, ' ') < 2)
    {
		print("Syntaxe de la commande ERR\r\nerr \33[32mexception\r\n\r\n exception\33[0m - code de l'exception\r\n");
        return;
    }
	strgetitem(commandline, &arg, ' ', 1);
    argint=strtoint(&arg);
    switch (argint)
    {
    case 0:
        print("Creation d'une erreur de division par 0 !\r\n");
        asm("movl $0x0,%ecx; divl %ecx");
        break;
    case 1:
        print("Creation d'un breakpoint !\r\n");
        asm("movl %[address],%%dr0 \n \
         movl $0x00000003, %%eax\n \
         movl %%eax, %%dr7"::[address] "a" (&test):);
        break;
    case 2:
        print("NON GERE!\r\n");
        break;
    case 3:
        print("Creation d'une erreur interruption 3 !\r\n");
        asm("int $0x3");
        break;
    case 4:
        print("NON GERE!\r\n");
        break;
    case 5:
        print("NON GERE!\r\n");
        break;
    case 6:
 	    print("Creation d'une erreur d'opcode invalide !\r\n");
        asm("mov %cr7, %eax");       
        break;
    case 7:
         print("NON GERE!\r\n");       
        break;
    case 8:
        print("NON GERE!\r\n");        
        break;
    case 9:
         print("NON GERE!\r\n");       
        break;
    case 10:
        print("NON GERE!\r\n");        
        break;
    case 11:
        print("Creation d'une erreur segment non present !\r\n");
	    setidt(&err, SEL_KERNEL_CODE, INTGATE, 104); 
        asm("int $0x68");
        break;   
    case 12:
        print("NON GERE!\r\n");          
        break;   
    case 13:
        print("Creation d'une erreur general fault !\r\n");
        asm("mov $0x666, %ax; ltr %ax");
        break;   
    case 14:
        if (random(0, 100)>50) {
        	print("Creation d'une erreur de page en ecriture !\r\n");
            asm("movl $0x66666666,(0xE0000000)");
        }
        else {
	        print("Creation d'une erreur de page en lecture !\r\n");
            asm("movl (0xD0000000),%eax");
        }
        break;  
    case 15:
        print("NON GERE!\r\n");          
        break;
    case 16:
        print("NON GERE!\r\n");          
        break;  
    case 17:
        print("NON GERE!\r\n");          
        break;  
    case 18:
        print("NON GERE!\r\n");          
        break;  
    default:
        print("Exception qui n'existe pas !!!\r\n");  
        break;
    }   
	return 0;
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
int mode(u8* commandline)
{
	u8 arg[] = "       \000";
    u32 argint;
	if (strgetnbitems(commandline, ' ') < 2)
    {
		print("Syntaxe de la commande MODE\r\nmode \33[32mmodevideo\r\n\r\n modevideo\33[0m - mode video a initialiser (>0x80 = graphique)\r\n");
        return;
    }
	strgetitem(commandline, &arg, ' ', 1);
    argint=strtoint(&arg);
	setvmode(argint);
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
