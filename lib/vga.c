/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "vga.h"
#include "video.h"
#include "memory.h"
#include "asm.h"
#include "types.h"
#include "VGA/modes.c"
#include "VGA/8x8fnt.c"
#include "VGA/8x16fnt.c"

static videoinfos infos;

/*******************************************************************************/
/* Detecte si le hardware est disponible, return NULL ou pointeur sur le type de pilote */
u8 *VGA_detect_hardware(void) {
    return "LEGACY";
}

/*******************************************************************************/
/* Renvoie l'adresse du segment video */

u32 getbase(void)
{
	u32 base;
	/*outb(GRAPHICS, 6);
	base = inb(GRAPHICS + 1);
	base >>= 2;
	base &= 3;*/
    base = modes[infos.currentmode].graphic.Miscellaneous_Graphics_Register;
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
/* Change le mode video courant */
/* ERR 0 aucune
/* ERR 1 mode non existant */

u8 VGA_setvideo_mode(u8 mode)
{
    u32 index=0;
    while(vgacapabilities[index].modenumber!=0xFF) {
        if (vgacapabilities[index].modenumber==mode) {    
            infos.currentmode=vgacapabilities[index].modenumber;     
            break;
        }
        index++;
    }
    if (infos.currentmode!=mode)
        return 1;
    infos.currentwidth=vgacapabilities[index].width;
    infos.currentheight=vgacapabilities[index].height;
    infos.currentdepth=vgacapabilities[index].depth;
    infos.currentactivepage=0;
    infos.currentshowedpage=0;
    infos.currentcursorX=0;
    infos.currentcursorY=0;  
    infos.currentfont1=0;
    infos.currentfont2=0; 
    infos.isgraphic=vgacapabilities[index].graphic; 
    infos.isblinking=false;
    infos.iscursorvisible=false;  
    if (infos.isgraphic) {
        switch (infos.currentdepth) {
		    case 1:
			    /* mode N&B */
			    infos.currentpitch = infos.currentwidth;
			    break;
		    case 2:
			    /* mode 4 couleurs */
			    infos.currentpitch = (infos.currentwidth << 1);
			    break;
		    case 4:
			    /* mode 16 couleurs */
			    infos.currentpitch = infos.currentwidth;
			    break;
		    case 8:
			    /* mode 256 couleurs */
			    if (modes[index].sequencer.Sequencer_Memory_Mode_Register == 0x0E) {
				    /* mode chainé (plus rapide mais limité en mémoire) */
				    infos.currentpitch = (infos.currentwidth << 3);
			    } else {
				    /* mode non chainé */
				    infos.currentpitch = (infos.currentwidth << 1);
			    }
			    break;
		    default:
			    break;
		    }
		infos.pagesize = ((infos.currentheight * infos.currentpitch) << 3);
	}
    else {
		VGA_font_load(font8x8, 8, 1);
		VGA_font_load(font8x16, 16, 0);
        infos.currentpitch= infos.currentwidth * 2;
        infos.pagesize=infos.currentheight * infos.currentpitch;
    }
    infos.pagesnumber=(PLANESIZE / infos.currentpitch); 
    infos.baseaddress=(modes[index].ctrc.Cursor_Location_High_Register << 8) + modes[index].ctrc.Cursor_Location_Low_Register + getbase();
	/* Initialise les registre "divers" */
	outb(MISC_WRITE, modes[index].misc);
	/* Initialise les registre d'etat */
	outb(STATE, 0x00);
	/* Initialise le séquenceur */
	outreg(SEQUENCER, modes[index].sequencer, 5);
	/* Debloque le verouillage des registres controleur CRT */
	outb(CCRT, 0x11);
	outb(CCRT + 1, 0x0E);
	/* Initialise le controleur CRT */
	outreg(CCRT, modes[index].ctrc, 25);
	/* Initialise le controleur graphique */
	outreg(GRAPHICS, modes[index].graphic, 9);
	inb(STATE);
	/* Initialise le controleur d'attributs */
	outregsame(ATTRIBS, modes[index].attribut, 21);
	inb(STATE);
	outb(ATTRIBS, 0x20);
	/* Initialise l'adresse des procedures de gestion graphique et les differentes
	   variables en fonction de la profondeur et du mode */
	return 0;
}

/*******************************************************************************/
/* Renvoie le nom du driver */
u8 *VGA_getvideo_drivername (void) {
    return "VGA";
}

/*******************************************************************************/
/* Renvoie un pointeur sur la structure des capacités graphiques */

u8 *VGA_getvideo_capabilities (void) {
    return vgacapabilities;
}

/*******************************************************************************/
/* Renvoie un pointeur sur l'état courant de la carte */
videoinfos *VGA_getvideo_info (void) {
    return &infos;
}

/*******************************************************************************/
/* Effecture un mouvement de la mémoire centrale vers la mémoire video (linéarisée) */
u32 VGA_mem_to_video (void *src,u32 dst, u32 size, bool increment_src) {

}

/*******************************************************************************/
/* Effecture un mouvement de la mémoire video (linéarisée) vers la mémoire centrale*/
u32 VGA_video_to_mem (u32 src,void *dst, u32 size) {

}

/*******************************************************************************/
/* Effecture un mouvement de la mémoire video (linéarisé) vers la mémoire vidéo (linéarisée) */
u32 VGA_video_to_video (u32 src,u32 dst, u32 size) {

}

/*******************************************************************************/
/* Fixe la page ecran de travail */

void VGA_page_set(u8 page)
{
	if (page < infos.pagesnumber)
		infos.currentactivepage = page;
}

/*******************************************************************************/
/* Affiche la page ecran specifié */

void VGA_page_show(u8 page)
{
	if (page < infos.pagesnumber) {
		u16 addr;
		addr = page * infos.pagesize / 2;
		outb(CCRT, 0x0C);
		outb(CCRT + 1, (addr >> 8));
		outb(CCRT, 0x0D);
		outb(CCRT + 1, (addr & 0xFF));
		infos.currentshowedpage = page;
	}
}

/*******************************************************************************/
/* Sépare l'écran en 2 a partir de la ligne Y */

static splitY=0;

void VGA_page_split(u16 y)
{
    if (y!=0) {
	    u16 addr;
	    if (!infos.isgraphic)
		    addr = (y << 3);
	    else
		    addr = y;
	    /* line compare pour ligne atteinte */
	    outb(CCRT, 0x18);
	    outb(CCRT + 1, (addr & 0xFF));
	    /* overflow pour le bit 8 */

	    outb(CCRT, 0x07);
	    outb(CCRT + 1, (inb(CCRT + 1) & ~16) | ((addr >> 4) & 16));

	    /*  Maximum Scan Line pour le bit 9 */

	    outb(CCRT, 0x09);
	    outb(CCRT + 1, (inb(CCRT + 1) & ~64) | ((addr >> 3) & 64));
	    splitY = y;
    }
    else
    {
        /* line compare pour ligne atteinte */
	    outb(CCRT, 0x18);
	    outb(CCRT + 1, 0);
	    /* overflow pour le bit 8 */

	    outb(CCRT, 0x07);
	    outb(CCRT + 1, inb(CCRT + 1) & ~16);

	    /*  Maximum Scan Line pour le bit 9 */

	    outb(CCRT, 0x09);
	    outb(CCRT + 1, inb(CCRT + 1) & ~64);
	    splitY = 0;
    }
}

/*******************************************************************************/
/* Attend la retrace verticale */

void VGA_wait_vretrace(void)
{
	while ((inb(STATE) & 8) == 0) ;
}

/*******************************************************************************/
/* Attend la retrace horizontale */

void VGA_wait_hretrace(void)
{
	while ((inb(STATE) & 1) == 0) ;
}

/*******************************************************************************/
/* Active l'affichage du curseur de texte */

void VGA_cursor_enable(void)
{
	u8 curs;
	/* active le curseur hardware */
	outb(CCRT, 10);
	curs = inb(CCRT + 1) & ~32;
	outb(CCRT + 1, curs);
    infos.isgraphic=true;
}

/*******************************************************************************/
/* Desactive l'affichage du curseur de texte */

void VGA_cursor_disable(void)
{
	u8 curs;
	/* Desactive le curseur hardware */
	outb(CCRT, 10);
	curs = inb(CCRT + 1) | 32;
	outb(CCRT + 1, curs);
    infos.isgraphic=false;
}

/*******************************************************************************/
/* Utilise le plan de bit spécifié */

void useplane(u8 plan)
{
	u8 mask;
	plan &= 3;
	mask = 1 << plan;
	/* choisi le plan de lecture */
	outb(GRAPHICS, 4);
	outb(GRAPHICS + 1, plan);
	/* choisi le plan d'ecriture */
	outb(SEQUENCER, 2);
	outb(SEQUENCER + 1, mask);
}

/*******************************************************************************/
/* fixe la position du curseur texte */

void VGA_cursor_set(u16 x, u16 y)
{
	u16 pos;
	if (splitY == 0)
		pos = (infos.currentshowedpage * infos.pagesize / 2 + x + y * infos.currentwidth);
	else
		pos = (x + y * infos.currentwidth);
	outb(CCRT, 0x0F);
	outb(CCRT + 1, (u8) (pos & 0x00FF));
	outb(CCRT, 0x0E);
	outb(CCRT + 1, (u8) ((pos & 0xFF00) >> 8));
    infos.currentcursorX=x;
    infos.currentcursorY=y;
}

/*******************************************************************************/
/* Charge une nouvelle police de caractère */
/* ERR 1 mode graphique activé*/

u32 VGA_font_load(u8 * def, u8 size, u8 font)
{
	if (infos.isgraphic)
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
	outb(SEQUENCER, 2);
	oldregs[0] = inb(SEQUENCER + 1);
	outb(SEQUENCER, 4);
	oldregs[1] = inb(SEQUENCER + 1);
	/* Adressage paire/impair desactivé (lineaire) */
	outb(SEQUENCER + 1, oldregs[1] | 0x04);
	outb(GRAPHICS, 4);
	oldregs[2] = inb(GRAPHICS + 1);
	outb(GRAPHICS, 5);
	oldregs[3] = inb(GRAPHICS + 1);
	/* Adressage paire/impair desactivé (lineaire) */
	outb(GRAPHICS + 1, oldregs[3] & ~0x10);
	outb(GRAPHICS, 6);
	oldregs[4] = inb(GRAPHICS + 1);
	/* Adressage paire/impair desactivé (lineaire) */
	outb(GRAPHICS + 1, oldregs[4] & ~0x02);
	/* utilisation du plan N°2 */
	useplane(2);
	for (i = 0; i < 256; i++) {
		memcpy(def, base + i * 32, size, 1);
		def += size;
	}
	outb(SEQUENCER, 2);
	outb(SEQUENCER + 1, oldregs[0]);
	outb(SEQUENCER, 4);
	outb(SEQUENCER + 1, oldregs[1]);
	outb(GRAPHICS, 4);
	outb(GRAPHICS + 1, oldregs[2]);
	outb(GRAPHICS, 5);
	outb(GRAPHICS + 1, oldregs[3]);
	outb(GRAPHICS, 6);
	outb(GRAPHICS + 1, oldregs[4]);
	return 0;
}

/*******************************************************************************/
/* Fixe le N° de la police de caractère a utiliser */

void VGA_font1_set(u8 num)
{
	num &= 0x07;
	outb(SEQUENCER, 3);
	outb(SEQUENCER + 1,
	     (inb(SEQUENCER + 1) & 0xEC) | ((num & 0x03) +
					    ((num & 0x04) << 2)));
    infos.currentfont1=num;
}

/*******************************************************************************/
/* Fixe le N° de la police de caractère a utiliser */

void VGA_font2_set(u8 num)
{
	num &= 0x07;
	outb(SEQUENCER, 3);
	outb(SEQUENCER + 1,
	     (inb(SEQUENCER + 1) & 0xD3) | (((num & 0x03) << 2) +
					    ((num & 0x04) << 3)));
    infos.currentfont2=num;
}

/*******************************************************************************/
/* Autorise le clignotement */

void VGA_blink_enable(void)
{
	outb(CCRT, 0x10);
	outb(CCRT + 1, (inb(SEQUENCER + 1) | 0x04));
    infos.isblinking=true;
}

/*******************************************************************************/
/* Annule le clignotement */

void VGA_blink_disable(void)
{
	outb(CCRT, 0x10);
	outb(CCRT + 1, (inb(SEQUENCER + 1) & ~0x04));
    infos.isblinking=false;
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
