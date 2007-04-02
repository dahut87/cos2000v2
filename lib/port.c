#include "types.h"
#include "asm.h"

void outreg(u16 port,u8 *src,u16 num)
{
int i;	       
for(i=0;i<num;i++)
	{
	 	outb(port,i);
	 	outb(port+1,*src++);	
	}
}

void outregsame(u16 port,u8 *src,u16 num)
{
int i;	
for(i=0;i<num;i++)
	{
		inw(port);
	 	outb(port,i);
	 	outb(port,*src++);	
	}
}


void inreg(u16 port,u8 *src,u16 num)
{
int i;	
for(i=0;i<num;i++)
	{
	 	outb(port,i);
	 	*src++=inb(port+1);	
	}
}

void inregsame(u16 port,u8 *src,u16 num)
{
int i;	
for(i=0;i<num;i++)
	{
		inw(port);
	 	outb(port,i);
	 	*src++=inb(port);	
	}
}

void initreg(u16 port,u8 *src,u16 num)
{
int i;	       
for(i=0;i<num;i++)
	{
	 	outb(port,*src++);
	 	outb(port+1,*src++);	
	}
}

void initregsame(u16 port,u8 *src,u16 num)
{
int i;	       
for(i=0;i<num;i++)
	{
		inw(port);
	 	outb(port,*src++);
	 	outb(port,*src++);	
	}
}
