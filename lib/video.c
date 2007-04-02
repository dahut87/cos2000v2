#include "vga.h"
#include "video.h"

/*******************************************************************************/

/* affiche une chaine de caract�re a l'�cran */

void print(u8* string)
{
	u8 *source;		
	source = string;
	while(*source!=0x00)
        {
        showchar(*source++);
        }
}

/*******************************************************************************/

/* affiche un octet sous forme hexad�cimale a l'ecran */

void showhex(u8 src)
{
	static u8 hexadigit[16] = "0123456789ABCDEF";
	showchar(hexadigit[(src&0xF0)>>4]);
	showchar(hexadigit[src&0x0F]);
}

/*******************************************************************************/
