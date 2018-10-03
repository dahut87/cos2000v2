#include <types.h>

/* Ordre imposé par SYSENTER */
#define	SEL_KERNEL_CODE  0x8    /* Selecteur code du kernel */
#define	SEL_KERNEL_STACK 0x10  /* Selecteur pile du kernel */
#define	SEL_USER_CODE    0x18     /* Selecteur code utilisateur */
#define	SEL_USER_STACK   0x20    /* Selecteur pile utilisateur */
#define	SEL_KERNEL_DATA  0x28   /* Selecteur data du kernel */
#define	SEL_USER_DATA    0x30     /* Selecteur data utilisateur */

#define	SEL_TSS 0x38 /* Selecteur TSR */

#define GDT_SIZE		0x8	/* Nombre de descripteurs */

/* Drapeau des descripteurs GDT  */ 
#define GRANULARITY_4K  0b00001000  /* Granularité alignement 4096 octet*/
#define GRANULARITY_1B  0b00000000  /* Granularité alignement 1 octet */
#define OPSIZE_32B      0b00000100  /* Taille opérandes 32 bits*/
#define OPSIZE_16B      0b00000000  /* Taille opérandes 16 bits*/
#define SYS_AVAILABLE   0b00000001  /* Disponible pour le système (à définir)*/

#define SEG_ACCESSED    0b00000001  /* Segment accédé (code ou data) */

#define SEG_DATA        0b00000000  /* Segment de données */
#define SEG_CODE        0b00001000  /* Segment de données */

#define SEG_READ_WRITE  0b00000010  /* Segment lecture-ecriture (data) */
#define SEG_EXPAND_DOWN 0b00000100  /* Segment avec expand-down (data) */

#define SEG_READ        0b00000010  /* Segment lecture          (code) */
#define SEG_CONFORMING  0b00000100  /* Segment conforming       (code) */

#define SEG_PRESENT     0b10000000  /* Segment défini (obligatoire) */

#define SEG_RING0       0b00000000  /* Segment anneau 0 */
#define SEG_RING1       0b00100000  /* Segment anneau 0 */
#define SEG_RING2       0b01000000  /* Segment anneau 0 */
#define SEG_RING3       0b01100000  /* Segment anneau 0 */

#define SEG_NORMAL      0b00010000  /* Segment normal pile/data/code (0 pour système) */

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
u32 getdesbase(u16 sel);
u8 getdestype(u16 sel);
u32 getdessize(u16 sel);
u32 getdeslimit(u16 sel);
bool isdesvalid(u16 sel);
u8 getdesbit3(u16 sel);
u32 getdesdpl(u16 sel);
u16 getdesalign(u16 sel);
