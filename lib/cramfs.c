/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
/* Quelques portions modifiée proviennent de cramfsck - check a cramfs file system
 *
 * Copyright (C) 2000-2002 Transmeta Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * 1999/12/03: Linus Torvalds    (cramfs tester and unarchive program)
 * 2000/06/03: Daniel Quinlan    (CRC and length checking program)
 * 2000/06/04: Daniel Quinlan    (merged programs, added options, support
 *                               for special files, preserve permissions and
 *                               ownership, cramfs superblock v2, bogus mode
 *                               test, pathname length test, etc.)
 * 2000/06/06: Daniel Quinlan    (support for holes, pretty-printing,
 *                               symlink size test)
 * 2000/07/11: Daniel Quinlan    (file length tests, start at offset 0 or 512,
 *                               fsck-compatible exit codes)
 * 2000/07/15: Daniel Quinlan    (initial support for block devices)
 * 2002/01/10: Daniel Quinlan    (additional checks, test more return codes,
 *                               use read if mmap fails, standardize messages)
 * 2015/06/09: Yves-Noel Weweler (support for ignoring errors, when
 *                               extracting obscure cramfs files) */

#include "types.h"
#include "cramfs.h"
#include "boot.h"
#include "memory.h"

u8* initrambloc;
u8 start;
cramfs_super* super;
extern bootparams* allparams;

/*******************************************************************************/
/* Copie l'initram CRAMFS vers un malloc */
void remap_initram()
{
	virtual_range_use_kernel(allparams->ramdiskaddr, allparams->ramdiskaddr, allparams->ramdisksize, PAGE_NOFLAG);
	initrambloc=vmalloc(allparams->ramdisksize);
	memcpy(allparams->ramdiskaddr,initrambloc,allparams->ramdisksize,0);
	virtual_range_free_kernel(allparams->ramdiskaddr,allparams->ramdisksize);
}

/*******************************************************************************/
/* Lit l'inode et renvoie un pointeur grâce à vmalloc */

cramfs_inode *cramfs_iget(cramfs_inode * i)
{
	cramfs_inode *inode = vmalloc(sizeof(cramfs_inode));
	if (!inode) 
		return NULL;
	*inode = *i;
	return inode;
}

/*******************************************************************************/
/* Lit l'inode principal */

cramfs_inode *read_super(void)
{
	u32 offset = super.root.offset << 2;

	if (!S_ISDIR(super.root.mode))
		return NULL; /* fichier */
	if (!(super.flags & CRAMFS_FLAG_SHIFTED_ROOT_OFFSET) && ((offset != sizeof(cramfs_super)) &&
	     (offset != PAD_SIZE + sizeof(cramfs_super))))
		return NULL ; /*offset erronée */
	return cramfs_iget(&super.root);
}



static int uncompress_block(void *src, int len)
{
	int err;

	stream.next_in = src;
	stream.avail_in = len;

	stream.next_out = (unsigned char *) outbuffer;
	stream.avail_out = PAGE_CACHE_SIZE*2;

	inflateReset(&stream);

	if (len > PAGE_CACHE_SIZE*2) {
		die(FSCK_UNCORRECTED, 0, "data block too large");
	}
	err = inflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) {
		die(FSCK_UNCORRECTED, 0, "decompression error %p(%d): %s",
		    zError(err), src, len);
	}
	return stream.total_out;
}

static void do_uncompress(char *path, int fd, unsigned long offset, unsigned long size)
{
	unsigned long curr = offset + 4 * ((size + PAGE_CACHE_SIZE - 1) / PAGE_CACHE_SIZE);

	do {
		unsigned long out = PAGE_CACHE_SIZE;
		unsigned long next = *(u32 *) romfs_read(offset);

		if (next > end_data) {
			end_data = next;
		}

		offset += 4;
		if (curr == next) {
			if (opt_verbose > 1) {
				printf("  hole at %ld (%d)\n", curr, PAGE_CACHE_SIZE);
			}
			if (size < PAGE_CACHE_SIZE)
				out = size;
			memset(outbuffer, 0x00, out);
		}
		else {
			if (opt_verbose > 1) {
				printf("  uncompressing block at %ld to %ld (%ld)\n", curr, next, next - curr);
			}
			out = uncompress_block(romfs_read(curr), next - curr);
		}
		if (size >= PAGE_CACHE_SIZE) {
			if (out != PAGE_CACHE_SIZE) {
				die(FSCK_UNCORRECTED, 0, "non-block (%ld) bytes", out);
			}
		} else {
			if (out != size) {
				die(FSCK_UNCORRECTED, 0, "non-size (%ld vs %ld) bytes", out, size);
			}
		}
		size -= out;
		if (opt_extract) {
			if (write(fd, outbuffer, out) < 0) {
				die(FSCK_ERROR, 1, "write failed: %s", path);
			}
		}
		curr = next;
	} while (size);
}

