/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "3d.h"
#include "types.h"
#include "video.h"

/*******************************************************************************/
/* Crée une projection simple pour test */
void proj(vector4 list[], vertex2d plane[], vector4 origin[], u16 number, float factor)
{
    for (u32 i=0;i<number;i++)
    {   
        plane[i].x=(int)((list[i].x*factor)/(list[i].z+origin->z)+origin->x);
        plane[i].y=(int)((list[i].y*factor)/(list[i].z+origin->z)+origin->y);
    }   
}

/*******************************************************************************/
/* Crée une liste de vertex3D pour un cube */

void cube(vector4 list[], vector4 *origin, u16 size)
{
    list[0].x=origin->x;
    list[0].y=origin->y;
    list[0].z=origin->z;
    list[0].w=1.0f;
    list[1].x=origin->x+size;
    list[1].y=origin->y;
    list[1].z=origin->z;
    list[1].w=1.0f;
    list[2].x=origin->x;
    list[2].y=origin->y+size;
    list[2].z=origin->z;
    list[2].w=1.0f;
    list[3].x=origin->x+size;
    list[3].y=origin->y+size;
    list[3].z=origin->z;
    list[3].w=1.0f;
    list[4].x=origin->x;
    list[4].y=origin->y;
    list[4].z=origin->z+size;
    list[4].w=1.0f;
    list[5].x=origin->x+size;
    list[5].y=origin->y;
    list[5].z=origin->z+size;
    list[5].w=1.0f;
    list[6].x=origin->x;
    list[6].y=origin->y+size;
    list[6].z=origin->z+size;
    list[6].w=1.0f;
    list[7].x=origin->x+size;
    list[7].y=origin->y+size;
    list[7].z=origin->z+size;    
    list[7].w=1.0f;
}

/*******************************************************************************/
/* Charge un fichier 3DS */

void load3ds(u8 *pointer, model3d *model)
{
    u16 chunk;
    u32 size;
    bool formatok;
    while(true) {
        chunk=(u16) *(pointer);
        pointer+=2;
        size=(u32) *(pointer);
        pointer+=4;
        switch(chunk)
        {
            case MAIN3DS:
                break;
            case EDIT3DS:
                break;
            case EDIT_OBJECT:
                break;
            case OBJ_TRIMESH:
                break;
            case TRI_VERTEXL:
                break;
            case TRI_FACEL1:
                break;
            case TRI_LOCAL:
                break;
            default:
                break;
        }
    }
}

