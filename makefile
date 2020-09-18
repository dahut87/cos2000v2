all:	makall

makall: boot/boot12.bin lib/libs.o system/system.sys 
	sync

clean:	
	(cd system; make clean)
	(cd boot; make clean)
	(cd lib;make clean)
	(cd final;make clean)
	sync

backup: clean
	(tar cf - . | gzip -f - > backup.tar.gz)
		
copy: 
	(cd final; make)

test: all copy qemu

killer: 
	killall qemu-system-i386 || true

view:
	(hexdump  -C ./final/cos2000.img|head -c10000)

debug-boot: all copy qemu-debug
	(sleep 2;cgdb -x ./debug/boot.txt;spicy --uri=spice://127.0.0.1?port=5900)

debug-loader: all copy qemu-debug
	(sleep 2;cgdb -x ./debug/loader.txt;spicy --uri=spice://127.0.0.1?port=5900)

debug-system: all copy qemu-debug
	(sleep 2;cgdb -x ./debug/system.txt;spicy --uri=spice://127.0.0.1?port=5900)

qemu-debug:
	(qemu-system-i386 -vga qxl -spice port=5900,addr=127.0.0.1,disable-ticketing -m 1G -fda ./final/cos2000.img -s -S &)

qemu:
	qemu-system-i386 -vga qxl -spice port=5900,addr=127.0.0.1,disable-ticketing -m 1G -fda ./final/cos2000.img -s &
	sleep 2
	spicy --uri=spice://127.0.0.1?port=5900    
	
system/system.sys:
	(cd system; make)
	
boot/boot12.bin:
	(cd boot; make)	

lib/libs.o:
	(cd lib; make)	
