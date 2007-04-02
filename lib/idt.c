#include "idt.h"
#include "types.h"
#include "asm.h"
#include "memory.h"
#include "video.h"

 /* registre idt */
static struct dtr idtreg;

/* table de IDT */
static idtdes idt[256];


void initpic(void)
{
	/* MASTER */
	/* Initialisation de ICW1 */
	outb(0x20,0x11);
	nop();
	/* Initialisation de ICW2 - vecteur de depart = 32 */
	outb(0x21,0x20);    
 	nop();        
	/* Initialisation de ICW3 */
	outb(0x21,0x04);
	nop();
	/* Initialisation de ICW4 */
	outb(0x21,0x01);
	nop();
	/* masquage des interruptions */
	outb(0x21,0xFF);
	nop();
	/* SLAVE */
	/* Initialisation de ICW1 */
	outb(0xA0,0x11);
	nop();
	/* Initialisation de ICW2 - vecteur de depart = 96 */
	outb(0xA1,0x70);   
	nop();   
	/* Initialisation de ICW3 */
	outb(0xA1,0x02);
	nop();
	/* Initialisation de ICW4 */
	outb(0xA1,0x01);
	nop();
	/* masquage des interruptions */
	outb(0xA1,0xFF);
	nop();
	/* Demasquage des irqs sauf clavier
	outb(0x21,0xFD);
	nop();
	*/
}

void enableirq(u8 irq)
{
u16 port;
cli();
port = (((irq & 0x08) << 4) + 0x21);
outb(port,inb(port) & ~(1 << (irq & 7)));
sti();
}

void disableirq(u8 irq)
{
u16 port;
cli();
port = (((irq & 0x08) << 4) + 0x21);
outb(port,inb(port) | (1 << (irq & 7)));
sti();
}

void makeidtdes(u32 offset, u16 select, u16 type, idtdes* desc) 
{
	desc->offset0_15 = (offset & 0xffff);
	desc->select = select;
	desc->type = type;
	desc->offset16_31 = (offset & 0xffff0000) >> 16;
	return;
}

void setidt(u32 offset, u16 select, u16 type,u16 index) 
{
cli();
idtdes *desc;
desc=idtreg.base;
	desc[index].offset0_15 = (offset & 0xffff);
	desc[index].select = select;
	desc[index].type = type;
	desc[index].offset16_31 = (offset & 0xffff0000) >> 16;
sti();
}

void putidt(u32 offset, u16 select, u16 type,u16 index) 
{
idtdes temp;
makeidtdes(offset,select,type,&temp);
idt[index]=temp;
}

void interruption()
{
  cli();
	print("Appel d'une interruption");
  sti();
	iret();
}

void exception0() 
{
	print("divide error");
  iret();
 }

void exception1() 
{
	print("debug exception");
  iret();
  }

void exception2()
 {
	print("non-maskable hardware interrupt");
  iret();
  }

void exception3() 
{
	print("INT3 instruction");
  iret();
  }

void exception4()
 {
	print("INTO instruction detected overflow");
  iret();
  }

void exception5()
 {
	print("BOUND instruction detected overrange");
  iret();
  }

void exception6() 
{
	print("invalid instruction opcode");
  iret();
  }

void exception7()
 {
	print("no coprocessor");
  iret();
  }

void exception8() 
{
	print("double fault");
  iret();
  }

void exception9()
 {
	print("coprocessor segment overrun");
  iret();
  }

void exception10() 
{
	print("invalid task state segment (TSS)");
  iret();
  }

void exception11()
 {
	print("segment not present");
  iret();
  }

void exception12() 
{
	print("stack fault");
  iret();
  }

void exception13()
 {
	print("general protection fault (GPF)");
  iret();
  }

void exception14()
 {
	print("page fault");
  iret();
  }

void exception15() 
{
	print("(reserved)");
  iret();
  }
  
void exception16()
 {
	print("coprocessor error");
  iret();
  }

void exception17() 
{
	print("alignment check");
  iret();
  }

void exception18()
 {
	print("machine check");
  iret();
  }

void irq0()
 {
   cli();
	print("irq 0");
	irqendmaster();
	sti();
  iret();
  }

  void irq1()
 {
   cli();
	print("irq 1");
	irqendmaster();
	sti();
  iret();
  }

  void irq2()
 {
   cli();
	print("irq 2");
	irqendmaster();
  	sti();
  iret();
  }
  
  void irq3()
 {
   cli();
	print("irq 3");
	irqendmaster();
		sti();
  iret();
  }

  void irq4()
 {
   cli();
	print("irq 4");
	irqendmaster();
		sti();
  iret();
  }

  void irq5()
 {
   cli();
	print("irq 5");
	irqendmaster();
		sti();
  iret();
  }

  void irq6()
 {
   cli();
	print("irq 6");
	irqendmaster();
		sti();
  iret();
  }

