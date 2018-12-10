CC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -Wall -w -m32 -F elf_i386 -fno-pie -no-pie -I ../include
LINK=ld -m elf_i386 -r -o
SRCS= $(wildcard *.c)
OBJS= $(SRCS:.c=.o)

all: makeall

makeall: libs.o

libs.o:$(OBJS)
	$(LINK) libs.o $(OBJS)

.o: .c 
	$(CC) $^

clean:
	rm -f *.o
	rm -f *.c~
	rm -f ./VGA/*.c~

indent: 
	indent -linux -i8 -ts8 *.c
	indent -linux -i8 -ts8 ./VGA/*.c
