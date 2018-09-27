GCC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -w -I ../include -m32 -c -o

LINK=ld -m elf_i386 -T linker.lds -e main -n -o

all: system.sys
	sync

system.sys:
	$(GCC) system.o system.c
	$(LINK) system.sys system.o ../lib/libs.o

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.bin
	rm -f *.sys
	rm -f *.s


