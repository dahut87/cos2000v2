#include "types.h"

/*******************************************************************************/

/* Copie un octet une ou plusieurs fois en mémoire */
void memset(void *dst, u8 val, u32 count,u32 size)
{
	u8 *temp;
	for(temp = (u8 *)dst; count != 0; count--)
	{
		temp+=size;
		*temp = val;
	}
}

/*******************************************************************************/

/* Copie une portion de mémoire vers une autre */
void memcpy(void *src, void *dst, u32 count, u32 size)
{
	char *s, *d;
	u32 i;
	s = (u8*) src;
	d = (u8*) dst;
	for(i=0;i<count;i++){
		*(d+i*size) = *(s+i);	
	}
}

/*******************************************************************************/

/* Compare 2 portions de mémoire */
u32 memcmp(void *src, void *dst, u32 count, u32 size)
{
	const u8 *mem1 = (const u8 *)src;
	const u8 *mem2 = (const u8 *)dst;
	for(; count != 0; count--)
	{
		if(*mem1 != *mem2)
		return *mem1 -  *mem2;
		mem1+=size;
		mem2+=size;
	}
}

/*******************************************************************************/
