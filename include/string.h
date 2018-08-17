#include "types.h"

s8 strcmp(const u8 *src,const u8 *des);
u32 strlen(const u8 *src);
u8 *strchr(const u8 *src, u8 achar);
u8 *strncpy(const u8 *src,u8 *des,u32 count);
u8 *strcat(const u8 *src,u8 *des);
u8 *strcpy(const u8 *src, u8 *des);
void strtolower(u8 *src);
void strtoupper(u8 *src);
void stronecase(u8 *src);
void strsetlen(u8 *src, u32 size);
void strinvert(u8 *src);
void strreplace(u8 *src, u8 search, u8 replaced);
u8 *strfill(u8 *dst, u8 pattern, u32 size);
void strright(u8 *src, u8 *dest, u32 size);
void strleft(u8 *src, u8 *dest, u32 size);
void strdelete(u8 *src, u32 index, u32 size);
void strcompressdelimiter(u8 *src, u8 delim);
void strinsert(u8 *src, u8 *dest, u32 index);
