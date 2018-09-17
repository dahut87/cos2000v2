#include <types.h>

#define	SEL_KERNEL_CODE 0x8 /* selecteur code du kernel */
#define	SEL_KERNEL_DATA 0x10 /* selecteur data du kernel */
#define	SEL_KERNEL_STACK 0x18 /* selecteur pile du kernel */
#define	STACK_OFFSET 0x20000 /* adresse de la pile du kernel */

#define SIZEGDT		0x4	/* nombre de descripteurs */

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



