/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "timer.h"
#include "math.h"

/*******************************************************************************/
/* Arithmétique 64 bits  */

unsigned long long __udivdi3(unsigned long long num,
			     unsigned long long den)
{
	unsigned long long quot, qbit;
	quot = 0;
	qbit = 1;
	if (den == 0)
	{
		return 0;
	}
	while ((long long) den >= 0)
	{
		den <<= 1;
		qbit <<= 1;
	}
	while (qbit)
	{
		if (den <= num)
		{
			num -= den;
			quot += qbit;
		}
		den >>= 1;
		qbit >>= 1;
	}
	return quot;
}

unsigned long long __umoddi3(unsigned long long n, unsigned long long d)
{
	return n - d * __udivdi3(n, d);
}

/******************************************************************************/
/* Fonctions qui retournent le sinus et cosinus */

double cos(double x)
{
	if (x < 0.0)
		x = -x;
	while (M_PI < x)
		x -= M_2_PI;
	double  result =
		1.0 - (x * x / 2.0) * (1.0 -
				       (x * x / 12.0) * (1.0 -
							 (x * x / 30.0) *
							 (1.0 -
							  x * x / 56.0)));
	return result;
}

double sin(double x)
{
	return cos(x - M_PI_2);
}

float cosf(float x)
{
	if (x < 0.0f)
		x = -x;
	while (M_PI < x)
		x -= M_2_PI;
	float   result =
		1.0f - (x * x / 2.0f) * (1.0f -
					 (x * x / 12.0f) * (1.0f -
							    (x * x /
							     30.0f) *
							    (1.0f -
							     x * x /
							     56.0f)));
	return result;
}

float sinf(float x)
{
	return cosf(x - M_PI_2);
}

/******************************************************************************/
/* Fonction qui retourne la valeur absolue */

float fabsf(float n)
{
	return (*((int *) &n) &= 0x7fffffff);
}

double fabs(double n)
{
	return (*(((int *) &n) + 1) &= 0x7fffffff);
}

/******************************************************************************/
/* Fonction qui retourne la racine */

float sqrtf(float n)
{
	float   x = n;
	float   y = 1;
	double  e = 0.000001;
	while (x - y > e)
	{
		x = (x + y) / 2;
		y = n / x;
	}
	return x;
}

double sqrt(double n)
{
	double  x = n;
	double  y = 1;
	double  e = 0.000001;
	while (x - y > e)
	{
		x = (x + y) / 2;
		y = n / x;
	}
	return x;
}

/******************************************************************************/
/* Fonction qui retourne l'inverse de la racine */

float rsqrtf(float n)
{
	return 1 / sqrt(n);
}

double rsqrt(double n)
{
	return 1 / sqrt(n);
}

/******************************************************************************/
/* Fonction qui retourne la puissance n de a */

u32 pow(u32 a, u8 n)
{
	u32     r = 1;
	while (n > 0)
	{
		if (n & 1)
			r *= a;
		a *= a;
		n >>= 1;
	}
	return r;
}

/******************************************************************************/
/* Fonction qui retourne le logarithme 2 */

u8 log2(u64 n)
{
	if (n == 0)
		return 0;
	int     logValue = -1;
	while (n)
	{

		logValue++;

		n >>= 1;

	}
	return logValue + 1;
}

/******************************************************************************/
/* Fonction qui retourne le logarithme 10 */

u8 log10(u64 n)
{

	return (n >= 10000000000000000000u) ? 19 : (n >=
						    100000000000000000u) ?
		18 : (n >= 100000000000000000u) ? 17 : (n >=
							10000000000000000u)
		? 16 : (n >= 1000000000000000u) ? 15 : (n >=
							100000000000000u) ?
		14 : (n >= 10000000000000u) ? 13 : (n >=
						    1000000000000u) ? 12
		: (n >= 100000000000u) ? 11 : (n >=
					       10000000000u) ? 10 : (n >=
								     1000000000u)
		? 9 : (n >= 100000000u) ? 8 : (n >= 10000000u) ? 7 : (n >=
								      1000000u)
		? 6 : (n >= 100000u) ? 5 : (n >= 10000u) ? 4 : (n >=
								1000u) ? 3
		: (n >= 100u) ? 2 : (n >= 10u) ? 1u : 0u;

}


/******************************************************************************/
/* Fonction qui retourne la valeur absolue */

u32 abs(int x)
{
	if (x < 0)

		x = -x;

	return (u32) x;
}

/******************************************************************************/
/* Fonction qui initialise le générateur de nombre aléatoire */

static u32 seed = 0x12341234;

void randomize(void)
{
	seed = gettimer();
}


/******************************************************************************/
/* Fonction qui renvoie un nombre aléatoire */

u32 rand(void)
{
	u32     next = seed;
	int     result;
	next *= 1103515245;
	next += 12345;
	result = (unsigned int) (next / 65536) % 2048;
	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (unsigned int) (next / 65536) % 1024;
	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (unsigned int) (next / 65536) % 1024;
	seed = next;
	return result;
}

/******************************************************************************/
/* Fonction qui renvoie un nombre aléatoire borné */

u32 random(u32 lower, u32 upper)
{
	return (rand() % (upper - lower + 1)) + lower;
}

/*******************************************************************************/
