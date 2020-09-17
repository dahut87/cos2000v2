CC=gcc -DCONFIG_X86_32 -I . -o 
SRCS= $(wildcard *.c)
EXECS= $(SRCS:.c=)
CONVERT=dos2unix
INDENT=indent -nhnl -l75 -ppi3 -ts8 -bls -nbc -di8 -nbad -nbap -nsob -i8 -bl -bli0 -ncdw -nce -cli8 -cbi0 -npcs -cs -saf -sai -saw -nprs -lp -npsl
REMOVE=rm -f
CHANGELF=elfedit --output-osabi FenixOS
CHANGEPERM=chmod 644
SYNC=sync

all: $(EXECS) 
	$(SYNC)

togit: clean indent

%: %.c    
	$(CC) $@ $<    
clean:
	$(REMOVE) *.o
	$(REMOVE) *.c~
	find . -maxdepth 1 -type f ! -name makefile -perm /u=x -regex '.+/\.?[^\.]+' -exec $(REMOVE) {} \;
	$(SYNC)

indent: 
	$(CHANGEPERM) *.c
	$(CONVERT) *.c
	$(INDENT) *.c
	$(REMOVE) *.c~
	$(SYNC)

