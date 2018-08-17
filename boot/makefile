CC=nasm -f bin -o 

all:	makall

makall: boot12.bin loader.sys

boot12.bin: boot12.asm
	$(CC) $@ $^	

loader.sys: loader.asm
	$(CC) $@ $^	

clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.sys
	rm -f *.com
	

