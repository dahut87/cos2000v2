CC=gcc -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -I ../Include 
LINK=ld -r -o
OBJS=	memory.o vga.o port.o video.o mouse.o idt.o timer.o keyboard.o types.o string.o graph.o 3d.o math.o cpu.o

all: makeall

makeall: libs.o

libs.o:$(OBJS)
	$(LINK) libs.o $(OBJS)

.o: .c 
	$(CC) $^
clean:
	rm -f *.o

