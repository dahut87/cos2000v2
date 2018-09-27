#include <string.h>

/******************************************************************************/

/* Compare 2 chaines de caractère et renvoie la premiere distance (diff) */

s8 strcmp(const u8 * src, const u8 * dest)
{
	register s8 result;
	do {
		if ((result = *src - *dest++) != 0)
			break;
	}
	while (*src++ != 0);
	return result;
}

/******************************************************************************/

/* Trouve la premiere occurence d'un caractère dans une chaine et renvoie un pointeur */

u8 *strchr(const u8 * src, u8 achar)
{
	for (; *src != achar; ++src)
		if (*src == 0)
			return 0;
	return (u8 *) src;
}

/******************************************************************************/

/* Trouve la premiere occurence d'un caractère dans une chaine  */

u32 strchrpos(const u8 * src, u8 achar)
{
	u32 i;
	for (i = 0; *(src + i) != achar; ++i)
		if (*(src + i) == 0)
			return 0;
	return i;
}

/******************************************************************************/

/* Renvoie la taille de la chaine */

u32 strlen(const u8 * src)
{
	u32 size;

	for (size = 0; *(src + size) != 0; size++) ;
	return size;
}

/******************************************************************************/

/* copie une chaine dans une autre */
u8 *strcpy(const u8 * src, u8 * dest)
{
	u8 *temp = dest;
	while ((*dest++ = *src++) != 0) ;
	return temp;
}

/******************************************************************************/

/* copie une portion limité d'une chaine asciiZ*/

u8 *strncpy(const u8 * src, u8 * dest, u32 count)
{
	u8 *temp = dest;
	while (count) {
		if ((*temp = *src) != 0)
			src++;
		temp++;
		count--;
	}
	return dest;
}

/******************************************************************************/

/* concatene 2 chaines de caractère */

u8 *strcat(const u8 * src, u8 * dest)
{
	u8 *temp = dest;
	while (*dest != 0)
		dest++;
	while ((*dest++ = *src++) != 0) ;
	return temp;
}

/******************************************************************************/

/* Met en minuscule */

void strtolower(u8 * src)
{
	while (*src != 0) {
		if ((*src >= 'A') && (*src <= 'Z'))
			*src = *src + 'a' - 'A';
		src++;
	}
}

/******************************************************************************/

/* Met en majuscule */

void strtoupper(u8 * src)
{
	while (*src != 0) {
		if ((*src >= 'a') && (*src <= 'z'))
			*src = *src - ('a' - 'A');
		src++;
	}
}

/******************************************************************************/

/* inverse la casse */

void strinvertcase(u8 * src)
{
	while (*src != 0) {
		if ((*src >= 'A') && (*src <= 'Z'))
			*src = *src + 'a' - 'A';
		else if ((*src >= 'a') && (*src <= 'z'))
			*src = *src - ('a' - 'A');
		src++;
	}
}

/******************************************************************************/

/* Met une majuscule et met le reste en minuscule */

void stronecase(u8 * src)
{
	if ((*src >= 'a') && (*src <= 'z'))
		*src = *src - ('a' - 'A');
	while (*src != 0) {
		src++;
		if ((*src >= 'A') && (*src <= 'Z'))
			*src = *src + 'a' - 'A';
	}
}

/******************************************************************************/

/* Fixe la taille de la chaine */

void strsetlen(u8 * src, u32 size)
{
	*(src + size) = '\000';
}

/******************************************************************************/

/* Inverse la chaine */

void strinvert(u8 * src)
{
	u8 *dst = src + strlen(src) - 1;
	while (src < dst) {
		u8 char1 = *dst;
		u8 char2 = *src;
		*dst = char2;
		*src = char1;
		src++;
		dst--;
	}
}

/******************************************************************************/

/* Remplace tout les caractères rechercher par un autre */

void strreplace(u8 * src, u8 search, u8 replaced)
{
	u8 *pos = strchr(src, search);
	while (pos != 0) {
		*pos = replaced;
		pos = strchr(src, search);
	}
}

/******************************************************************************/

/* Rempli de caractère */

u8 *strfill(u8 * dst, u8 pattern, u32 size)
{
	u32 i;
	for (i = 0; i <= size; i++)
		*(dst + i) = pattern;
	*(dst + i + 1) = '\000';
	return dst;
}

/******************************************************************************/

/* Renvoie la partie gauche d'une chaine */

void strleft(u8 * src, u8 * dest, u32 size)
{
	u32 i;
	for (i = 0; (*(src + i) != 0) && (i < size); i++)
		*dest++ = *(src + i);
	*dest++ = '\000';
}

/******************************************************************************/

/* Renvoie la partie droite d'une chaine */

void strright(u8 * src, u8 * dest, u32 size)
{
	u32 i;
	u32 begin = strlen(src) - size;
	for (i = 0; (*(src + i + begin) != 0) && (i < size); i++)
		*dest++ = *(src + i + begin);
	*dest++ = '\000';
}

/******************************************************************************/

/* Supprime une portion de chaine */

