#include "gdt.h"
#include "asm.h"
#include "types.h"

#define SIZEGDT		0x4	/* nombre de descripteurs */

#define BASEGDT		0x00000800	/* addr de la GDT */

 /* registre gdt */
static struct gdtr gdtreg;

/* table de GDT */
static gdtdes gdt[SIZEGDT];

/*******************************************************************************/

/* Initialise la GDT */

void initgdt()
{
	makegdtdes(0x0, 0x00000, 0x00, 0x00, &gdt[0]);
	makegdtdes(0x0, 0xFFFFF, 0x9B, 0x0D, &gdt[1]);	/* code */
	makegdtdes(0x0, 0xFFFFF, 0x93, 0x0D, &gdt[2]);	/* data */
	makegdtdes(0x0, 0x00000, 0x97, 0x0D, &gdt[3]);   /* pile */
	/* initialise le registre gdt */
	gdtreg.limite = SIZEGDT * 8;
	gdtreg.base = BASEGDT;
	/* recopie de la GDT a son adresse */
	memcpy(&gdt, (u8 *) gdtreg.base, gdtreg.limite, 1);
	/* chargement du registre GDT */
	lgdt(&gdtreg);
	/* initialisation des segments */
	asm("   movw $0x10, %ax	\n \
            movw %ax, %ds	\n \
            movw %ax, %es	\n \
            movw %ax, %fs	\n \
            movw %ax, %gs   \n \
            movl 0x0C(%esp), %ebx \n \
            movw $0x18, %ax \n \
            movw %ax, %ss \n \
            movl $0x20000, %esp \n \
            ljmp $0x08, $raz	\n \
            raz:		\n \
            pushl %ebx \n \
            ret\n");
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



