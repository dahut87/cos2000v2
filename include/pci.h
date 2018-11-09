/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

typedef struct pcidev {
  union {
    struct {
        u16 vendor;
        u16 device;
    };
   u32 dword;
  };
} pcidev  __attribute__ ((packed));	

u16 pciConfigReadWord(const u8 bus, const u8 dev, const u8 function, const u8 offset);
void scanPCI(void);
pcidev getPCImininfo(const u8 bus, const u8 dev, const u8 function);
