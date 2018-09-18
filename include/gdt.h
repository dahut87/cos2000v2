#include <types.h>

/* Ordre imposé par SYSENTER */
#define	SEL_KERNEL_CODE 0x8 /* selecteur code du kernel */
#define	SEL_KERNEL_STACK 0x10 /* selecteur pile du kernel */
#define	SEL_USER_CODE 0x18 /* selecteur code utilisateur */
#define	SEL_USER_STACK 0x20 /* selecteur pile utilisateur */
#define	SEL_KERNEL_DATA 0x28 /* selecteur data du kernel */
#define	SEL_USER_DATA 0x30 /* selecteur data utilisateur */

#define	STACK_OFFSET 0xFFFF /* adresse de la pile du kernel */

#define SIZEGDT		0x7	/* nombre de descripteurs */
#define BASEGDT		0x00000800	/* addr de la GDT */


typedef struct gdtdes {
    u16 lim0_15;    
    u16 base0_15;
    u8 base16_23;
    u8 acces;
    u8 lim16_19 : 4;
    u8 flags : 4;
    u8 base24_31;
} gdtdes __attribute__ ((packed));

struct gdtr {
	u16 limite;
	u32 base;
} __attribute__ ((packed));

struct tss {
	u16 previous_task, __previous_task_unused;
	u32 esp0;
	u16 ss0, __ss0_unused;
	u32 esp1;
	u16 ss1, __ss1_unused;
	u32 esp2;
	u16 ss2, __ss2_unused;
	u32 cr3;
	u32 eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	u16 es, __es_unused;
	u16 cs, __cs_unused;
	u16 ss, __ss_unused;
	u16 ds, __ds_unused;
	u16 fs, __fs_unused;
	u16 gs, __gs_unused;
	u16 ldt_selector, __ldt_sel_unused;
	u16 debug_flag, io_map;
} __attribute__ ((packed));
