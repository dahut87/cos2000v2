CC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -Wall -w -m32 -I ./include -fno-pie -no-pie -c -o
LINK=ld -m elf_i386 -T linker.lds -n -o
SRCS= $(wildcard *.c)
EXECS= $(SRCS:.c=)
CONVERT=dos2unix
INDENT=indent -nhnl -l75 -ppi3 -ts8 -bls -nbc -di8 -nbad -nbap -nsob -i8 -bl -bli0 -ncdw -nce -cli8 -cbi0 -npcs -cs -saf -sai -saw -nprs -lp -npsl
REMOVE=rm -f
CHANGELF=elfedit --output-osabi FenixOS
CHANGEPERM=chmod 644
MAKE=make -C
SYNC=sync

all: lib/libs.a $(EXECS) 
	$(SYNC)

togit: clean indent

lib/libs.a:
	$(MAKE) lib

%: %.c    
	$(CC) $@.o $<
	$(LINK) $@ $@.o lib/libs.a
	$(CHANGEPERM) $@
	$(CHANGELF) $@
    
clean:
	$(MAKE) lib clean
	$(REMOVE) *.o
	$(REMOVE) *.c~
	find . -maxdepth 1 -type f ! -perm /u=x -regex '.+/\.?[^\.]+' -exec $(REMOVE) {} \;
	$(SYNC)

indent: 
	$(MAKE) lib indent
	$(CHANGEPERM) *.c
	$(CONVERT) *.c
	$(INDENT) *.c
	$(REMOVE) *.c~
	$(SYNC)

