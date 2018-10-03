/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
typedef struct command
{	
u8 name[64];
u8 params[64];
int (*function)(void)
} command  __attribute__ ((packed));	

int rebootnow();
int test2d();
int readidt();
int readgdt();
int detectcpu();
int mode();
int clear();
int regs();
int info();
int pgfaultr();
int pgfaultw();
int divzerr();
