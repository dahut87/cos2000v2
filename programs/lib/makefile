CC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -Wall -w -m32 -F elf-i386 -fno-pie -no-pie -I ../include
LINK=ld -m elf_i386 -r -o
SRCS= $(wildcard *.c)
OBJS= $(SRCS:.c=.o)
ARCH=ar rcs

all: makeall

makeall: libs.a

libs.a:libs.o
	$(ARCH) $@ $^

libs.o:$(OBJS)
	$(LINK) libs.o $(OBJS)

.o: .c 
	$(CC) $^

clean:
	rm -f *.o
	rm -f *.c~
	rm -f *.a

indent: 
	indent -linux -i8 -ts8 *.c
