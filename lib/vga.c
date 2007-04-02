#include "vga.h"
#include "memory.h"
#include "asm.h"
#include "port.h"

#include "modes.c"
#include "8x8fnt.c"
#include "8x16fnt.c"

/* Registres VGAs */
#define sequencer 0x3c4
#define misc 0x3c2
#define ccrt 0x3D4
#define attribs 0x3c0
#define graphics 0x3ce
#define state 0x3da
/* Taille d'un plan de bit */
#define planesize 0x10000


static u16 resX,resY;        /* resolution x,y en caractères*/
static u16 cursX,cursY;      /* position du curseur */		
static u8  pages,activepage; /* nombre de pages disponibles N° de la page active*/
static u32 linesize,pagesize;/* Taille d'une ligne et d'une page */
static u8  vmode=0xFF,color; /* mode en cours d'utilisation et profondeur */
static u32 basemem; 	     /* Adresse de la mémoire vidéo */
static bool scrolling,graphic;/* Activation du défilement, Flag du mode graphique */
static u8  font;	      /* n° font active */

/*******************************************************************************/

/* Attend la retrace verticale */

void waitvretrace (void)
{
	while ((inb(state)&8)==0);
}

/*******************************************************************************/

/* Attend la retrace horizontale */

void waithretrace (void)
{
	while ((inb(state)&1)==0);
}

/*******************************************************************************/

/* Active l'affichage du curseur de texte */

void enablecursor (void)
{
	u8 curs;
	/* active le curseur hardware */
	outb(ccrt, 10);
	curs = inb(ccrt+1)&~32;
	outb(ccrt+1, curs);	
	/* fixe la position du curseur hardware */
	gotoscr(cursX,cursY);	
}

/*******************************************************************************/

/* Desactive l'affichage du curseur de texte */

void disablecursor (void)
{
	u8 curs;
	/* desactive le curseur hardware */
	outb(ccrt, 10);
	curs = inb(ccrt+1)|32;
	outb(ccrt+1, curs);		
}

/*******************************************************************************/

/* Active le scrolling en cas de débordement d'écran */

void enablescroll (void)
{
	scrolling=true;
}

/*******************************************************************************/

/* Desactive le scrolling en cas de débordement d'écran */

void disablescroll (void)
{
	scrolling=false;
}

/*******************************************************************************/

/* Utilise le plan de bit spécifié */

void useplane(u8 plan)
{
	u8 mask;
	plan &= 3;
	mask = 1 << plan;
	/* choisi le plan de lecture */
	outb(graphics, 4);
	outb(graphics+1, plan);
	/* choisi le plan d'ecriture */
	outb(sequencer, 2);
	outb(sequencer+1, mask);
}

/*******************************************************************************/

/* Renvoie l'adresse du segment video */

u32 getbase(void)
{
	u32 base;
	outb(graphics, 6);
	base = inb(graphics+1);
	base >>= 2;
	base &= 3;
	switch(base)
	{
	case 0:
	case 1:
		base = 0xA0000;
		break;
	case 2:
		base = 0xB0000;
		break;
	case 3:
		base = 0xB8000;
		break;
	}
	return base;
}

/*******************************************************************************/

/* efface l'écran */

void (*cls)(void);

void cls_text (void)
{
	gotoscr(0,0);
	memset((u8 *)(basemem+activepage*pagesize),0,pagesize,1);
}

void cls_chain (void)
{
	gotoscr(0,0);
	memset((u8 *)(basemem+activepage*pagesize),0,pagesize,1);
}

void cls_unchain (void)
{
	gotoscr(0,0);
	int i;
	for(i=0;i<4;i++)
	{
		useplane(i);
		memset((u8 *)(basemem+activepage*pagesize),0,pagesize,1);
	}
}

/*******************************************************************************/

/* fixe la position du curseur texte */

void gotoscr(u16 x,u16 y)
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

/*******************************************************************************/

/* Fait defiler l'ecran de n lignes vers le haut */

void (*scroll)(u8 lines);

void scroll_unchain(u8 lines)
{
	u8 i;
	for(i=0;i<4;i++)
	{
		useplane(i);
		memcpy((u8*)(basemem+linesize*8*lines),(u8*)basemem,pagesize-linesize*8*lines,1);
		memset((u8*)(basemem+pagesize-linesize*8*lines),0,linesize*8*lines,1);
	}
}

void scroll_chain(u8 lines)
{
	memcpy((u8*)basemem+linesize*8*lines,(u8*)basemem,pagesize-linesize*8*lines,1);
	memset((u8*)(basemem+pagesize-linesize*8*lines),0,linesize*8*lines,1);
}

