DEBUG=exec gnome-terminal --geometry=120x55+1+1 -x ./debug/debug.sh
REMOVE=rm -f
INSTALL=sudo apt-get install
COPY=cp
EMULATOR=bochs -f
GIT=git status
MAKECALL=python makesyscall.py

all: tools programs bits32 bits64 harddisk uefi
	sync

bits32: ARCH=bits32 
bits32: lib/libs.o system/system.sys
	sync

bits64: ARCH=bits64
bits64: lib/libs.o system/system.sys
	sync

tools: tools/build
	
tools/build:
	make -C tools

syscall: clean remakeapi all

remakeapi:
	$(MAKECALL)

programs: 
	make -C programs

harddisk: final/harddisk.img.final

uefi: final/harddiskuefi.img.final

install:
	$(INSTALL) gcc qemu fusefat fuseext2 gdb ovmf bsdmainutils tar bsdmainutils cramfsprogs indent binutils bochs bochs-x bochsbios dos2unix gnome-terminal

togit:	
	make -C system togit
	make -C lib togit
	make -C final togit
	make -C programs togit
	make -C tools togit
	$(GIT)
	sync

clean:	
	$(RM) -f .gdb_history	
	make -C system clean
	make -C lib clean
	make -C final clean
	make -C programs clean
	make -C tools clean
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
	make -C tools indent
	sync

backup: clean
	cd .. 
	tar cf - Source\ C | gzip -f - > backup.tar.gz

test: tools programs bits32 harddisk qemu

test64: tools programs bits64 uefi qemu64

retest: littleclean test

retest64: littleclean test64

testbochs: tools programs bits32 harddisk bochs-debug

view:
	hexdump  -C ./final/harddisk.img.final|head -c10000

debug: debug-system

redebug: littleclean debug-system

debug64: debug-system64

redebug64: littleclean debug-system64

kernel: debug-kernel

debug-boot: tools programs bits32 harddisk qemu-debug
	sleep 2
	$(DEBUG) ./debug/boot.txt

debug-system: tools programs bits32 harddisk qemu-debug
	sleep 2
	$(DEBUG) ./debug/system.txt

debug-system64: tools programs bits64 uefi qemu-debug64
	sleep 2
	$(DEBUG) ./debug/system.txt

bochs-debug: killer
	$(EMULATOR) ./debug/config.bochs

killer: 
	killall bochs-debug || true
	killall qemu-system-x86_64 || true
	killall qemu-system-i386 || true
	killall gnome-terminal-server || true
	tmux kill-session -t debug || true

qemu-debug: killer
	qemu-system-i386 -monitor telnet:127.0.0.1:6666,server,nowait -m 1G -drive format=raw,file=./final/harddisk.img.final -s -S &

qemu-debug64: killer
	qemu-system-x86_64 -monitor telnet:127.0.0.1:6666,server,nowait -m 5G -drive format=raw,file=./final/harddiskuefi.img.final --bios /usr/share/qemu/OVMF.fd -s -S &

qemu: killer
	qemu-system-i386 -m 1G -drive format=raw,file=./final/harddisk.img.final --enable-kvm -cpu host -s &

qemu64: killer
	qemu-system-x86_64 -m 5G -drive format=raw,file=./final/harddiskuefi.img.final --bios /usr/share/qemu/OVMF.fd --enable-kvm -cpu host -s &
	
system/system.sys:
	make -C system

final/harddisk.img.final:
	make -C final harddisk.img.final

final/harddiskuefi.img.final:
	make -C final harddiskuefi.img.final	

lib/libs.o:
	make -C lib	
