/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "vga.h"
#include "3d.h"
#include "matrix.h"
#include "video.h"
#include "interrupts.h"
#include "asm.h"
#include "cpu.h"
#include "string.h"
#include "gdt.h"
#include "shell.h"
#include "math.h"
#include "debug.h"
#include "VGA/ansi.c"
#include "3D/sphere.c"
#include "3D/man.c"
#include "memory.h"
#include "syscall.h"
#include "process.h"

static command commands[] = {
	{"reboot", "", &rebootnow},
	{"clear", "", &clear},
	{"mode", "", &mode},
	{"detectcpu", "", &detectcpu},
	{"test2d", "", &test2d},
	{"regs", "", &showregs},
	{"gdt", "", &showgdt},
	{"idt", "", &showidt},
	{"info", "", &showinfo},
	{"err", "", &err},
	{"test", "", &test},
	{"view", "", &view},
	{"disasm", "", &disas},
	{"bpset", "", &bpset},
	{"bpclr", "", &bpclr},
	{"help", "", &help},
	{"logo", "", &logo},
	{"font", "", &sfont},
	{"test3d", "", &test3d},
	{"detectpci", "", &detectpci},
	{"mem", "", &showmem},
	{"testmem", "", &testmem},
	{"testtask", "", &testtask},
	{"ps", "", &ps},
};

/*******************************************************************************/
/* Shell, traite les commandes */

