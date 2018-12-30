GCC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -Wall -w -I ../../include -m16 -fomit-frame-pointer -fno-pic -mno-mmx -mno-sse -mno-80387 -mno-fp-ret-in-387 -c -o
ASM=gcc -nostdinc -ffreestanding -fno-builtin -m16 -fomit-frame-pointer -fno-pic -mno-mmx -mno-sse -mno-80387 -mno-fp-ret-in-387 -c -o
LINK=ld -m elf_i386 -n -o
CONVERT=dos2unix
INDENT=indent -nhnl -l75 -ppi3 -ts8 -bls -nbc -di8 -nbad -nbap -nsob -i8 -bl -bli0 -ncdw -nce -cli8 -cbi0 -npcs -cs -saf -sai -saw -nprs -lp -npsl
REMOVE=rm -f
CHANGEPERM=chmod 644

all: setup.bin 
	sync

setup.bin: setupc.o setup.o 
	$(LINK) -T setup.ld setupc.o setup.o
	$(OBJDUMP)

setupc.o: setup.c
	$(GCC) $@ $^

setup.o: setup.S
	$(ASM) $@ $^

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
