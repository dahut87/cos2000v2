#include "types.h"
#include "asm.h"

/*******************************************************************************/

/* Envoie une s�rie d'octet a destination d'une portion de m�moire
vers le registre video sp�cifi� */

void outreg(u16 port,u8 *src,u16 num)
{
int i;	       
for(i=0;i<num;i++)
	{
	 	outb(port,i);
	 	outb(port+1,*src++);	
	}
}


/*******************************************************************************/

/* Envoie une s�rie d'octet a destination d'une portion de m�moire
vers le registre video sp�cifi� (acc�s data et index confondu) */

void outregsame(u16 port,u8 *src,u16 num)
{
int i;	
for(i=0;i<num;i++)
	{
		inb(port);
	 	outb(port,i);
	 	outb(port,*src++);	
	}
}

/*******************************************************************************/

/* R�cup�re une s�rie d'octet en provenance d'un registre video sp�cifi�
vers portion de m�moire */

void inreg(u16 port,u8 *src,u16 num)
{
int i;	
for(i=0;i<num;i++)
	{
	 	outb(port,i);
	 	*src++=inb(port+1);	
	}
}

/*******************************************************************************/

/* R�cup�re une s�rie d'octet en provenance d'un registre video sp�cifi�
vers portion de m�moire (acc�s data et index confondu) */

void inregsame(u16 port,u8 *src,u16 num)
{
int i;	
for(i=0;i<num;i++)
	{
		inb(port);
	 	outb(port,i);
	 	*src++=inb(port);	
	}
}

/*******************************************************************************/

