#include "types.h"

/******************************************************************************/ 
    
/* Fonction qui retourne la valeur absolue */ 
    
u32 abs(int x) {
    if (x < 0)
		x = -x;	
    return (u32) x;
}