void scroll_text(u8 lines)
{
	memcpy((u8*)basemem+linesize*lines,(u8*)basemem,pagesize-linesize*lines,1);
	memset((u8*)(basemem+pagesize-linesize*lines),0,linesize*lines,1);
}

/*******************************************************************************/

/* Affiche le caractère a l'écran */

void (*showchar)(u8 thechar);

void showchar_graphic(u8 thechar)
{
	u8 x,y,pattern,color;
	for(y=0;y<8;y++)
	{
		pattern=font8x8[thechar*8+y];
		for(x=0;x<8;x++)
		{
			color=((pattern>>(7-x))&0x1);   /* mettre un ROL importé depuis asm */
			writepxl(cursX*8+x,cursY*8+y,color*7);
		}
	}
	if (++cursX>=resX)
	{
		cursX=0;
		if (++cursY>=resY)
		{
			scroll(1);
			cursY=resY-1;
		}
	}
}

void showchar_text(u8 thechar)
{
	u8 *screen;		
	screen = (u8 *)basemem+activepage*pagesize+2*(cursX+cursY*resX);
	*screen = thechar;
	*(++screen) = 0x07;
	if (++cursX>=resX)
	{
		cursX=0;
		if (++cursY>=resY)
		{
			scroll(1);
			cursY=resY-1;
		}
	}
	gotoscr(cursX,cursY);
}

/*******************************************************************************/

/* Ecrit un pixel a l'écran */

void (*writepxl)(u16 x, u16 y, u32 c);

void writepxl_1bit(u16 x, u16 y, u32 c)
{
	u8* off;
	u8 mask;
	c = (c & 1) * 0xFF;
	off = (u8*)(basemem+activepage*pagesize+linesize * y + x / 8);
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	*off= ((*off) & ~mask) | (c & mask);
}

void writepxl_2bits(u16 x, u16 y, u32 c)
{
	u8 *off;
	u8 mask;
	c = (c & 3) * 0x55;
	off = (u8*)(basemem+activepage*pagesize+linesize * y + x / 4);
	x = (x & 3) * 2;
	mask = 0xC0 >> x;
	*off= ((*off) & ~mask) | (c & mask);
}

void writepxl_4bits(u16 x, u16 y, u32 c)
{
	u8*  off;
	u8   mask, p, pmask;
	off = (u8*)(basemem+activepage*pagesize+linesize * y + x / 8);
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	pmask = 1;
	for(p = 0; p < 4; p++)
	{
		useplane(p);
		if(pmask & c)
		*off= ((*off) | mask);
		else
		*off= ((*off) & ~mask);
		pmask <<= 1;
	}
}

void writepxl_8bits(u16 x, u16 y, u32 c)
{
	u8* off;
	off = (u8*)(basemem+activepage*pagesize+linesize * y + x);
	*off=c;
}

void writepxl_8bitsunchain(u16 x, u16 y, u32 c)
{
	u8* off;
	off = (u8*)(basemem+activepage*pagesize+linesize * y + x / 4);
	useplane(x & 3);
	*off=c;
}

/*******************************************************************************/

/* Change le mode video courant */

