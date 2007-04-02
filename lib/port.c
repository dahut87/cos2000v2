#include "types.h"
#include "asm.h"

/*******************************************************************************/

/* Envoie une série d'octet a destination d'une portion de mémoire
vers le registre video spécifié */

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

/* Envoie une série d'octet a destination d'une portion de mémoire
vers le registre video spécifié (accés data et index confondu) */

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

/* Récupère une série d'octet en provenance d'un registre video spécifié
vers portion de mémoire */

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

/* Récupère une série d'octet en provenance d'un registre video spécifié
vers portion de mémoire (accés data et index confondu) */

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

