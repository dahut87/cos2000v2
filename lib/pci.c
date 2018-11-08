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
/* Detecte s */

pcidev getPCIinfo(const u8 bus, const u8 dev, const u8 function)
{
    pcidev result;
    const u32 registry = 0; 
    u32 addr = (0x80000000|(bus << 16)|(dev << 11)|(function << 8)|(registry & 0xFC));
    outd(0xCF8, addr);
    result.dword = ind(0xCFC);
    if (result.dword == 0xFFFFFFFF)
        result.dword = 0x0;
    return result;
}

/*******************************************************************************/
/* Scan le bus PCI et affiche les périphériques */

void scanPCI(void)
{
    u32 i, bus, device, function;
    pcidev result;    
    i = 0;
    for (bus=0; bus<MAX_BUS_SCAN; ++bus)
      for (device=0; device<MAX_DEVICE_SCAN; ++device)
        for (function=0; function<MAX_FUNCTION_SCAN; ++function)
        {
          result = getPCIinfo(bus, device, function);
          if (result.dword)
            printf("%d) Bus: 0x%x, Device: 0x%x. Function: 0x%x (0x%hx:0x%hx)\r\n",
                   ++i, bus, device, function, result.vendor, result.device);
        }
    return;
}




/*******************************************************************************/

