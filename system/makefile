FREEC=gcc -O2 -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -I ../Include -c -o 
LINK=ld -N -Ttext 0x100000 -e main -o

all: system.sys
	sync

system.sys:
	nasm -f bin -o loader.bin loader.asm
	nasm -f elf -o system.o system.asm

	$(FREEC) systemc.o system.c
	$(LINK) system.bin systemc.o system.o ../lib/libs.o --oformat binary
	cat loader.bin>system.sys
	cat system.bin>>system.sys

clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.sys


