/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "types.h"
#include "process.h"
#include "memory.h"
#include "gdt.h"

process *processes;
tid_t current;
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

void processexit(void)
{
	deleteprocess(getcurrentpid());
	switchtask(maketid(0,0));
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
	code = iself(src);
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

void initprocesses(void)
{
	u32     i = 1;
	processes = (process *) vmalloc(sizeof(process) * MAXNUMPROCESS);
	while (i < MAXNUMPROCESS)
	{
		processes[i].pid = NULL;
		processes[i++].status = PROCESS_STATUS_FREE;
	}
	createtask(0,getinitretry(),true);
	processes[0].result = 0;
	processes[0].status = PROCESS_STATUS_READY;
	processes[0].iskernel = true;
	current = maketid(0,0);
	lastpid = NULL;
}

/*******************************************************************************/
/* Récupère un emplacement dans la liste des processus */

pid_t getfreepid(void)
{
	u32     i = lastpid;
	u32     parsed = 0;
	while (processes[++i].status != PROCESS_STATUS_FREE
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
/* Récupère un emplacement dans la liste des tâche du processus donné */

tid_t getfreeptid(pid_t pid)
{
	tid_t new;
	new.pid=pid;
	task_t *task_head= &processes[(u32)pid].task_head;
	task *next;
	TAILQ_FOREACH(next, task_head, tailq)
		if (next->tid.number>new.number)
			new.number=next->tid.number;
	next->tid.number++;
	return new;
}

/*******************************************************************************/
/* Récupère le PID du processus courant */

pid_t getcurrentpid(void)
{
	return current.pid;
}

/*******************************************************************************/
/* Récupère le TID de la tâche courante */

tid_t getcurrenttid(void)
{
	return current;
}

/*******************************************************************************/
/* Change la tâche désigné dans le TID */

tid_t	maketid(pid_t pid, u32 number)
{
	tid_t newtid;
	newtid.pid=pid;
	newtid.number=number;
	return newtid;
}

/*******************************************************************************/
/* Récupère l'adresse d'un processus */

process* findprocess(pid_t pid)
{
	return &processes[(u32)pid];
}

/*******************************************************************************/
/* Récupère l'adresse du processus courant */

process* findcurrentprocess(void)
{
	return &processes[(u32)getcurrentpid()];
}

/*******************************************************************************/
/* Determine le dernier PID occupé */

void usepid(pid_t pid)
{
	lastpid = pid;
}

/*******************************************************************************/
/* Bascule vers une tâche */

void switchtask(tid_t tid)
{
	tid_t previous = current;
	task *atask = findtask(tid);
	process *aprocess=findprocess(tid.pid);
	if (!aprocess->iskernel)
		setTSS(atask->kernel_stack.ss0, atask->kernel_stack.esp0);
	else
		setTSS(0x0, 0x0);
	atask->dump.eflags = (atask->dump.eflags | 0x200) & 0xFFFFBFFF;
	createdump(atask->dump);
	if (atask->dump.cs==SEL_KERNEL_CODE)
		restcpu_kernel();
	else
		restcpu_user();
	iret();
}

/*******************************************************************************/
/* Cherche l'adresse d'une tâche */

task* findtask(tid_t tid)
{
	task_t *task_head= &processes[(u32)tid.pid].task_head;
	task *next;
	TAILQ_FOREACH(next, task_head, tailq)
		if (next->tid.number==tid.number)
			return next;
}

/*******************************************************************************/
/* Cherche l'adresse de la tâche courante */

task* findcurrenttask(void)
{
	return findtask(getcurrenttid());
}


/*******************************************************************************/
/* Détruit une tâche */

void deletetask(tid_t tid)
{
	stoptask(tid);
	process* aprocess=findprocess(tid.pid);
	task *atask=findtask(tid);
	TAILQ_REMOVE(&aprocess->task_head, atask, tailq);
	vfree(atask);
}

/*******************************************************************************/
/* Execute une tâche */

void runtask(tid_t tid)
{
	task *atask=findtask(tid);
	if (atask->status == TASK_STATUS_READY)
	{
		atask->status = TASK_STATUS_RUN;
		switchtask(tid);
	}
}

/*******************************************************************************/
/* Initialise une tâche */

tid_t createtask(pid_t pid,u8 *entry, bool kerneltask)
{
	tid_t tid;
	tid.pid=pid;
	process* aprocess=findprocess(pid);
	task *new = (task *) vmalloc(sizeof(task));
	TAILQ_INSERT_TAIL(&aprocess->task_head, new, tailq);
	page *astack = virtual_page_getfree();
	if (kerneltask)
	{
		new->dump.ss = SEL_KERNEL_STACK;
		new->dump.esp =
			(u32) astack->vaddr + PAGESIZE - 16;
		new->dump.eflags = 0x0;
		new->dump.cs = SEL_KERNEL_CODE;
		new->dump.ds = SEL_KERNEL_DATA;
		new->dump.es = SEL_KERNEL_DATA;
		new->dump.fs = SEL_KERNEL_DATA;
		new->dump.gs = SEL_KERNEL_DATA;
		new->dump.cr3 = KERNEL_PD_ADDR;
	}
	else
	{
		new->kernel_stack.ss0 = SEL_KERNEL_STACK;
		new->kernel_stack.esp0 =
			(u32) astack->vaddr + PAGESIZE - 16;
		new->dump.ss = SEL_USER_STACK | RPL_RING3;
		new->dump.esp = USER_STACK - 16;
		new->dump.eflags = 0x0;
		new->dump.cs = SEL_USER_CODE | RPL_RING3;
		new->dump.ds = SEL_USER_DATA | RPL_RING3;
		new->dump.es = SEL_USER_DATA | RPL_RING3;
		new->dump.fs = SEL_USER_DATA | RPL_RING3;
		new->dump.gs = SEL_USER_DATA | RPL_RING3;
		new->dump.cr3 = aprocess->pdd->addr->paddr;
	}
	new->tid=getfreeptid(pid);
	new->dump.eip = aprocess->entry;
	new->status=TASK_STATUS_READY;
	new->dump.eax = 0;
	new->dump.ecx = 0;
	new->dump.edx = 0;
	new->dump.ebx = 0;
	new->dump.ebp = 0;
	new->dump.esi = 0;
	new->dump.edi = 0;
	new->status = TASK_STATUS_READY;
	return new->tid;
}

/*******************************************************************************/
/* Arrête une tâche */

void stoptask(tid_t tid)
{
	task *current=findtask(tid);
	current->status=TASK_STATUS_STOP;
}

/*******************************************************************************/
/* Initialise un processus */

pid_t createprocess(u8 *src, bool kerneltask)
{
	tid_t previous = current;
	current.pid = getfreepid();
	current.number = 0;
	usepid(current.pid);
	process* new=findcurrentprocess();
	new->pid = current.pid;
	new->pdd = virtual_pd_create();
	TAILQ_INIT(&new->page_head);
	new->iskernel=kerneltask;
	setCR3(new->pdd->addr->paddr);
	new->entry = loadelf(src, new->pid);
	createtask(new->pid,new->entry, new->iskernel);
	current = previous;
	process* old=findcurrentprocess();
	setCR3(old->pdd->addr->paddr);
	return new->pid;
}

/*******************************************************************************/
/* Détruit un processus */

void deleteprocess(pid_t pid)
{
	stopprocess(pid);
	process* aprocess=findprocess(pid);
	task *next;
	TAILQ_FOREACH(next, &aprocess->task_head, tailq)
		deletetask(next->tid);
	aprocess->status = PROCESS_STATUS_FREE;
	
}

/*******************************************************************************/
/* Execute un processus */

void runprocess(pid_t pid)
{
	process* aprocess=findprocess(pid);
	if (aprocess->status == PROCESS_STATUS_READY)
	{
		aprocess->status = PROCESS_STATUS_RUN;
		tid_t tid=maketid(pid,0);
		task *atask=findtask(tid);
		atask->status=TASK_STATUS_RUN;
		switchtask(tid);
	}
}

/*******************************************************************************/
/* Arrête un processus */

void stopprocess(pid_t pid)
{
	process* aprocess=findprocess(pid);
	if (aprocess->status == PROCESS_STATUS_RUN)
	{
		aprocess->status = PROCESS_STATUS_READY;
		task *next;
		TAILQ_FOREACH(next, &aprocess->task_head, tailq)
		next->status=TASK_STATUS_READY;
	}
}

/*******************************************************************************/
