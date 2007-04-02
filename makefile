all:	makall

makall: boot/boot12.bin lib/libs.o system/system.sys 
	sync

clean:	
	(cd system; make clean)
	(cd boot; make clean)
	(cd lib;make clean)
	sync

backup: clean
	(cd .. ; tar cf - cosc | gzip -f - > backup.tar.gz ; cd cosc)

allbackup: backup
	(echo Inserez une disquette; sleep ; cp ../backup.tar.bz2 /dev/fd0) 	
		
copy: 
	(cp system/system.sys /cygdrive/a)

copy2: 
	(cp system/system.sys /cygdrive/b)
	
test: clean all copy2
	
system/system.sys:
	(cd system; make)
	
boot/boot12.bin:
	(cd boot; make)	

lib/libs.o:
	(cd lib; make)	
