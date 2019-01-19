CC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -Wall -w -m32 -I ./include -fno-pie -no-pie -c -o
LINK=ld -m elf_i386 -T linker.lds -n -o
SRCS= $(wildcard *.c)
EXECS= $(SRCS:.c=)
CONVERT=dos2unix
INDENT=indent -nhnl -l75 -ppi3 -ts8 -bls -nbc -di8 -nbad -nbap -nsob -i8 -bl -bli0 -ncdw -nce -cli8 -cbi0 -npcs -cs -saf -sai -saw -nprs -lp -npsl
REMOVE=rm -f
CHANGELF=elfedit --output-osabi FenixOS
CHANGEPERM=chmod 644
MOVE=mv

all: lib/libs.a ../final/initram.img
	sync

togit: clean indent

../final/initram.img: $(EXECS)
	(find . -type f ! -perm /u=x -maxdepth 1 -regex '.+/\.?[^\.]+' -exec $(MOVE) {} ../final/cramfs/executables/ \;;true)	
	mkcramfs ../final/cramfs/ ../final/initram.img

lib/libs.a:
	make -C lib

%: %.c    
	$(CC) $@.o $<
	$(LINK) $@ $@.o lib/libs.a
	$(CHANGEPERM) $@
	$(CHANGELF) $@
    
clean:
	make -C lib clean
	$(REMOVE) *.o
	$(REMOVE) *.c~
	(find ../final/cramfs/executables -type f ! -perm /u=x -maxdepth 1 -regex '.+/\.?[^\.]+' -exec $(REMOVE) {} \;;true)
	(find . -type f ! -perm /u=x -maxdepth 1 -regex '.+/\.?[^\.]+' -exec $(REMOVE) {} \;;true)
	sync

indent: 
	make -C lib indent
	$(CHANGEPERM) *.c
	$(CONVERT) *.c
	$(INDENT) *.c
	$(REMOVE) *.c~
	sync
