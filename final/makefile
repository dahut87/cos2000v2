REMOVE=rm -f
INSTALL=sudo apt-get install
COPY=cp
DISKCOPY=dd
COMPRESS=xz
SYNC=sync
CREATEDIR=mkdir
CAT=cat

all: harddisk.img.final harddiskuefi.img.final

harddiskuefi.img.before:
	$(COMPRESS) -d -k harddiskuefi.img.xz
	$(DISKCOPY) if=harddiskuefi.img of=harddiskuefi.img.part1 skip=13672 bs=512
	$(DISKCOPY) if=harddiskuefi.img of=harddiskuefi.img.before count=13672 bs=512
	$(SYNC)
	$(CREATEDIR) ./mnt -p

harddiskuefi.img.final: harddiskuefi.img.before
	mount -t ext2 -o rw harddiskuefi.img.part1 ./mnt
	$(COPY) ../system/system.sys ./mnt/boot/
	$(SYNC)
	umount ./mnt
	cat harddiskuefi.img.before > harddiskuefi.img.final
	cat harddiskuefi.img.part1 >> harddiskuefi.img.final
	$(SYNC)

harddisk.img.before:
	$(COMPRESS) -d -k harddisk.img.xz
	$(DISKCOPY) if=harddisk.img of=harddisk.img.part1 skip=63 bs=512
	$(DISKCOPY) if=harddisk.img of=harddisk.img.before count=63 bs=512
	$(SYNC)
	$(CREATEDIR) ./mnt -p

harddisk.img.final: harddisk.img.before
	mount -t ext2 -o rw harddisk.img.part1 ./mnt
	$(COPY) ../system/system.sys ./mnt/boot/
	$(SYNC)
	umount ./mnt
	cat harddisk.img.before > harddisk.img.final
	cat harddisk.img.part1 >> harddisk.img.final
	$(SYNC)

initram.img:
	$(MAKE) ../programs

littleclean:
	$(REMOVE) *.final

togit: clean

clean:
	$(REMOVE) *.before
	$(REMOVE) *.part1
	$(REMOVE) *.img
	$(REMOVE) *.final

