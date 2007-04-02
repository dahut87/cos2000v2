FREEC=gcc -O2 -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -I ../Include -c 
PARTIAL=ld -r -o
OBJS=	memory.o vga.o port.o video.o idt.o timer.o keyboard.o types.o

all: makeall

makeall: libs.o

libs.o:$(OBJS)
	$(PARTIAL) libs.o $(OBJS)

vga.o:vga.c 
	$(FREEC) $^
	
types.o:types.c 
	$(FREEC) $^

idt.o:idt.c 
	$(FREEC) $^
	
keyboard.o:keyboard.c 
	$(FREEC) $^
	
timer.o:timer.c 
	$(FREEC) $^

video.o:video.c 
	$(FREEC) $^

port.o:port.c
	$(FREEC) $^

memory.o:memory.c 
	$(FREEC) $^
clean:
	rm -f *.o

