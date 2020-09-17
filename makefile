##### Variables
GODEBUG=exec gnome-terminal --geometry=120x55+1+1 -x ./debug/debug.sh
REMOVE=rm -f
INSTALL=sudo apt-get install
COPY=cp
OLDEMUX86=bochs -f
GIT=git status
MAKECALL=python makesyscall.py
MAKE=make -C
SYNC=sync
KILL=killall
TAR=tar cf - . | gzip -f - > ./backup.tar.gz
HEXDUMP=hexdump  -C ./final/harddisk.img.final|head -c10000
TMUXKILL=tmux kill-session -t
TRUE=|| true
SPICE=spicy --uri=spice://127.0.0.1?port=5900
SOURCE=xxd -i 
EMUX86=qemu-system-i386
EMUX64=qemu-system-x86_64
LEGACY=-m 1G -drive format=raw,file=./final/harddisk.img.final --enable-kvm -cpu host -s -vga qxl -spice port=5900,addr=127.0.0.1,disable-ticketing
UEFI=-m 5G -drive format=raw,file=./final/harddiskuefi.img.final --bios /usr/share/qemu/OVMF.fd --enable-kvm -cpu host -s -vga qxl -spice port=5900,addr=127.0.0.1,disable-ticketing
DEBUG=-S -monitor telnet:127.0.0.1:6666,server,nowait
WAIT2S=sleep 2

##### Construction

all: tools programs system32 system64 harddisk harddiskuefi
	$(SYNC)

system32: ARCH=bits32 
system32: lib/libs.o system/system.sys
	$(SYNC)

system64: ARCH=bits64
system64: lib/libs.o system/system.sys
	$(SYNC)

tools: tools/build
	
tools/build:
	$(MAKE) tools

syscall: clean remakeapi all

remakeapi:
	$(MAKECALL)

programs: programs/test lib/TEST/test.c lib/TEST/test2.c

lib/TEST/test.c:
	$(SOURCE) programs/test lib/TEST/test.c

lib/TEST/test2.c:
	$(SOURCE) programs/test2 lib/TEST/test2.c

programs/test:
	$(MAKE) programs

harddisk: final/harddisk.img.final

harddiskuefi: final/harddiskuefi.img.final

system/system.sys:
	$(MAKE) system

final/harddisk.img.final:
	$(MAKE) final harddisk.img.final

final/harddiskuefi.img.final:
	$(MAKE) final harddiskuefi.img.final	

lib/libs.o:
	$(MAKE) libsystem/system.sys:
	$(MAKE) system

final/harddisk.img.final:
	$(MAKE) final harddisk.img.final

final/harddiskuefi.img.final:
	$(MAKE) final harddiskuefi.img.final	

lib/libs.o:
	$(MAKE) lib

##### Divers

togit:	
	$(MAKE) system togit
	$(MAKE) lib togit
	$(MAKE) final togit
	$(MAKE) programs togit
	$(MAKE) tools togit
	$(GIT)
	$(SYNC)

clean:	
	$(REMOVE) ./syscalls.txt
	$(REMOVE) .gdb_history	
	$(MAKE) system clean
	$(MAKE) lib clean
	$(MAKE) final clean
	$(MAKE) programs clean
	$(MAKE) tools clean
	$(SYNC)

littleclean:	
	$(MAKE) system clean
	$(MAKE) lib clean
	$(MAKE) final littleclean
	$(MAKE) programs clean
	$(SYNC)

indent:
	$(MAKE) system indent
	$(MAKE) lib indent
	$(MAKE) programs indent
	$(MAKE) tools indent
	$(SYNC)

killer: 
	$(KILL) bochs-debug $(TRUE)
	$(KILL) qemu-system-x86_64 $(TRUE)
	$(KILL) qemu-system-i386 $(TRUE)
	$(KILL) gnome-terminal-server $(TRUE)
	$(TMUXKILL) debug $(TRUE)

backup: clean
	$(TAR)

view:
	$(HEXDUMP)

##### Alias

test: test32

retest: retest32

test32: tools programs system32 harddisk qemu32

test64: tools programs system64 harddiskuefi qemu64

retest32: littleclean test32

retest64: littleclean test64

testbochs: tools programs system32 harddisk bochs-debug

##### Debuguage

debug: debug-system32

redebug32: littleclean debug-system32

debug64: debug-system64

redebug64: littleclean debug-system64

kernel: debug-kernel

debug-boot: tools programs system32 harddisk qemu-debug32
	$(WAIT2S)
	$(GODEBUG) ./debug/boot.txt

debug-system32: tools programs system32 harddisk qemu-debug32
	$(WAIT2S)
	$(GODEBUG) ./debug/system.txt

debug-system64: tools programs system32 harddiskuefi qemu-debug64
	$(WAIT2S)
	$(GODEBUG) ./debug/system.txt

bochs-debug: killer
	$(OLDEMUX86) ./debug/config.bochs

##### Emulation

qemu-debug32: killer
	$(EMUX86) $(LEGACY) $(DEBUG) &
	$(SPICE) &

qemu-debug64: killer
	$(EMUX64) $(UEFI) $(DEBUG) &
	$(SPICE) &

qemu32: killer
	$(EMUX86) $(LEGACY) &
	$(WAIT2S)
	$(SPICE)

qemu64: killer
	$(EMUX64) $(UEFI) &
	$(WAIT2S)
	$(SPICE)

