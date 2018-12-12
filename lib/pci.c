/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#include "asm.h"
#include "types.h"
#include "pci.h"
#include "PCI/class.c"

#define MAX_BUS_SCAN        256
#define MAX_DEVICE_SCAN     32
#define MAX_FUNCTION_SCAN   8

/*******************************************************************************/
/* Retourne une chaine correspondant à l'ID class/subclass PCI */

u8     *pcigetclassname(const pcidev * device)
{
	pciclass *class = NULL;
	if (device == NULL)
	{
		return NULL;
	}
	if (device->base_class == 0xff)
	{
		return "Unassigned class";
	}
	class = classcodes[device->base_class];
	if (!class)
	{
		return "Unknowned class";
	}
	while (class->name != NULL)
	{
		if (class->number == device->sub_class)
		{
			return class->name;
		}
		class++;
	}

	return "Unknowned class";
}

/*******************************************************************************/
/* Récupère les identifiants vendeur / periphérique du periphérique PCI donnée */

pcidevmini getPCImininfo(const u8 bus, const u8 dev, const u8 function)
{
	pcidevmini result;
	if ((result.vendor_id =
	     pciConfigReadWord(bus, dev, function, 0x0)) != 0xFFFF)
	{
		result.device_id =
			pciConfigReadWord(bus, dev, function, 0x2);
	}
	return result;
}

/*******************************************************************************/
/* Récupère toutes les informations du periphérique PCI donnée */

bool getPCInfo(pcidev * device, const u8 bus, const u8 dev,
	       const u8 function)
{
	if ((device->vendor_id =
	     pciConfigReadWord(bus, dev, function, 0x0)) != 0xFFFF)
	{
		u16     temp;
		device->device_id =
			pciConfigReadWord(bus, dev, function, 0x2);
		device->command =
			pciConfigReadWord(bus, dev, function, 0x4);
		device->status =
			pciConfigReadWord(bus, dev, function, 0x6);
		temp = pciConfigReadWord(bus, dev, function, 0x8);
		device->revision_id = temp & 0xFF;
		device->interface = temp >> 8;
		temp = pciConfigReadWord(bus, dev, function, 0xA);
		device->sub_class = temp & 0xFF;
		device->base_class = temp >> 8;
		temp = pciConfigReadWord(bus, dev, function, 0xC);
		device->cache_line_size = temp & 0xFF;
		device->latency_timer = temp >> 8;
		temp = pciConfigReadWord(bus, dev, function, 0xE);
		device->header_type = temp & 0xFF;
		device->bist = temp >> 8;
		return true;
	}
	else
		return false;
}

/*******************************************************************************/
/* Récupère les informations sur le periphérique PCI donnée */

u16 pciConfigReadWord(const u8 bus, const u8 dev, const u8 function,
		      const u8 offset)
{
	u16     tmp = 0;
	u32     addr =
		(0x80000000 | (bus << 16) | (dev << 11) | (function << 8) |
		 (offset & 0xFC));
	outd(0xCF8, addr);
	tmp = (u16) ((ind(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
	return tmp;
}

/*******************************************************************************/
/* Scan le bus PCI et affiche les périphériques */

void scanPCImini(void)
{
	u16     bus, device, function;
	pcidevmini result;
	for (bus = 0; bus < MAX_BUS_SCAN; ++bus)
		for (device = 0; device < MAX_DEVICE_SCAN; ++device)
			for (function = 0; function < MAX_FUNCTION_SCAN;
			     ++function)
			{
				result = getPCImininfo(bus, device,
						       function);
				if (result.vendor_id != 0xFFFF)
					printf("Bus:%hx Dev:%hx Func:%hx %hx:%hx\r\n", bus, device, function, result.vendor_id, result.device_id);
			}
	return;
}

/*******************************************************************************/

/* Scan le bus PCI et affiche les périphériques */

void scanPCI(void)
{
	u16     bus, device, function;
	pcidev  result;
	for (bus = 0; bus < MAX_BUS_SCAN; ++bus)
		for (device = 0; device < MAX_DEVICE_SCAN; ++device)
			for (function = 0; function < MAX_FUNCTION_SCAN;
			     ++function)
				if (getPCInfo
				    (&result, bus, device, function))
					printf("Bus:%hx Dev:%hx Func:%hx %hx:%hx ->%s\r\n", bus, device, function, result.vendor_id, result.device_id, pcigetclassname(&result));
	return;
}

/*******************************************************************************/
