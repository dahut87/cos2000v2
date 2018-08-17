#include <string.h>

/******************************************************************************/

/* Compare 2 chaines de caractère et renvoie la premiere distance (diff) */

s8 strcmp(const u8 * src, const u8 * des)
{
	register s8 result;
	do {
		if ((result = *src - *des++) != 0)
			break;
	}
	while (*src++ != 0);
	return result;
}

/******************************************************************************/

/* Trouve la premiere occurence d'un caractère dans une chaine */

u8 *strchr(const u8 * src, u8 achar)
{
	for (; *src != achar; ++src)
		if (*src == 0)
			return 0;
	return (u8 *) src;
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
u8 *strcpy(const u8 * src, u8 * des)
{
	u8 *temp = des;
	while ((*des++ = *src++) != 0) ;
	return temp;
}

/******************************************************************************/

/* copie une portion limité d'une chaine asciiZ*/

u8 *strncpy(const u8 * src, u8 * des, u32 count)
{
	u8 *temp = des;
	while (count) {
		if ((*temp = *src) != 0)
			src++;
		temp++;
		count--;
	}
	return des;
}

/******************************************************************************/

/* concatene 2 chaines de caractère */
u8 *strcat(const u8 * src, u8 * des)
{
	u8 *temp = des;
	while (*des != 0)
		des++;
	while ((*des++ = *src++) != 0) ;
	return temp;
}
