all:	makall

makall: boot/boot12.bin lib/libs.o system/system.sys 
	sync

install:
	(sudo apt-get install nasm gcc qemu fusefat cgdb)

clean:	
	(cd system; make clean)
	(cd boot; make clean)
	(cd lib;make clean)
	(cd final;make clean)
	sync

backup: clean
	(cd .. ; tar cf - cosc | gzip -f - > backup.tar.gz ; cd cosc)
		
copy: 
	(cd final; make)

test: all copy qemu

view:
	(hexdump  -C ./final/cos2000.img|head -c10000)

debug-boot: all copy qemu-debug
	(sleep 2;cgdb -x ./debug/boot.txt)

debug-loader: all copy qemu-debug
	(sleep 2;cgdb -x ./debug/loader.txt)

debug-system: all copy qemu-debug
	(sleep 2;cgdb -x ./debug/system.txt)

qemu-debug:
	(qemu-system-i386 -m 1G -fda ./final/cos2000.img -s -S &)

qemu:
	(qemu-system-i386 -m 1G -fda ./final/cos2000.img -s)    
	
system/system.sys:
	(cd system; make)
	
boot/boot12.bin:
	(cd boot; make)	

lib/libs.o:
	(cd lib; make)	
