/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
typedef struct command
{	
u8 name[64];
u8 params[64];
int (*function)()
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
int err();
int view();
int test(void);
int disas(u8* commandline);
int bpset(u8* commandline);
int bpclr(u8* commandline);
int sfont(u8* commandline);
int help();
int logo();
