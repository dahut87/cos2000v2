/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "asm.h"
#include "types.h"
#include "pci.h"

#define MAX_BUS_SCAN        256
#define MAX_DEVICE_SCAN     32
#define MAX_FUNCTION_SCAN   8

/*******************************************************************************/
/* Récupère les identifiants vendeur / periphérique du periphérique PCI donnée */

pcidev getPCImininfo(const u8 bus, const u8 dev, const u8 function)
{
    pcidev result;
    if ((result.vendor = pciConfigReadWord(bus,dev,function,0x0)) != 0xFFFF) {
       result.device = pciConfigReadWord(bus,dev,function,0x2);
    }
    return result;
}

/*******************************************************************************/
/* Récupère les informations sur le periphérique PCI donnée */

u16 pciConfigReadWord(const u8 bus, const u8 dev, const u8 function, const u8 offset)
{
    u16 tmp = 0;
    u32 addr = (0x80000000|(bus << 16)|(dev << 11)|(function << 8)|(offset & 0xFC));
    outd(0xCF8, addr);
    tmp = (u16)((ind(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return tmp;
}

/*******************************************************************************/
/* Scan le bus PCI et affiche les périphériques */

void scanPCI(void)
{
    u16 bus,device,function;
    pcidev result;    
    for (bus=0; bus<MAX_BUS_SCAN; ++bus)
      for (device=0; device<MAX_DEVICE_SCAN; ++device)
        for (function=0; function<MAX_FUNCTION_SCAN; ++function)
        {
          result = getPCImininfo(bus,device,function);
          if (result.vendor != 0xFFFF)
            printf("Bus:%hx Dev:%hx Func:%hx %hx:%hx\r\n",bus, device, function, result.vendor, result.device);
        }
    return;
}

/*******************************************************************************/

