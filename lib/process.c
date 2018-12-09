/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "types.h"
#include "process.h"
#include "memory.h"
#include "gdt.h"

process *processes;
process *current;
u32 lastpid;


u8 elf_errors1[]="Aucune signature ELF";
u8 elf_errors2[]="Fichier au format ELF mais non 32 bits";
u8 elf_errors3[]="ELF non LSB";
u8 elf_errors4[]="ELF mauvaise version";
u8 elf_errors5[]="ELF pour OS ne correspondant pas";
u8 elf_errors6[]="Mauvais type de machine";
u8 *elf_errors[6]={&elf_errors1,&elf_errors2,&elf_errors3,&elf_errors4,&elf_errors5,&elf_errors6};

/*******************************************************************************/
/* Vérifie la signature ELF 
 0 - RAS
 1 - Pas la signature ELF
 2 - pas ELF32
 3 - pas bon organisation LSB/MSB
 4 - pas bonne version ELF
 5 - pas bon OS
 6 - pas bon type machine */
 
u32 elf_test(u8 *src)
{
    elf32 *header=(elf32 *) src;
	if (header->e_ident[EI_MAG0] == ELFMAG0 && header->e_ident[EI_MAG1] == ELFMAG1
	    && header->e_ident[EI_MAG2] == ELFMAG2 && header->e_ident[EI_MAG3] == ELFMAG3)
        {
        if (header->e_ident[EI_CLASS]!=ELFCLASS32)
            return 2;
        if (header->e_ident[EI_DATA]!=ELFDATA2LSB)
            return 3;
        if (header->e_ident[EI_VERSION]!=EV_CURRENT || header->e_version!=EV_CURRENT)
            return 4;
        if (header->e_ident[EI_OSABI]!=ELFOSABI_COS2000)
            return 5;
        if (header->e_machine==EM_386)
            return 6;
        return 0;
        }
	else
		return 1;
}

/*******************************************************************************/
/* Charge le fichier ELF en mémoire et mets à jour les informations sur le processus */

u32 elf_load(u8 *src, u32 pid)
{
	u8 *ptr;
    u8 code;
	u32 v_begin, v_end;
	elf32 *header;
	elf32p *program;
	u32 i;

	header = (elf32 *) src;
	program = (elf32p *) (src + header->e_phoff);
    code=elf_test(src);	
    if (code!=0) {
		printf("Mauvais format ELF : N°%s !\r\n",elf_errors[code-1]);
		return NULL;
	}
	for (i = 0; i < header->e_phnum; i++, program++) {
		if (program->p_type == PT_LOAD) {
			v_begin = program->p_vaddr;
			v_end = program->p_vaddr + program->p_memsz;
			if (v_begin < USER_CODE) {
				printf ("Ne peut charger l'executable en desssous de l'adresse %X\r\n", USER_CODE);
				return 0;
			}
			if (v_end > USER_STACK) {
				printf ("Ne peut charger l'executable au desssus de l'adresse %X\r\n", USER_STACK);
				return 0;
			}
			if (program->p_flags == PF_X + PF_R) {	
				processes[pid].exec_low = (u8*) v_begin;
				processes[pid].exec_high = (u8*) v_end;
			}
			if (program->p_flags == PF_W + PF_R) {	
				processes[pid].bss_low = (u8*) v_begin;
				processes[pid].bss_high = (u8*) v_end;
			}
			memcpy((u8 *) v_begin, (u8 *) (src + program->p_offset), program->p_filesz,0);
			if (program->p_memsz > program->p_filesz)
				for (i = program->p_filesz, ptr = (u8 *) program->p_vaddr; i < program->p_memsz; i++)
					ptr[i] = 0;
		}
	}
	return header->e_entry;
}

/*******************************************************************************/
/* Initialise la liste des processus */

void task_init() {
    u32 i=0;
    processes=(process*)vmalloc(sizeof(process)*MAXNUMPROCESS);
    while (i++ < MAXNUMPROCESS) {
        processes[i].pid=NULL;
        processes[i].status=STATUS_FREE;    
    }
    current=NULL;
    lastpid=NULL;
}

/*******************************************************************************/
/* Récupère un emplacement dans la liste des processus */

u32 task_getfreePID ()
{
	u32 i = lastpid;
    u32 parsed = 0;
	while (processes[++i].status != STATUS_FREE && ++parsed < MAXNUMPROCESS)
    {
        if (i>=MAXNUMPROCESS)
            i=0;
    }
    if (parsed>MAXNUMPROCESS) {
		printf("PANIC: plus d'emplacement disponible pour un novueau processus\n");
		return NULL;
    }
	return i;
}

/*******************************************************************************/
/* Determine le dernier PID occupé */

u32 task_usePID (u32 pid)
{
	lastpid=pid;
}

/*******************************************************************************/
/* Initialise une tâche */

u32 task_create(u8 *code)
{
    process *previous=current; 
    u32 pid=task_getfreePID();
    task_usePID(pid);
    page *kstack;
	processes[pid].pid = pid;
	processes[pid].pdd = virtual_pd_create();
	TAILQ_INIT(&processes[pid].page_head);
	current = &processes[pid];
	setcr3(processes[pid].pdd->addr->paddr);
	kstack = virtual_page_getfree();
	processes[pid].dump.ss = SEL_USER_STACK || RPL_RING3;
	processes[pid].dump.esp = USER_STACK;
	processes[pid].dump.eflags = 0x0;
	processes[pid].dump.cs = SEL_USER_CODE  || RPL_RING3;
	processes[pid].dump.eip = elf_load(code,pid);
	processes[pid].dump.ds = SEL_USER_DATA || RPL_RING3;
	processes[pid].dump.es = SEL_USER_DATA || RPL_RING3;
	processes[pid].dump.fs = SEL_USER_DATA || RPL_RING3;
	processes[pid].dump.gs = SEL_USER_DATA || RPL_RING3;
	processes[pid].dump.cr3 = (u32) processes[pid].pdd->addr->paddr;
	processes[pid].kstack.ss0 = SEL_KERNEL_STACK;
	processes[pid].kstack.esp0 = (u32) kstack->vaddr + PAGESIZE - 16;
	processes[pid].dump.eax = 0;
	processes[pid].dump.ecx = 0;
	processes[pid].dump.edx = 0;
	processes[pid].dump.ebx = 0;
	processes[pid].dump.ebp = 0;
	processes[pid].dump.esi = 0;
	processes[pid].dump.edi = 0;
	processes[pid].result = 0;
	processes[pid].status = STATUS_READY;
	current = previous;
	setcr3(current->dump.cr3);
	return pid;
}
