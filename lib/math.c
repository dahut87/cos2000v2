#include "types.h"
#include "timer.h"

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
