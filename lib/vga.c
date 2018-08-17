#include "vga.h"
#include "memory.h"
#include "asm.h"
#include "types.h"
#include "VGA/modes.c"
#include "VGA/8x8fnt.c"
#include "VGA/8x16fnt.c"

/* Registres VGAs */

#define sequencer 0x3c4
#define misc 0x3c2
#define ccrt 0x3D4
#define attribs 0x3c0
#define graphics 0x3ce
#define state 0x3da

/* Taille d'un plan de bit */

#define planesize 0x10000

static u16 resX, resY, color, splitY;	/* resolution x,y en caractères et profondeur */

static u8 pages, activepage, showedpage;	/* nombre de pages disponibles N° de la page active */
static u32 linesize, pagesize;	/* Taille d'une ligne et d'une page */
static u8 vmode = 0xFF;		/* mode en cours d'utilisation */
static u32 basemem;		/* Adresse de la mémoire vidéo */
static bool scrolling, graphic, blink;	/* Activation du défilement, Flag du mode graphique */

/*******************************************************************************/

/* Donne la resolution max horizontale */

u16 getxres()
{
	return resX;
}

/*******************************************************************************/

/* Donne la profondeur en bit */

u8 getdepth()
{
	return color;
}

/*******************************************************************************/

/* Donne la resolution max verticale */

u16 getyres()
{
	return resY - splitY;
}

/*******************************************************************************/

/* Donne le nombre max de page ecran dispo */

u16 getnbpages()
{
	return pages;
}

/*******************************************************************************/

/* Fixe la page ecran de travail */

void setpage(u8 page)
{
	if (page < pages)
		activepage = page;
}

/*******************************************************************************/

/* Recupere la page ecran de travail */

u8 getpage()
{
	return activepage;
}

/*******************************************************************************/

/* Affiche la page ecran specifié */

void showpage(u8 page)
{
	if (page < pages) {
		u16 addr;
		addr = page * pagesize / 2;
		outb(ccrt, 0x0C);
		outb(ccrt + 1, (addr >> 8));
		outb(ccrt, 0x0D);
		outb(ccrt + 1, (addr & 0xFF));
		showedpage = page;
	}
}

/*******************************************************************************/

/* Sépare l'écran en 2 a partir de la ligne Y */

void split(u16 y)
{
	u16 addr;
	if (graphic == 0)
		addr = (y << 3);
	else
		addr = y;
	/* line compare pour ligne atteinte */
	outb(ccrt, 0x18);
	outb(ccrt + 1, (addr & 0xFF));
	/* overflow pour le bit 8 */

	outb(ccrt, 0x07);
	outb(ccrt + 1, (inb(ccrt + 1) & ~16) | ((addr >> 4) & 16));

	/*  Maximum Scan Line pour le bit 9 */

	outb(ccrt, 0x09);
	outb(ccrt + 1, (inb(ccrt + 1) & ~64) | ((addr >> 3) & 64));
	splitY = y;
}

/*******************************************************************************/

/* Sépare l'écran en 2 a partir de la ligne Y */

void unsplit()
{
	/* line compare pour ligne atteinte */
	outb(ccrt, 0x18);
	outb(ccrt + 1, 0);
	/* overflow pour le bit 8 */

	outb(ccrt, 0x07);
	outb(ccrt + 1, inb(ccrt + 1) & ~16);

	/*  Maximum Scan Line pour le bit 9 */

	outb(ccrt, 0x09);
	outb(ccrt + 1, inb(ccrt + 1) & ~64);
	splitY = 0;
}

/*******************************************************************************/

/* Attend la retrace verticale */

void waitvretrace(void)
{
	while ((inb(state) & 8) == 0) ;
}

/*******************************************************************************/

/* Attend la retrace horizontale */

void waithretrace(void)
{
	while ((inb(state) & 1) == 0) ;
}

/*******************************************************************************/

/* Active l'affichage du curseur de texte */

void enablecursor(void)
{
	u8 curs;
	/* active le curseur hardware */
	outb(ccrt, 10);
	curs = inb(ccrt + 1) & ~32;
	outb(ccrt + 1, curs);
}

