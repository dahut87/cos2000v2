FREEC=gcc -O2 -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -I ../Include -c 
PARTIAL=-r
OBJS=	memory.o port.o vgatxt.o 

all: makeall

makeall: $(OBJS)
	  (sync;ld $(PARTIAL) -o libs.o $(OBJS))

vgatxt.o:vgatxt.c 
	$(FREEC) $^

memory.o:memory.c 
	$(FREEC) $^

port.o:port.c 
	$(FREEC) $^
clean:
	rm -f *.o

