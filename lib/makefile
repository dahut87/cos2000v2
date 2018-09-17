CC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -w -m32 -F pe-i386 -I ../include
LINK=ld -m elf_i386 -r -o
OBJS=memory.o vga.o video.o mouse.o interrupts.o timer.o keyboard.o types.o string.o 2d.o 3d.o math.o cpu.o gdt.o shell.o

all: makeall

makeall: libs.o

libs.o:$(OBJS)
	$(LINK) libs.o $(OBJS)

.o: .c 
	$(CC) $^
clean:
	rm -f *.o

