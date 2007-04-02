#include "types.h"

#define TEXTSCREEN 0xB8000	/* debut de la memoire video texte*/
#define GRPHSCREEN 0xA0000	/* debut de la memoire video graphique*/

typedef u8 mode_def[64]; 

void 	print (char* string);
void 	clearscreen (void);
u16 	setvmode(u8);
void 	showchar (u8 thechar);
void 	showhex (u8 src);
void 	fillscreen (u8 color);
u8 	getfont(u8 num);
void 	setfont(u8 num);
void 	gotoscreen(u16 x,u16 y);