#include "types.h"
#include "vga.h"
#include "video.h"
#include "graph.h"
#include "math.h"                                        

/******************************************************************************/

/* Affiche une ligne entre les points spécifiés */

void line(u32 x1, u32 y1, u32 x2, u32 y2, u8 color)
{
  s32 dx,dy,sdx,sdy;
  u32 i,dxabs,dyabs,x,y,px,py;

  dx=x2-x1;      /*  distance horizontale de la line */
  dy=y2-y1;      /* distance verticale de la line **/
  dxabs=abs(dx);
  dyabs=abs(dy);
  sdx=sgn(dx);
  sdy=sgn(dy);
  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=y1;
  
  writepxl(px,py,color);

  if (dxabs>=dyabs) /* la ligne est plus horizontale que verticale */
  {
    for(i=0;i<dxabs;i++)
    {
      y+=dyabs;
      if (y>=dxabs)
      {
        y-=dxabs;
        py+=sdy;
      }
      px+=sdx;
      writepxl(px,py,color);
    }
  }
  else /* la ligne est plus verticale que horizontale */
  {
    for(i=0;i<dyabs;i++)
    {
      x+=dxabs;
      if (x>=dyabs)
      {
        x-=dyabs;
        px+=sdx;
      }
      py+=sdy;
      writepxl(px,py,color);
    }
  }
}

