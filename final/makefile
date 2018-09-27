all: harddisk.img.final floppy.img.final

harddisk.img.before:
	(xz -d -k harddisk.img.xz)
	(dd if=harddisk.img of=harddisk.img.part1 skip=63 bs=512)
	(dd if=harddisk.img of=harddisk.img.before count=63 bs=512;sync)
	(mkdir ./mnt -p)

harddisk.img.final: harddisk.img.before
	(mount.fuse-ext2 -o rw+ harddisk.img.part1 ./mnt)
	(cp ../system/system.sys ./mnt/boot/;sync)
	(fusermount ./mnt -u)
	(cat harddisk.img.before > harddisk.img.final)
	(cat harddisk.img.part1 >> harddisk.img.final;sync)

floppy.img.final:
	(dd if=/dev/zero of=floppy.img.final count=2880 bs=512)
	(mkfs.msdos -F 12 -n "COS2000" floppy.img.final)
	(mkdir ./mnt -p)
	(fusefat floppy.img.final ./mnt -o rw+)
	(cp ../boot/loader.sys ./mnt/)
	(cp ../system/system.sys ./mnt/;sync)
	(fusermount ./mnt -u)
	(dd if=../boot/boot12.bin of=floppy.img.final seek=0 count=1 conv=notrunc;sync)

littleclean:
	rm -f *.final

clean:
	rm -f *.before
	rm -f *.part1
	rm -f *.img
	rm -f *.final
