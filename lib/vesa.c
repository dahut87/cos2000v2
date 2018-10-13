/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "vesa.h"
#include "video.h"
#include "memory.h"
#include "asm.h"
#include "types.h"

static videoinfos infos;

static capabilities vesacapabilities[] = {
    {0xFF,000,000,false, 0,  0},
};

/*******************************************************************************/
/* Detecte si le hardware est disponible, return NULL ou pointeur sur le type de pilote */
u8 *VESA_detect_hardware(void) {
    return "LEGACY";
}

/*******************************************************************************/
/* Change le mode video courant */
/* ERR 0 aucune
/* ERR 1 mode non existant */

static u8 realsize;

u8 VESA_setvideo_mode(u8 mode)
{
   
}

/*******************************************************************************/
/* Renvoie le nom du driver */
u8 *VESA_getvideo_drivername (void) {
    return "VESA";
}

/*******************************************************************************/
/* Renvoie un pointeur sur la structure des capacités graphiques */

u8 *VESA_getvideo_capabilities (void) {
    return vesacapabilities;
}

/*******************************************************************************/
/* Renvoie un pointeur sur l'état courant de la carte */
videoinfos *VESA_getvideo_info (void) {
    return &infos;
}

/*******************************************************************************/
/* Effecture un mouvement de la mémoire centrale vers la mémoire video (linéarisée) */
u32 VESA_mem_to_video (void *src,u32 dst, u32 size, bool increment_src) {
   
}

/*******************************************************************************/
/* Effecture un mouvement de la mémoire video (linéarisée) vers la mémoire centrale*/
u32 VESA_video_to_mem (u32 src,void *dst, u32 size)
{
 
}

/*******************************************************************************/
/* Effecture un mouvement de la mémoire video (linéarisé) vers la mémoire vidéo (linéarisée) */
u32 VESA_video_to_video (u32 src,u32 dst, u32 size) 
{
 
}

/*******************************************************************************/
/* Fixe la page ecran de travail */

void VESA_page_set(u8 page)
{

}

/*******************************************************************************/
/* Affiche la page ecran specifié */

void VESA_page_show(u8 page)
{
	
}

/*******************************************************************************/
/* Fonction inutile */

void VESA_dummy(void)
{
  
}

/*******************************************************************************/
/* Attend la retrace verticale */

void VESA_wait_vretrace(void)
{
	while ((inb(STATE) & 8) == 0) ;
}

/*******************************************************************************/
/* Attend la retrace horizontale */

void VESA_wait_hretrace(void)
{
	while ((inb(STATE) & 1) == 0) ;
}

