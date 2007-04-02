#include "types.h"

#define TEXTSCREEN 0xB8000	/* debut de la memoire video texte*/
#define GRPHSCREEN 0xA0000	/* debut de la memoire video graphique*/
typedef u8 mode_def[64];


u32 setvmode(u8);
u32 loadfont(u8* def,u8 size,u8 font);
void gotoscr(u16 x,u16 y);
void useplane(u8 plan);
u8 getfont(u8 num);
void setfont(u8 num);
void waitvretrace (void);
void waithretrace (void);
void enablecursor (void);
void disablecursor (void);
void enablescroll (void);
void disablescroll (void);
void (*writepxl)(u16 x, u16 y, u32 c);
void (*showchar)(u8 thechar);
void (*cls)(void);
void (*scroll)(u8 lines);

