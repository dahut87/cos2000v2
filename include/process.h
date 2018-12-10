/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "types.h"
#include "interrupts.h"
#include "memory.h"

#define MAXPIDVALUE	    0xFFFF
#define MAXNUMPROCESS	256

#define STATUS_FREE   0x0
#define STATUS_ZOMBIE 0xFF
#define STATUS_READY  0xF0
#define STATUS_RUN    0x1
#define STATUS_SLEEP  0x2

/* ELF type */
#define ET_NONE 	0 	    //No file type
#define ET_REL 	    1 	    //Relocatable file
#define ET_EXEC 	2 	    //Executable file
#define ET_DYN 	    3 	    //Shared object file
#define ET_CORE 	4 	    //Core file
#define ET_LOOS 	0xfe00 	//Operating system-specific
#define ET_HIOS 	0xfeff 	//Operating system-specific
#define ET_LOPROC 	0xff00 	//Processor-specific
#define ET_HIPROC 	0xffff 	//Processor-specific

/* ELF identification */
#define	EI_MAG0		    0       //File identification
#define	EI_MAG1		    1       //File identification
#define	EI_MAG2		    2       //File identification
#define	EI_MAG3		    3       //File identification
#define	EI_CLASS	    4       //File class
#define	EI_DATA		    5       //Data encoding
#define	EI_VERSION	    6       //File version
#define	EI_OSABI 	    7 	    //Operating system/ABI identification
#define	EI_ABIVERSION 	8 	    //ABI version
#define	EI_PAD 	        9 	    //Start of padding bytes
#define	EI_NIDENT 	    16 	    //Size of e_ident[]   

/* ELF version */
#define EV_NONE 	0 	//Invalid version
#define EV_CURRENT 	1 	//Current version

/* ELF machine type */

#define EM_NONE     0 	//No machine
#define EM_386 	    3 	//Intel 80386
#define EM_IA_64 	50 	//Intel IA-64 processor architecture
#define EM_X86_64 	62 	//AMD x86-64 architecture

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
#define ELFOSABI_NONE 	0 	//No extensions or unspecified
#define ELFOSABI_LINUX 	3 	//Linux
#define ELFOSABI_COS2000 16 //COS2000

/* ELF header */
typedef struct elf32 {
        u8  e_ident[EI_NIDENT];
        u16      e_type;
        u16      e_machine;
        u32      e_version;
        u8*      e_entry;
        u32      e_phoff;
        u32      e_shoff;
        u32      e_flags;
        u16      e_ehsize;
        u16      e_phentsize;
        u16      e_phnum;
        u16      e_shentsize;
        u16      e_shnum;
        u16      e_shstrndx;
} elf32;

typedef struct elf32p{
	u32 	p_type;
	u32 	p_offset;
	u8* 	p_vaddr;
	u8* 	p_paddr;
	u32 	p_filesz;
	u32	    p_memsz;
	u32	    p_flags;
	u32 	p_align;
} elf32p;


typedef	struct stackdef {
		u32 esp0;
		u16 ss0;
	} stackdef __attribute__ ((packed));


typedef struct process {
	u32 pid;
    regs dump;
	stackdef kstack;
    pd *pdd;
    u32 result;
    u8 status;
    u8 *exec_low;
    u8 *exec_high;
    u8 *bss_low;
    u8 *bss_high;
    struct process *parent;
    page_t page_head;
    u32 entry;
} process __attribute__ ((packed));

void task_init();
u32 task_getfreePID ();
u32 task_usePID (u32 pid);
u32 task_create();
u32 elf_test(u8 *src);
u32 elf_load(u8 *src, u32 pid);
process *getcurrentprocess();
void task_run(u32 pid);
