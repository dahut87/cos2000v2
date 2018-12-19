/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "types.h"
#include "interrupts.h"
#include "memory.h"

#define MAXPIDVALUE	    0xFFFF
#define MAXNUMPROCESS	256

#define PROCESS_STATUS_FREE   0x0
#define PROCESS_STATUS_READY  0xF0
#define PROCESS_STATUS_RUN    0x1
#define PROCESS_STATUS_SLEEP  0x2

#define TASK_STATUS_READY	0x0
#define TASK_STATUS_RUN		0x1
#define TASK_STATUS_STOP	0xFF

/* ELF type */
#define ET_NONE 	0	//No file type
#define ET_REL 	    1		//Relocatable file
#define ET_EXEC 	2	//Executable file
#define ET_DYN 	    3		//Shared object file
#define ET_CORE 	4	//Core file
#define ET_LOOS 	0xfe00	//Operating system-specific
#define ET_HIOS 	0xfeff	//Operating system-specific
#define ET_LOPROC 	0xff00	//Processor-specific
#define ET_HIPROC 	0xffff	//Processor-specific

/* ELF identification */
#define	EI_MAG0		    0	//File identification
#define	EI_MAG1		    1	//File identification
#define	EI_MAG2		    2	//File identification
#define	EI_MAG3		    3	//File identification
#define	EI_CLASS	    4	//File class
#define	EI_DATA		    5	//Data encoding
#define	EI_VERSION	    6	//File version
#define	EI_OSABI 	    7	//Operating system/ABI identification
#define	EI_ABIVERSION 	8	//ABI version
#define	EI_PAD 	        9	//Start of padding bytes
#define	EI_NIDENT 	    16	//Size of e_ident[]

/* ELF version */
#define EV_NONE 	0	//Invalid version
#define EV_CURRENT 	1	//Current version

/* ELF machine type */

#define EM_NONE     0		//No machine
#define EM_386 	    3		//Intel 80386
#define EM_IA_64 	50	//Intel IA-64 processor architecture
#define EM_X86_64 	62	//AMD x86-64 architecture

/* EI signature */
#define	ELFMAG0		0x7f
#define	ELFMAG1		'E'
#define	ELFMAG2		'L'
#define	ELFMAG3		'F'

/* EI file class  */
#define	ELFCLASSNONE	0	/* invalid class */
#define	ELFCLASS32	    1	/* 32-bit objects */
#define	ELFCLASS64	    2	/* 64-bit objects */

/* EI data structure */
#define	ELFDATANONE	0	/* invalide data encoding */
#define	ELFDATA2LSB	1	/* least significant byte first (0x01020304 is 0x04 0x03 0x02 0x01) */
#define	ELFDATA2MSB	2	/* most significant byte first (0x01020304 is 0x01 0x02 0x03 0x04) */

/* p type */
#define	PT_NULL             0
#define	PT_LOAD             1
#define	PT_DYNAMIC          2
#define	PT_INTERP           3
#define	PT_NOTE             4
#define	PT_SHLIB            5
#define	PT_PHDR             6
#define	PT_LOPROC  0x70000000
#define	PT_HIPROC  0x7fffffff

/* p flags */
#define PF_X	0x1
#define PF_W	0x2
#define PF_R	0x4

/* OS identification */
#define ELFOSABI_NONE 	0	//No extensions or unspecified
#define ELFOSABI_LINUX 	3	//Linux
#define ELFOSABI_COS2000 16	//COS2000

/* ELF header */
typedef struct elf32
{
	u8      e_ident[EI_NIDENT];
	u16     e_type;
	u16     e_machine;
	u32     e_version;
	u8     *e_entry;
	u32     e_phoff;
	u32     e_shoff;
	u32     e_flags;
	u16     e_ehsize;
	u16     e_phentsize;
	u16     e_phnum;
	u16     e_shentsize;
	u16     e_shnum;
	u16     e_shstrndx;
} elf32  __attribute__ ((packed));

typedef struct elf32p
{
	u32     p_type;
	u32     p_offset;
	u8     *p_vaddr;
	u8     *p_paddr;
	u32     p_filesz;
	u32     p_memsz;
	u32     p_flags;
	u32     p_align;
} elf32p  __attribute__ ((packed));

typedef unsigned int pid_t;

typedef struct tid_t
{
	pid_t pid;
	u32 number;
} tid_t __attribute__ ((packed));

typedef struct stack
{
	u32     esp0;
	u16     ss0;
} stack __attribute__ ((packed));

typedef struct task
{
	tid_t tid;
	stack kernel_stack;
	u32	status;
	regs    dump;
      TAILQ_ENTRY(task) tailq;
} task __attribute__ ((packed));

typedef TAILQ_HEAD(task_s, task) task_t;

typedef struct child
{
	pid_t	pid;
      TAILQ_ENTRY(child) tailq;
} child __attribute__ ((packed));

typedef TAILQ_HEAD(child_s, child) child_t;

typedef struct other
{
	pid_t	pid;
      TAILQ_ENTRY(other) tailq;
} other __attribute__ ((packed));

typedef TAILQ_HEAD(other_s, other) other_t;

typedef struct process
{
	pid_t   pid;
	pid_t   parent;
	bool    iskernel;
	pd     *pdd;
	s8	  priority;
	u32     result;
	u8      status;
	u8     *exec_low;
	u8     *exec_high;
	u8     *bss_low;
	u8     *bss_high;
	page_t  page_head;
	task_t  task_head;
      child_t child_head;
	other_t other_head;
	u32     entry;
} process __attribute__ ((packed));

pid_t     getcurrentpid(void);
pid_t     getparentpid(void);
pid_t     getfreepid(void);
tid_t	getcurrenttid(void);
tid_t	maketid(pid_t pid, u32 number);

void      stop(void);
void      wait(void);
pid_t     fork(void);
pid_t     clone(void);
pid_t     exec(u8* entry, u8* args, bool kerneltask);

void      switchtask(tid_t tid);
tid_t     getnexttask(void);

task* findtask(tid_t tid);
task* findcurrenttask(void);
process* findprocess(pid_t pid);
process* findcurrentprocess(void);

tid_t createtask(pid_t pid,u8 *entry, bool kerneltask);
void      deletetask(tid_t tid);
void      runtask(tid_t tid);
void      stoptask(tid_t tid);

pid_t createprocess(u8 *src, bool kerneltask);
void      deleteprocess(pid_t pid);
void      runprocess(pid_t pid);
void      stopprocess(pid_t pid);


void      setpriority(pid_t pid,s8 priority);

void      initprocesses(void);

u32     iself(u8 * src);
u32     loadelf(u8 * src, pid_t pid);
