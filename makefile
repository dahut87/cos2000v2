all:	makall

makall: drivers/drivers.o boot/boot12.bin installer/mbrol.com lib/libs.o system/system.sys 
	sync

clean:	
	(cd drivers; make clean)
	(cd system; make clean)
	(cd boot; make clean)
	(cd install; make clean)
	(cd lib;make clean)
	sync

backup: clean
	(cd .. ; tar cf - cos | gzip -f - > backup.tar.gz)

install: all
	(cp system/system.sys /mnt/fd0)

allbackup: backup
	(echo Inserez une disquette; sleep ; cp ../backup.tar.bz2 /mnt/fd0) 	
		
testboot:boot/boot12.bin
	(cd test ; cat ../boot/boot12.bin /dev/zero | dd of=a.img bs=512 count=2880 ; bochs)
	
system/system.sys:
	(cd system; make)
	
drivers/drivers.o:
	(cd drivers; make)
	
boot/boot12.bin:
	(cd boot; make)	

installer/mbrol.com:
	(cd install; make)	

lib/libs.o:
	(cd lib; make)	