u32 setvmode(u8 mode)
{
	u8 *def,i,gmode;
	/* Récupere la definition des registres VGA en fonction du mode
graphique : >0x80
text      : 0x00 - 0x7F
*/
	if (mode>=0x80)
	{
		gmode=mode-0x80;
		if (gmode>maxgraphmode) return 1; /* mode inexistant */	
		def=graphmodes[gmode];
		graphic=true;
	}
	else
	{
		if (mode>maxtextmode) return 1; /* mode inexistant */	
		def=textmodes[mode];
		graphic=false;
		loadfont(font8x8,8,1);
		loadfont(font8x16,16,0);
	}
	/* Initialise les registre "divers" */
	outb(misc,def[0]);
	/* Initialise les registre d'etat */
	outb(state,0x00);
	/* Initialise le séquenceur */
	outreg(sequencer,&def[1],5);
	/* Debloque le verouillage des registres controleur CRT */
	outb(ccrt,0x11);
	outb(ccrt+1,0x0E);
	/* Initialise le controleur CRT */	
	outreg(ccrt,&def[6],25);
	/* Initialise le controleur graphique */ 	
	outreg(graphics,&def[31],9);	 	
	inb(state);	
	/* Initialise le controleur d'attributs */	
	outregsame(attribs,&def[40],21);	
	inb(state);
	outb(attribs,0x20);
	/* Récupere depuis la table de définition des mode la résolution et la
profondeur (en bits) */
	resX=def[61];
	resY=def[62];
	color=def[63];	
	/* Remet la position du curseur logiciel a 0,0 */
	cursX=0;
	cursY=0;
	/* Initialise l'adresse des procedures de gestion graphique et les differentes
variables en fonction de la profondeur et du mode*/
	if (!graphic)
	{
		/* mode texte */
		linesize=resX*2;
		writepxl=NULL;           /* pas d'affichage de pixels */
		showchar=showchar_text;
		scroll=scroll_text;
		cls=cls_text;
		pagesize=resY*linesize;
	}
	else
	{
		switch(color)
		{
		case 1:
			/* mode N&B */
			linesize=resX;
			writepxl=writepxl_1bit;
			cls=cls_chain;
			scroll=scroll_chain;
			break;
		case 2:
			/* mode 4 couleurs */
			linesize=(resX<<1);
			writepxl=writepxl_2bits;
			cls=cls_chain;
			scroll=scroll_chain;
			break;
		case 4:
			/* mode 16 couleurs */
			linesize=resX;
			writepxl=writepxl_4bits;
			cls=cls_unchain;
			scroll=scroll_unchain;
			break;
		case 8:
			/* mode 256 couleurs */
			if (def[5]==0x0E)
			{
				/* mode chainé (plus rapide mais limité en mémoire) */
				linesize=(resX<<3);
				writepxl=writepxl_8bits;
				scroll=scroll_chain;
				cls=cls_chain;
			}
			else
			{
				/* mode non chainé */
				linesize=(resX<<1);
				writepxl=writepxl_8bitsunchain;
				scroll=scroll_unchain;
				cls=cls_unchain;
			}
			break;
		default:
			break;
		}
		showchar=showchar_graphic;
		pagesize=((resY*linesize)<<3);
	}
	/* calcul des variables d'état video */
	activepage=0;
	vmode=mode;
	pages=(planesize/pagesize);
	basemem=(def[20]<<8)+def[21]+getbase();		
	return 0;
}

/*******************************************************************************/

/* Récupère le mode vidéo en cours */

u8 getvmode(void)
{
	return vmode;
}

/*******************************************************************************/

/* Charge une nouvelle police de caractère */

u32 loadfont(u8* def,u8 size,u8 font)
{
	if (graphics==1) return 1;
	u8 oldregs[5]={0,0,0,0,0};
	u8* base;
	u16 i;
	if (font>7) return 1;	
	if (font<4)
	base = (u8 *)(getbase()+(font<<14));
	else
	base = (u8 *)(getbase()+((((font-4)<<1)+1)<<13));
	/* sauve les anciens registres */
	outb(sequencer,2);
	oldregs[0]=inb(sequencer+1);
	
	outb(sequencer,4);
	oldregs[1]=inb(sequencer+1);
	/* Adressage paire/impair desactivé (lineaire) */
	outb(sequencer+1, oldregs[1]|0x04);
	
	outb(graphics,4);
	oldregs[2]=inb(graphics+1);
	
	outb(graphics,5);
	oldregs[3]=inb(graphics+1);
	/* Adressage paire/impair desactivé (lineaire) */
	outb(graphics+1, oldregs[3]&~0x10);
	
	outb(graphics,6);
	oldregs[4]=inb(graphics+1);
	/* Adressage paire/impair desactivé (lineaire) */
	outb(graphics+1, oldregs[4] & ~0x02);
	/* utilisation du plan N°2 */
	useplane(2);	
	for(i=0; i < 256; i++)
	{
		memcpy(def,base+i*32,size,1);
		def += size;
	}

	outb(sequencer,2);
	outb(sequencer+1,oldregs[0]);
	outb(sequencer,4);
	outb(sequencer+1,oldregs[1]);
	outb(graphics,4);
	outb(graphics+1,oldregs[2]);
	outb(graphics,5);
	outb(graphics+1,oldregs[3]);
	outb(graphics,6);
	outb(graphics+1,oldregs[4]);
	return 0;
}

/*******************************************************************************/

/* Récupere le N° de la police de caractère en cours d'utilisation */

u8 getfont(u8 num)
{
	return font;	
}

/*******************************************************************************/

/* Fixe le N° de la police de caractère a utiliser */

void setfont(u8 num)
{
	font=num&0x07;
	outb(sequencer,3);
	outb(sequencer+1,(num&0x03)+((num&0x04)<<2));	
}

/*******************************************************************************/


