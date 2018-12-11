/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "video.h"

#define STATE       0x3da

/* fonction obligatoires */
void VESA_remap_memory(u32 vaddr);
u8 *VESA_detect_hardware (void);
u8 VESA_setvideo_mode (u8 mode);
u8 *VESA_getvideo_drivername (void);
u8 *VESA_getvideo_capabilities (void);
videoinfos *VESA_getvideo_info (void);
u32 VESA_mem_to_video (void *src,u32 dst, u32 size, bool increment_src);
u32 VESA_video_to_mem (u32 src,void *dst, u32 size);
u32 VESA_video_to_video (u32 src,u32 dst, u32 size);
void VESA_wait_vretrace (void);
void VESA_wait_hretrace (void);
void VESA_page_set (u8 page);
void VESA_page_show (u8 page);
void VESA_dummy ();

static videofonction vesafonctions = 
{
    &VESA_remap_memory,
    &VESA_detect_hardware,
    &VESA_setvideo_mode,
    &VESA_getvideo_drivername,
    &VESA_getvideo_capabilities,
    &VESA_getvideo_info,
    &VESA_mem_to_video,
    &VESA_video_to_mem,
    &VESA_video_to_video,
    &VESA_wait_vretrace,
    &VESA_wait_hretrace,
    &VESA_page_set,
    &VESA_page_show,
    &VESA_dummy,
    &VESA_dummy,
    &VESA_dummy,
    &VESA_dummy,
    &VESA_dummy,
    &VESA_dummy,
    &VESA_dummy,
    &VESA_dummy,
    &VESA_dummy
};
