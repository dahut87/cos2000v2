/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#define sgn(x) ((x<0)?-1:((x>0)?1:0));
                                        
u32 abs(int x);
random(u32 lower, u32 upper);
u32 rand(void);
void randomize(void);
u8 log2(u32 n);
u8 log10(u32 n);
unsigned long long __udivdi3 (unsigned long long num, unsigned long long den);
unsigned long long __umoddi3 (unsigned long long n, unsigned long long d);
