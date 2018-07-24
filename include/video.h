
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
void print(u8* string);
void printf (const u8 *string, ...);
void changevc(u8 vc);

