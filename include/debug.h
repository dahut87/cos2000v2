/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"

#define opBufSz 32

#define decodeSIB()                 \
    base = r32[*b&0x07];            \
    indx = r32[(*b&0x38)>>3];       \
    scale = ((*b&0xc0)>>6)*2;       \
    if(scale && !indx){             \
        print("Invalid SIB byte.");  \
        return;                   \
    }

#define setG()                              \
    reg = (*b&0x38)>>3;                     \
    if(!Gsz){                               \
    } else if(Gsz == 1){                    \
        G = r8[reg];                        \
    } else if(Gsz == 2){                    \
        G = r16[reg];                       \
    } else if(Gsz == 3){                    \
        G = r32[reg];                       \
    } else if(Gsz == 4){                    \
        if(reg < 8){                        \
            G = rseg[reg];                  \
        } else{                             \
            print("Invalid Mod R/M byte.");  \
            return;                       \
        }                                   \
    } else{                                 \
        print("Invalid Mod R/M byte.");      \
        return;                           \
    }

