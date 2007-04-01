all:	makall

makall:	mbrol.com

mbrol.com:
	nasm -f bin -o mbr.bin mbr.asm
	nasm -f bin -o loader.bin loader.asm
	sync
	nasm -f bin -o mbrol.com mbrol.asm

clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.sys
	rm -f *.com
	

