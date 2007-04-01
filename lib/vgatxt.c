#include "vga.h"
#include "string.h"
#include "asm.h"

#include "modes.c"
#include "8x8fnt.c"

#define sequencer 0x3c4
#define misc 0x3c2
#define ccrt 0x3D4
#define attribs 0x3c0
#define graphics 0x3ce
#define state 0x3da


static u16 resX,resY,cursX,cursY;  /* resolution x,y en caractères et position du curseur */
static u16 pages,pagesize,activepage; /* nombre de pages disponibles et taille d'une page */
static u8 vmode; /* mode en cours d'utilisation */
static u16 basemem; /* debut de la mémoire vidéo */
static bool scrolling,graphic; /* Activation du défilement, Flag du mode graphique */


void print (u8* string)
{
	u8 *source,*screen;		
	source = string;
	screen = (u8 *)TEXTSCREEN;
	while(*source!=0x00)
	{	
		*screen = *source;	
		screen+=2;
		source++;
	}
}

void cls (void)
{
	memset((u8 *)TEXTSCREEN,0x20,(80*25*2),2);
}


u16 setvmode(u8 choosedmode)
{
	u8 *def,i,mode;
  
  mode=choosedmode&0x7F;
	if (choosedmode>0x7F)
	{
		if (mode>maxgraphmode) return 1; /* mode inexistant */	
		def=(u8 *)&graphmodes[mode];
		graphic=true;
	}
	else
	{
		if (mode>maxtextmode) return 1; /* mode inexistant */	
		def=(u8 *)&textmodes[mode];
		graphic=false;
	}
	outb(misc,*def++);
	outb(state,*def++);
	for(i=0;i<5;i++)
	{
	 	outb(sequencer,i);
	 	outb(sequencer+1,*def++);	
	}
	outb(ccrt,0x11);
	outb(ccrt+1,0x0E);
	for(i=0;i<25;i++)
	{
	 	outb(ccrt,i);
	 	outb(ccrt+1,*def++);	
	}
	for(i=0;i<9;i++)
	{
	 	outb(graphics,i);
	 	outb(graphics+1,*def++);	
	}
	inb(state);	
 for(i=0;i<21;i++)
	{
		inb(attribs);
	 	outb(attribs,i);
	 	outb(attribs,*def++);	
	}
	inb(state);
	outb(attribs,0x20);
	return 0;
}
