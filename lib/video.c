/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
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
/* Fonction d'affichage (pour printf) */

u32 printstr(u8* src, u8** dest, u32 len) {
	for(u32 i=0;i<len;i++)
		putchar(*(src++));
	return len;
}

/*******************************************************************************/
/* Fonction d'enregistrement dans une variable (pour sprintf) */

u32 storestr(u8* src, u8** dest, u32 len) {
    memcpy(src, *dest, len, 1);
    *dest=*dest+len;
    return len;
}

/*******************************************************************************/
/* affiche une chaine de caractère formaté a l'ecran */

u32 printf(const u8 * string, ...)
{
	va_list args;
	va_start(args, string);
    format(string, args, &printstr, NULL);
    va_end(args);
}

/*******************************************************************************/
/* met une chaine de caractère formaté dans une variable */

u32 sprintf(u8 *variable, const u8 *string, ...)
{
	va_list args;
	va_start(args, string);

    format(string, args, &storestr, variable);
    va_end(args);
}


/*******************************************************************************/
/* affiche une chaine de caractère formaté a l'ecran */

#define maxsize 1024

u32 format(const u8 * string, va_list args, u32 (*fonction)(u8* src, u8** dest, u32 len), u8* dest)
{
	u64 sizes[] = { 0xFF, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
	u8 units[][4] = { "o\000\000", "kio", "mio", "gio", "tio", "pio" };
	u8 strbase2[] = "0xb\000";
	u8 strbase8[] = "0xo\000";
	u8 strbase16[] = "0x\000";
	u8 hexadecimal[] = "*0x\000";
	u8 achar, temp;
	u8 asize, charadd, unit, precisioni, precisionf;
	u8 buffer[maxsize];
    u8* bufferend;
    u32 buffersize;
	u8 *str = string;
	u8 *strtemp;
	u32 i = 0, counter = 0;
	u64 num;
	bool flag = false, intok = false, decok = false;

	for (achar = *str; achar != '\000'; i++, achar = *(str + i)) {
		if (achar != '%' && !flag) {
			fonction((str + i),&dest,1);
			counter++;
			asize = 2;
            precisioni = 0;
            precisionf = 0;
            intok = false;
            decok = false;
			charadd = 0xFF;
		} else if (achar == '%' || flag) {
			if (!flag)
				++i;
			achar = *(str + i);
			switch (achar) {
			case 'z':
				charadd = achar;
				flag = true;
				break;
			case ' ':
				charadd = achar;
				flag = true;
				break;
			case 'h':
				asize--;
                if (asize<0) asize=0;
				flag = true;
				break;
			case 'l':
				asize++;
                if (asize>3) asize=3;
				flag = true;
				break;
            case '.':
                intok=true;
                decok=false;
                flag = true;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
                if (!intok)
                {
                    if (!decok)
                    {
                        precisioni=achar-'0';
                        decok=true;
                    }
                    else {
                        precisioni*=10;
                        precisioni+=achar-'0';
                        decok=false;
                    }
                }
                else {
                    if (!decok)
                    {
                        precisionf=achar-'0';
                        decok=true;
                    }
                    else {
                        precisionf*=10;
                        precisionf+=achar-'0';
                        decok=false;
                    }
                }               
				flag = true;
				break;
            case 'f':
            case 'e':
                if (achar=='e') {
                    precisioni=1;
                    precisionf=8;
                }
                if (asize==0) {
				    num = (u64) va_arg(args, u8);
                    break;
                }
                else if (asize==1) {
                    num = (u64) va_arg(args, u16);
                    break;
                }
                else if (asize==2)
                    bufferend=rtoasingle((float)va_arg(args, double), &buffer, precisioni, precisionf);
                else
                    bufferend=rtoadouble((double)va_arg(args, double), &buffer, precisioni, precisionf);
                buffersize=bufferend-&buffer[0];
				counter += fonction(&buffer,&dest,buffersize);
				flag = false;
				break;
			case 'u':
                if (asize==0)
				    num = (u64) va_arg(args, u8);
                else if (asize==1)
                    num = (u64) va_arg(args, u16);
                else if (asize==2)
                    num = (u64) va_arg(args, u32);
                else
                    num = (u64) va_arg(args, u64);
				if (charadd == 0xFF)
					charadd = '0';
				bufferend=itoa(num, &buffer, 10, sizes[asize], charadd);
                buffersize=bufferend-&buffer[0];
				counter += fonction(&buffer,&dest,buffersize);
				flag = false;
				break;
			case 'o':
                if (asize==0)
				    num = (u64) va_arg(args, u8);
                else if (asize==1)
                    num = (u64) va_arg(args, u16);
                else if (asize==2)
                    num = (u64) va_arg(args, u32);
                else
                    num = (u64) va_arg(args, u64);
				if (charadd == 0xFF)
					charadd = '0';
				counter += fonction(&strbase8,&dest,1);
				bufferend=itoa(num, &buffer, 8, sizes[asize], charadd);
                buffersize=bufferend-&buffer[0];
				counter += fonction(&buffer,&dest,buffersize);
				flag = false;
				break;
			case 'c':
				temp = (u8) va_arg(args, u8);
			    fonction(&temp,string,1);
				counter++;
				flag = false;
				break;
            case 'H':
                if (asize==0)
				    num = (u64) va_arg(args, u8);
                else if (asize==1)
                    num = (u64) va_arg(args, u16);
                else if (asize==2)
                    num = (u64) va_arg(args, u32);
                else
                    num = (u64) va_arg(args, u64);
				if (charadd == 0xFF)
					charadd = ' ';
                unit=0;     
                while(num>1024*10)
                {
                    num=num>>10;
                    unit++;
                }   
				bufferend=sitoa(num, &buffer, sizes[asize]);
                buffersize=bufferend-&buffer[0];
				counter += fonction(&buffer,&dest,buffersize);
				counter += fonction(units[unit],&dest,3);
				flag = false;
				break;
			case 'd':
			case 'i':
                if (asize==0)
				    num = (u64) va_arg(args, u8);
                else if (asize==1)
                    num = (u64) va_arg(args, u16);
                else if (asize==2)
                    num = (u64) va_arg(args, u32);
                else
                    num = (u64) va_arg(args, u64);
				if (charadd == 0xFF)
					charadd = ' ';
				bufferend=sitoa(num, &buffer, sizes[asize]);
                buffersize=bufferend-&buffer[0];
				counter += fonction(&buffer,&dest,buffersize);
				flag = false;
				break;
			case 's':
				strtemp = (u8 *) va_arg(args, u8 *);
				counter += fonction(strtemp,&dest,strlen(strtemp));
				flag = false;
				break;
			case 'p':
				num = (u32) va_arg(args, int);
				if (charadd == 0xFF)
					charadd = '0';
                counter += fonction(&hexadecimal,&dest,3);
				bufferend=itoa(num, &buffer, 16, sizes[asize], '0');
                buffersize=bufferend-&buffer[0];
				counter += fonction(&buffer,&dest,buffersize);
				flag = false;
				break;
			case 'x':
			case 'X':
			case 'y':
			case 'Y':
                if (asize==0)
				    num = (u64) va_arg(args, u8);
                else if (asize==1)
                    num = (u64) va_arg(args, u16);
                else if (asize==2)
                    num = (u64) va_arg(args, u32);
                else
                    num = (u64) va_arg(args, u64);
				if (charadd == 0xFF)
					charadd = '0';
				if (achar == 'X') counter += fonction(&strbase16,&dest,2);
				bufferend=itoa(num, &buffer, 16, sizes[asize], charadd);
                buffersize=bufferend-&buffer[0];
				if (achar == 'X'||achar == 'Y')
					strtoupper(&buffer);
				counter += fonction(&buffer,&dest,buffersize);
				flag = false;
				break;
			case 'b':
                if (asize==0)
				    num = (u64) va_arg(args, u8);
                else if (asize==1)
                    num = (u64) va_arg(args, u16);
                else if (asize==2)
                    num = (u64) va_arg(args, u32);
                else
                    num = (u64) va_arg(args, u64);
				if (charadd == 0xFF)
					charadd = '0';
                counter += fonction(&strbase2,&dest,2);
				bufferend=itoa(num, &buffer, 2, sizes[asize], charadd);
                buffersize=bufferend-&buffer[0];
				counter += fonction(&buffer,&dest,buffersize);
				flag = false;
				break;
			default:
				break;
			}
		}
	}
    
	return counter;
}

/*******************************************************************************/
/* converti un réel signé en chaine de caractère */

u8 *rtoadouble(double num, u8 * str, u8 precisioni , u8 precisionf) {
  s8 power10;
  u8 *pointer=str;
  u8 i,j,integerpart,fracpart;
  if (precisioni==0) precisioni=12;
  if (precisionf==0) precisionf=8;  
  double round=0.5;
  for (i=0;i<precisionf;i++) round/=10;
  num+=round;
  bool intok=false;     
  if (num<0) {                                                  
      num=-num;
      *(pointer++) = '-';            
  }
  power10=0;
  {
    while (num>=10.0) {
        num/=10;               
        power10++;
    }
  } 
  if (power10<precisioni)
  {  
      fracpart=power10;                       
      power10=0;  
  }
 else
{
   fracpart=precisioni;
   power10=power10-precisioni+1;
}                
  if (power10==0)         
    {
        if (num!=0.0)          
        {                  
          while (num<1.0)     
          {              
              num*=10;   
              power10--;
          }
        }
     }   

  i=j=0;  
  while(num>0) 
  {  
    if (!intok & (i>fracpart || i>=precisioni)) {
        *(pointer++) = '.';
        intok=true;
     }
    if (intok && j>=precisionf)
        break;
      num-= (integerpart=num);
      *(pointer++) = integerpart+'0';
      num*=10.0;
    if (!intok) 
        i++;
    else
        j++;
  }
  while((*(pointer-1))=='0' && pointer>str+1)
    pointer--;
  if ((*(pointer-1))=='.') pointer--;
  if (abs(power10)>0)
  {
    *(pointer++) = 'e'; 
    if (power10<0) { 
            power10=-power10; 
            *(pointer++) = '-';
        }
        *(pointer++) = (power10/10+'0');
        *(pointer++) = (power10%10+'0');
   }
    *(pointer++) = 0;
}

/*******************************************************************************/
/* converti un réel signé en chaine de caractère */

u8 *rtoasingle(float num, u8 * str, u8 precisioni , u8 precisionf) {
   return rtoadouble((double)num, str, precisioni, precisionf);
}

/*******************************************************************************/
/* converti un entier non signé en chaine de caractère */

u8 *itoa(u64 orignum, u8 * str, u8 base, u64 dim, u8 achar)
{
	u8 *pointer = str, i, size = 0;
	u64 num = orignum;
    if (dim>=0xFF)
    {
	    num &= dim;
	    if ((num == 0) && (achar == 0)) {
		    *(pointer++) = '0';
		    *pointer = '\000';
		    return pointer;
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
    }
    else
        size=dim;
	for (i = 0; i < size; i++) {
		if (num == 0) {
            if (i==0)
                *(pointer++) = '0';
            else
                *(pointer++) = achar;
            if (achar == 0) break;
        }
        else
        {
		u64 result = num % (u32) base;
		*(pointer++) = (result > 9) ? (result - 10) + 'a' : result + '0';
		num = num / (u32) base;
        }
	}
	*pointer = '\000';
	strinvert(str);
	return pointer;
}

/*******************************************************************************/
/* converti un entier en chaine de caractère */

u8 *sitoa(u64 num, u8 * str, u64 dim)
{
	u8 *pointer = str;
	bool isNegative = false;
	num &= dim;
	if (num == 0) {
		*(pointer++) = '0';
		*pointer = '\000';
		return pointer;
	}
	if ((((dim+1)>>1)&num)>0) {
		isNegative = true;
		num = (~num&dim)+1;
	}
	while (num != 0) {
		u64 result = num % 10;
		*(pointer++) =
		    (result > 9) ? (result - 10) + 'a' : result + '0';
		num = num / 10;
	}
	if (isNegative)
		*(pointer++) = '-';
	*pointer = '\000';
	strinvert(str);
	return pointer;
}
/*******************************************************************************/
