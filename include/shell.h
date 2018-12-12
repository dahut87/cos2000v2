/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
typedef struct command
{
	u8      name[64];
	u8      params[64];
int     (*function) ()} command __attribute__ ((packed));

int     rebootnow();
int     test2d();
int     test3d();
int     showidt();
int     showgdt();
int     detectcpu();
int     mode();
int     clear();
int     showregs();
int     showinfo();
int     err();
int     view();
int     test(void);
int     disas(u8 * commandline);
int     bpset(u8 * commandline);
int     bpclr(u8 * commandline);
int     sfont(u8 * commandline);
int     help();
int     logo();
int     detectpci();
int     showmem();
int     testmem();
int     testcall();
int     testtask();
