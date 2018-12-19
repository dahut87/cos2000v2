/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "types.h"
#include "process.h"
#include "memory.h"
#include "gdt.h"

process *processes;
pid_t current;
pid_t lastpid;


static u8 elf_errors1[] = "Aucune signature ELF";
static u8 elf_errors2[] = "Fichier au format ELF mais non 32 bits";
static u8 elf_errors3[] = "ELF non LSB";
static u8 elf_errors4[] = "ELF mauvaise version";
static u8 elf_errors5[] = "ELF pour OS ne correspondant pas";
static u8 elf_errors6[] = "Mauvais type de machine";
static u8 *elf_errors[] =
	{ &elf_errors1, &elf_errors2, &elf_errors3, &elf_errors4,
	&elf_errors5, &elf_errors6
};

/*******************************************************************************/
/* Vérifie la signature ELF 
 0 - RAS
 1 - Pas la signature ELF
 2 - pas ELF32
 3 - pas bon organisation LSB/MSB
 4 - pas bonne version ELF
 5 - pas bon OS
 6 - pas bon type machine */

u32 iself(u8 * src)
{
	elf32  *header = (elf32 *) src;
	if (header->e_ident[EI_MAG0] == ELFMAG0
	    && header->e_ident[EI_MAG1] == ELFMAG1
	    && header->e_ident[EI_MAG2] == ELFMAG2
	    && header->e_ident[EI_MAG3] == ELFMAG3)
	{
		if (header->e_ident[EI_CLASS] != ELFCLASS32)
			return 2;
		if (header->e_ident[EI_DATA] != ELFDATA2LSB)
			return 3;
		if (header->e_ident[EI_VERSION] != EV_CURRENT
		    || header->e_version != EV_CURRENT)
			return 4;
		if (header->e_ident[EI_OSABI] != ELFOSABI_COS2000)
			return 5;
		if (header->e_machine != EM_386)
			return 6;
		return 0;
	}
	else
		return 1;
}

/*******************************************************************************/
/* Met fin à une tâche */
/* SYSCALL 
{
"ID":5,
"LIBRARY":"libsys",
"NAME":"exit",
"INTERNALNAME":"processexit",
"DESCRIPTION":"End a task for user or kernel domain",
"ARGS": [
{"TYPE":"u32","NAME":"resultcode","DESCRIPTION":"Code result of the execution"}
],
"RETURN":"void"
}
END */

void processexit()
{
	deletetask(getcurrentpid());
	switchtask(0, false);
}

/*******************************************************************************/
/* Charge le fichier ELF en mémoire et mets à jour les informations sur le processus */

u32 loadelf(u8 * src, pid_t pid)
{
	u8     *ptr;
	u8      code;
	u32     v_begin, v_end;
	elf32  *header;
	elf32p *program;
	u32     i;

	header = (elf32 *) src;
	program = (elf32p *) (src + header->e_phoff);
	code = elf_test(src);
	if (code != 0)
	{
		printf("Erreur de chargement ELF, %s !\r\n",
		       elf_errors[code - 1]);
		return NULL;
	}
	for (i = 0; i < header->e_phnum; i++, program++)
	{
		if (program->p_type == PT_LOAD)
		{
			v_begin = program->p_vaddr;
			v_end = program->p_vaddr + program->p_memsz;
			if (v_begin < USER_CODE)
			{
				printf("Ne peut charger l'executable en desssous de l'adresse %X\r\n", USER_CODE);
				return 0;
			}
			if (v_end > USER_STACK)
			{
				printf("Ne peut charger l'executable au desssus de l'adresse %X\r\n", USER_STACK);
				return 0;
			}
			if (program->p_flags == PF_X + PF_R)
			{
				processes[(u32)pid].exec_low = (u8 *) v_begin;
				processes[(u32)pid].exec_high = (u8 *) v_end;
			}
			if (program->p_flags == PF_W + PF_R)
			{
				processes[(u32)pid].bss_low = (u8 *) v_begin;
				processes[(u32)pid].bss_high = (u8 *) v_end;
			}
			memcpy((u8 *) (src + program->p_offset),
			       (u8 *) v_begin, program->p_filesz, 0);
			if (program->p_memsz > program->p_filesz)
				for (i = program->p_filesz, ptr =
				     (u8 *) program->p_vaddr;
				     i < program->p_memsz; i++)
					ptr[i] = 0;
		}
	}
	return header->e_entry;
}

/*******************************************************************************/
/* Initialise la liste des processus */

void initprocesses()
{
	u32     i = 1;
	processes = (process *) vmalloc(sizeof(process) * MAXNUMPROCESS);
	while (i < MAXNUMPROCESS)
	{
		processes[i].pid = NULL;
		processes[i++].status = STATUS_FREE;
	}
	createtask(0,getinitretry());
	processes[0].result = 0;
	processes[0].status = STATUS_READY;
	processes[0].iskernel = true;
	current = 0;
	lastpid = NULL;
}

