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

static videofonction fonctions = 
{
    VGA_detect_hardware;
    VGA_setvideo_mode;
    VGA_getvideo_drivername;
    VGA_getvideo_capabilities;
    VGA_getvideo_info;
    VGA_mem_to_video;
    VGA_video_to_mem;
    VGA_video_to_video;
    VGA_wait_vretrace;
    VGA_wait_hretrace;
    VGA_page_set;
    VGA_page_show;
    VGA_page_split;
    VGA_cursor_enable;
    VGA_cursor_disable;
    VGA_cursor_set;
    VGA_font_load;
    VGA_font1_set;
    VGA_font2_set;
    VGA_blink_enable;
    VGA_blink_disable;
}

/*******************************************************************************/
/* Detecte si le hardware est disponible, return NULL ou pointeur sur le type de pilote */
u8 *VGA_detect_hardware {
    return "LEGACY",
};

/*******************************************************************************/
/* Renvoie l'adresse du segment video */

u32 getbase(void)
{
	u32 base;
	/*outb(graphics, 6);
	base = inb(graphics + 1);
	base >>= 2;
	base &= 3;*/
    base = modes[infos.currentmode].graphics.Miscellaneous_Graphics_Register;
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

u32 VGA_setvideo_mode(u8 mode)
{
    u32 index;
    while(vgacapabilities[index].modenumber!=0xFF) {
        if (vgacapabilities[index].modenumber==mode) {    
            infos.currentmode=vgacapabilities[index].modenumber;     
            break;
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
		loadfont(font8x8, 8, 1);
		loadfont(font8x16, 16, 0);
        switch (infos.depth) {
		    case 1:
			    /* mode N&B */
			    infos.currentpitch = infos.width;
			    break;
		    case 2:
			    /* mode 4 couleurs */
			    infos.currentpitch = (infos.width << 1);
			    break;
		    case 4:
			    /* mode 16 couleurs */
			    infos.currentpitch = infos.width;
			    break;
		    case 8:
			    /* mode 256 couleurs */
			    if (modes[index].sequencer.Sequencer_Memory_Mode_Register == 0x0E) {
				    /* mode chainé (plus rapide mais limité en mémoire) */
				    infos.currentpitch = (infos.width << 3);
			    } else {
				    /* mode non chainé */
				    infos.currentpitch = (infos.width << 1);
			    }
			    break;
		    default:
			    break;
		    }
		infos.pagesize = ((infos.height * infos.currentpitch) << 3);
	}
    else {
        infos.currentpitch= infos.width * 2;
        infos.pagesize=infos.height * infos.currentpitch;
    }
    infos.pagesnumber=(planesize / infos.currentpitch); 
    infos.baseaddress=(modes[index].ccrt.Cursor_Location_High_Register << 8) + modes[index].ccrt.Cursor_Location_Low_Register + getbase();
	/* Initialise les registre "divers" */
	outb(misc, modes[index].misc);
	/* Initialise les registre d'etat */
	outb(state, 0x00);
	/* Initialise le séquenceur */
	outreg(sequencer, modes[index].sequencer, 5);
	/* Debloque le verouillage des registres controleur CRT */
	outb(ccrt, 0x11);
	outb(ccrt + 1, 0x0E);
	/* Initialise le controleur CRT */
	outreg(ccrt, ctrc, 25);
	/* Initialise le controleur graphique */
	outreg(graphics, modes[index].graphic, 9);
	inb(state);
	/* Initialise le controleur d'attributs */
	outregsame(attribs, modes[index].attributs, 21);
	inb(state);
	outb(attribs, 0x20);
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
    return infos;
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
		outb(ccrt, 0x0C);
		outb(ccrt + 1, (addr >> 8));
		outb(ccrt, 0x0D);
		outb(ccrt + 1, (addr & 0xFF));
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
    else
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
}

/*******************************************************************************/
/* Attend la retrace verticale */

void VGA_wait_vretrace(void)
{
	while ((inb(state) & 8) == 0) ;
}

/*******************************************************************************/
/* Attend la retrace horizontale */

void VGA_wait_hretrace(void)
{
	while ((inb(state) & 1) == 0) ;
}

/*******************************************************************************/
/* Active l'affichage du curseur de texte */

void VGA_cursor_enable(void)
{
	u8 curs;
	/* active le curseur hardware */
	outb(ccrt, 10);
	curs = inb(ccrt + 1) & ~32;
	outb(ccrt + 1, curs);
    infos.isgraphic=true;
}

/*******************************************************************************/
/* Desactive l'affichage du curseur de texte */

void VGA_cursor_disable(void)
{
	u8 curs;
	/* Desactive le curseur hardware */
	outb(ccrt, 10);
	curs = inb(ccrt + 1) | 32;
	outb(ccrt + 1, curs);
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
	outb(graphics, 4);
	outb(graphics + 1, plan);
	/* choisi le plan d'ecriture */
	outb(sequencer, 2);
	outb(sequencer + 1, mask);
}

/*******************************************************************************/
/* fixe la position du curseur texte */

void VGA_cursor_set(u16 x, u16 y)
{
	u16 pos;
	if (splitY == 0)
		pos = (infos.currentshowedpage * infos.pagesize / 2 + x + y * infos.width);
	else
		pos = (x + y * infos.width);
	outb(ccrt, 0x0F);
	outb(ccrt + 1, (u8) (pos & 0x00FF));
	outb(ccrt, 0x0E);
	outb(ccrt + 1, (u8) ((pos & 0xFF00) >> 8));
    info.currentcursorX=x;
    info.currentcursorY=y;
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
/* Fixe le N° de la police de caractère a utiliser */

void VGA_font1_set(u8 num)
{
	num &= 0x07;
	outb(sequencer, 3);
	outb(sequencer + 1,
	     (inb(sequencer + 1) & 0xEC) | ((num & 0x03) +
					    ((num & 0x04) << 2)));
    info.currentfont1=num;
}

/*******************************************************************************/
/* Fixe le N° de la police de caractère a utiliser */

void VGA_font2_set(u8 num)
{
	num &= 0x07;
	outb(sequencer, 3);
	outb(sequencer + 1,
	     (inb(sequencer + 1) & 0xD3) | (((num & 0x03) << 2) +
					    ((num & 0x04) << 3)));
    info.currentfont2=num;
}

/*******************************************************************************/
/* Autorise le clignotement */

void VGA_blink_enable(void)
{
	outb(ccrt, 0x10);
	outb(ccrt + 1, (inb(sequencer + 1) | 0x04));
    info.isblinking=true;
}

/*******************************************************************************/
/* Annule le clignotement */

void VGA_blink_disable(void)
{
	outb(ccrt, 0x10);
	outb(ccrt + 1, (inb(sequencer + 1) & ~0x04));
    info.isblinking=false;
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
