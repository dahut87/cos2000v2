/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
/* Sources modifiées du noyau Linux linux-source-4.15.0/arch/x86/include/uapi/asm/bootparam.h
/* Sources modifiées du noyau Linux linux-source-4.15.0/arch/x86/include/asm/e820/type.h

/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */

#ifndef _BOOT
#define _BOOT 

#include "types.h"
#include "gdt.h"

#define E820_MAX_ENTRIES 128

typedef struct entrye820 {
	u64 addr;
	u64 size;
	u32 type;
} entrye820 __attribute__((packed));

typedef struct bootparams {
	entrye820 e820_table[E820_MAX_ENTRIES];
	u32	e820_numbers;
	u8* cmdline;
	u8 kbflag;
	u32 ramdisksize;
	u32 ramdiskaddr;
} bootparams;


enum e820_type {
	E820_TYPE_RAM		= 1,
	E820_TYPE_RESERVED	= 2,
	E820_TYPE_ACPI		= 3,
	E820_TYPE_NVS		= 4,
	E820_TYPE_UNUSABLE	= 5,
	E820_TYPE_PMEM		= 7,
	E820_TYPE_PRAM		= 12,
	E820_TYPE_RESERVED_KERN	= 128,
};

typedef struct header {
	u8	setup_sects;
	u16	root_flags;
	u32	syssize;
	u16	ram_size;
	u16	vid_mode;
	u16	root_dev;
	u16	boot_flag;
	u16	jump;
	u32	header;
	u16	version;
	u32	realmode_swtch;
	u16	start_sys_seg;
	u16	kernel_version;
	u8	type_of_loader;
	u8	loadflags;
	u16	setup_move_size;
	u32	code32_start;
	u32	ramdisk_image;
	u32	ramdisk_size;
	u32	bootsect_kludge;
	u16	heap_end_ptr;
	u8	ext_loader_ver;
	u8	ext_loader_type;
	u32	cmd_line_ptr;
	u32	initrd_addr_max;
	u32	kernel_alignment;
	u8	relocatable_kernel;
	u8	min_alignment;
	u16	xloadflags;
	u32	cmdline_size;
	u32	hardware_subarch;
	u64	hardware_subarch_data;
	u32	payload_offset;
	u32	payload_length;
	u64	setup_data;
	u64	pref_address;
	u32	init_size;
	u32	handover_offset;
} __attribute__((packed)) header;

#endif
