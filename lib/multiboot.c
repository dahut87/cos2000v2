#include "multiboot2.h"

static u32 infobloc;

void initmultiboot(const u32 addr)
{
    infobloc=addr;
}

void getbootinfo(void)
{
u32 addr=infobloc;
struct multiboot_tag *tag;
unsigned size = *(unsigned *) addr;
if (addr & 7) print("Non aligne...");
printf(" Taille :% 4u\r\n", (u32)size);
for (tag = (struct multiboot_tag *) (addr + 8);
            tag->type != MULTIBOOT_TAG_TYPE_END;
            tag = (struct multiboot_tag *) ((u8 *) tag + ((tag->size + 7) & ~7)))
         {
           printf ("--- Tag % 4u, Taille % 4u\r\n", tag->type, tag->size);
           switch (tag->type)
             {
             case MULTIBOOT_TAG_TYPE_CMDLINE:
               printf ("line de lancement : %s\r\n",
                       ((struct multiboot_tag_string *) tag)->string);
               break;
             case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
               printf ("Chargeur de boot : %s\r\n",
                       ((struct multiboot_tag_string *) tag)->string);
               break;
             case MULTIBOOT_TAG_TYPE_MODULE:
               printf ("Module %X-%X. Command line %s\r\n",
                       ((struct multiboot_tag_module *) tag)->mod_start,
                       ((struct multiboot_tag_module *) tag)->mod_end,
                       ((struct multiboot_tag_module *) tag)->cmdline);
               break;
             case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
               printf ("Memoire basse : %H, memoire haute = %lH\r\n",
                       ((struct multiboot_tag_basic_meminfo *) tag)->mem_lower<<10,
                       ((u64)((struct multiboot_tag_basic_meminfo *) tag)->mem_upper)<<10);
               break;
             case MULTIBOOT_TAG_TYPE_BOOTDEV:
               printf ("Peripherique de demarrage : %x,%u,%u\r\n\r\n",
                       ((struct multiboot_tag_bootdev *) tag)->biosdev,
                       ((struct multiboot_tag_bootdev *) tag)->slice,
                       ((struct multiboot_tag_bootdev *) tag)->part);
               break;
             case MULTIBOOT_TAG_TYPE_MMAP:
               {
                 multiboot_memory_map_t *mmap;
                 printf ("*** Plan de memoire ***\r\n");
                 for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;(u8 *) mmap < (u8 *) tag + tag->size; mmap = (multiboot_memory_map_t *)
                        ((unsigned long) mmap + ((struct multiboot_tag_mmap *) tag)->entry_size))
                   printf (" adresse: %lx,"" taille:%lx, type:%x\r\n",
                           (u64) (mmap->addr),
                           (u64) (mmap->len),
                           (u32) (mmap->type));
               }
               break;
             case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
               {
                 u32 color;
                 unsigned i;
                 struct multiboot_tag_framebuffer *tagfb
                   = (struct multiboot_tag_framebuffer *) tag;
                 void *fb = (void *) (unsigned long) tagfb->common.framebuffer_addr;
     
                 switch (tagfb->common.framebuffer_type)
                   {
                   case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
                     {
                       unsigned best_distance, distance;
                       struct multiboot_color *palette;
     
                       palette = tagfb->framebuffer_palette;
     
                       color = 0;
                       best_distance = 4*256*256;
     
                       for (i = 0; i < tagfb->framebuffer_palette_num_colors; i++)
                         {
                           distance = (0xff - palette[i].blue)
                             * (0xff - palette[i].blue)
                             + palette[i].red * palette[i].red
                             + palette[i].green * palette[i].green;
                           if (distance < best_distance)
                             {
                               color = i;
                               best_distance = distance;
                             }
                         }
                     }
                     break;
     
                   case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
                     color = ((1 << tagfb->framebuffer_blue_mask_size) - 1)
                       << tagfb->framebuffer_blue_field_position;
                     break;
     
                   case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
                     color = '\\' | 0x0100;
                     break;
     
                   default:
                     color = 0xffffffff;
                     break;
                   }
     
                 for (i = 0; i < tagfb->common.framebuffer_width
                        && i < tagfb->common.framebuffer_height; i++)
                   {
                     switch (tagfb->common.framebuffer_bpp)
                       {
                       case 8:
                         {
                           u8 *pixel = fb
                             + tagfb->common.framebuffer_pitch * i + i;
                           *pixel = color;
                         }
                         break;
                       case 15:
                       case 16:
                         {
                           u16 *pixel
                             = fb + tagfb->common.framebuffer_pitch * i + 2 * i;
                           *pixel = color;
                         }
                         break;
                       case 24:
                         {
                           u32 *pixel
                             = fb + tagfb->common.framebuffer_pitch * i + 3 * i;
                           *pixel = (color & 0xffffff) | (*pixel & 0xff000000);
                         }
                         break;
     
                       case 32:
                         {
                           u32 *pixel
                             = fb + tagfb->common.framebuffer_pitch * i + 4 * i;
                           *pixel = color;
                         }
                         break;
                       }
                   }
                 break;
               }
     
             }
         }
}

