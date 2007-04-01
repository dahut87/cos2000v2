FREEC=gcc -O2 -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -I ../Include -c -o 


LINK=ld -Ttext 0x100000 -e __main -o

all: system.sys
	sync

copy: 
	(cp system.sys /cygdrive/a)

copy2: 
	(cp system.sys /cygdrive/b)

system.sys:
	nasm -f bin -o loader.bin loader.asm
	nasm -f elf -o system.o system.asm

	$(FREEC) systemc.o system.c
	$(LINK) system.out  systemc.o system.o ../lib/libs.o
	objcopy -O binary system.out system.bin
	cat loader.bin>system.sys
	cat system.bin>>system.sys

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.bin
	rm -f *.sys


