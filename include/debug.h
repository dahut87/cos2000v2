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


#define DBG_EXEC        0b00
#define DBG_WRITE       0b01
#define DBG_CLEAR       0b10
#define DBG_READWRITE   0b11

#define DBG_ONEBYTE     0b00
#define DBG_TWOBYTE     0b01
#define DBG_FOURBYTE    0b11


void setdebugreg(u8 number,u8 *address, u8 type);
u8* getdebugreg(u8 number);
u32 disasm(u8 *a, u8 *string, bool show);