void irq7()
 {
   cli();
	print("irq 7");
	irqendmaster();
		sti();
  iret();
  }

void irq8()
 {
   cli();
	print("irq 8");
	irqendslave();
		sti();
  iret();
  }

void irq9()
 {
   cli();
	print("irq 9");
	irqendslave();
		sti();
  iret();
  }
  
void irq10()
 {
   cli();
	print("irq 10");
	irqendslave();
		sti();
  iret();
  }
  
void irq11()
 {
   cli();
	print("irq 11");
	irqendslave();
		sti();
  iret();
  }
  
void irq12()
 {
   cli();
	print("irq 12");
	irqendslave();
		sti();
  iret();
  }
  
  void irq13()
 {
   cli();
	print("irq 13");
	irqendslave();
		sti();
  iret();
  }
  
void irq14()
 {
   cli();
	print("irq 14");
	irqendslave();
		sti();
  iret();
  }

void irq15()
 {
   cli();
	print("irq 15");
	irqendslave();
		sti();
  iret();
  }  

 void initidt(void)
{
u16 i;
	putidt((u32)exception0, 0x30, INTGATE, 0);
	putidt((u32)exception1, 0x30, INTGATE, 1);
	putidt((u32)exception2, 0x30, INTGATE, 2);
	putidt((u32)exception3, 0x30, INTGATE, 3);
	putidt((u32)exception4, 0x30, INTGATE, 4);
	putidt((u32)exception5, 0x30, INTGATE, 5);
	putidt((u32)exception6, 0x30, INTGATE, 6);
	putidt((u32)exception7, 0x30, INTGATE, 7);
	putidt((u32)exception8, 0x30, INTGATE, 8);
	putidt((u32)exception9, 0x30, INTGATE, 9);
	putidt((u32)exception10, 0x30, INTGATE, 10);
	putidt((u32)exception11, 0x30, INTGATE, 11);
	putidt((u32)exception12, 0x30, INTGATE, 12);
	putidt((u32)exception13, 0x30, INTGATE, 13);
	putidt((u32)exception14, 0x30, INTGATE, 14);
	putidt((u32)exception15, 0x30, INTGATE, 15);
	putidt((u32)exception16, 0x30, INTGATE, 16);
	putidt((u32)exception17, 0x30, INTGATE, 17);
	putidt((u32)exception18, 0x30, INTGATE, 18);
	for(i=19;i<32;i++)
  {
	putidt((u32)interruption, 0x30, INTGATE, i);  
  } 
	putidt((u32)irq0, 0x30, INTGATE, 32);
	putidt((u32)irq1, 0x30, INTGATE, 33);
	putidt((u32)irq2, 0x30, INTGATE, 34);
	putidt((u32)irq3, 0x30, INTGATE, 35);
	putidt((u32)irq4, 0x30, INTGATE, 36);
	putidt((u32)irq5, 0x30, INTGATE, 37);
	putidt((u32)irq6, 0x30, INTGATE, 38);
	putidt((u32)irq7, 0x30, INTGATE, 39);
	for(i=40;i<112;i++)
  {
	putidt((u32)interruption, 0x30, INTGATE, i);  
  } 
	putidt((u32)irq8, 0x30, INTGATE, 112);
	putidt((u32)irq9, 0x30, INTGATE, 113);
	putidt((u32)irq10, 0x30, INTGATE, 114);
	putidt((u32)irq11, 0x30, INTGATE, 115);
	putidt((u32)irq12, 0x30, INTGATE, 116);
	putidt((u32)irq13, 0x30, INTGATE, 117);
	putidt((u32)irq14, 0x30, INTGATE, 118);
	putidt((u32)irq15, 0x30, INTGATE, 119);  
	for(i=120;i<255;i++)
  {
	putidt((u32)interruption, 0x30, INTGATE, i);  
  } 
  /* initialise le registre idt */
	idtreg.limite = 256*8;
	idtreg.base = 0x0000000;
	/* recopie de la IDT a son adresse */
	memcpy(&idt, (u8*)idtreg.base, idtreg.limite,1);
	/* chargement du registre IDTR */
	lidt(&idtreg);

}

