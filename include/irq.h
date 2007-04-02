#define INTGATE 0x8E00  /* utilise pour gerer les interruptions */
#define TRAPGATE 0x8F00 /* utilise pour faire des appels systemes */
#define TASKGATE 0x8500 /* utilise pour commuter des taches */
#define CALLGATE 0x8C00 /* utilise pour appeler du code */
#define LDTDES 0x8200 /* utilise pour pointer une LDT */

/* descripteur de segment */
typedef struct idtdesc {
	u16 offset0_15;	
	u16 select;
	u16 type;
	u16 offset16_31;	
} idtdesc  __attribute__ ((packed));

/* registre IDTR */
struct idtr {
	u16 limite;
	u32 base;
}  __attribute__ ((packed));
