#include "types.h"

void memset(void *dest, u8 val, u16 count, u32 size)
{
 char *temp;
 for(temp = (u8 *)dest; count != 0; count--) 
{
*temp = val;
temp+=size;
}
}

void memcpy(void *dest, const void *src, u32 size)
{
	u8 *s, *d;
	u32 i;
	s = (u8 *) src;
	d = (u8 *) dest;
	for(i=0;i<size;i++){
		*(d+i) = *(s+i);	
	}
}



int memcmp(const void *src, const void *dest, int count)
{
 const unsigned char *mem1 = (const unsigned char *)src;
 const unsigned char *mem2 = (const unsigned char *)dest;
 for(; count != 0; count--)
 {
  if(*mem1 != *mem2)
  return *mem1 -  *mem2;
  mem1++;
  mem2++;
 }
}

