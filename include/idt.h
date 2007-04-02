#include "types.h"



#define INTGATE 0x8E00  /* utilise pour gerer les interruptions */
#define TRAPGATE 0x8F00 /* utilise pour faire des appels systemes */
#define TASKGATE 0x8500 /* utilise pour commuter des taches */
#define CALLGATE 0x8C00 /* utilise pour appeler du code */
#define LDTDES 0x8200 /* utilise pour pointer une LDT */

/* descripteur de segment */
typedef struct idtdes {
	u16 offset0_15;	
	u16 select;
	u16 type;
	u16 offset16_31;	
} idtdes  __attribute__ ((packed));



 void initidt(void);
 void setidt(u32 offset, u16 select, u16 type,u16 index);
 void makeidtdes(u32 offset, u16 select, u16 type, idtdes* desc);
 void initpic(void);
 void enableirq(u8 irq);
 void disableirq(u8 irq);

 

 

