CC=gcc -O0 -g -nostdinc -ffreestanding -fno-builtin -Wall -w -m32 -F elf_i386 -fno-pie -no-pie -I ../include
LINK=ld -m elf_i386 -r -o
SRCS= $(wildcard *.c)
OBJS= $(SRCS:.c=.o)
CONVERT=dos2unix
INDENT=indent -nhnl -l75 -ppi3 -ts8 -bls -nbc -di8 -nbad -nbap -nsob -i8 -bl -bli0 -ncdw -nce -cli8 -cbi0 -npcs -cs -saf -sai -saw -nprs -lp -npsl
REMOVE=rm -f
CHANGEPERM=chmod 644

all: libs.o
	sync

togit: clean indent

libs.o:$(OBJS)
	$(LINK) libs.o $(OBJS)

.o: .c 
	$(CC) $^

clean:
	$(REMOVE) *.o
	$(REMOVE) *.c~
	$(REMOVE) */*.c~
	$(REMOVE) ../include/*.h~
	sync

indent:
	$(CHANGEPERM) *.c
	$(CONVERT) *.c
	$(INDENT) *.c
	$(REMOVE) *.c~
	$(CHANGEPERM) ./*/*.c
	$(CONVERT) ./*/*.c
	$(INDENT) ./*/*.c
	$(REMOVE) */*.c~
	$(CHANGEPERM) ../include/*.h
	$(CONVERT) ../include/*.h
	$(INDENT) ../include/*.h
	$(REMOVE) ../include/*.h~
	sync