void shell()
{
	static u8 field[] =
		"                                                                                \000";
	static u8 item[] = "                       \000";
	int     i;
	bool    found;
	while (true)
	{
		print("\r\n# ");
		getstring(&field);
		print("\r\n");
		if (strgetnbitems(&field, ' ') < 1)
			continue;
		strgetitem(&field, &item, ' ', 0);
		strtolower(&item);
		found = false;
		for (i = 0; i < sizeof(commands) / sizeof(commands[0]);
		     i++)
		{
			if (strcmp(&item, &commands[i].name) == 0)
			{
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
	return;
}

/*******************************************************************************/
/* Afiche les processus */

static u8* processstatus[] = {"LIBRE","PRET ","EXEC.","PAUSE"};
static u8* taskstatus[] = {"PRET ","EXEC.","PAUSE"};

/*******************************************************************************/
/* Affiche les tâches et processus */

int ps()
{
	print("*** Processus en memoire\r\n|   PID  |  Parent|Status|K|P.|Directo.|Pages...\r\n");
	process* aprocess=findprocess((pid_t)1);	
	while(true)
	{
		printf("|%Y|%Y| %s|%c|%hh u|%Y|",(u32)aprocess->pid,(u32)aprocess->parent,processstatus[aprocess->status],(aprocess->iskernel?'X':' '),aprocess->priority,(u32)aprocess->pdd);
		if (aprocess->pdd!=NULL) virtual_pd_show(aprocess->pdd);
		print("\r\n");
		aprocess=getnextprocess(aprocess,PROCESS_STATUS_ALL);
		if (aprocess==NULL || aprocess->pid==(pid_t)1) break; 
	}
	print("\r\n\r\n*** Taches en memoire\r\n|   TID  |   PID  |Status| CS |   EIP  | SS |   ESP  |\r\n");
	aprocess=findprocess((pid_t)1);
	task* atask;
	while(true)
	{	
		atask=findfirsttask(aprocess);
		while(true)
		{
			printf("|%Y|%Y| %s|%hY|%Y|%hY|%Y|\r\n",(u32)atask->tid.number,(u32)atask->tid.pid,taskstatus[atask->status],atask->dump.cs,atask->dump.eip,atask->dump.ss,atask->dump.esp);
			atask=getnexttask(atask,TASK_STATUS_ALL);
			if (atask==NULL || (atask->tid.number==1 && atask->tid.pid==1)) break; 
		}
		aprocess=getnextprocess(aprocess,PROCESS_STATUS_ALL);
		if (aprocess==NULL || aprocess->pid==(pid_t)1) break; 
	}
}

/*******************************************************************************/
/* Test l'usage de création de tâche */

int testtask()
{
	print("*** Creation d'une tache\r\n");
	pid_t pid;
	pid = createprocess(NULL, false);
	runprocess(pid);
	pid = createprocess(NULL, false);
	runprocess(pid);
}

/*******************************************************************************/
/* Test la memoire */

int testmem()
{
	u8     *test;
	print("**** AVANT ALLOCATION\r\n");
	showmem();
	test = vmalloc(150 * 1024 * 1024);	/* 10 pages */
	print("**** APRES ALLOCATION\r\n");
	showmem();
	vfree(test);
	print("**** APRES LIBERATION\r\n");
	showmem();
}

/*******************************************************************************/
/* Affiche des informations sur la mémoire */

int showmem()
{
	u32     libre = getmemoryfree();
	u32     total = physical_getmemorysize();
	printf("Memoire physique (TOTAL)\r\n -libre \33[40D\33[15C%H (%.2f%%)\r\n -occupee \33[40D\33[15C%H\r\n -total \33[40D\33[15C%H\r\n\r\n", libre, ((float) libre / total * 100.0f), total - libre, total);
	printf("Memoire HEAP (NOYAU) - % u blocs\r\n -libre \33[40D\33[15C%H\r\n -occupee \33[40D\33[15C%H\r\n -allouables \33[40D\33[15C%H\r\n\r\n", getmallocnb(), getmallocfree(), getmallocused(), getmallocnonallocated());
	printf("Plan de memoire (NOYAU)\r\n -IDT \33[40D\33[15C%X\r\n -GDT \33[40D\33[15C%X\r\n -PGD \33[40D\33[15C%X\r\n -STACK \33[40D\33[15C%X\r\n -CODE \33[40D\33[15C%X\r\n -PAGES \33[40D\33[15C%X\r\n -HEAP \33[40D\33[15C%X\r\n -VESAFB \33[40D\33[15C%X\r\n\r\n", IDT_ADDR, GDT_ADDR, KERNEL_PD_ADDR, KERNEL_STACK_ADDR, KERNEL_CODE_ADDR, KERNEL_PAGES, KERNEL_HEAP, VESA_FBMEM);
	printf("Memoire Virtuelle (NOYAU)\r\n -pages libres \33[40D\33[16C% u\r\n -pages occupees \33[40D\33[16C% u\r\n", virtual_getpagesfree(), virtual_getpagesused());
	return;
}

/*******************************************************************************/
/* Affiche les périphériques PCI */

int detectpci()
{
	scanPCI();
}

/*******************************************************************************/
/* Change la police courante */

int sfont(u8 * commandline)
{
	if (strgetnbitems(commandline, ' ') < 2)
	{
		print("Syntaxe de la commande FONT\r\nfont \33[32mpolice\r\n\r\n \33[32mpolice\33[0m\33[0m\33[25D\33[10C - \33[Nom de la police de caractere\r\n");
		return;
	}
	setfont(strgetpointeritem(commandline, ' ', 1));
}

/*******************************************************************************/
/* Affiche le logo */

int logo()
{
	print(ansilogo);
	return;
}

/*******************************************************************************/
/* Renvoie les commandes disponibles */

int help()
{
	print("Commandes disponibles :\r\n\r\n");
	for (u32 i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
	{
		printf("%s \r\n", &commands[i].name);
	}
	return 0;
}
/*******************************************************************************/
/* Met un breakpoint */

int bpset(u8 * commandline)
{
	u8      arg[] = "       \000";
	u8     *numero;
	u8     *pointer;
	u8      type = DBG_EXEC;
	if (strgetnbitems(commandline, ' ') < 3)
	{
		print("Syntaxe de la commande BPSET\r\nbpset \33[32mnumero address [type]\r\n\r\n \33[32mnumero\33[0m\33[0m\33[25D\33[10C - numero du breakpoint (0-3)\r\n \33[32madresse\33[0m\33[25D\33[10C - adresse du breakpoint\r\n \33[32mtype\33[0m\33[25D\33[10C - type de breakpoint (0-3)\r\n");
		return;
	}
	strgetitem(commandline, &arg, ' ', 1);
	numero = strtoint(&arg);
	if (numero > 3)
	{
		print("numero incorrect");
		return;
	}
	strgetitem(commandline, &arg, ' ', 2);
	pointer = strtoint(&arg);
	if (strgetnbitems(commandline, ' ') == 4)
	{
		strgetitem(commandline, &arg, ' ', 3);
		type = strtoint(&arg);
	}
	if (type > DBG_READWRITE)
	{
		print("type incorrect");
		return;
	}
	setdebugreg(numero, pointer, type);
}

/*******************************************************************************/
/* Retire un breakpoint */

int bpclr(u8 * commandline)
{
	u8      arg[] = "       \000";
	u8     *numero;

	if (strgetnbitems(commandline, ' ') < 2)
	{
		print("Syntaxe de la commande BPCLR\r\nbpclr \33[32mnumero\r\n\r\n \33[32mnumero\33[0m\33[0m\33[25D\33[10C - numero du breakpoint (0-3)\r\n");
		return;
	}
	strgetitem(commandline, &arg, ' ', 1);
	numero = strtoint(&arg);
	if (numero > 3)
	{
		print("numero incorrect");
		return;
	}
	setdebugreg(numero, 0x0, DBG_CLEAR);
}

/*******************************************************************************/
/* Desassemble une zone de mémoire donnée */

int disas(u8 * commandline)
{
	u8      arg[] = "       \000";
	u8     *size;
	u8     *pointer;

	if (strgetnbitems(commandline, ' ') < 3)
	{
		print("Syntaxe de la commande DISASM\r\ndisasm \33[32madresse taille\r\n\r\n \33[32madresse\33[0m\33[0m\33[25D\33[10C - Adresse a visualiser\r\n \33[32mtaille\33[0m\33[25D\33[10C - nombre d'octets a desassembler <256\r\n");
		return;
	}
	strgetitem(commandline, &arg, ' ', 1);
	pointer = strtoint(&arg);
	size = pointer;
	strgetitem(commandline, &arg, ' ', 2);
	size += strtoint(&arg);
	while (pointer < size)
	{
		pointer += disasm(pointer, NULL, true);
	}
}

/*******************************************************************************/
/* Génère des exceptions */

int view(u8 * commandline)
{
	u8      arg[] = "       \000";
	u32     address;
	u8      size;
	u8     *pointerb;
	u16    *pointerw;
	u32    *pointerd;
	u8      format;
	u8      nbligne;
	if (strgetnbitems(commandline, ' ') < 3)
	{
		print("Syntaxe de la commande VIEW\r\nview \33[32madresse taille [size] [nbligne]\r\n\r\n \33[32madresse\33[0m\33[0m\33[25D\33[10C - Adresse a visualiser\r\n \33[32mtaille\33[0m\33[25D\33[10C - nombre d'octets a visualiser <256\r\n \33[32mformat\33[0m\33[25D\33[10C - c (ascii) b (octet) w (mot) d (double mot)\r\n \33[32mnbligne\33[0m\33[25D\33[10C - nombre d'octets a visualiser par ligne\r\n");
		return;
	}
	strgetitem(commandline, &arg, ' ', 1);
	address = strtoint(&arg);
	strgetitem(commandline, &arg, ' ', 2);
	size = strtoint(&arg);
	if (strgetnbitems(commandline, ' ') < 4)
		format = 'b';
	else
	{
		strgetitem(commandline, &arg, ' ', 3);
		format = arg[0];
	}
	switch (format)
	{
		case 'c':
			pointerb = address;
			nbligne = 12;
			break;
		case 'b':
			pointerb = address;
			nbligne = 22;
			break;
		case 'w':
			pointerw = address;
			nbligne = 13;
			break;
		case 'd':
			pointerd = address;
			nbligne = 7;
			break;
	}
	if (strgetnbitems(commandline, ' ') == 5)
	{
		strgetitem(commandline, &arg, ' ', 4);
		nbligne = strtoint(&arg);
	}
	printf("Adresse %Y - % hhu", address, size);
	for (u32 i = 0; i < size; i++)
		switch (format)
		{
			case 'c':
				if (i % nbligne == 0)
					printf("\r\n:%Y - ", pointerb);
				printf("%hhY \33[40C%c\33[41D",
				       *(pointerb), *(pointerb++));
				break;
			case 'b':
				if (i % nbligne == 0)
					printf("\r\n:%Y - ", pointerb);
				printf("%hhY ", *(pointerb++));
				break;
			case 'w':
				if (i % nbligne == 0)
					printf("\r\n:%Y - ", pointerw);
				printf("%hY ", *(pointerw++));
				break;
			case 'd':
				if (i % nbligne == 0)
					printf("\r\n:%Y - ", pointerd);
				printf("%Y ", *(pointerd++));
				break;
		}
}
/*******************************************************************************/
/* Génère des exceptions */

int err(u8 * commandline)
{
	u8      arg[] = "       \000";
	u32     argint;
	if (strgetnbitems(commandline, ' ') < 2)
	{
		print("Syntaxe de la commande ERR\r\nerr \33[32mexception\r\n\r\n exception\33[0m\33[25D\33[10C - code de l'exception\r\n");
		return;
	}
	strgetitem(commandline, &arg, ' ', 1);
	argint = strtoint(&arg);
	switch (argint)
	{
		case 0:
			print("Creation d'une erreur de division par 0 !\r\n");
			asm("movl $0x0,%ecx; divl %ecx");
			break;
		case 1:
			print("Creation d'un breakpoint !\r\n");
			setdebugreg(0, &test, DBG_EXEC);
			test();
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
			if (random(0, 100) > 50)
			{
				print("Creation d'une erreur de page en ecriture !\r\n");
				asm("movl $0x66666666,(0xE0000000)");
			}
			else
			{
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
int showinfo()
{
	return 0;
}

/*******************************************************************************/
/* Affiche les registres */

int showregs()
{
	int20;
	return 0;
}

/*******************************************************************************/
/* Change le mode */
int mode(u8 * commandline)
{
	u8      arg[] = "       \000";
	u32     argint;
	if (strgetnbitems(commandline, ' ') < 2)
	{
		print("Syntaxe de la commande MODE\r\nmode \33[32mmodevideo\r\n\r\n modevideo\33[0m\33[25D\33[10C - mode video a initialiser (>0x80 = graphique)\r\n");
		return;
	}
	strgetitem(commandline, &arg, ' ', 1);
	argint = strtoint(&arg);
	changemode(argint);
	return 0;
}

/*******************************************************************************/
/* Efface l'écran */

int clear()
{
	clearscreen();
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
/* Teste les fonctionnalités 3D */

int test3d()
{
	videoinfos *vinfo = getvideo_info();
	if (!vinfo->isgraphic)
	{
		print("Mode graphique necessaire afin de lancer ce programme\r\n");
		return 1;
	}
	model3d model;
	float   factor = 100.0f;
	type3D  type = TYPE3D_POINTS;
	matrix44 rotatex, rotatey, rotatez, mrotatex, mrotatey, mrotatez,
		identity;
	matrix44_homogen(&identity);
	matrix44_rotation_x(0.1f, &rotatex);
	matrix44_rotation_y(0.1f, &rotatey);
	matrix44_rotation_z(0.1f, &rotatez);
	matrix44_rotation_x(-0.1f, &mrotatex);
	matrix44_rotation_y(-0.1f, &mrotatey);
	matrix44_rotation_z(-0.1f, &mrotatez);
	matrix44 *transformation = &identity;
	vector4 origin = { 0.0f, 0.0f, 0.0f, 0.0f };
	vector4 cubeorigin = { 0.0f, 0.0f, 0.0f, 0.0f };
	origin.x = vinfo->currentwidth / 2.0f;
	origin.y = vinfo->currentheight / 2.0f;
	origin.z = 70.0f;
	cube(&model, &cubeorigin, 35.0f);
	u8      achar = ' ';
	u8      i;
	while (achar != 'q' && achar != 'Q')
	{
		clearscreen();
		show3dmodel(&model, transformation, &origin, factor, type);
		achar = waitascii();
		switch (achar)
		{
			case '1':
				load3ds(&man, sizeof(man), &model);
				break;
			case '2':
				load3ds(&sphere, sizeof(sphere), &model);
				break;
			case 17:
				transformation = &rotatex;
				break;
			case 18:
				transformation = &mrotatex;
				break;
			case 19:
				transformation = &rotatey;
				break;
			case 20:
				transformation = &mrotatey;
				break;
			case 2:
				transformation = &rotatez;
				break;
			case 3:
				transformation = &mrotatez;
				break;
			case '-':
				factor -= 10.0;
				break;
			case '+':
				factor += 10.0;
				break;
			case '*':
				switch (type)
				{
					case TYPE3D_POINTS:
						type = TYPE3D_LINES;
						break;
					case TYPE3D_LINES:
						type = TYPE3D_POINTS;
						break;
				}
				break;
		}

	}
	return 0;
}

/*******************************************************************************/
/* Teste les fonctionnalités 2D graphiques */

int test2d()
{
	videoinfos *vinfo = getvideo_info();
	if (!vinfo->isgraphic)
	{
		print("Mode graphique necessaire afin de lancer ce programme\r\n");
		return 1;
	}
	struct vertex2d a, b, c;
	randomize();
	u32     color;
	for (int i = 0; i < 2000; i++)
	{
		a.x = random(0, vinfo->currentwidth);
		a.y = random(0, vinfo->currentheight);
		b.x = random(0, vinfo->currentwidth);
		b.y = random(0, vinfo->currentheight);
		if (vinfo->currentdepth > 24)
			color = egatorgb(random(0, 16));
		else if (vinfo->currentdepth == 8)
			color = random(0, 63);
		else
			color = random(0, 16);
		v_line(&a, &b, color);
	}
	waitascii();
	for (int i = 0; i < 2000; i++)
	{
		a.x = random(0, vinfo->currentwidth);
		a.y = random(0, vinfo->currentheight);
		b.x = random(0, vinfo->currentwidth);
		b.y = random(0, vinfo->currentheight);
		c.x = random(0, vinfo->currentwidth);
		c.y = random(0, vinfo->currentheight);
		if (vinfo->currentdepth > 24)
			color = egatorgb(random(0, 16));
		else if (vinfo->currentdepth == 8)
			color = random(0, 63);
		else
			color = random(0, 16);
		trianglefilled(&a, &b, &c, color);
		triangle(&a, &b, &c, egatorgb(4));
	}
	return 0;
}

/*******************************************************************************/
/* Lit l'IDT et l'affiche */

int showidt()
{
	u32     index, i = 0;
	idtdes *desc;
	struct idtr idtreg;
	sidt(idtreg);
	printf("Information sur l'IDT\r\nAdresse:%X Limite:%hX\r\n",
	       idtreg.base, (u32) idtreg.limite);
	desc = idtreg.base;
	for (index = 0; index < idtreg.limite / sizeof(idtdes); index++)
	{
		u32     select = desc[index].select;
		u32     offset =
			desc[index].offset0_15 +
			(desc[index].offset16_31 << 16);
		u32     type = desc[index].type & 0x0F00;
		u8     *typestr1, *typestr2;
		if (i >= 32 & i <= 39)
			typestr1 = "IRQ master";
		else if (i >= 96 & i <= 103)
			typestr1 = "IRQ slave ";
		else if (i < 19)
			typestr1 = "EXCEPTION ";
		else
			typestr1 = "INTERRUPT ";
		if (type == INTGATE)
			typestr2 = "INTGATE";
		else if (type == TRAPGATE)
			typestr2 = "TRAPGATE";
		else if (type == TASKGATE)
			typestr2 = "TASKGATE";
		else if (type == CALLGATE)
			typestr2 = "CALLGATE";
		else if (type == LDTDES)
			typestr2 = "LDTDES";
		else
			print("inconnu");
		printf("%s % hu %hY:%Y - %s\r\n", typestr1, i++, select,
		       offset, typestr2);

		if (i % 32 == 0)
		{
			print("\r\n<Appuyez sur une touche>\r\n");
			waitascii();
		}
	}
	return 0;
}

/*******************************************************************************/
/* Lit les descripteurs GDT et les affiche */

int showgdt()
{
	u32     index;
	struct gdtr gdtreg;
	sgdt(gdtreg);
	printf("Information sur la GDT\r\nAdresse:%X Limite:%hX\r\n",
	       gdtreg.base, gdtreg.limite);
	for (index = 0; index < gdtreg.limite; index += sizeof(gdtdes))
	{
		if (!isdesvalid(index))
			printf("\033[31m");
		printf("SEL =%hY %Y %Y DPL=%d %cS%d [%c%c%c] %h ub\033[0m\r\n", index, getdesbase(index), getdeslimit(index), getdesdpl(index), getdestype(index), getdessize(index), getdesbit3(index), getdesbit2(index), getdesbit1(index), getdesalign(index));
	}
	return 0;
}

/*******************************************************************************/
/* Detecte et affiche les information sur le CPU */

int detectcpu()
{
	cpuinfo cpu;
	u8      noproc[] = "\033[31mInconnu\033[0m\000";
	strcpy(&noproc, &cpu.detectedname);
	getcpuinfos(&cpu);
	printf("\r\nDetection du processeur\r\033[1m Revision \t:%d\r Modele \t:%d\r Famille \t:%d\r Nom cpuid\t:%s\rJeux d'instruction\t:%s\033[0m\r\n\000", cpu.stepping, cpu.models, cpu.family, &cpu.detectedname, &cpu.techs);
	return 0;
}

/*******************************************************************************/
