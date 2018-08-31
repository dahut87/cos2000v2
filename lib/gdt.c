#include "gdt.h"
#include "asm.h"
#include "types.h"

/*******************************************************************************/

/* Initialise la GDT */

void initgdt()
{
	
}

/*******************************************************************************/

/* Créé un descripteur GDT */

void makegdtdes(u32 base, u32 limite, u8 acces, u8 flags, gdtdes *desc)
{
        desc->lim0_15 = (limite & 0xffff);
        desc->base0_15 = (base & 0xffff);
        desc->base16_23 = (base & 0xff0000) >> 16;
        desc->acces = acces;
        desc->lim16_19 = (limite & 0xf0000) >> 16;
        desc->flags = (flags & 0xf);
        desc->base24_31 = (base & 0xff000000) >> 24;
        return;
}



