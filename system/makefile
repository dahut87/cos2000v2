GCC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -Wall -w -I ../include -m32 -fno-pie -no-pie -c -o
GCCREAL=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -Wall -w -I ../include -m16 -fomit-frame-pointer -fno-pic -mno-mmx -mno-sse -mstack-alignment=4 -mno-80387 -mno-fp-ret-in-387 -c -o
ASM=gcc -nostdinc -ffreestanding -fno-builtin -m32 -c -fno-pie -no-pie
LINK=ld -m elf_i386 -n -o
CONVERT=dos2unix
INDENT=indent -nhnl -l75 -ppi3 -ts8 -bls -nbc -di8 -nbad -nbap -nsob -i8 -bl -bli0 -ncdw -nce -cli8 -cbi0 -npcs -cs -saf -sai -saw -nprs -lp -npsl
REMOVE=rm -f
CHANGEPERM=chmod 644

all: system.sys 

system.sys:	setup.bin system.bin
	tools/build setup.bin system.bin zoffset.h system.sys
	sync

togit: clean indent

system.bin: system.o ../lib/libs.o
	$(LINK) -T linker.lds multiboot.o system.o ../lib/libs.o

setup.bin: setup.o
	$(LINK) $@ $^
	$(OBJDUMP)

setup.o:
	$(GCCREAL) $@ $^	

system.o:
	$(GCC) $@ $^

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
