#include "types.h"

#define TEXTSCREEN 0xB8000	/* debut de la memoire video texte*/
#define GRPHSCREEN 0xA0000	/* debut de la memoire video graphique*/

typedef u8 mode_def[64];


u32 setvmode(u8);
u32 loadfont(u8* def,u8 size,u8 font);
void gotoscr(u16 x,u16 y);
void useplane(u8 plan);
u8 getfont();
void setfont(u8 num);
void waitvretrace (void);
void waithretrace (void);
void enablecursor (void);
void disablecursor (void);
void enablescroll (void);
void disablescroll (void);
void (*writepxl)(u16 x, u16 y, u32 c);
void (*showchar)(u16 coordx,u16 coordy,u8 thechar,u8 attrib);
void (*fill)(u8 attrib);
void (*scroll)(u8 lines,u8 attrib);
void split(u16 x);
void showpage(u8 page);
u8 getpage();
void setpage(u8 page);
u16 getnbpages();
u16 getyres();
u8 getdepth();
u16 getxres();
u8 getfont2();
void enableblink();
void disableblink();
u8 (*getchar)(u16 coordx,u16 coordy);
u8 (*getattrib)(u16 coordx,u16 coordy);
