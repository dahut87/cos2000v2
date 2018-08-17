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
            *src=*src+'a'-'A';
		src++;
    }
}

/******************************************************************************/

/* Met en majuscule */

void strtoupper(u8 * src)
{
	while (*src != 0) {
        if ((*src >= 'a') && (*src <= 'z'))
            *src=*src-('a'-'A');
		src++;
    }
}

/******************************************************************************/

/* inverse la casse */

void strinvertcase(u8 * src)
{
	while (*src != 0) {
        if ((*src >= 'A') && (*src <= 'Z'))
            *src=*src+'a'-'A';
        else if ((*src >= 'a') && (*src <= 'z'))
            *src=*src-('a'-'A');
		src++;
    }
}

/******************************************************************************/

/* Met une majuscule et met le reste en minuscule */

void stronecase(u8 * src)
{
    if ((*src >= 'a') && (*src <= 'z'))
        *src=*src-('a'-'A');
    while (*src != 0) {
		src++;
        if ((*src >= 'A') && (*src <= 'Z'))
            *src=*src+'a'-'A';
    }
}

/******************************************************************************/

/* Fixe la taille de la chaine */

void strsetlen(u8 * src, u32 size)
{
    *(src+size)='\000';
}

/******************************************************************************/

/* Inverse la chaine */

void strinvert(u8 *src)
{
    u8 *dst=src+strlen(src)-1;
    while (src < dst) {
        u8 char1=*dst;
        u8 char2=*src;
        *dst=char2;
        *src=char1;        
		src++;
        dst--;
    }
}

/******************************************************************************/

/* Remplace tout les caractères rechercher par un autre */

void strreplace(u8 *src, u8 search, u8 replaced)
{
    u8 *pos=strchr(src, search);
    while (pos!=0)
    {   
        *pos=replaced;
        pos=strchr(src, search);
    }
}

/******************************************************************************/

/* Rempli de caractère */

u8 *strfill(u8 *dst, u8 pattern, u32 size)
{
   u32 i;
   for (i=0; i <= size; i++)
        *(dst+i)=pattern;
   *(dst+i+1)='\000';
   return dst;
}

/******************************************************************************/

/* Renvoie la partie gauche d'une chaine */

void strright(u8 *src, u8 *dest, u32 size) {
    u32 i;
    for (i = 0; (*(src + i) != 0) && (i<size); i++)
        *dest++=*(src + i);
    *dest++='\000';
}

/******************************************************************************/

/* Renvoie la partie droite d'une chaine */

void strleft(u8 *src, u8 *dest, u32 size) {
    u32 i;
    u32 begin=strlen(src)-size;
    for (i = 0; (*(src + i + begin) != 0) && (i<size); i++)
        *dest++=*(src + i + begin);
    *dest++='\000';
}

/******************************************************************************/

/* Supprime une portion de chaine */

void strdelete(u8 *src, u32 index, u32 size) {
    u32 i;
    u32 realsize=strlen(src)-index-size;
    for (i = 0; (*(src+index+size+i) != 0) && (i<realsize); i++)
        *(src+index+i)=*(src+index+size+i);
    *(src+index+i)='\000';
}

/******************************************************************************/

/* Insert une portion dans la chaine */

void strinsert(u8 *src, u8 *dest, u32 index) {
    u32 i;
    u32 realsize=strlen(src);
    u32 copysize=strlen(dest)-index;
    for (i = 0; i<=copysize; i++)
        *(dest+index+realsize+copysize-i)=*(dest+index+copysize-i);
    for (i = 0; (*(src+i) != 0) ; i++)
        *(dest+index+i)=*(src+i);
}

/******************************************************************************/

/* Supprime les délimiteurs consécutifs */

void strcompressdelimiter(u8 *src, u8 delim) {
    u8 *pos=strchr(src, delim);
    while (pos!=0)
    {   
        u8 i;
        for (i = 0; (*(pos+i) != 0) && (*(pos+i) == delim); i++);
        if (i>1)
            strdelete(pos,1,i-1);
        pos=strchr(pos+1, delim);
    }
}
