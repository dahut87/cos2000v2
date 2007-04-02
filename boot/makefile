CC=nasm -f bin -o 
BINS=boot12.bin boot16.bin boottest.bin bootcp.com loader.sys

all:	makall

makall:$(BINS)

boot12.bin: boot12.asm
	$(CC) $@ $^	

boot16.bin: boot16.asm
	$(CC) $@ $^	

boottest.bin: boottest.asm
	$(CC) $@ $^	

bootcp.com: bootcp.asm
	$(CC) $@ $^	

loader.sys: loader.asm
	$(CC) $@ $^	

clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.sys
	rm -f *.com
	

