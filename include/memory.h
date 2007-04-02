#include "types.h"
#include "asm.h"

void memset(void *dest, u8 val, u16 count,int size);
void memcpy(void *dest, const void *src, int size);
int memcmp(const void *memptr1, const void *memptr2, int count);