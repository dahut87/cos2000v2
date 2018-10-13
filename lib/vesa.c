/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "vesa.h"
#include "video.h"
#include "memory.h"
#include "asm.h"
#include "types.h"
#include "multiboot2.h"

static videoinfos infos;

static capabilities vesacapabilities[] = {
    {0xFF,000,000,false, 0,  0},
};


/*******************************************************************************/
/* Detecte si le hardware est disponible, return NULL ou pointeur sur le type de pilote */
u8 *VESA_detect_hardware(void) {
    u32 addr=getblockinfo();
    struct multiboot_tag *tag;
    unsigned size = *(unsigned *) addr;
    for (tag = (struct multiboot_tag *) (addr + 8);
            tag->type != MULTIBOOT_TAG_TYPE_END;
            tag = (struct multiboot_tag *) ((u8 *) tag + ((tag->size + 7) & ~7)))
            {
           switch (tag->type)
             {
             case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
               {
                 struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *) tag;
     
                 switch (tagfb->common.framebuffer_type)
                   {
                   case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
                     vesacapabilities[0].modenumber=0x0;
                     vesacapabilities[0].width=tagfb->common.framebuffer_width;
                     vesacapabilities[0].height=tagfb->common.framebuffer_height;
                     vesacapabilities[0].graphic=true;
                     vesacapabilities[0].depth=tagfb->common.framebuffer_bpp;
                     vesacapabilities[0].refresh=0x0;
                     infos.baseaddress=tagfb->common.framebuffer_addr;
                     infos.currentpitch=tagfb->common.framebuffer_pitch;
                     return "LEGACY";
                     break;

                    default:
                   case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
                   case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
                     return NULL;
                     break;
                   }
     
                 break;
               }
     
             }
        }
}

/*******************************************************************************/
/* Change le mode video courant */
/* ERR 0 aucune
/* ERR 1 mode non existant */

static u8 realsize;

u8 VESA_setvideo_mode(u8 mode)
{
    if (mode>0) return 1;
    infos.currentmode=mode;
    infos.currentwidth=vesacapabilities[0].width;
    infos.currentheight=vesacapabilities[0].height;
    infos.currentdepth=vesacapabilities[0].depth;
    infos.currentactivepage=0;
    infos.currentshowedpage=0;
    infos.currentcursorX=0;
    infos.currentcursorY=0;
    infos.currentfont1=0;
    infos.currentfont2=0;
    infos.isgraphic=vesacapabilities[0].graphic;
    infos.isblinking=false;
    infos.iscursorvisible=false;
    infos.pagesnumber=1;
    infos.pagesize=infos.currentheight*infos.currentpitch;
   return;
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
   u32 realdst=infos.baseaddress + infos.currentactivepage * infos.pagesize+dst;
   switch (infos.currentdepth) {
        case 32:
          if (!increment_src)
          {
              u32 pattern = (u32) src;
              stosb(pattern,realdst,size);  
            }
         else
            {


           }   
        break;
    }
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
     u32 realsrc=infos.baseaddress + infos.currentactivepage * infos.pagesize+src;
    u32 realdst=infos.baseaddress + infos.currentactivepage * infos.pagesize+dst;
   switch (infos.currentdepth) {
        case 32:
               movsd(realsrc,realdst,size>>2);  
        break;
    }
}

/*******************************************************************************/
/* Fixe la page ecran de travail */

void VESA_page_set(u8 page)
{
    return;
}

/*******************************************************************************/
/* Affiche la page ecran specifié */

void VESA_page_show(u8 page)
{
    return;
}

/*******************************************************************************/
/* Fonction inutile */

void VESA_dummy(void)
{
    return;
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

