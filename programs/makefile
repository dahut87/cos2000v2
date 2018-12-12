CC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -Wall -w -m32 -I ./include -fno-pie -no-pie -c -o
LINK=ld -m elf_i386 -T linker.lds -n -o
SRCS= $(wildcard *.c)
EXECS= $(SRCS:.c=)

all: lib/libs.a $(EXECS) 

lib/libs.a:
	make -C lib

%: %.c    
	$(CC) $@.o $<
	$(LINK) $@ $@.o lib/libs.a
	chmod 644 $@
	elfedit --output-osabi FenixOS $@
    
clean:
	make -C lib clean
	rm -f *.o
	rm -f *.c~
	find . -type f ! -perm /u=x -maxdepth 1 -regex '.+/\.?[^\.]+' -exec rm {} \;

indent: 
	indent -linux -i8 -ts8 *.c
