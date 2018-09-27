GCC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -w -I ../include -m32 -c -o
ASM=nasm
LINK=ld -m elf_i386 -T linker.lds -n -o

all: system.sys
	sync

system.sys: multiboot.o system.o ../lib/libs.o
	$(LINK) system.sys multiboot.o system.o ../lib/libs.o

system.o:
	$(GCC) system.o system.c

multiboot.o:
	$(ASM) -f elf -o multiboot.o multiboot.asm -dARCH=$(ARCH)

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.bin
	rm -f *.sys
	rm -f *.s

indent: 
	indent -linux -i8 -ts8 *.c
