all: cos2000.img

cos2000.img:
	(dd if=/dev/zero of=cos2000.img count=2880 bs=512)
	(mkfs.msdos -F 12 -n "COS2000" cos2000.img)
	(mkdir ./mnt -p)
	(fusefat cos2000.img ./mnt -o rw+)
	(cp ../boot/loader.sys ./mnt/)
	(cp ../system/system.sys ./mnt/;sync)
	(fusermount ./mnt -u)
	(dd if=../boot/boot12.bin of=cos2000.img seek=0 count=1 conv=notrunc;sync)

clean:
	rm -f *.img
