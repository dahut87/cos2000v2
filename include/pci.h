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

