/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#define sgn(x) ((x<0)?-1:((x>0)?1:0));
#define M_PI 3.14159265358979323846	//Pi
#define M_2_PI 0.636619772367581343076	//2 fois la réciproque de Pi
#define M_PI_2 1.57079632679489661923	//Pi divisé par two
#define EPSILON 1E-40
#define degtorad(deg) (deg * PI / 180.0)
#define radtodeg(rad) (rad * 180.0 / PI)
u32 crc32(u32 inCrc32, u8 *buf, u32 size);
double  cos(double x);
double  sin(double x);
float   cosf(float x);
float   sinf(float x);
float   fabsf(float n);
double  fabs(double n);
float   sqrtf(float n);
float   rsqrtf(float n);
double  sqrt(double n);
double  rsqrt(double n);
u32     abs(int x);
u32     random(u32 lower, u32 upper);
u32     rand(void);
void    randomize(void);
u8      log2(u64 n);
u8      log10(u64 n);
unsigned long long __udivdi3(unsigned long long num,
			     unsigned long long den);
unsigned long long __umoddi3(unsigned long long n, unsigned long long d);
u32     pow(u32 a, u8 n);
