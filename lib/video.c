#include "vga.h"
#include "video.h"
#include "stdarg.h"

console vc[8] = {
	{0x07, 0, 0, 0, 0, 0, 0, 0}
	,
	{0x07, 0, 0, 0, 0, 0, 0, 0}
	,
	{0x07, 0, 0, 0, 0, 0, 0, 0}
	,
	{0x07, 0, 0, 0, 0, 0, 0, 0}
	,
	{0x07, 0, 0, 0, 0, 0, 0, 0}
	,
	{0x07, 0, 0, 0, 0, 0, 0, 0}
	,
	{0x07, 0, 0, 0, 0, 0, 0, 0}
	,
	{0x07, 0, 0, 0, 0, 0, 0, 0}
};

u8 usedvc = 0;

/*******************************************************************************/

/* Fixe l'attribut courant */

void setattrib(u8 att)
{
	static const u8 ansitovga[] = {
		0, 4, 2, 6, 1, 5, 3, 7
	};
	u8 tempattr;

	tempattr = vc[usedvc].attrib;
	if (att == 0)
		tempattr = 0x07;	/* Couleur Grise sur fond noir */
	else if (att == 5)
		tempattr |= 0x80;
	else if (att == 7)
		tempattr = ((tempattr & 0x0F) << 4) + ((tempattr & 0xF0) >> 4);
	else if (att == 8)
		tempattr = 0;
	else if (att == 1)
		tempattr |= 0x08;	/* Forte intensité */
	else if (att >= 30 && att <= 37) {
		att = ansitovga[att - 30];
		tempattr = (tempattr & ~0x07) | att;	/* couleur de premier plan */
	} else if (att >= 40 && att <= 47) {
		att = ansitovga[att - 40] << 4;
		tempattr = (tempattr & ~0x70) | att;	/* couleur de fond */
	}
	vc[usedvc].attrib = tempattr;
}

/*******************************************************************************/

/* gere l'ansi */

bool makeansi(u8 c)
{
/* state machine to handle the escape sequences */
	switch (vc[usedvc].ansi) {
	case 0:
/* ESC -- next state */
		if (c == 0x1B) {
			vc[usedvc].ansi++;
			return 1;	/* "I handled it" */
		}
		break;
/* ESC */
	case 1:
		if (c == '[') {
			vc[usedvc].ansi++;
			vc[usedvc].param1 = 0;
			return 1;
		}
		break;
/* ESC[ */
	case 2:
		if (isdigit(c)) {
			vc[usedvc].param1 = vc[usedvc].param1 * 10 + c - '0';
			return 1;
		} else if (c == ';') {
			vc[usedvc].ansi++;
			vc[usedvc].param2 = 0;
			return 1;
		}
/* ESC[2J -- efface l'ecran */
		else if (c == 'J') {
			if (vc[usedvc].param1 == 2) {
				fill(vc[usedvc].attrib);
				vc[usedvc].cursX = 0;
				vc[usedvc].cursY = 0;
				gotoscr(0, 0);
				vc[usedvc].ansi = 0;
				return 1;
			}
		}
/* ESC[num1m -- met l'attribut num1 */
		else if (c == 'm') {
			setattrib(vc[usedvc].param1);
			vc[usedvc].ansi = 0;
			return 1;
		}
/* ESC[num1A -- bouge le curseur de num1 vers le haut */
		else if (c == 'A') {
			vc[usedvc].cursY -= vc[usedvc].param1;
			if (vc[usedvc].cursY < 0)
				vc[usedvc].cursY = 0;
			vc[usedvc].ansi = 0;
			gotoscr(vc[usedvc].cursX, vc[usedvc].cursY);
			return 1;
		}
/* ESC[num1B -- bouge le curseur de num1 vers le bas */
		else if (c == 'B') {
			vc[usedvc].cursY += vc[usedvc].param1;
			if (vc[usedvc].cursY >= getyres() - 1)
				vc[usedvc].cursY = getyres();
			vc[usedvc].ansi = 0;
			gotoscr(vc[usedvc].cursX, vc[usedvc].cursY);
			return 1;
		}
/* ESC[num1D -- bouge le curseur de num1 vers la gauche */
		else if (c == 'D') {
			vc[usedvc].cursX -= vc[usedvc].param1;
			if (vc[usedvc].cursX < 0)
				vc[usedvc].cursX = 0;
			vc[usedvc].ansi = 0;
			gotoscr(vc[usedvc].cursX, vc[usedvc].cursY);
			return 1;
		}
/* ESC[num1C -- bouge le curseur de num1 vers la droite */
		else if (c == 'C') {
			vc[usedvc].cursX += vc[usedvc].param1;
			if (vc[usedvc].cursX >= getxres() - 1)
				vc[usedvc].cursX = getxres();
			vc[usedvc].ansi = 0;
			gotoscr(vc[usedvc].cursX, vc[usedvc].cursY);
			return 1;
		}
		break;
/* ESC[num1; */
	case 3:
		if (isdigit(c)) {
			vc[usedvc].param2 = vc[usedvc].param2 * 10 + c - '0';
			return 1;
		} else if (c == ';') {
			vc[usedvc].ansi++;
			vc[usedvc].param3 = 0;
			return 1;
		}
/* ESC[num1;num2H ou ESC[num1;num2f-- bouge le curseur en num1,num2 */
		else if ((c == 'H') || (c == 'f')) {
			/* Remet la position du curseur matériel  a num1,num2 */
			gotoscr(vc[usedvc].param2, vc[usedvc].param1);
			/* Remet la position du curseur logiciel  a num1,num2 */
			vc[usedvc].cursX = vc[usedvc].param2;
			vc[usedvc].cursY = vc[usedvc].param1;
			vc[usedvc].ansi = 0;
			return 1;
		}
/* ESC[num1;num2m -- met les attributs num1,num2 */
		else if (c == 'm') {
			setattrib(vc[usedvc].param1);
			setattrib(vc[usedvc].param2);
			vc[usedvc].ansi = 0;
			return 1;
		}
		break;
/* ESC[num1;num2;num3 */
	case 4:
		if (isdigit(c)) {
			vc[usedvc].param3 = vc[usedvc].param3 * 10 + c - '0';
			return 1;
		}
/* ESC[num1;num2;num3m -- met les attributs num1,num2,num3 */
		else if (c == 'm') {
			setattrib(vc[usedvc].param1);
			setattrib(vc[usedvc].param2);
			setattrib(vc[usedvc].param3);
			vc[usedvc].ansi = 0;
			return 1;
		}
		break;
/* Mauvais etat >> reset */
	default:
		vc[usedvc].ansi = 0;
		break;
	}
	vc[usedvc].ansi = 0;
	return 0;		/* Ansi fini ;) */
}

