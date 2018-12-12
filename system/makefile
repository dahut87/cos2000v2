GCC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -Wall -w -I ../include -m32 -fno-pie -no-pie -c -o
ASM=nasm -f elf -o
LINK=ld -m elf_i386 -T linker.lds -n -o
CONVERT=dos2unix
INDENT=indent -nhnl -l75 -ppi3 -ts8 -bls -nbc -di8 -nbad -nbap -nsob -i8 -bl -bli0 -ncdw -nce -cli8 -cbi0 -npcs -cs -saf -sai -saw -nprs -lp -npsl
REMOVE=rm -f
CHANGEPERM=chmod 644

all: system.sys
	sync

togit: clean indent

system.sys: multiboot.o system.o ../lib/libs.o
	$(LINK) system.sys multiboot.o system.o ../lib/libs.o

system.o:
	$(GCC) system.o system.c

multiboot.o:
	$(ASM) multiboot.o multiboot.asm -dVESA=$(VESA)

clean:
	$(REMOVE) *.o
	$(REMOVE) *.out
	$(REMOVE) *.bin
	$(REMOVE) *.sys
	$(REMOVE) *.s
	$(REMOVE) *.c~
	sync

indent:
	$(CHANGEPERM) *.c
	$(CONVERT) *.c
	$(INDENT) *.c
	$(REMOVE) *.c~
	sync
