/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"

#ifndef _INTERRUPTS
#define _INTERRUPTS

#define PIC1_CMD    0x20 /*PIC 8259A Commandes n°1 */
#define PIC1_DATA   0x21 /*PIC 8259A Données n°1 */
#define PIC2_CMD    0xa0 /*PIC 8259A Commandes n°2 */
#define PIC2_DATA   0xa1 /*PIC 8259A Données n°1 */

#define ICW1_ICW4       0x01    /* ICW4 ou pas*/
#define ICW1_SINGLE     0x02    /* mode seul ou cascadé */
#define ICW1_INTERVAL4  0x04    /* adresses appel d'interval 4 ou 8 */
#define ICW1_LEVEL      0x08    /* déclenchement sur niveau ou sur front */
#define ICW1_INIT       0x10    /* Initialization */
 
#define ICW4_8086       0x01    /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02    /* Auto EOI ou normal */
#define ICW4_BUF_SLAVE  0x08    /* mode/slave avec tampon*/
#define ICW4_BUF_MASTER 0x0C    /* mode/master avec tampon*/
#define ICW4_SFNM       0x10    /* Complètement lié ou non */

#define INTGATE     0x0E00  /* utilise pour gerer les interruptions */
#define TRAPGATE    0x0F00 /* utilise pour faire des appels systemes */
#define TASKGATE    0x0500 /* utilise pour commuter des taches */
#define CALLGATE    0x0C00 /* utilise pour appeler du code */
#define LDTDES      0x0200 /* utilise pour pointer une LDT */

#define ENTRY_PRESENT     0b1000000000000000  /* Segment défini (obligatoire) */

#define ENTRY_STORAGE     0b0001000000000000  /* Segment défini (obligatoire) */

#define ENTRY_RING0       0b0000000000000000  /* Segment anneau 0 */
#define ENTRY_RING1       0b0010000000000000  /* Segment anneau 1 */
#define ENTRY_RING2       0b0100000000000000  /* Segment anneau 2 */
#define ENTRY_RING3       0b0110000000000000  /* Segment anneau 3 */

/* 00-11-010-0 : Compteur 0 - LSB puis MSB - generateur taux - binaire */
#define TIMER0         0x40 /* port E/S pour le timer canal 0 */
#define TIMER_MODE     0x43 /* port E/S pour le mode controle du timer */
#define RATE_GENERATOR 0x34 /* générateur de fréquence */
#define SQUARE_WAVE    0x36 /* générateur d'onde carrée */
#define TIMER_FREQ     1193180 /* fréquence pour timer dans un PC ou AT */
#define HZ             100  /* Fréquence d'horloge (ajutste logiciellement sur IBM-PC) */

/* save pile */
typedef struct save_stack {
   u64 efer;
   u32 dr7;
   u32 dr6;
   u32 dr5;
   u32 dr4;
   u32 dr3;
   u32 dr2;
   u32 dr1;
   u32 dr0;
   u32 cr4;
   u32 cr3;
   u32 cr2;
   u32 cr0;
   u32 eflags;
   u32 ss;
   u32 gs;
   u32 fs;
   u32 es;
   u32 ds;
   u32 eip;
   u32 cs;
   u32 esp;
   u32 ebp;
   u32 edi;
   u32 esi;
   u32 edx;
   u32 ecx;
   u32 ebx;
   u32 eax;
} save_stack __attribute__ ((packed));
/* exception pile */
typedef struct exception_stack {
    u32 error_code;
    u32 eip;
    u32 cs;
    u32 eflags;
} exception_stack __attribute__ ((packed));
/* sans code erreur */
typedef struct exception_stack_noerror {
    u32 eip;
    u32 cs;
    u32 eflags;
} exception_stack_noerror __attribute__ ((packed));


/* descripteur de segment */
typedef struct idtdes {
	u16 offset0_15;	
	u16 select;
	u16 type;
	u16 offset16_31;	
} idtdes  __attribute__ ((packed));


struct idtr {
	u16 limite;
	u32 base;
} __attribute__ ((packed));

 void initidt(void);
 void setidt(u32 offset, u16 select, u16 type,u16 index);
 void makeidtdes(u32 offset, u16 select, u16 type, idtdes* desc);
 void initpic(void);
 void enableirq(u8 irq);
 void disableirq(u8 irq);
 void cpuerror(const u8 * src, const save_stack *stack);

#endif

