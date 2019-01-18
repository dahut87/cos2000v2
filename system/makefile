GCC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -Wall -w -I ../include -m32 -fno-pie -no-pie -c -o
ASM=gcc -nostdinc -ffreestanding -fno-builtin -m32 -c -fno-pie -no-pie -I ../include -D__ASSEMBLY__ -c -o
LINK=ld -m elf_i386 -n
CONVERT=dos2unix
INDENT=indent -nhnl -l75 -ppi3 -ts8 -bls -nbc -di8 -nbad -nbap -nsob -i8 -bl -bli0 -ncdw -nce -cli8 -cbi0 -npcs -cs -saf -sai -saw -nprs -lp -npsl
REMOVE=rm -f
CHANGEPERM=chmod 644
NM=nm
OBJCOPY=objcopy -O binary  -R .note -R .comment -S
OBJDEBUG=objcopy --only-keep-debug
ZOFFSET=sed -n -e 's/^\([0-9a-fA-F]*\) [ABCDGRSTVW] \(startup_32\|startup_64\|efi32_stub_entry\|efi64_stub_entry\|efi_pe_entry\|input_data\|_end\|_ehead\|_text\|z_.*\)$$/\#define ZO_\2 0x\1/p'
VOFFSET=sed -n -e 's/^\([0-9a-fA-F]*\) [ABCDGRSTVW] \(_text\|__bss_start\|_end\)$$/\#define VO_\2 _AC(0x\1,UL)/p'
COMP=gzip -9 -f
BUILD=../tools/build
MKPIGGY=../tools/mkpiggy

all: system.sys

system.sys:	piggy.o voffset.h zoffset.h realmode/setup.bin
	$(BUILD) realmode/setup.bin system.bin zoffset.h system.sys
	sync

allpiggy.o: piggy.o decompress.o header.o
	$(LINK) -T allpiggy piggy.o decompress.o header.o

voffset.h: system
	$(NM) system|$(VOFFSET)>voffset.h

zoffset.h: piggy.o
	$(NM) piggy.o|$(ZOFFSET)>zoffset.h

togit: clean indent

piggy.o: piggy.S
	$(ASM) $@ $^ 

system: system.o system_asm.o ../lib/libs.o
	$(LINK) -T system.ld system.o system_asm.o ../lib/libs.o
	$(OBJDEBUG) system system.sym
	$(NM) system > system.map

system.bin: system
	$(OBJCOPY) $^ $@  

system.bin.gz: system.bin
	cat $^|$(COMP) > $@ 

piggy.S: system.bin.gz
	$(MKPIGGY) $^ > $@ 

realmode/setup.bin:
	make -C realmode

system.o: system.c
	$(GCC) $@ $^

system_asm.o: system_asm.S
	$(ASM) $@ $^

clean:
	make -C realmode clean
	$(REMOVE) system  
	$(REMOVE) piggy.S
	$(REMOVE) *.o
	$(REMOVE) *.tmp
	$(REMOVE) *.sym
	$(REMOVE) *.map
	$(REMOVE) *.gz
	$(REMOVE) *.h
	$(REMOVE) *.out
	$(REMOVE) *.bin
	$(REMOVE) *.sys
	$(REMOVE) *.s
	$(REMOVE) *.c~
	sync

indent:
	make -C realmode indent
	$(CHANGEPERM) *.c
	$(CONVERT) *.c
	$(INDENT) *.c
	$(REMOVE) *.c~
	sync
