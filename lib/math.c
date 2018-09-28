/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "timer.h"

/*******************************************************************************/
/* Arithmétique 64 bits  */

unsigned long long __udivdi3 (unsigned long long num, unsigned long long den)
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

unsigned long long __umoddi3 (unsigned long long n, unsigned long long d) 
{
  return n - d * __udivdi3 (n, d);
}

/******************************************************************************/ 
/* Fonction qui retourne le logarithme 2 */ 
    
u8 log2(u64 n)
{
	
if (n == 0)
		return 0;
	
int logValue = -1;
	
while (n) {
		
logValue++;
		
n >>= 1;
	
}
	
return logValue + 1;

}
 
/******************************************************************************/ 
/* Fonction qui retourne le logarithme 10 */ 
    
u8 log10(u64 n) 
{
	
return  (n >= 10000000000000000000u) ? 19 : (n >= 100000000000000000u) ? 18 :
        (n >= 100000000000000000u) ? 17 : (n >= 10000000000000000u) ? 16 :
        (n >= 1000000000000000u) ? 15 : (n >= 100000000000000u) ? 14 :
        (n >= 10000000000000u) ? 13 : (n >= 1000000000000u) ? 12 :
        (n >= 100000000000u) ? 11 : (n >= 10000000000u) ? 10 :
        (n >= 1000000000u) ? 9 : (n >= 100000000u) ? 8 : 
	    (n >= 10000000u) ? 7 : (n >= 1000000u) ? 6 : 
	    (n >= 100000u) ? 5 : (n >= 10000u) ? 4 : 
	    (n >= 1000u) ? 3 : (n >= 100u) ? 2 : (n >= 10u) ? 1u : 0u;

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
u32 next = seed;
int result;
next *= 1103515245;
next += 12345;
result = (unsigned int)(next / 65536) % 2048;
next *= 1103515245;
next += 12345;
result <<= 10;
result ^= (unsigned int)(next / 65536) % 1024;
next *= 1103515245;
next += 12345;
result <<= 10;
result ^= (unsigned int)(next / 65536) % 1024;
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