tatic void change_file_status(char *path, struct cramfs_inode *i)
{
	struct utimbuf epoch = { 0, 0 };

	if (euid == 0) {
		if (lchown(path, i->uid, i->gid) < 0) {
			die(FSCK_ERROR, 1, "lchown failed: %s", path);
		}
		if (S_ISLNK(i->mode))
			return;
		if ((S_ISUID | S_ISGID) & i->mode) {
			if (chmod(path, i->mode) < 0) {
				die(FSCK_ERROR, 1, "chown failed: %s", path);
			}
		}
	}
	if (S_ISLNK(i->mode))
		return;
	if (utime(path, &epoch) < 0) {
		die(FSCK_ERROR, 1, "utime failed: %s", path);
	}
}

static void do_directory(char *path, struct cramfs_inode *i)
{
	int pathlen = strlen(path);
	int count = i->size;
	unsigned long offset = i->offset << 2;
	char *newpath = malloc(pathlen + 256);

	if (!newpath) {
		die(FSCK_ERROR, 1, "malloc failed");
	}
	if (offset == 0 && count != 0) {
		die(FSCK_UNCORRECTED, 0, "directory inode has zero offset and non-zero size: %s", path);
	}
	if (offset != 0 && offset < start_dir) {
		start_dir = offset;
	}
	/* TODO: Do we need to check end_dir for empty case? */
	memcpy(newpath, path, pathlen);
	newpath[pathlen] = '/';
	pathlen++;
	if (opt_verbose) {
		print_node('d', i, path);
	}
	if (opt_extract) {
		if (mkdir(path, i->mode) < 0) {
			die(FSCK_ERROR, 1, "mkdir failed: %s", path);
		}
		change_file_status(path, i);
	}
	while (count > 0) {
		struct cramfs_inode *child = iget(offset);
		int size;
		int newlen = child->namelen << 2;

		size = sizeof(struct cramfs_inode) + newlen;
		count -= size;

		offset += sizeof(struct cramfs_inode);

		memcpy(newpath + pathlen, romfs_read(offset), newlen);
		newpath[pathlen + newlen] = 0;
		if (newlen == 0) {
			die(FSCK_UNCORRECTED, 0, "filename length is zero");
		}
		if ((pathlen + newlen) - strlen(newpath) > 3) {
			die(FSCK_UNCORRECTED, 0, "bad filename length");
		}
		expand_fs(newpath, child);

		offset += newlen;

		if (offset <= start_dir) {
			die(FSCK_UNCORRECTED, 0, "bad inode offset");
		}
		if (offset > end_dir) {
			end_dir = offset;
		}
		iput(child); /* free(child) */
	}
	free(newpath);
}

void do_file(u8 *path, cramfs_inode *i)
{
	u32 offset = i->offset << 2;
	if (offset == 0 && i->size != 0)
		return /*null de taille non nulle */
	if (i->size == 0 && offset != 0) 
		return /*non null de taille nulle */
	if (offset != 0 && offset < start_data)
		start_data = offset;
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, i->mode);
	if (i->size)
	do_uncompress(path, fd, offset, i->size);
	close(fd);
	change_file_status(path, i);
}

void expand_fs(u8 *path, cramfs_inode *inode)
{
	if (S_ISDIR(inode->mode)) {
		do_directory(path, inode);
	}
	else if (S_ISREG(inode->mode)) {
		do_file(path, inode);
	}
	else if (S_ISLNK(inode->mode)) {
		/*do_symlink(path, inode);*/
	}
	else {
		/*do_special_inode(path, inode);*/
	}
}

/*******************************************************************************/
/* Vérification d'une image cramfs */
/* ERREURS :
/* 1=taille du super bloc trop petite */
/* 2=pas de magic */
/* 3=pas géré */
/* 4=superbloc trop petit */
/* 5=aucune fichier */
/* 6=taille entrée des fichiers trop petite */
/* 7=taille entrée des fichiers trop grande */
/* 8=version de cramfs non supportée */
/* 9=crc erroné */

u32 test_super(u8 *src, u32 length) {
	super=(cramfs_super*)src;
	start = 0xFF;
	if (length < sizeof(cramfs_super)) {
		return 1;
	}
	if (super->magic == CRAMFS_MAGIC) {
		start = 0;
	}
	else if (length >= (PAD_SIZE + sizeof(super))) {
		if (super->magic == CRAMFS_MAGIC) {
			start = PAD_SIZE; 
		}
	}
	else
		return 2;
	if (super->flags & ~CRAMFS_SUPPORTED_FLAGS)
		return 3;
	if (super->size < PAGE_CACHE_SIZE)
		return 4;
	if (super->flags & CRAMFS_FLAG_FSID_VERSION_2) {
		if (super->fsid.files == 0) 
			return 5;
		if (length < super->size)
			return 6;
		else if (length > super->size)
			return 7;
	}
	else
		return 8;
	u32 oldcrc=super->fsid.crc;
	super->fsid.crc = crc32(0, 0, 0);
	u32 newcrc=crc32(0,src+*start,super->size-*start);
	super->fsid.crc = oldcrc; 
	if (newcrc!=oldcrc)
		return 9;
	return NULL;
}