/*******************************************************************************/

/* Desactive l'affichage du curseur de texte */

void disablecursor(void)
{
	u8 curs;
	/* Desactive le curseur hardware */
	outb(ccrt, 10);
	curs = inb(ccrt + 1) | 32;
	outb(ccrt + 1, curs);
}

/*******************************************************************************/

/* Active le scrolling en cas de débordement d'écran */

void enablescroll(void)
{
	scrolling = true;
}

/*******************************************************************************/

/* Desactive le scrolling en cas de débordement d'écran */

void disablescroll(void)
{
	scrolling = false;
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
	outb(graphics + 1, plan);
	/* choisi le plan d'ecriture */
	outb(sequencer, 2);
	outb(sequencer + 1, mask);
}

/*******************************************************************************/

/* Renvoie l'adresse du segment video */

u32 getbase(void)
{
	u32 base;
	outb(graphics, 6);
	base = inb(graphics + 1);
	base >>= 2;
	base &= 3;
	switch (base) {
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

void (*fill) (u8 attrib);

void fill_text(u8 attrib)
{
	memset((u8 *) (basemem + activepage * pagesize), ' ', pagesize / 2, 2);
	memset((u8 *) (basemem + activepage * pagesize + 1), attrib,
	       pagesize / 2, 2);
}

void fill_chain(u8 attrib)
{
	memset((u8 *) (basemem + activepage * pagesize), attrib & 0x0F,
	       pagesize, 1);
}

void fill_unchain(u8 attrib)
{
	int i;
	for (i = 0; i < 4; i++) {
		useplane(i);
		memset((u8 *) (basemem + activepage * pagesize), attrib & 0x0F,
		       pagesize, 1);
	}
}

/*******************************************************************************/

/* fixe la position du curseur texte */

void gotoscr(u16 x, u16 y)
{
	u16 pos;
	if (splitY == 0)
		pos = (showedpage * pagesize / 2 + x + y * resX);
	else
		pos = (x + y * resX);
	outb(ccrt, 0x0F);
	outb(ccrt + 1, (u8) (pos & 0x00FF));
	outb(ccrt, 0x0E);
	outb(ccrt + 1, (u8) ((pos & 0xFF00) >> 8));
}

/*******************************************************************************/

/* Fait defiler l'ecran de n lignes vers le haut */

void (*scroll) (u8 lines, u8 attrib);

void scroll_unchain(u8 lines, u8 attrib)
{
	if (scrolling) {
		u8 i;
		for (i = 0; i < 4; i++) {
			useplane(i);
			memcpy((u8 *) (basemem + activepage * pagesize +
				       linesize * 8 * lines), (u8 *) basemem,
			       pagesize - linesize * 8 * lines, 1);
			memset((u8 *) (basemem + activepage * pagesize +
				       pagesize - linesize * 8 * lines),
			       attrib & 0x0F, linesize * 8 * lines, 1);
		}
	}
}

void scroll_chain(u8 lines, u8 attrib)
{
	if (scrolling) {
		memcpy((u8 *) basemem + activepage * pagesize +
		       linesize * 8 * lines,
		       (u8 *) basemem + activepage * pagesize,
		       pagesize - linesize * 8 * lines, 1);
		memset((u8 *) (basemem + activepage * pagesize + pagesize -
			       linesize * 8 * lines), attrib & 0x0F,
		       linesize * 8 * lines, 1);
	}
}

void scroll_text(u8 lines, u8 attrib)
{
	if (scrolling) {
		memcpy((u8 *) basemem + activepage * pagesize +
		       linesize * lines, (u8 *) basemem + activepage * pagesize,
		       pagesize - linesize * lines, 1);
		memset((u8 *) (basemem + activepage * pagesize + pagesize -
			       linesize * lines - 2), ' ',
		       (linesize * lines) / 2, 2);
		memset((u8 *) (basemem + activepage * pagesize + pagesize -
			       linesize * lines - 1), attrib,
		       (linesize * lines) / 2, 2);
	}
}

/*******************************************************************************/

/* Affiche le caractère a l'écran */

void (*showchar) (u16 coordx, u16 coordy, u8 thechar, u8 attrib);

void showchar_graphic(u16 coordx, u16 coordy, u8 thechar, u8 attrib)
{
	u8 x, y, pattern, set;
	for (y = 0; y < 8; y++) {
		pattern = font8x8[thechar * 8 + y];
		for (x = 0; x < 8; x++) {
			set = ((pattern >> (7 - x)) & 0x1);	/* mettre un ROL importé depuis asm */
			if (set == 0)
				writepxl(coordx * 8 + x, coordy * 8 + y,
					 ((attrib & 0xF0) >> 8) * set);
			else
				writepxl(coordx * 8 + x, coordy * 8 + y,
					 (attrib & 0x0F) * set);
		}
	}
}

void showchar_text(u16 coordx, u16 coordy, u8 thechar, u8 attrib)
{
	u8 *screen;
	screen =
	    (u8 *) basemem + activepage * pagesize + 2 * (coordx +
							  coordy * resX);
	*screen = thechar;
	*(++screen) = attrib;
}

/*******************************************************************************/

/* Recupere le caractère a l'écran */

u8(*getchar) (u16 coordx, u16 coordy);

u8 getchar_text(u16 coordx, u16 coordy)
{
	u8 *screen;
	screen =
	    (u8 *) basemem + activepage * pagesize + 2 * (coordx +
							  coordy * resX);
	return *screen;
}

/*******************************************************************************/

/* Recupere les attributs a l'écran */

u8(*getattrib) (u16 coordx, u16 coordy);

u8 getattrib_text(u16 coordx, u16 coordy)
{
	u8 *screen;
	screen =
	    (u8 *) basemem + activepage * pagesize + 2 * (coordx +
							  coordy * resX) + 1;
	return *screen;
}

/*******************************************************************************/

/* Ecrit un pixel a l'écran */

void (*writepxl) (u16 x, u16 y, u32 c);

void writepxl_1bit(u16 x, u16 y, u32 c)
{
	u8 *off;
	u8 mask;
	c = (c & 1) * 0xFF;
	off = (u8 *) (basemem + activepage * pagesize + linesize * y + x / 8);
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	*off = ((*off) & ~mask) | (c & mask);
}

void writepxl_2bits(u16 x, u16 y, u32 c)
{
	u8 *off;
	u8 mask;
	c = (c & 3) * 0x55;
	off = (u8 *) (basemem + activepage * pagesize + linesize * y + x / 4);
	x = (x & 3) * 2;
	mask = 0xC0 >> x;
	*off = ((*off) & ~mask) | (c & mask);
}

void writepxl_4bits(u16 x, u16 y, u32 c)
{
	u8 *off;
	u8 mask, p, pmask;
	off = (u8 *) (basemem + activepage * pagesize + linesize * y + x / 8);
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	pmask = 1;
	for (p = 0; p < 4; p++) {
		useplane(p);
		if (pmask & c)
			*off = ((*off) | mask);
		else
			*off = ((*off) & ~mask);
		pmask <<= 1;
	}
}

void writepxl_8bits(u16 x, u16 y, u32 c)
{
	u8 *off;
	off = (u8 *) (basemem + activepage * pagesize + linesize * y + x);
	*off = c;
}

void writepxl_8bitsunchain(u16 x, u16 y, u32 c)
{
	u8 *off;
	off = (u8 *) (basemem + activepage * pagesize + linesize * y + x / 4);
	useplane(x & 3);
	*off = c;
}

/*******************************************************************************/

/* Change le mode video courant */

u32 setvmode(u8 mode)
{
	u8 *def, gmode;
	/* Récupere la definition des registres VGA en fonction du mode
	   graphique : >0x80
	   text      : 0x00 - 0x7F
	 */
	if (mode >= 0x80) {
		gmode = mode - 0x80;
		if (gmode > maxgraphmode)
			return 1;	/* mode inexistant */
		def = graphmodes[gmode];
		graphic = true;
	} else {
		if (mode > maxtextmode)
			return 1;	/* mode inexistant */
		def = textmodes[mode];
		graphic = false;
		loadfont(font8x8, 8, 1);
		loadfont(font8x16, 16, 0);
	}
	/* Initialise les registre "divers" */
	outb(misc, def[0]);
	/* Initialise les registre d'etat */
	outb(state, 0x00);
	/* Initialise le séquenceur */
	outreg(sequencer, &def[1], 5);
	/* Debloque le verouillage des registres controleur CRT */
	outb(ccrt, 0x11);
	outb(ccrt + 1, 0x0E);
	/* Initialise le controleur CRT */
	outreg(ccrt, &def[6], 25);
	/* Initialise le controleur graphique */
	outreg(graphics, &def[31], 9);
	inb(state);
	/* Initialise le controleur d'attributs */
	outregsame(attribs, &def[40], 21);
	inb(state);
	outb(attribs, 0x20);
	/* Récupere depuis la table de définition des mode la résolution et la
	   profondeur (en bits) */
	resX = def[61];
	resY = def[62];
	color = def[63];
	/* Initialise l'adresse des procedures de gestion graphique et les differentes
	   variables en fonction de la profondeur et du mode */
	if (!graphic) {
		/* mode texte */
		linesize = resX * 2;
		writepxl = NULL;	/* pas d'affichage de pixels */
		showchar = showchar_text;
		scroll = scroll_text;
		fill = fill_text;
		pagesize = resY * linesize;
		getchar = getchar_text;
		getattrib = getattrib_text;
	} else {
		switch (color) {
		case 1:
			/* mode N&B */
			linesize = resX;
			writepxl = writepxl_1bit;
			fill = fill_chain;
			scroll = scroll_chain;
			break;
		case 2:
			/* mode 4 couleurs */
			linesize = (resX << 1);
			writepxl = writepxl_2bits;
			fill = fill_chain;
			scroll = scroll_chain;
			break;
		case 4:
			/* mode 16 couleurs */
			linesize = resX;
			writepxl = writepxl_4bits;
			fill = fill_unchain;
			scroll = scroll_unchain;
			break;
		case 8:
			/* mode 256 couleurs */
			if (def[5] == 0x0E) {
				/* mode chainé (plus rapide mais limité en mémoire) */
				linesize = (resX << 3);
				writepxl = writepxl_8bits;
				scroll = scroll_chain;
				fill = fill_chain;
			} else {
				/* mode non chainé */
				linesize = (resX << 1);
				writepxl = writepxl_8bitsunchain;
				scroll = scroll_unchain;
				fill = fill_unchain;
			}
			break;
		default:
			break;
		}
		showchar = showchar_graphic;
		pagesize = ((resY * linesize) << 3);
	}
	/* calcul des variables d'état video */
	activepage = 0;
	showedpage = 0;
	splitY = 0;
	vmode = mode;
	scrolling = 1;
	pages = (planesize / pagesize);
	basemem = (def[20] << 8) + def[21] + getbase();
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

u32 loadfont(u8 * def, u8 size, u8 font)
{
	if (graphics == 1)
		return 1;
	u8 oldregs[5] = { 0, 0, 0, 0, 0 };
	u8 *base;
	u16 i;
	if (font > 7)
		return 1;
	if (font < 4)
		base = (u8 *) (getbase() + (font << 14));
	else
		base = (u8 *) (getbase() + ((((font - 4) << 1) + 1) << 13));
	/* sauve les anciens registres */
	outb(sequencer, 2);
	oldregs[0] = inb(sequencer + 1);
	outb(sequencer, 4);
	oldregs[1] = inb(sequencer + 1);
	/* Adressage paire/impair desactivé (lineaire) */
	outb(sequencer + 1, oldregs[1] | 0x04);
	outb(graphics, 4);
	oldregs[2] = inb(graphics + 1);
	outb(graphics, 5);
	oldregs[3] = inb(graphics + 1);
	/* Adressage paire/impair desactivé (lineaire) */
	outb(graphics + 1, oldregs[3] & ~0x10);
	outb(graphics, 6);
	oldregs[4] = inb(graphics + 1);
	/* Adressage paire/impair desactivé (lineaire) */
	outb(graphics + 1, oldregs[4] & ~0x02);
	/* utilisation du plan N°2 */
	useplane(2);
	for (i = 0; i < 256; i++) {
		memcpy(def, base + i * 32, size, 1);
		def += size;
	}
	outb(sequencer, 2);
	outb(sequencer + 1, oldregs[0]);
	outb(sequencer, 4);
	outb(sequencer + 1, oldregs[1]);
	outb(graphics, 4);
	outb(graphics + 1, oldregs[2]);
	outb(graphics, 5);
	outb(graphics + 1, oldregs[3]);
	outb(graphics, 6);
	outb(graphics + 1, oldregs[4]);
	return 0;
}

/*******************************************************************************/

/* Récupere le N° de la police de caractère en cours d'utilisation */

u8 getfont()
{
	u8 num, tmp;
	outb(sequencer, 3);
	tmp = inb(sequencer + 1);
	num = (tmp & 0x03) | ((tmp & 0x10) >> 2);
	return num;
}

/*******************************************************************************/

/* Récupere le N° de la police de caractère en cours d'utilisation */

u8 getfont2()
{
	u8 num, tmp;
	outb(sequencer, 3);
	tmp = inb(sequencer + 1);
	num = ((tmp & 0x0C) >> 2) | ((tmp & 0x20) >> 3);
	return num;
}

/*******************************************************************************/

/* Fixe le N° de la police de caractère a utiliser */

void setfont(u8 num)
{
	num &= 0x07;
	outb(sequencer, 3);
	outb(sequencer + 1,
	     (inb(sequencer + 1) & 0xEC) | ((num & 0x03) +
					    ((num & 0x04) << 2)));
}

/*******************************************************************************/

/* Fixe le N° de la police de caractère a utiliser */

void setfont2(u8 num)
{
	num &= 0x07;
	outb(sequencer, 3);
	outb(sequencer + 1,
	     (inb(sequencer + 1) & 0xD3) | (((num & 0x03) << 2) +
					    ((num & 0x04) << 3)));
}

/*******************************************************************************/

/* Autorise le clignotement */

void enableblink()
{
	outb(ccrt, 0x10);
	outb(ccrt + 1, (inb(sequencer + 1) | 0x04));
}

/*******************************************************************************/

/* Annule le clignotement */

void disableblink()
{
	outb(ccrt, 0x10);
	outb(ccrt + 1, (inb(sequencer + 1) & ~0x04));
}

/*******************************************************************************/

/* Envoie une série d'octet a destination d'une portion de mémoire
vers le registre spécifié */

void outreg(u16 port, u8 * src, u16 num)
{
	int i;
	for (i = 0; i < num; i++) {
		outb(port, i);
		outb(port + 1, *src++);
	}
}

/*******************************************************************************/

/* Envoie une série d'octet a destination d'une portion de mémoire
vers le registre spécifié (accés data et index confondu) */

void outregsame(u16 port, u8 * src, u16 num)
{
	int i;
	for (i = 0; i < num; i++) {
		inb(port);
		outb(port, i);
		outb(port, *src++);
	}
}

/*******************************************************************************/

/* Récupère une série d'octet en provenance d'un registre spécifié
vers portion de mémoire */

void inreg(u16 port, u8 * src, u16 num)
{
	int i;
	for (i = 0; i < num; i++) {
		outb(port, i);
		*src++ = inb(port + 1);
	}
}

/*******************************************************************************/

/* Récupère une série d'octet en provenance d'un registre spécifié
vers portion de mémoire (accés data et index confondu) */

void inregsame(u16 port, u8 * src, u16 num)
{
	int i;
	for (i = 0; i < num; i++) {
		inb(port);
		outb(port, i);
		*src++ = inb(port);
	}
}

/*******************************************************************************/
