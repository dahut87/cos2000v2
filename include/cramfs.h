/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

/* Sources modifiées du noyau Linux cramfs_fs.h
/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */

#include "types.h"

#ifndef _CRAMFS
#define _CRAMFS

#define CRAMFS_SIGNATURE	"Compressed ROMFS"

#define CRAMFS_MODE_WIDTH 16
#define CRAMFS_UID_WIDTH 16
#define CRAMFS_SIZE_WIDTH 24
#define CRAMFS_GID_WIDTH 8
#define CRAMFS_NAMELEN_WIDTH 6
#define CRAMFS_OFFSET_WIDTH 26
#define CRAMFS_MAGIC                0x28cd3d45
#define CRAMFS_MAXPATHLEN (((1 << CRAMFS_NAMELEN_WIDTH) - 1) << 2)
#define PAD_SIZE 512
#define PAGE_CACHE_SIZE (4096)
typedef struct cramfs_inode {
	u32 mode:CRAMFS_MODE_WIDTH;
	u32 uid:CRAMFS_UID_WIDTH;
	u32 size:CRAMFS_SIZE_WIDTH;
	u32 gid:CRAMFS_GID_WIDTH;
	u32 namelen:CRAMFS_NAMELEN_WIDTH;
	u32 offset:CRAMFS_OFFSET_WIDTH;
} cramfs_inode;

typedef struct cramfs_info {
	u32 crc;
	u32 edition;
	u32 blocks;
	u32 files;
} cramfs_info;

typedef struct cramfs_super {
	u32 magic;			/* 0x28cd3d45 - random number */
	u32 size;			/* length in bytes */
	u32 flags;			/* feature flags */
	u32 future;			/* reserved for future use */
	u8 signature[16];		/* "Compressed ROMFS" */
	cramfs_info fsid;		/* unique filesystem info */
	u8 name[16];		/* user-defined name */
	cramfs_inode root;	/* root inode data */
} cramfs_super;

#define	S_ISDIR(m)	((m & 0170000) == 0040000)	/* directory */
#define	S_ISCHR(m)	((m & 0170000) == 0020000)	/* char special */
#define	S_ISBLK(m)	((m & 0170000) == 0060000)	/* block special */
#define	S_ISREG(m)	((m & 0170000) == 0100000)	/* regular file */
#define	S_ISFIFO(m)	((m & 0170000) == 0010000)	/* fifo */
#define	S_ISLNK(m)	((m & 0170000) == 0120000)	/* symbolic link */
#define	S_ISSOCK(m)	((m & 0170000) == 0140000)	/* socket */

#define CRAMFS_FLAG_FSID_VERSION_2	0x00000001	/* fsid version #2 */
#define CRAMFS_FLAG_SORTED_DIRS		0x00000002	/* sorted dirs */
#define CRAMFS_FLAG_HOLES		0x00000100	/* support for holes */
#define CRAMFS_FLAG_WRONG_SIGNATURE	0x00000200	/* reserved */
#define CRAMFS_FLAG_SHIFTED_ROOT_OFFSET	0x00000400	/* shifted root fs */
#define CRAMFS_FLAG_EXT_BLOCK_POINTERS	0x00000800	/* block pointer extensions */

#define CRAMFS_SUPPORTED_FLAGS	( 0x000000ff \
				| CRAMFS_FLAG_HOLES \
				| CRAMFS_FLAG_WRONG_SIGNATURE \
				| CRAMFS_FLAG_SHIFTED_ROOT_OFFSET \
				| CRAMFS_FLAG_EXT_BLOCK_POINTERS )

#define CRAMFS_BLK_FLAG_UNCOMPRESSED	(1 << 31)
#define CRAMFS_BLK_FLAG_DIRECT_PTR	(1 << 30)

#define CRAMFS_BLK_FLAGS	( CRAMFS_BLK_FLAG_UNCOMPRESSED \
				| CRAMFS_BLK_FLAG_DIRECT_PTR )

#define CRAMFS_BLK_DIRECT_PTR_SHIFT	2

u32 test_super(u8 *src, u32 length);

#endif
