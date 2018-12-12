/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "types.h"
#include "process.h"
#include "memory.h"
#include "gdt.h"

process *processes;
process *current;
u32     lastpid;


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

u32 elf_test(u8 * src)
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
/* Charge le fichier ELF en mémoire et mets à jour les informations sur le processus */

u32 elf_load(u8 * src, u32 pid)
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
				processes[pid].exec_low = (u8 *) v_begin;
				processes[pid].exec_high = (u8 *) v_end;
			}
			if (program->p_flags == PF_W + PF_R)
			{
				processes[pid].bss_low = (u8 *) v_begin;
				processes[pid].bss_high = (u8 *) v_end;
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
	processes[pid].entry = header->e_entry;
	return header->e_entry;
}

/*******************************************************************************/
/* Initialise la liste des processus */

void task_init()
{
	u32     i = 0;
	processes = (process *) vmalloc(sizeof(process) * MAXNUMPROCESS);
	while (i++ < MAXNUMPROCESS)
	{
		processes[i].pid = NULL;
		processes[i].status = STATUS_FREE;
	}
	processes[0].dump.ss = SEL_KERNEL_STACK;
	processes[0].dump.esp = KERNEL_STACK_ADDR;
	processes[0].dump.eflags = 0x0;
	processes[0].dump.cs = SEL_KERNEL_CODE;
	processes[0].dump.eip = getinitretry();
	processes[0].dump.ds = SEL_KERNEL_DATA;
	processes[0].dump.es = SEL_KERNEL_DATA;
	processes[0].dump.fs = SEL_KERNEL_DATA;
	processes[0].dump.gs = SEL_KERNEL_DATA;
	processes[0].dump.cr3 = KERNEL_PD_ADDR;
	processes[0].dump.eax = 0;
	processes[0].dump.ecx = 0;
	processes[0].dump.edx = 0;
	processes[0].dump.ebx = 0;
	processes[0].dump.ebp = 0;
	processes[0].dump.esi = 0;
	processes[0].dump.edi = 0;
	processes[0].result = 0;
	processes[0].status = STATUS_READY;
	processes[0].kernel = true;
	current = &processes[0];
	lastpid = NULL;
}

/*******************************************************************************/
/* Récupère un emplacement dans la liste des processus */

u32 task_getfreePID()
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
	return i;
}

/*******************************************************************************/
/* Récupère les informations sur le processus courant */

process *getcurrentprocess()
{
	return current;
}

/*******************************************************************************/
/* Récupère le PID du processus courant */

u32 getcurrentpid()
{
	return current->pid;
}


/*******************************************************************************/
/* Determine le dernier PID occupé */

u32 task_usePID(u32 pid)
{
	lastpid = pid;
}

/*******************************************************************************/
/* Bascule vers une tâche */

void task_switch(u32 pid, bool fromkernelmode)
{
	process *previous = current;
	current = &processes[pid];
	if (!current->kernel)
		setTSS(current->kstack.ss0, current->kstack.esp0);
	else
		setTSS(0x0, 0x0);
	current->dump.eflags = (current->dump.eflags | 0x200) & 0xFFFFBFFF;
	createdump(current->dump);
	restdebugcpu();
	iret();
}

/*******************************************************************************/
/* Execute une tâche */

void task_run(u32 pid)
{
	if (processes[pid].status == STATUS_READY)
	{
		processes[pid].status = STATUS_RUN;
		task_switch(pid, false);
	}
}

/*******************************************************************************/
/* Execute une tâche */

void task_delete(u32 pid)
{
	if (processes[pid].status == STATUS_READY
	    || processes[pid].status == STATUS_RUN)
	{
		processes[pid].status = STATUS_FREE;
	}
}

/*******************************************************************************/
/* Initialise une tâche */

u32 task_create(u8 * code, bool kerneltask)
{
	process *previous = current;
	u32     pid = task_getfreePID();
	task_usePID(pid);
	page   *kstack;
	processes[pid].pid = pid;
	processes[pid].pdd = virtual_pd_create();
	TAILQ_INIT(&processes[pid].page_head);
	if (kerneltask)
	{
		processes[pid].dump.ss = SEL_KERNEL_STACK;
		processes[pid].dump.esp =
			(u32) kstack->vaddr + PAGESIZE - 16;
		processes[pid].dump.eflags = 0x0;
		processes[pid].dump.cs = SEL_KERNEL_CODE;
		processes[pid].dump.eip = elf_load(code, pid);
		if (processes[pid].dump.eip == NULL)
			return NULL;
		processes[pid].dump.ds = SEL_KERNEL_DATA;
		processes[pid].dump.es = SEL_KERNEL_DATA;
		processes[pid].dump.fs = SEL_KERNEL_DATA;
		processes[pid].dump.gs = SEL_KERNEL_DATA;
		processes[pid].dump.cr3 = KERNEL_PD_ADDR;
		processes[pid].dump.eax = 0;
		processes[pid].dump.ecx = 0;
		processes[pid].dump.edx = 0;
		processes[pid].dump.ebx = 0;
		processes[pid].dump.ebp = 0;
		processes[pid].dump.esi = 0;
		processes[pid].dump.edi = 0;
		processes[pid].result = 0;
		processes[pid].status = STATUS_READY;
		processes[pid].kernel = true;
		current = previous;
	}
	else
	{
		current = &processes[pid];
		setcr3(processes[pid].pdd->addr->paddr);
		kstack = virtual_page_getfree();
		processes[pid].dump.ss = SEL_USER_STACK | RPL_RING3;
		processes[pid].dump.esp = USER_STACK - 16;
		processes[pid].dump.eflags = 0x0;
		processes[pid].dump.cs = SEL_USER_CODE | RPL_RING3;
		processes[pid].dump.eip = elf_load(code, pid);
		if (processes[pid].dump.eip == NULL)
			return NULL;
		processes[pid].dump.ds = SEL_USER_DATA | RPL_RING3;
		processes[pid].dump.es = SEL_USER_DATA | RPL_RING3;
		processes[pid].dump.fs = SEL_USER_DATA | RPL_RING3;
		processes[pid].dump.gs = SEL_USER_DATA | RPL_RING3;
		processes[pid].dump.cr3 =
			(u32) processes[pid].pdd->addr->paddr;
		processes[pid].kstack.ss0 = SEL_KERNEL_STACK;
		processes[pid].kstack.esp0 =
			(u32) kstack->vaddr + PAGESIZE - 16;
		processes[pid].dump.eax = 0;
		processes[pid].dump.ecx = 0;
		processes[pid].dump.edx = 0;
		processes[pid].dump.ebx = 0;
		processes[pid].dump.ebp = 0;
		processes[pid].dump.esi = 0;
		processes[pid].dump.edi = 0;
		processes[pid].result = 0;
		processes[pid].status = STATUS_READY;
		processes[pid].kernel = false;
		current = previous;
		setcr3(current->dump.cr3);
	}
	return pid;
}
