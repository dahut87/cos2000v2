#include "types.h"

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

#define INTGATE 0x8E00  /* utilise pour gerer les interruptions */
#define TRAPGATE 0x8F00 /* utilise pour faire des appels systemes */
#define TASKGATE 0x8500 /* utilise pour commuter des taches */
#define CALLGATE 0x8C00 /* utilise pour appeler du code */
#define LDTDES 0x8200 /* utilise pour pointer une LDT */

/* 00-11-010-0 : Compteur 0 - LSB puis MSB - generateur taux - binaire */
#define TIMER0         0x40 /* port E/S pour le timer canal 0 */
#define TIMER_MODE     0x43 /* port E/S pour le mode controle du timer */
#define RATE_GENERATOR 0x34 /* générateur de fréquence */
#define SQUARE_WAVE    0x36 /* générateur d'onde carrée */
#define TIMER_FREQ     1193180 /* fréquence pour timer dans un PC ou AT */
#define HZ             100  /* Fréquence d'horloge (ajutste logiciellement sur IBM-PC) */


/* descripteur de segment */
typedef struct idtdes {
	u16 offset0_15;	
	u16 select;
	u16 type;
	u16 offset16_31;	
} idtdes  __attribute__ ((packed));


struct dtr {
	u16 limite;
	u32 base;
} __attribute__ ((packed));

 void initidt(void);
 void setidt(u32 offset, u16 select, u16 type,u16 index);
 void makeidtdes(u32 offset, u16 select, u16 type, idtdes* desc);
 void initpic(void);
 void enableirq(u8 irq);
 void disableirq(u8 irq);
 void cpuerror(const u8 *src);
 

 

