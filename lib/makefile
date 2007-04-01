FREEC=gcc -O2 -nostdinc -ffreestanding -fno-builtin -fomit-frame-pointer -Wall -I ../Include -c 
PARTIAL=-r
OBJS=	string.o vgatxt.o 

all: makeall

makeall: $(OBJS)
	  (sync;ld $(PARTIAL) -o libs.o $(OBJS))

vgatxt.o:vgatxt.c 
	$(FREEC) $^

string.o:string.c 
	$(FREEC) $^
clean:
	rm -f *.o

