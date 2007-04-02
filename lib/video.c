#include "vga.h"
#include "video.h"

u8 attrib=0x07;
u16 cursX,cursY;      /* position du curseur */
extern u16 resX,resY;        /* resolution x,y en caractères*/	
u8 ansi,param1,param2,param3;


/*******************************************************************************/

/* Fixe l'attribut courant */

void setattrib(u8 att)
{
	static const u8 ansitovga[] =
	{
		0, 4, 2, 6, 1, 5, 3, 7
	};
	u8 tempattr;

	tempattr = attrib;
	if(att == 0)
		tempattr &= ~0x08;		/* Faible intensité */
	else if(att == 1)
		tempattr |= 0x08;		/* Forte intensité */
	else if(att >= 30 && att <= 37)
	{
		att = ansitovga[att - 30];
		tempattr = (tempattr & ~0x07) | att;/* couleur de premier plan */
	}
	else if(att >= 40 && att <= 47)
	{
		att = ansitovga[att - 40] << 4;
		tempattr = (tempattr & ~0x70) | att;/* couleur de fond */
	}
	attrib = tempattr;
}

/*******************************************************************************/

/* gere l'ansi */

bool makeansi(u8 c)
{
/* state machine to handle the escape sequences */
	switch(ansi)
	{
	case 0:
/* ESC -- next state */
		if(c == 0x1B)
		{
			ansi++;
			return 1; /* "I handled it" */
		}
		break;
/* ESC */
	case 1:
		if(c == '[')
		{
			ansi++;
			param1 = 0;
			return 1;
		}
		break;
/* ESC[ */
	case 2:
		if(isdigit(c))
		{
			param1 = param1 * 10 + c - '0';
			return 1;
		}
		else if(c == ';')
		{
			ansi++;
			param2 = 0;
			return 1;
		}
/* ESC[2J -- efface l'ecran */
		else if(c == 'J')
		{
			if(param1 == 2)
			{
				fill(attrib);
				ansi = 0;
				return 1;
			}
		}
/* ESC[num1m -- met l'attribut num1 */
		else if(c == 'm')
		{
			setattrib(param1);
			ansi = 0;
			return 1;
		}
/* ESC[num1A -- bouge le curseur de num1 vers le haut */
		else if(c == 'A')
		{
     cursY-=param1;
			ansi = 0;
			gotoscr(cursX,cursY);
			return 1;    
    }
/* ESC[num1B -- bouge le curseur de num1 vers le bas */
		else if(c == 'B')
		{
     cursY+=param1;
			ansi = 0;
		gotoscr(cursX,cursY);
			return 1;    
    }
/* ESC[num1C -- bouge le curseur de num1 vers la gauche */
		else if(c == 'C')
		{
     cursX-=param1;
			ansi = 0;
		gotoscr(cursX,cursY);
			return 1;    
    }
/* ESC[num1D -- bouge le curseur de num1 vers la droite */
		else if(c == 'D')
		{
     cursX+=param1;
			ansi = 0;
		gotoscr(cursX,cursY);
			return 1;    
    }
		break;
/* ESC[num1; */
	case 3:
		if(isdigit(c))
		{
			param2 = param2 * 10 + c - '0';
			return 1;
		}
		else if(c == ';')
		{
			ansi++;
			param3 = 0;
			return 1;
		}
/* ESC[num1;num2H ou ESC[num1;num2f-- bouge le curseur en num1,num2 */
		else if((c == 'H')||(c == 'f'))
		{
	/* Remet la position du curseur matériel  a num1,num2 */
			gotoscr(param2,param1);
	/* Remet la position du curseur logiciel  a num1,num2 */
	    cursX=param2;
	    cursY=param1;			
			ansi = 0;
			return 1;
		}
/* ESC[num1;num2m -- met les attributs num1,num2 */
		else if(c == 'm')
		{
			setattrib(param1);
			setattrib(param2);
			ansi = 0;
			return 1;
		}
		break;
/* ESC[num1;num2;num3 */
	case 4:
		if(isdigit(c))
		{
			param3 = param3 * 10 + c - '0';
			return 1;
		}
/* ESC[num1;num2;num3m -- met les attributs num1,num2,num3 */
		else if(c == 'm')
		{
			setattrib(param1);
			setattrib(param2);
			setattrib(param3);
			ansi = 0;
			return 1;
		}
		break;
/* Mauvais etat >> reset */
	default:
		ansi = 0;
		break;
	}
	ansi = 0;
	return 0; /* Ansi fini ;)*/
}

/*******************************************************************************/

/* affiche un caractère a l'écran */

void putchar(u8 thechar)
{
if(makeansi(thechar)) return;
switch (thechar)
{
case 0x11:
if (cursY>0) cursY--;
break;
case 0x12:
if (cursY<resY-1) cursY++;
break;
case 0x13:
if (cursX>0) cursX--;
break;
case 0x14:
if (cursX<resX-1) cursX++;
break;
case 0x2:
cursX=0;
cursY=0;
break;
case 0x3:
cursX=0;
cursY=resY-1;
break;
case 0x19:
cursX=resX-1;
break;
case '\b':
if (cursX==0) 
{
if (cursY>0)
{
cursX=resX-1;
cursY--;
}
}
else
{
cursX--;
}
showchar(cursX,cursY,' ',attrib);
break;
case '\t':
cursX=(cursX + 8) & ~(8 - 1);
break;
case '\n':
cursX=0;
break;
case '\r':
cursX=0;
cursY++;
break;
default:
if (thechar>=' ') 
{
showchar(cursX,cursY,thechar,attrib);
cursX++;
}
break;
}
if (cursX>=resX)
	{
		cursX=0;
		cursY++;
	}
if (cursY>=resY)
		{
			scroll(1,attrib);
			cursY=resY-1;
		}
	gotoscr(cursX,cursY);
}

/*******************************************************************************/

/* affiche une chaine de caractère a l'écran */

void print(u8* string)
{
	u8 *source;		
	source = string;
	while(*source!=0x00)
        {
        putchar(*source++);
        }
}

/*******************************************************************************/

/* affiche un octet sous forme hexadécimale a l'ecran */

void showhex(u8 src)
{
	static u8 hexadigit[16] = "0123456789ABCDEF";
	putchar(hexadigit[(src&0xF0)>>4]);
	putchar(hexadigit[src&0x0F]);
}

/*******************************************************************************/

