CC=gcc -O1 -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -I ../Include -c -o 


LINK=ld -e __main -o

all: system.sys
	sync

system.sys:
	nasm -f elf -o system.o system.asm

	$(CC) systemc.o system.c
	$(LINK) system.sys  systemc.o system.o ../lib/libs.o

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.bin
	rm -f *.sys


