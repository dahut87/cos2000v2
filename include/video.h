/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
typedef struct console {
  u8 attrib;
  s16 cursX;
  s16 cursY; 
  u8 ansi;
  u8 param1;
  u8 param2;
  u8 param3;
  u8 page;
} console  __attribute__ ((packed));


void showhex(u8 src);
void putchar(u8 thechar);
u32 print(u8* string);
u32 printf (const u8 *string, ...);
void changevc(u8 vc);
u8* itoa(u64 num, u8* str, u8 base, u64 dim, u8 achar);
u8* sitoa(u64 num, u8 * str, u64 dim);
u8* rtoadouble(double num, u8 * str, u8 precisioni , u8 precisionf);
u8* rtoasingle(float num, u8 * str, u8 precisioni , u8 precisionf);
