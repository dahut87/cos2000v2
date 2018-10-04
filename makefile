all: bits32 bits64 floppy harddisk uefi
	sync

bits32: ARCH=bits32 
bits32: lib/libs.o system/system.sys
	sync

bits64: ARCH=bits64
bits64: lib/libs.o system/system.sys
	sync

floppy: boot/boot12.bin final/floppy.img.final

harddisk: final/harddisk.img.final

uefi: final/harddiskuefi.img.final

install:
	(sudo apt-get install nasm gcc qemu fusefat fuseext2 cgdb ovmf bsdmainutils tar bsdmainutils indent binutils)

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

indent:
	(cd system; make indent)
	(cd lib;make indent)
	sync

backup: clean
	(cd .. ; tar cf - Source\ C | gzip -f - > backup.tar.gz)

test: bits32 harddisk qemu

test64: bits64 uefi qemu64

retest: littleclean test

retest64: littleclean test64

floppytest: bits32 floppy qemu-floppy

refloppytest: littleclean floppytest

view:
	(hexdump  -C ./final/harddisk.img.final|head -c10000)

debug: debug-system

redebug: littleclean debug-system

debug64: debug-system64

debug-boot: bits32 harddisk qemu-debug
	(sleep 2;cgdb -x ./debug/boot.txt)

debug-loader: bits32 harddisk qemu-debug
	(sleep 2;cgdb -x ./debug/loader.txt)

debug-system: bits32 harddisk qemu-debug
	(sleep 2;cgdb -x ./debug/system.txt)

debug-system64: bits64 uefi qemu-debug64
	(sleep 2;cgdb -x ./debug/system.txt)

qemu-debug:
	(killall qemu-system-i386;qemu-system-i386 -m 1G -drive format=raw,file=./final/harddisk.img.final -s -S &)

qemu-debug64:
	(killall qemu-system-x86_64;qemu-system-x86_64 -m 1G -drive format=raw,file=./final/harddiskuefi.img.final --bios /usr/share/qemu/OVMF.fd -s -S &)

qemu:
	(killall qemu-system-i386;qemu-system-i386 -m 1G -drive format=raw,file=./final/harddisk.img.final --enable-kvm -cpu host -s &)  

qemu64:
	(killall qemu-system-x86_64;qemu-system-x86_64 -m 1G -drive format=raw,file=./final/harddiskuefi.img.final --bios /usr/share/qemu/OVMF.fd --enable-kvm -cpu host -s &)  

qemu-floppy:
	(killall qemu-system-i386;qemu-system-i386 -m 1G -fda ./final/floppy.img.final --enable-kvm -cpu host -s &)  
	
system/system.sys:
	(cd system; make ARCH=$(ARCH))
	
boot/boot12.bin:
	(cd boot; make)	

final/floppy.img.final:
	(cd final; make floppy.img.final)

final/harddisk.img.final:
	(cd final; make harddisk.img.final)

final/harddiskuefi.img.final:
	(cd final; make harddiskuefi.img.final)	

lib/libs.o:
	(cd lib; make)	
