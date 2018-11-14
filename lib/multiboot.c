/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "multiboot2.h"

static u32 infobloc;

/*******************************************************************************/
/* Initialise avec l'adresse du bloc multiboot2 */

void initmultiboot(const u32 addr)
{
    infobloc=addr;
}

/*******************************************************************************/
/* Renvoie les informations souhaitées multiboot2 */

u32 getgrubinfo(u8 type)
{
struct multiboot_tag *tag;
unsigned size = *(unsigned *) infobloc;
for (tag = (struct multiboot_tag *) (infobloc + 8);
            tag->type != MULTIBOOT_TAG_TYPE_END;
            tag = (struct multiboot_tag *) ((u8 *) tag + ((tag->size + 7) & ~7)))
         if (tag->type==type) return tag;
}

/*******************************************************************************/
/* Renvoie la ligne de commande */

u8 *getgrubinfo_cmdline(void)
{
    struct multiboot_tag_string *tag=getgrubinfo(MULTIBOOT_TAG_TYPE_CMDLINE);
    return tag->string;
}

/*******************************************************************************/
/* Renvoie la quantité de mémoire */

u32 getgrubinfo_ram(void)
{
    struct multiboot_tag_basic_meminfo *tag=getgrubinfo(MULTIBOOT_TAG_TYPE_BASIC_MEMINFO);
    return tag->mem_upper;
}

/*******************************************************************************/
/* Renvoie le plan de la mémoire */

struct multiboot_memory_map_t *getgrubinfo_mem(void)
{
    struct multiboot_memory_map_t *tag=getgrubinfo(MULTIBOOT_TAG_TYPE_MMAP);
    return tag;
}

/*******************************************************************************/
/* Renvoie les information sur le framebuffer */

struct multiboot_tag_framebuffer *getgrubinfo_fb(void)
{
    struct multiboot_tag_framebuffer *tag=getgrubinfo(MULTIBOOT_TAG_TYPE_FRAMEBUFFER);
    return tag;
}

/*******************************************************************************/
/* Affiche les informations multiboot2 ;*/

void getgrubinfo_all(void)
{
struct multiboot_tag *tag;
u32 size = *(unsigned *) infobloc;
if (infobloc & 7) print("Attention : Bloc non aligne...");
printf(" Taille :% 4u\r\n", (u32)size);
for (tag = (struct multiboot_tag *) (infobloc + 8);
            tag->type != MULTIBOOT_TAG_TYPE_END;
            tag = (struct multiboot_tag *) ((u8 *) tag + ((tag->size + 7) & ~7)))
         {
           printf ("--- Tag % hu, Taille % hu\r\n", tag->type, tag->size);
           switch (tag->type)
             {
             case MULTIBOOT_TAG_TYPE_CMDLINE:
               printf (" Ligne de lancement : %s\r\n",
                       ((struct multiboot_tag_string *) tag)->string);
               break;
             case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
               printf (" Chargeur de boot : %s\r\n",
                       ((struct multiboot_tag_string *) tag)->string);
               break;
             case MULTIBOOT_TAG_TYPE_MODULE:
               printf (" Module %X-%X. Command line %s\r\n",
                       ((struct multiboot_tag_module *) tag)->mod_start,
                       ((struct multiboot_tag_module *) tag)->mod_end,
                       ((struct multiboot_tag_module *) tag)->cmdline);
               break;
             case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
               printf (" Memoire basse : %H, memoire haute = %lH\r\n",
                       ((struct multiboot_tag_basic_meminfo *) tag)->mem_lower<<10,
                       ((u64)((struct multiboot_tag_basic_meminfo *) tag)->mem_upper)<<10);
               break;
             case MULTIBOOT_TAG_TYPE_BOOTDEV:
               printf (" Peripherique de demarrage : %Y,%u,%u\r\n\r\n",
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
                   printf (" adresse: %lY, taille:%lY, type:%Y\r\n",
                           (u64) (mmap->addr),
                           (u64) (mmap->len),
                           (u32) (mmap->type));
               break;
             }
             case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
             {
                 struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *) tag;
                 printf (" Framebuffer, resolution %d*%d*%d adresse: %X, ",tagfb->common.framebuffer_width,tagfb->common.framebuffer_height,tagfb->common.framebuffer_bpp,tagfb->common.framebuffer_addr);
                 switch (tagfb->common.framebuffer_type)
                 {
                   case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
                     printf ("mode graphique indexé\r\n");
                     break;
                   case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
                     printf ("mode graphique RGB\r\n");
                     break;
                   case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
                     printf ("mode texte EGA\r\n");
                     break;
                 }
                 break;
            }
            case MULTIBOOT_TAG_TYPE_VBE:
            {
                struct multiboot_tag_vbe *tagvbe = (struct multiboot_tag_framebuffer *) tag;
                 printf (" VBE 2.0, mode %Y, adresse %hY:%hY-%hY\r\n", tagvbe->vbe_mode,tagvbe->vbe_interface_seg,tagvbe->vbe_interface_off,tagvbe->vbe_interface_len);
             }
        }
     } 
}
/*******************************************************************************/
