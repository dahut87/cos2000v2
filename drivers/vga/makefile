FREEC=gcc -O4 -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -I ../../Include -c -o 
PARTIAL=-r
OBJS=	vgahard.o \
	vgatxt.o

all:
	nasm -f elf -o vgahard.o vgahard.asm

	$(FREEC) vgatxt.o vgatxt.c 

	ld $(PARTIAL) -o vga.o $(OBJS)

clean:
	rm -f *.o
	rm -f *.bin


