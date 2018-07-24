#include "idt.h"
#include "types.h"
#include "asm.h"
#include "memory.h"
#include "timer.h"
#include "vga.h"


static u8 curs[4]={"-\\|/"};

static u8 curspos=0;


void timer()
 {
  cli();
	showchar(0,0,curs[curspos],7);
	curspos=(curspos+1)&0x3;	
	irqendmaster();
	sti();
  asm("addl $0x1C,%esp;iret;");
  }
