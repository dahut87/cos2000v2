all:	makall

makall: lib/libs.o system/system.sys final/harddisk.img.final
	sync

floppy: boot/boot12.bin final/floppy.img.final

harddisk: final/harddisk.img.final

install:
	(sudo apt-get install nasm gcc qemu fusefat fuseext2 cgdb)

clean:	
	(cd system; make clean)
	(cd boot; make clean)
	(cd lib;make clean)
	(cd final;make clean)
	sync

littleclean:	
	(cd system; make clean)
	(cd boot; make clean)
	(cd lib;make clean)
	(cd final;make littleclean)
	sync

backup: clean
	(cd .. ; tar cf - Source\ C | gzip -f - > backup.tar.gz)

test: all harddisk qemu

retest: littleclean test

floppytest: floppy qemu-floppy

view:
	(hexdump  -C ./final/harddisk.img.final|head -c10000)

debug: debug-system

debug-boot: all harddisk qemu-debug
	(sleep 2;cgdb -x ./debug/boot.txt)

debug-loader: all harddisk qemu-debug
	(sleep 2;cgdb -x ./debug/loader.txt)

debug-system: all harddisk qemu-debug
	(sleep 2;cgdb -x ./debug/system.txt)

qemu-debug:
	(killall qemu-system-i386;qemu-system-i386 -m 1G -drive format=raw,file=./final/harddisk.img.final -s -S &)

qemu:
	(killall qemu-system-i386;qemu-system-i386 -m 1G -drive format=raw,file=./final/harddisk.img.final --enable-kvm -cpu host -s &)  

qemu-floppy:
	(killall qemu-system-i386;qemu-system-i386 -m 1G -fda ./final/floppy.img.final --enable-kvm -cpu host -s &)  
	
system/system.sys:
	(cd system; make)
	
boot/boot12.bin:
	(cd boot; make)	

final/floppy.img.final:
	(cd final; make floppy.img.final)

final/harddisk.img.final:
	(cd final; make harddisk.img.final)	  

lib/libs.o:
	(cd lib; make)	
