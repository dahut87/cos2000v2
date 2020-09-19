##### Variables
GODEBUG=exec gnome-terminal --geometry=120x55+1+1 -x ./debug/debug.sh
REMOVE=rm -f
INSTALL=sudo apt-get install
COPY=cp
GIT=git status
MAKECALL=python makesyscall.py
MAKE=make -C
SYNC=sync
KILL=killall
TAR=tar cf - . | gzip -f - > ./backup.tar.gz
HEXDUMPDSK=hexdump  -C ./final/harddisk.img.final|head -c10000
HEXDUMPSYS=hexdump  -C ./system/system.sys
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

all: tools programs system harddisk harddiskuefi
	$(SYNC)

system: lib/libs.o system/system.sys
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
	$(REMOVE) ./configuration
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
	$(MAKE) final littlecleaechn
	$(MAKE) programs clean
	$(SYNC)

indent:
	$(MAKE) system indent
	$(MAKE) lib indent
	$(MAKE) programs indent
	$(MAKE) tools indent
	$(SYNC)

killer: 
	$(KILL) qemu-system-x86_64 $(TRUE)
	$(KILL) qemu-system-i386 $(TRUE)
	$(KILL) gnome-terminal-server $(TRUE)
	$(TMUXKILL) debug $(TRUE)

backup: clean
	$(TAR)

view-dsk:
	$(HEXDUMPDSK)

view-sys:
	$(HEXDUMPSYS)

##### Alias

test: test32

retest: retest32

test32: all qemu32

test64: all qemu64

retest32: littleclean test32

retest64: littleclean test64

##### Debuguage

debug: debug-system32

redebug32: littleclean debug-system32

debug64: debug-system64

redebug64: littleclean debug-system64

kernel: debug-kernel

debug-boot32: all qemu-debug32
	$(WAIT2S)
	$(GODEBUG) ./debug/boot.txt

debug-boot64: all qemu-debug64
	$(WAIT2S)
	$(GODEBUG) ./debug/boot.txt

debug-system32: all qemu-debug32
	$(WAIT2S)
	$(GODEBUG) ./debug/system.txt

debug-system64: all qemu-debug64
	$(WAIT2S)
	$(GODEBUG) ./debug/system.txt

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

config:
	@echo "*** Options de compilation"
	@echo "Quelle distribution utiliser avec Docker comme environnement de compilation ?"
	@echo "Alpine Linux ou Debian [A*/D]"
	@read line; if [ $$line = "D" ]; then sed -i -r 's/compilation=.*/compilation=debian/'  configuration ; else sed -i -r 's/compilation=.*/compilation=alpine/'  configuration ; fi
	@echo "Quel mode vidéo préférez vous utiliser ?"
	@echo "VESA ou VGA [E*/G]"
	@read line; if [ $$line = "G" ]; then sed -i -r 's/video=.*/video=vga/'  configuration ; else sed -i -r 's/video=.*/video=vesa/'  configuration ; fi

