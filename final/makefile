harddiskuefi.img.before:
	xz -d -k harddiskuefi.img.xz
	dd if=harddiskuefi.img of=harddiskuefi.img.part1 skip=13672 bs=512
	dd if=harddiskuefi.img of=harddiskuefi.img.before count=13672 bs=512
	mkdir ./mnt -p
	sync

harddiskuefi.img.final: initram.img harddiskuefi.img.before
	mount.fuse-ext2 -o rw+ harddiskuefi.img.part1 ./mnt
	cp ../system/system.sys ./mnt/boot/
	cp ./initram.img ./mnt/boot/
	sync
	fusermount ./mnt -u
	cat harddiskuefi.img.before > harddiskuefi.img.final
	cat harddiskuefi.img.part1 >> harddiskuefi.img.final
	sync

harddisk.img.before:
	xz -d -k harddisk.img.xz
	dd if=harddisk.img of=harddisk.img.part1 skip=63 bs=512
	dd if=harddisk.img of=harddisk.img.before count=63 bs=512
	mkdir ./mnt -p
	sync

harddisk.img.final: initram.img harddisk.img.before
	mount.fuse-ext2 -o rw+ harddisk.img.part1 ./mnt
	cp ../system/system.sys ./mnt/boot/
	cp ./initram.img ./mnt/boot/
	sync
	fusermount ./mnt -u
	cat harddisk.img.before > harddisk.img.final
	cat harddisk.img.part1 >> harddisk.img.final
	sync	

initram.img:
	make -C ../programs

littleclean:
	rm -f *.final

togit: clean

clean:
	rm -f *.before
	rm -f *.part1
	rm -f *.img
	rm -f *.final
