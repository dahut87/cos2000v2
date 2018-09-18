#include <types.h>

/* Ordre imposé par SYSENTER */
#define	SEL_KERNEL_CODE 0x8 /* selecteur code du kernel */
#define	SEL_KERNEL_STACK 0x10 /* selecteur pile du kernel */
#define	SEL_USER_CODE 0x18 /* selecteur code utilisateur */
#define	SEL_USER_STACK 0x20 /* selecteur pile utilisateur */
#define	SEL_KERNEL_DATA 0x28 /* selecteur data du kernel */
#define	SEL_USER_DATA 0x30 /* selecteur data utilisateur */

#define	SEL_TSS 0x38 /* selecteur TSR */

#define	STACK_OFFSET 0xFFFF /* adresse de la pile du kernel */

#define SIZEGDT		0x8	/* nombre de descripteurs */
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

typedef struct tss {
	u16 prevtask, reserved00;
	u32 esp0;
	u16 ss0, reserved0;
	u32 esp1;
	u16 ss1, reserved1;
	u32 esp2;
	u16 ss2, reserved2;
	u32 cr3;
	u32 eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	u16 es, reserved3;
	u16 cs, reserved4;
	u16 ss, reserved5;
	u16 ds, reserved6;
	u16 fs, reserved7;
	u16 gs, reserved8;
	u16 ldt_selector, reserved9;
	u16 trapflag, iomap;
} __attribute__ ((packed));

void inittr(void);
void initgdt(u32 offset);
void makegdtdes(u32 base, u32 limite, u8 acces, u8 flags, gdtdes *desc);
