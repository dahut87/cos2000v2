#include <types.h>

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