/*******************************************************************************/
/* Récupère un emplacement dans la liste des processus */

pid_t getfreepid()
{
	u32     i = lastpid;
	u32     parsed = 0;
	while (processes[++i].status != STATUS_FREE
	       && ++parsed < MAXNUMPROCESS)
	{
		if (i >= MAXNUMPROCESS)
			i = 0;
	}
	if (parsed > MAXNUMPROCESS)
	{
		printf("PANIC: plus d'emplacement disponible pour un novueau processus\n");
		return NULL;
	}
	return (pid_t)i;
}

/*******************************************************************************/
/* Récupère le PID du processus courant */

pid_t getcurrentpid()
{
	return current->pid;
}


/*******************************************************************************/
/* Determine le dernier PID occupé */

void usepid(pid_t pid)
{
	lastpid = pid;
}

/*******************************************************************************/
/* Bascule vers une tâche */

void switchtask(tid_t pid, bool fromkernelmode)
{
	pid_t previous = current;
	current = &processes[(u32)pid];
	if (!current->kernel)
		setTSS(current->kstack.ss0, current->kstack.esp0);
	else
		setTSS(0x0, 0x0);
	current->dump.eflags = (current->dump.eflags | 0x200) & 0xFFFFBFFF;
	createdump(current->dump);
	if (current->dump.cs==SEL_KERNEL_CODE)
		restcpu_kernel();
	else
		restcpu_user();
	iret();
}

/*******************************************************************************/
/* Détruit une tâche */

void deletetask(tid_t tid)
{
	stoptask

}

/*******************************************************************************/
/* Execute une tâche */

void runtask(tid_t tid)
{
	if (processes[(u32)pid].status == STATUS_READY)
	{
		processes[(u32)pid].status = STATUS_RUN;
		switchtask(u32)pid, false);
	}
}

/*******************************************************************************/
/* Initialise une tâche */

tid_t createtask(pid_t pid,u8 *entry)
{
	pid_t previous = current;
	pid_t pid = getfreepid();
	usepid(pid);
	page   *kstack;
	processes[(u32)pid].pid = (u32)pid;
	processes[(u32)pid].pdd = virtual_pd_create();
	TAILQ_INIT(&processes[(u32)pid].page_head);
	if (&processes[(u32)pid].iskernel)
	{
		processes[(u32)pid].dump.ss = SEL_KERNEL_STACK;
		processes[(u32)pid].dump.esp =
			(u32) kstack->vaddr + PAGESIZE - 16;
		processes[(u32)pid].dump.eflags = 0x0;
		processes[(u32)pid].dump.cs = SEL_KERNEL_CODE;
		processes[(u32)pid].dump.eip = elf_load(code, pid);
		if (processes[(u32)pid].dump.eip == NULL)
			return NULL;
		processes[(u32)pid].dump.ds = SEL_KERNEL_DATA;
		processes[(u32)pid].dump.es = SEL_KERNEL_DATA;
		processes[(u32)pid].dump.fs = SEL_KERNEL_DATA;
		processes[(u32)pid].dump.gs = SEL_KERNEL_DATA;
		processes[(u32)pid].dump.cr3 = KERNEL_PD_ADDR;
		processes[(u32)pid].dump.eax = 0;
		processes[(u32)pid].dump.ecx = 0;
		processes[(u32)pid].dump.edx = 0;
		processes[(u32)pid].dump.ebx = 0;
		processes[(u32)pid].dump.ebp = 0;
		processes[(u32)pid].dump.esi = 0;
		processes[(u32)pid].dump.edi = 0;
		processes[(u32)pid].result = 0;
		processes[(u32)pid].status = STATUS_READY;
		processes[(u32)pid].kernel = true;
		current = previous;
	}
	else
	{
		current = &processes[(u32)pid];
		setCR3(processes[(u32)pid].pdd->addr->paddr);
		kstack = virtual_page_getfree();
		processes[(u32)pid].dump.ss = SEL_USER_STACK | RPL_RING3;
		processes[(u32)pid].dump.esp = USER_STACK - 16;
		processes[(u32)pid].dump.eflags = 0x0;
		processes[(u32)pid].dump.cs = SEL_USER_CODE | RPL_RING3;
		processes[(u32)pid].dump.eip = elf_load(code, pid);
		if (processes[(u32)pid].dump.eip == NULL)
			return NULL;
		processes[(u32)pid].dump.ds = SEL_USER_DATA | RPL_RING3;
		processes[(u32)pid].dump.es = SEL_USER_DATA | RPL_RING3;
		processes[(u32)pid].dump.fs = SEL_USER_DATA | RPL_RING3;
		processes[(u32)pid].dump.gs = SEL_USER_DATA | RPL_RING3;
		processes[(u32)pid].dump.cr3 =
			(u32) processes[(u32)pid].pdd->addr->paddr;
		processes[(u32)pid].kstack.ss0 = SEL_KERNEL_STACK;
		processes[(u32)pid].kstack.esp0 =
			(u32) kstack->vaddr + PAGESIZE - 16;
		processes[(u32)pid].dump.eax = 0;
		processes[(u32)pid].dump.ecx = 0;
		processes[(u32)pid].dump.edx = 0;
		processes[(u32)pid].dump.ebx = 0;
		processes[(u32)pid].dump.ebp = 0;
		processes[(u32)pid].dump.esi = 0;
		processes[(u32)pid].dump.edi = 0;
		processes[(u32)pid].result = 0;
		processes[(u32)pid].status = STATUS_READY;
		processes[(u32)pid].kernel = false;
		current = previous;
		setCR3(current->dump.cr3);
	}
	return pid;
}

