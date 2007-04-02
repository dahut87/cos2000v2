#include "types.h"

#define TEXTSCREEN 0xB8000	/* debut de la memoire video texte*/
#define GRPHSCREEN 0xA0000	/* debut de la memoire video graphique*/

typedef u8 mode_def[64]; 

void print (u8* string);
void cls (void);
u16 setvmode(u8);
