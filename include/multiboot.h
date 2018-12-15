/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "multiboot2.h"

u32     getgrubinfo(u8 type);
u8     *getgrubinfo_cmdline(void);
u32     getgrubinfo_ram(void);
struct multiboot_tag_mmap *getgrubinfo_mem(void);
struct multiboot_tag_framebuffer *getgrubinfo_fb(void);
void    getgrubinfo_all(void);
void    initmultiboot(const u32 addr);
