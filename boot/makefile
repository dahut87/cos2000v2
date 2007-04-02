all:	makall

makall: boot12.bin boot16.bin boottest.bin bootcp.com
	sync

boot12.bin:
	nasm -f bin -o boot12.bin boot12.asm
	
boot16.bin:
	nasm -f bin -o boot16.bin boot16.asm

boottest.bin:
	nasm -f bin -o boottest.bin boottest.asm

bootcp.com:
	nasm -f bin -o bootcp.com bootcp.asm
	
clean:
	rm -f *.o
	rm -f *.bin
	rm -f *.sys
	rm -f *.com
	

