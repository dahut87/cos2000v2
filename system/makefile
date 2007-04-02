FINALCC=gcc -O4 -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -w -I ../include -m32 -c -o
GCC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -w -I ../include -m32 -c -o

LINK=ld -m elf_i386 -T linker.lds -e main -o

all: system.sys
	sync

system.sys:
	nasm -f elf -o system.o system.asm
	$(GCC) systemc.o system.c
	$(LINK) system.sys  systemc.o system.o ../lib/libs.o

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.bin
	rm -f *.sys
	rm -f *.s


