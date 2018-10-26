/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "types.h"
#include "matrix.h"
#include "video.h"

typedef struct vertex3d{
  union {
    struct {
        float x;	
	    float y;
	    float z;
   };
    float v[3];
  };
} vertex3d __attribute__ ((packed));

void proj(vector4 list[], vertex2d plane[], vector4 origin[], u16 number, float factor);
void cube(vector4 list[], vector4 *origin, u16 size);