/*******************************************************************************/
/* Arrête une tâche */

void stoptask(tid_t tid)
{

}

/*******************************************************************************/
/* Initialise un processus */

pid_t createprocess(u8 *src, bool kerneltask)
{
	pid_t previous = current;
	pid_t pid = getfreepid();
	usepid(pid);
	page   *kstack;
	processes[(u32)pid].pid = (u32)pid;
	processes[(u32)pid].pdd = virtual_pd_create();
	TAILQ_INIT(&processes[(u32)pid].page_head);
	processes[(u32)pid].entry = elf_load(code, pid);
		
if (processes[(u32)pid].dump.eip == NULL)
			return NULL;
		processes[(u32)pid].dump.ds = SEL_KERNEL_DATA;
		processes[(u32)pid].dump.es = SEL_KERNEL_DATA;
		processes[(u32)pid].dump.fs = SEL_KERNEL_DATA;
		processes[(u32)pid].dump.gs = SEL_KERNEL_DATA;
		processes[(u32)pid].dump.cr3 = KERNEL_PD_ADDR;
		processes[(u32)pid].dump.eax = 0;
		processes[(u32)pid].dump.ecx = 0;
		processes[(u32)pid].dump.edx = 0;
		processes[(u32)pid].dump.ebx = 0;
		processes[(u32)pid].dump.ebp = 0;
		processes[(u32)pid].dump.esi = 0;
		processes[(u32)pid].dump.edi = 0;
		processes[(u32)pid].result = 0;
		processes[(u32)pid].status = STATUS_READY;
		processes[(u32)pid].kernel = true;
		current = previous;
	}
	else
	{
		current = &processes[(u32)pid];
		setCR3(processes[(u32)pid].pdd->addr->paddr);
		kstack = virtual_page_getfree();
		processes[(u32)pid].dump.ss = SEL_USER_STACK | RPL_RING3;
		processes[(u32)pid].dump.esp = USER_STACK - 16;
		processes[(u32)pid].dump.eflags = 0x0;
		processes[(u32)pid].dump.cs = SEL_USER_CODE | RPL_RING3;
		processes[(u32)pid].dump.eip = elf_load(code, pid);
		if (processes[(u32)pid].dump.eip == NULL)
			return NULL;
		processes[(u32)pid].dump.ds = SEL_USER_DATA | RPL_RING3;
		processes[(u32)pid].dump.es = SEL_USER_DATA | RPL_RING3;
		processes[(u32)pid].dump.fs = SEL_USER_DATA | RPL_RING3;
		processes[(u32)pid].dump.gs = SEL_USER_DATA | RPL_RING3;
		processes[(u32)pid].dump.cr3 =
			(u32) processes[(u32)pid].pdd->addr->paddr;
		processes[(u32)pid].kstack.ss0 = SEL_KERNEL_STACK;
		processes[(u32)pid].kstack.esp0 =
			(u32) kstack->vaddr + PAGESIZE - 16;
		processes[(u32)pid].dump.eax = 0;
		processes[(u32)pid].dump.ecx = 0;
		processes[(u32)pid].dump.edx = 0;
		processes[(u32)pid].dump.ebx = 0;
		processes[(u32)pid].dump.ebp = 0;
		processes[(u32)pid].dump.esi = 0;
		processes[(u32)pid].dump.edi = 0;
		processes[(u32)pid].result = 0;
		processes[(u32)pid].status = STATUS_READY;
		processes[(u32)pid].kernel = false;
		current = previous;
		setCR3(current->dump.cr3);
	}
	return pid;
}

/*******************************************************************************/
/* Détruit un processus */

void deleteprocess(pid_t pid)
{

}

/*******************************************************************************/
/* Execute un processus */

void runprocess(pid_t pid)
{

}

/*******************************************************************************/
/* Arrête un processus */

void stopprocess(pid_t pid)
{

}

/*******************************************************************************/
