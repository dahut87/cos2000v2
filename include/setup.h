/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"

typedef struct entrye820 {
	u64 addr;
	u64 size;
	u32 type;
} entrye820 __attribute__((packed));

typedef struct miniregs {
	union {
		struct {
			u32 eax;
			u32 ebx;
			u32 ecx;
			u32 edx;
			u32 esi;
			u32 edi;
			u32 ebp;
			u32 eflags;
		};
		struct {
			u16 ax, hax;
			u16 bx, hbx;
			u16 cx, hcx;
			u16 dx, hdx;
			u16 si, hsi;
			u16 di, hdi;
			u16 bp, hbp;
			u16 flags, hflags;
		};
		struct {
			u8 bl, bh, hbx2, hbx3;
			u8 dl, dh, hdx2, hdx3;
			u8 cl, ch, hcx2, hcx3;
			u8 al, ah, hax2, hax3;
		};
	};
} miniregs  __attribute__ ((packed));
