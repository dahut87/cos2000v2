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

u32 print(u8 * string)
{
	u8 *source;
	u32 i = 0;
	source = string;
	while (*source != 0) {
		putchar(*source++);
		i++;
	}
	return i;
}

/*******************************************************************************/

#define buffersize 1024

/* affiche une chaine de caractère formaté a l'ecran */

u32 printf(const u8 * string, ...)
{
	va_list args;
	u32 sizes[] = { 0xFF, 0xFFFF, 0xFFFFFFFF };
	u8 strbase2[] = "b\000";
	u8 strbase8[] = "o\000";
	u8 strbase16[] = "h\000";
	u8 hexadecimal[] = "*0x\000";
	u8 achar, temp;
	u8 asize, charadd;
	u8 buffer[buffersize];
	u8 *str = string;
	u8 *strtemp;
	u32 i = 0, counter = 0;
	u32 num;
	bool flag = false;

	va_start(args, string);
	for (achar = *str; achar != '\000'; i++, achar = *(str + i)) {
		if (achar != '%' && !flag) {
			putchar(achar);
			counter++;
			asize = 2;
			charadd = 0xFF;
		} else if (achar == '%' || flag) {
			if (!flag)
				++i;
			achar = *(str + i);
			switch (achar) {
			case '0':
				charadd = '0';
				flag = true;
				break;
			case ' ':
				charadd = ' ';
				flag = true;
				break;
			case 'h':
				asize--;
				flag = true;
				break;
			case 'l':
				asize++;
				flag = true;
				break;
			case 'u':
				num = (u32) va_arg(args, int);
				if (charadd == 0xFF)
					charadd = '0';
				itoa(num, &buffer, 10, sizes[asize], charadd);
				counter += print(&buffer);
				flag = false;
				break;
			case 'o':
				num = (u32) va_arg(args, int);
				if (charadd == 0xFF)
					charadd = '0';
				itoa(num, &buffer, 8, sizes[asize], charadd);
				counter += print(&buffer) + 1;
				print(&strbase8);
				flag = false;
				break;
			case 'c':
				temp = (u8) va_arg(args, int);
				putchar(temp);
				counter++;
				flag = false;
				break;
			case 'd':
			case 'i':
				num = (int)va_arg(args, int);
				if (charadd == 0xFF)
					charadd = ' ';
				sitoa(num, &buffer, sizes[asize]);
				counter += print(&buffer);
				flag = false;
				break;
			case 's':
				strtemp = (u8 *) va_arg(args, u8 *);
				counter += print(strtemp);
				flag = false;
				break;
			case 'p':
				num = (u32) va_arg(args, int);
				if (charadd == 0xFF)
					charadd = '0';
				print(&hexadecimal);
				itoa(num, buffer, 16, sizes[asize], '0');
				counter += print(&buffer) + 2;
				flag = false;
				break;
			case 'x':
			case 'X':
				num = (u32) va_arg(args, int);
				if (charadd == 0xFF)
					charadd = '0';
				itoa(num, &buffer, 16, sizes[asize], charadd);
				if (achar == 'X')
					strtoupper(&buffer);
				counter += print(&buffer) + 1;
				print(&strbase16);
				flag = false;
				break;
			case 'b':
				num = (u32) va_arg(args, int);
				if (charadd == 0xFF)
					charadd = '0';
				itoa(num, &buffer, 2, sizes[asize], charadd);
				counter += print(&buffer) + 1;
				print(&strbase2);
				flag = false;
				break;
			default:
				break;
			}
		}
	}
	va_end(args);
	return counter;
}

/*******************************************************************************/

/* converti un entier non signé en chaine de caractère */

u8 *itoa(u32 orignum, u8 * str, u8 base, u32 dim, u8 achar)
{
	u32 sizes[] = { 0xFF, 0xFFFF, 0xFFFFFFFF };
	u8 *pointer = str, i, size = 0;
	u32 num = orignum;
	num &= dim;
	if (num == 0 && (achar == 0)) {
		*(pointer++) = '0';
		*pointer = '\000';
		return str;
	}
	switch (base) {
	case 2:
		size = log2(dim);
		break;
	case 8:
		size = log2(dim) / 2;
		break;
	case 10:
		size = log10(dim);
		break;
	case 16:
		size = log2(dim) / 4;
		break;
	}
	for (i = 0; i < size; i++) {
		u32 result = num % (u32) base;
		*(pointer++) =
		    (result > 9) ? (result - 10) + 'a' : result + '0';
		num = num / (u32) base;
		if ((num == 0) && (achar == 0))
			break;
	}
	*pointer = '\000';
	strinvert(str);
	return str;
}

/*******************************************************************************/

/* converti un entier en chaine de caractère */

u8 *sitoa(int num, u8 * str, u32 dim)
{
	u8 *pointer = str;
	bool isNegative = false;
	num &= dim;
	if (num == 0) {
		*pointer++ = '0';
		*pointer = '\000';
		return str;
	}
	if (num < 0) {
		isNegative = true;
		num = -num;
	}
	while (num != 0) {
		u32 result = num % 10;
		*(pointer++) =
		    (result > 9) ? (result - 10) + 'a' : result + '0';
		num = num / 10;
	}
	if (isNegative)
		*(pointer++) = '-';
	*pointer = '\000';
	strinvert(str);
	return str;
}