void strdelete(u8 * src, u32 index, u32 size)
{
	u32 i;
	u32 realsize = strlen(src) - index - size;
	for (i = 0; (*(src + index + size + i) != 0) && (i < realsize); i++)
		*(src + index + i) = *(src + index + size + i);
	*(src + index + i) = '\000';
}

/******************************************************************************/

/* Insert une portion dans la chaine */

void strinsert(u8 * src, u8 * dest, u32 index)
{
	u32 i;
	u32 realsize = strlen(src);
	u32 copysize = strlen(dest) - index;
	for (i = 0; i <= copysize; i++)
		*(dest + index + realsize + copysize - i) =
		    *(dest + index + copysize - i);
	for (i = 0; (*(src + i) != 0); i++)
		*(dest + index + i) = *(src + i);
}

/******************************************************************************/

/* Supprime les délimiteurs consécutifs */

void strcompressdelimiter(u8 * src, u8 delim)
{
	u8 *pos = strchr(src, delim);
	while (pos != 0) {
		u8 i;
		for (i = 0; (*(pos + i) != 0) && (*(pos + i) == delim); i++) ;
		if (i > 1)
			strdelete(pos, 1, i - 1);
		pos = strchr(pos + 1, delim);
	}
}

/******************************************************************************/

/* Récupérère l'élément N d'une liste utilisant un délimiteur */

u8 *strgetitem(u8 * src, u8 * dest, u8 delim, u32 index)
{
	u32 i;
	u8 *pos = strgetpointeritem(src, delim, index);
	for (i = 0; (*(pos + i) != 0) && (*(pos + i) != delim); i++)
		*(dest + i) = *(pos + i);
	*(dest + i) = '\000';
	return dest;
}

/******************************************************************************/

/* Récupérère un pointeur sur l'élément N d'une liste utilisant un délimiteur */

u8 *strgetpointeritem(u8 * src, u8 delim, u32 index)
{
	u32 i;
	u8 *pos = src;
	for (i = 0; i < index; i++)
		pos = strchr(pos + 1, delim);
	if (*pos == delim)
		pos++;
	return pos;
}

/******************************************************************************/

/* Récupérère le nombre d'éléments d'une liste utilisant un délimiteur */

u32 strgetnbitems(u8 * src, u8 delim)
{
	u32 number = 0;
	if (*src == 0)
		return 0;
	number++;
	u8 *pos = strchr(src, delim);
	while (pos != 0) {
		pos = strchr(pos + 1, delim);
		number++;
	}
	return number;
}

/******************************************************************************/

/* Renvoie la base minimum du nombre */

u8 base[] = " 0123456789ABCDEF\000";

u8 strgetminbase(u8 * src)
{
	u8 temp[] = "                                           \000";
	u8 *dst = &temp;
	strtoupper(strcpy(src, &temp));
	u8 max = 0;
	while (*dst != 0) {
		u32 result = strchrpos(&base, *dst++);
		if (result == 0)
			return 0;
		if (result > max)
			max = result;
	}
	if (max > 10)
		return 16;
	else if (max > 8)
		return 10;
	else if (max > 2)
		return 8;
	else
		return 2;
}

/******************************************************************************/

/* Renvoie la base du nombre */

u8 hexa[] = "0x\000";
u8 bases[] = " bodh\000";
u8 basesnum[] = { 0, 2, 8, 10, 16 };

u8 declaredbase = 10;
u8 minbase = 0;
u8 strgetbase(u8 * src)
{
	u8 temp[] = "                               \000";
	strleft(src, &temp, 2);
	if (strcmp(&temp, &hexa) == 0) {
		declaredbase = 16;
		u8 size = strlen(src);
		strright(src, &temp, size - 2);
		minbase = strgetminbase(&temp);
	} else {
		strright(src, &temp, 1);
		declaredbase = basesnum[strchrpos(&bases, temp[0])];
		if (declaredbase > 0) {
			u8 size = strlen(src);
			strleft(src, &temp, size - 1);
			minbase = strgetminbase(&temp);
		} else {
			minbase = strgetminbase(src);
			declaredbase = minbase;
		}
	}
	if (declaredbase >= minbase && minbase != 0)
		return declaredbase;
	else
		return 0;
}

/******************************************************************************/

/* Transforme une chaine en nombre */

u32 strtoint(u8 * src)
{
	u8 *temp = src;
	u32 result = 0;
	u8 thebase = strgetbase(src);
	u32 multi = 1;
	u8 shorter = 0;

	if (thebase == 0)
		return 0;
	if (*(src + 1) == 'x')
		shorter = 2;
	while (*++temp != 0) ;
	while (*temp == 0 || *temp == 'b' || *temp == 'o' || *temp == 'd'
	       || *temp == 'h')
		temp--;
	while (src + shorter <= temp) {
		u8 achar = *temp--;
		if ((achar >= 'a') && (achar <= 'z'))
			achar = achar - ('a' - 'A');
		result = result + multi * (strchrpos(&base, achar) - 1);
		multi = multi * thebase;
	}
	return result;
}
