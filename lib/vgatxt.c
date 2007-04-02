#include "vga.h"
#include "memory.h"
#include "port.h"
#include "asm.h"

#include "modes.c"
#include "fonts.c"

#define sequencer	0x3c4
#define misc 		0x3c2
#define ccrt 		0x3D4
#define attribs 	0x3c0
#define graphics 	0x3ce
#define state 		0x3da

static u16 resX,resY,cursX,cursY;  		/* resolution x,y en caractères et position du curseur */
static u16 pages,pagesize,activepage,showedpage;/* nombre de pages disponibles et taille d'une page ansi que la page active et affiché en cours*/
static u8  vmode=0xFF; 				/* mode en cours d'utilisation */
static u32 basemem; 				/* debut de la mémoire vidéo */
static bool scrolling,graphic;			/* Activation du défilement, Flag du mode graphique */
static u8  font;				/* n° font actuelle */

void showchar (u8 thechar)
{
	u8 *screen;		
	screen = (u8 *)basemem+activepage*pagesize+2*(cursX+cursY*resX);
	*screen = thechar;
	*(++screen) = 0x07;
	if (cursX++>=resX) 
	{
	cursX=0;
	cursY++;
	} 
	gotoscreen(cursX,cursY);
}

void print (char* string)
{
	u8 *source;		
	source = string;
	while(*source!=0x00) showchar (*source++);
}


void showhex (u8 src)
{
	static u8 hexadigit[16] = "0123456789ABCDEF";
	showchar(hexadigit[(src&0xF0)>>4]);
	showchar(hexadigit[src&0x0F]); 
}

void clearscreen (void)
{
	gotoscreen(0,0);
	memset((u8 *)(basemem+activepage*pagesize),0,2*(resY*resX),1);
}

void gotoscreen(u16 x,u16 y)
{
	u16 pos;
	pos=(cursX+cursY*resX)/2;
	cursX=x;
	cursY=y;
	outb(ccrt,0x0E);
	outb(ccrt+1,(u8)(pos&0x00FF));	
	outb(ccrt,0x0D);
	outb(ccrt+1,(u8)((pos&0xFF00)>>8));				
}

void fillscreen (u8 color)
{
	memset((u8 *)(basemem+activepage*pagesize+1),color,resY*resX,2);
}


void loadfont(u8 num,u8 *font,u8 size)
{
	u16	seqinit[4] 	= { 0x0100, 0x0402, 0x0704, 0x0300 },
		graphinit[3]  	= { 0x0204, 0x0005, 0x0006 },
		seqend [4] 	= { 0x0100, 0x0302, 0x0304, 0x0300 },
		graphend[3]  	= { 0x0004, 0x1005, 0x0E06 };
	u8 	*dest,*src;
	u8	i;
	dest=(u8 *)GRPHSCREEN;
	src=font;
	initreg(sequencer,(u8 *)seqinit  ,4);
	initreg(graphics ,(u8 *)graphinit,3);
	if (num>3)
	dest+= (((num-4)<<1)+1)<<13;
	else
	dest+=num<<14;	
	for(i=0;i<250;i++)
	{
		memcpy(dest,src,size);
		src+=size;
		dest+=size;
		memset(dest,0,(32-size),1);
		dest+=(32-size);
	}
	initreg(sequencer,(u8 *)seqend  ,4);
	initreg(graphics ,(u8 *)graphend,3);
}

u8 getfont(u8 num)
{
	return font;	
}

void setfont(u8 num)
{
	font=num&0x07;
	outb(sequencer,3);
	outb(sequencer+1,num&0x03+(num&(0x04)<<2));	
}

u16 setvmode(u8 choosedmode)
{
	u8 *def,mode;
	mode=(choosedmode&0x7F);
	if (vmode==0xFF) 
	{
	loadfont(1 ,(u8 *)font8x8, 8);
	}
	if (choosedmode>0x7F)
	{
		if (mode>=maxgraphmode) return 1; /* mode inexistant */	
		def=(u8 *)&graphmodes[mode];
		graphic=true;
	}
	else
	{
		if (mode>=maxtextmode) return 1; /* mode inexistant */	
		def=(u8 *)&textmodes[mode];
		graphic=false;
	}
	outb(misc,def[0]);
	outb(state,def[1]);
	outreg(sequencer,&def[2],5);
	outb(ccrt,0x11);
	outb(ccrt+1,0x0E);	
	outreg(ccrt,&def[7],25);	
	outreg(graphics,&def[32],9);	 	
	inb(state);		
	outregsame(attribs,&def[41],21);	
	outb(attribs,0x20);
	resX=def[63];
	resY=def[64];
	cursX=0;
	cursY=0;
	pagesize=2*resX*resY;
	activepage=0;
	vmode=mode;
	pages=32768/pagesize;
	basemem=(def[21]<<8)+def[22]+TEXTSCREEN;		
	return 0;
}