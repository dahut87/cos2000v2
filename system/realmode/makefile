GCC=gcc -O0 -g -nostdinc -ffreestanding -Wall -Wstrict-prototypes -fno-stack-protector -march=i386 -m16 -fno-pic -fno-strict-aliasing -mno-mmx -mno-sse -w -I ../../include -c
ASM=gcc -O0 -m16 -march=i386 -fno-pic -D__ASSEMBLY__ -I ../ -I ../../include -c -o
LINK=ld -m elf_i386 -n
CONVERT=dos2unix
INDENT=indent -nhnl -l75 -ppi3 -ts8 -bls -nbc -di8 -nbad -nbap -nsob -i8 -bl -bli0 -ncdw -nce -cli8 -cbi0 -npcs -cs -saf -sai -saw -nprs -lp -npsl
REMOVE=rm -f
CHANGEPERM=chmod 644
NM=nm
OBJCOPY=objcopy -O binary  -R .note -R .comment -S
OBJDEBUG=objcopy --only-keep-debug
$(eval VERSION=$(shell git describe --tags))

all: setup.bin 
	sync

setup: setupc.o setup.o setup32.o
	$(LINK) -T setup.ld setupc.o setup.o setup32.o

setup.bin: setup
	$(OBJCOPY) $^ $@	
	$(OBJDEBUG) setup setup.sym
	$(NM) setup > setup.map

setupc.o: setup.c
	$(GCC) -DVERSION=$(VERSION) -o $@ $^

setup.o: setup.S
	$(ASM) $@ $^

setup32.o: pmode.S
	$(ASM) $@ $^

clean:
	$(REMOVE) setup
	$(REMOVE) *.o
	$(REMOVE) *.sym
	$(REMOVE) *.map
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
