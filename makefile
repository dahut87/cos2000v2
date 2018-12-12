all: programs bits32 bits64 harddisk uefi
	sync

bits32: ARCH=bits32 
bits32: lib/libs.o system/system.sys
	sync

bits64: ARCH=bits64
bits64: lib/libs.o system/system.sys
	sync

programs: programs/test

programs/test:
	make -C programs

harddisk: final/harddisk.img.final

uefi: final/harddiskuefi.img.final

install:
	(sudo apt-get install nasm gcc qemu fusefat fuseext2 cgdb ovmf bsdmainutils tar bsdmainutils indent binutils bochs bochs-x bochsbios)

togit:	
	make -C system togit
	make -C lib togit
	make -C final togit
	make -C programs togit
	git status
	sync

clean:	
	make -C system clean
	make -C lib clean
	make -C final clean
	make -C programs clean
	sync

littleclean:	
	make -C system clean
	make -C lib clean
	make -C final littleclean
	make -C programs clean
	sync

indent:
	make -C system indent
	make -C lib indent
	make -C programs indent
	sync

backup: clean
	(cd .. ; tar cf - Source\ C | gzip -f - > backup.tar.gz)

test: programs bits32 harddisk qemu

test64: programs bits64 uefi qemu64

retest: littleclean test

retest64: littleclean test64

testbochs: programs bits32 harddisk bochs-debug

view:
	(hexdump  -C ./final/harddisk.img.final|head -c10000)

debug: debug-system

redebug: littleclean debug-system

debug64: debug-system64

redebug64: littleclean debug-system64

debug-boot: bits32 harddisk qemu-debug
	(sleep 2;cgdb -x ./debug/boot.txt)

debug-loader: bits32 harddisk qemu-debug
	(sleep 2;cgdb -x ./debug/loader.txt)

debug-system: bits32 harddisk qemu-debug
	(sleep 2;cgdb -x ./debug/system.txt)

debug-system64: bits64 uefi qemu-debug64
	(sleep 2;cgdb -x ./debug/system.txt)

bochs-debug:
	(killall bochs-debug;bochs -f ./debug/config.bochs)

qemu-debug:
	(killall qemu-system-i386;qemu-system-i386 -m 1G -drive format=raw,file=./final/harddisk.img.final -s -S &)

qemu-debug64:
	(killall qemu-system-x86_64;qemu-system-x86_64 -m 5G -drive format=raw,file=./final/harddiskuefi.img.final --bios /usr/share/qemu/OVMF.fd -s -S &)

qemu:
	(killall qemu-system-i386;qemu-system-i386 -m 1G -drive format=raw,file=./final/harddisk.img.final --enable-kvm -cpu host -s &)  

qemu64:
	(killall qemu-system-x86_64;qemu-system-x86_64 -m 5G -drive format=raw,file=./final/harddiskuefi.img.final --bios /usr/share/qemu/OVMF.fd --enable-kvm -cpu host -s &)  
	
system/system.sys:
	make -C system VESA=$(VESA)

final/harddisk.img.final:
	make -C final harddisk.img.final

final/harddiskuefi.img.final:
	make -C final harddiskuefi.img.final	

lib/libs.o:
	make -C lib	
