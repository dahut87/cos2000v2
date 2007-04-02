FREEC=gcc -O2 -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -I ../Include -c 
PARTIAL=ld -r -o
OBJS=	memory.o vga.o port.o video.o

all: makeall

makeall: libs.o

libs.o:$(OBJS)
	$(PARTIAL) libs.o $(OBJS)

vga.o:vga.c 
	$(FREEC) $^

video.o:video.c 
	$(FREEC) $^

port.o:port.c
	$(FREEC) $^

memory.o:memory.c 
	$(FREEC) $^
clean:
	rm -f *.o

