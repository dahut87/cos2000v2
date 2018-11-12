/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */



typedef struct pciclass {
	u16 number;
	u8 *name;
} pciclass  __attribute__ ((packed));	

typedef struct pcidev
{
    u16 vendor_id;
    u16 device_id;
    u16 command;
    u16 status;
    u8 revision_id;
    u8 interface;
    u8 sub_class;
    u8 base_class;
    u8 cache_line_size;
    u8 latency_timer;
    u8 header_type;
    u8 bist;
} pcidev  __attribute__ ((packed));	


typedef struct pcidevmini {
  union {
    struct {
        u16 vendor_id;
        u16 device_id;
    };
   u32 dword;
  };
} pcidevmini  __attribute__ ((packed));	

u16 pciConfigReadWord(const u8 bus, const u8 dev, const u8 function, const u8 offset);
void scanPCImini(void);
pcidevmini getPCImininfo(const u8 bus, const u8 dev, const u8 function);
bool getPCInfo(pcidev *device, const u8 bus, const u8 dev, const u8 function);
void scanPCI(void);
u8 * pcigetclassname( const pcidev *device);
