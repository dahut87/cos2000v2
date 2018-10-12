/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "stdarg.h"

#define maxdrivers 10

typedef struct drivers {
    u8*             nom;
    videofonction*  pointer;
}

typedef struct capabilities {
    u8      modenumber;
    u16     width;
    u16     height;
    bool    graphic;
    u8      depth;
    u8      refresh;    
}

typedef struct videoinfos {
    u8      currentmode
    u16     currentwidth;
    u16     currentheight;
    u8      currentdepth;
    u8      currentactivepage;
    u8      currentshowedpage;
    u16     currentcursorX;
    u16     currentcursorY;
    u8      currentfont1;
    u8      currentfont2;
    u16     currentpitch;
    bool    isgraphic;
    bool    isblinking;
    bool    iscursorvisible;
    u8      pagesnumber;
    u32     pagesize;
    u32     baseaddress;
} videoinfos __attribute__ ((packed));

typedef struct console {
    u8        attrib;
    s16       cursX;
    s16       cursY; 
    u8        ansi;
    u8        param1;
    u8        param2;
    u8        param3;
    u8        page;
} console  __attribute__ ((packed));

typedef struct videofonction {
    u8 *(*detect_hardware) (void);
    u8 (*setvideo_mode) (u8 mode);
    u8 *(*getvideo_drivername) (void);
    u8 *(*getvideo_capabilities) (void);
    videoinfos *(*getvideo_info) (void);
    u32 (*mem_to_video) (void *src,u32 dst, u32 size, bool increment_src);
    u32 (*video_to_mem) (u32 src,void *dst, u32 size);
    u32 (*video_to_video) (u32 src,u32 dst, u32 size);
    void (*wait_vretrace) (void);
    void (*wait_hretrace) (void);
    void (*page_set) (u8 page);
    void (*page_show) (u8 page);
    void (*page_split) (u16 y);
    void (*cursor_enable) (void);
    void (*cursor_disable) (void);
    void (*cursor_set)  (u16 x,u16 y);
    u32 (*font_load) (u8 * def, u8 size, u8 font);
    void (*font1_set) (u8 num);
    void (*font2_set) (u8 num);
    void (*blink_enable) (void);
    void (*blink_disable) (void);
} videofonction __attribute__ ((packed));

/* Fonctions de bas niveau */
void fill(u8 attrib);
void scroll (u8 lines, u8 attrib);
void scroll_enable(void);
void scroll_disable(void);
void showchar (u16 coordx, u16 coordy, u8 thechar, u8 attrib);
u8 getchar (u16 coordx, u16 coordy);
u8 getattrib (u16 coordx, u16 coordy);

/* Fonctions de console */
void changevc(u8 vc);
void putchar(u8 thechar);
void clearscreen(void);

/* Fonctions de haut niveau */
u32 print(u8* string);
u32 printf (const u8 *string, ...);
u32 sprintf(u8 *variable, const u8 *string, ...);
u32 snprintf(u8 *variable, u32 maxsize, const u8 *string, ...);
u32 vprintf(const u8 * string, va_list args);
u32 vsprintf(u8 *variable, const u8 *string, va_list args);
u32 vsnprintf(u8 *variable, u32 maxsize, const u8 *string, va_list args);

u8* itoa(u64 num, u8* str, u8 base, u64 dim, u8 achar);
u8* sitoa(u64 num, u8 * str, u64 dim);
u8* rtoadouble(double num, u8 * str, u8 precisioni , u8 precisionf);
u8* rtoasingle(float num, u8 * str, u8 precisioni , u8 precisionf);
u32 format(const u8 * string, va_list args, u32 maxsize, u32 (*fonction)(u8* src, u8** dest, u32 len), u8* dest);

/* Fonction pour gérer le pilote */
void initdriver();
void registerdriver(videofonction *pointer);
void apply_bestdriver(void);
void apply_nextdriver(void);
void apply_driver(u8* name);
void apply_nextvideomode(void);

/* Fonctions du pilote */
u8 *(*detect_hardware) (void);
u8 (*setvideo_mode) (u8 mode);
u8 *(*getvideo_drivername) (void);
u8 *(*getvideo_capabilities) (void);
videoinfos *(*getvideo_info) (void);
u32 (*mem_to_video) (void *src,u32 dst, u32 size, bool increment_src);
u32 (*video_to_mem) (u32 src,void *dst, u32 size);
u32 (*video_to_video) (u32 src,u32 dst, u32 size);
void (*wait_vretrace) (void);
void (*wait_hretrace) (void);
void (*page_set) (u8 page);
void (*page_show) (u8 page);
void (*page_split) (u16 y);
void (*cursor_enable) (void);
void (*cursor_disable) (void);
void (*cursor_set)  (u16 x,u16 y);
u32 (*font_load) (u8 * def, u8 size, u8 font);
void (*font1_set) (u8 num);
void (*font2_set) (u8 num);
void (*blink_enable) (void);
void (*blink_disable) (void);