/*******************************************************************************/

/* Change la console en cours d'utilisation */

void changevc(u8 avc)
{
	usedvc = avc;
	showpage(usedvc);
	setpage(usedvc);
	gotoscr(vc[usedvc].cursX, vc[usedvc].cursY);
}

/*******************************************************************************/

/* affiche un caractère a l'écran */

void putchar(u8 thechar)
{
	showpage(usedvc);
	setpage(usedvc);
	if (makeansi(thechar))
		return;
	switch (thechar) {
	case 0x11:
		if (vc[usedvc].cursY > 0)
			vc[usedvc].cursY--;
		break;
	case 0x12:
		if (vc[usedvc].cursY < getyres() - 1)
			vc[usedvc].cursY++;
		break;
	case 0x13:
		if (vc[usedvc].cursX > 0)
			vc[usedvc].cursX--;
		break;
	case 0x14:
		if (vc[usedvc].cursX < getxres() - 1)
			vc[usedvc].cursX++;
		break;
	case 0x2:
		vc[usedvc].cursX = 0;
		vc[usedvc].cursY = 0;
		break;
	case 0x3:
		vc[usedvc].cursX = 0;
		vc[usedvc].cursY = getyres() - 1;
		break;
	case 0x19:
		vc[usedvc].cursX = getxres() - 1;
		break;
	case '\b':
		if (vc[usedvc].cursX == 0) {
			if (vc[usedvc].cursY > 0) {
				vc[usedvc].cursX = getxres() - 1;
				vc[usedvc].cursY--;
			}
		} else {
			vc[usedvc].cursX--;
		}
		showchar(vc[usedvc].cursX, vc[usedvc].cursY, ' ',
			 vc[usedvc].attrib);
		break;
	case '\t':
		vc[usedvc].cursX = (vc[usedvc].cursX + 8) & ~(8 - 1);
		break;
	case '\n':
		vc[usedvc].cursX = 0;
		break;
	case '\r':
		vc[usedvc].cursX = 0;
		vc[usedvc].cursY++;
		break;
	default:
		if (thechar >= ' ') {
			showchar(vc[usedvc].cursX, vc[usedvc].cursY, thechar,
				 vc[usedvc].attrib);
			vc[usedvc].cursX++;
		}
		break;
	}
	if (vc[usedvc].cursX >= getxres()) {
		vc[usedvc].cursX = 0;
		vc[usedvc].cursY++;
	}
	if (vc[usedvc].cursY >= getyres()) {
		scroll(1, vc[usedvc].attrib);
		vc[usedvc].cursY = getyres() - 1;
	}
	gotoscr(vc[usedvc].cursX, vc[usedvc].cursY);
}

/*******************************************************************************/

/* affiche une chaine de caractère a l'écran */

void print(u8 * string)
{
	u8 *source;
	source = string;
	while (*source != 0) {
		putchar(*source++);
	}
}

/*******************************************************************************/

/* affiche une chaine de caractère formaté a l'ecran */

void printf(const u8 * string, ...)
{
	va_list ap;
	u8 buffer[50];
	u8 *pointer;

	u8 radix;
	bool signe;
	long num;

	va_start(ap, string);
	while (*string != 0) {
		if (*string != '%')
			putchar(*string);
		else {
			switch (*++string) {
			case 'c':
				putchar(va_arg(ap, int));
				break;
			case 'u':
				radix = 10;
				signe = 0;
 showstring:
				num = va_arg(ap, int);
				pointer = buffer + 50 - 1;
				*pointer = '\0';
				if ((signe == 1) && (num < 0)) {
					num = -num;
					signe++;
				}
				do {
					unsigned long temp;
					temp = (unsigned long)num % radix;
					pointer--;
					if (temp < 10)
						*pointer = temp + '0';
					else
						*pointer = temp - 10 + 'a';
					num = (unsigned long)num / radix;
				}
				while (num != 0);
				if (signe > 1)
					*(--pointer) = '-';
				while (*pointer != 0)
					putchar(*pointer++);
				break;
			case 'o':
				radix = 8;
				signe = 0;
				goto showstring;
			case 'd':
			case 'i':
				radix = 10;
				signe = 1;
				goto showstring;
			case 'x':
				radix = 16;
				signe = 0;
				goto showstring;
			case 's':
				pointer = va_arg(ap, u8 *);
				if (!pointer)
					pointer = "(null)";
				while (*pointer != 0)
					putchar(*pointer++);
				break;
			case '%':
				putchar('%');
				break;
			default:
				putchar(va_arg(ap, int));
				break;
			}
		}
		string++;
	}
	va_end(ap);
}

/*******************************************************************************/
