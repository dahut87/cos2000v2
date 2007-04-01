#include "types.h"

void memset(void *dst, u8 val, u16 count,u32 size)
{
 u8 *temp;
 for(temp = (u8 *)dst; count != 0; count--) 
{
temp+=size;
*temp = val;
}
}
