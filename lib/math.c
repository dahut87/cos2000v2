#include "types.h"
#include "timer.h"

/******************************************************************************/ 
    
/* Fonction qui retourne le logarithme 2 */ 

u8 log2(u32 n) {
     if (n==0) return 0;
     int logValue = -1;
     while (n) {
         logValue++;
         n >>= 1;
     }
     return logValue+1;
 }

/******************************************************************************/ 
    
/* Fonction qui retourne le logarithme 10 */ 

u8 log10(u32 n)
{
    return (n >= 1000000000u) ? 9 : (n >= 100000000u) ? 8 : 
        (n >= 10000000u) ? 7 : (n >= 1000000u) ? 6 : 
        (n >= 100000u) ? 5 : (n >= 10000u) ? 4 :
        (n >= 1000u) ? 3 : (n >= 100u) ? 2 : (n >= 10u) ? 1u : 0u; 
}


/******************************************************************************/ 
    
/* Fonction qui retourne la valeur absolue */ 
    
u32 abs(int x) {
    if (x < 0)
		x = -x;	
    return (u32) x;
}

/******************************************************************************/ 
    
/* Fonction qui initialise le générateur de nombre aléatoire */

static u32 seed=0x12341234;

void randomize() {
   seed=gettimer();
} 

/******************************************************************************/ 
    
/* Fonction qui renvoie un nombre aléatoire */

u32 rand()
{
  u32 next = seed;
  int result;

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

u32 random(u32 lower, u32 upper) {
    return (rand() % (upper - lower + 1)) + lower;
}
