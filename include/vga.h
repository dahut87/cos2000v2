/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "video.h"

#define TEXTSCREEN 0xB8000	/* debut de la memoire video texte*/
#define GRPHSCREEN 0xA0000	/* debut de la memoire video graphique*/

/* Registres VGAs */

#define SEQUENCER   0x3c4
#define MISC_WRITE  0x3c2
#define MISC_READ   0x3cc
#define CCRT        0x3D4
#define ATTRIBS     0x3c0
#define GRAPHICS    0x3ce
#define STATE       0x3da

/* Taille d'un plan de bit */

#define PLANESIZE 0x10000

/* Registres VGA */
typedef struct misc_regs {
    u8 Miscellaneous_Output_Register;  
} misc_regs __attribute__ ((packed));

typedef struct sequencer_regs {
    u8 Reset_Register;  
    u8 Clocking_Mode_Register;  
    u8 Map_Mask_Register;  
    u8 Character_Map_Select_Register;  
    u8 Sequencer_Memory_Mode_Register;  
} sequencer_regs __attribute__ ((packed));

typedef struct crtc_regs {
    u8 Horizontal_Total_Register;  
    u8 End_Horizontal_Display_Register;  
    u8 Start_Horizontal_Blanking_Register;  
    u8 End_Horizontal_Blanking_Register;  
    u8 Start_Horizontal_Retrace_Register;
    u8 End_Horizontal_Retrace_Register;
    u8 Vertical_Total_Register;
    u8 Overflow_Register;
    u8 Preset_Row_Scan_Register;
    u8 Maximum_Scan_Line_Register;
    u8 Cursor_Start_Register;
    u8 Cursor_End_Register;
    u8 Start_Address_High_Register;
    u8 Start_Address_Low_Register;
    u8 Cursor_Location_High_Register;
    u8 Cursor_Location_Low_Register;
    u8 Vertical_Retrace_Start_Register;
    u8 Vertical_Retrace_End_Register;
    u8 Vertical_Display_End_Register;
    u8 Offset_Register;
    u8 Underline_Location_Register;
    u8 Start_Vertical_Blanking_Register;
    u8 End_Vertical_Blanking;
    u8 CRTC_Mode_Control_Register;
    u8 Line_Compare_Register;
} crtc_regs __attribute__ ((packed));

typedef struct graphics_regs {
    u8 Set_Reset_Register;  
    u8 Enable_Set_Reset_Register;  
    u8 Color_Compare_Register;  
    u8 Data_Rotate_Register;  
    u8 Read_Map_Select_Register;
    u8 Graphics_Mode_Register;
    u8 Miscellaneous_Graphics_Register;
    u8 Color_Dont_Care_Register;
    u8 Bit_Mask_Register;
} graphics_regs __attribute__ ((packed));

typedef struct attributs_regs {
    u8 Palette_Registers[16];
    u8 Attribute_Mode_Control_Register;  
    u8 Overscan_Color_Register;  
    u8 Color_Plane_Enable_Register;
    u8 Horizontal_Pixel_Panning_Register;
    u8 Color_Select_Register;
} attributs_regs __attribute__ ((packed));

typedef struct vgamode {
    misc_regs        misc;
    sequencer_regs   sequencer;
    crtc_regs        ctrc;
    graphics_regs    graphic;
    attributs_regs   attribut;
} vgamode __attribute__ ((packed));

/* fonction obligatoires */
u8 *VGA_detect_hardware (void);
u8 VGA_setvideo_mode (u8 mode);
u8 *VGA_getvideo_drivername (void);
u8 *VGA_getvideo_capabilities (void);
videoinfos *VGA_getvideo_info (void);
u32 VGA_mem_to_video (void *src,u32 dst, u32 size, bool increment_src);
u32 VGA_video_to_mem (u32 src,void *dst, u32 size);
u32 VGA_video_to_video (u32 src,u32 dst, u32 size);
void VGA_wait_vretrace (void);
void VGA_wait_hretrace (void);
void VGA_page_set (u8 page);
void VGA_page_show (u8 page);
void VGA_page_split (u16 y);
void VGA_cursor_enable (void);
void VGA_cursor_disable (void);
void VGA_cursor_set  (u16 x,u16 y);
u32 VGA_font_load (u8 * def, u8 size, u8 font);
void VGA_font1_set (u8 num);
void VGA_font2_set (u8 num);
void VGA_blink_enable (void);
void VGA_blink_disable (void);

static videofonction fonctions = 
{
    &VGA_detect_hardware,
    &VGA_setvideo_mode,
    &VGA_getvideo_drivername,
    &VGA_getvideo_capabilities,
    &VGA_getvideo_info,
    &VGA_mem_to_video,
    &VGA_video_to_mem,
    &VGA_video_to_video,
    &VGA_wait_vretrace,
    &VGA_wait_hretrace,
    &VGA_page_set,
    &VGA_page_show,
    &VGA_page_split,
    &VGA_cursor_enable,
    &VGA_cursor_disable,
    &VGA_cursor_set,
    &VGA_font_load,
    &VGA_font1_set,
    &VGA_font2_set,
    &VGA_blink_enable,
    &VGA_blink_disable
};
