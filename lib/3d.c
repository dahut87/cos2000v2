/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "3d.h"
#include "types.h"
#include "video.h"
#include "string.h"

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

int load3ds(u8 *pointer,u32 size, model3d *model)
{
    u8 *ptr=pointer;
    u16 chunk_id;
    u32 chunk_size;
    u16 i;
    float *listfloat;
    u16 *listunsigned;
    dsState state=DS_READ_CHUNK_ID;
    bool dsfile=false;
    while(ptr-pointer<size) {
		switch(state)
		{
            case DS_READ_CHUNK_ID:
                chunk_id=*((u16*) ptr);
                ptr+=2;
                state=DS_READ_CHUNK_LENGTH;                   
			    break;
            case DS_READ_CHUNK_LENGTH:
                chunk_size=*((u32*) ptr);
                ptr+=4;
                switch(chunk_id)
                {
                    case MAIN3DS:
                        dsfile=true;
                        ptr+=10;
                        state=DS_READ_CHUNK_ID;                      
                        break;
                    case EDIT3DS:
                        state=DS_READ_CHUNK_ID;   
                        break;
                    case OBJ_TRIMESH:
                        state=DS_READ_CHUNK_ID;
                        break;
                    case EDIT_OBJECT:
                        state=DS_READ_OBJECT_NAME;   
                        break;
                    case TRI_VERTEXL:
                        state=DS_READ_POINT_COUNT;
                        break;
                    case TRI_FACEL1:
                        state=DS_READ_FACE_COUNT;
                        break;
                    case TRI_LOCAL:
                        state=DS_READ_MATRIX;
                        break;
                    default:
                        if (!dsfile) return 1;
                        ptr+=(chunk_size-6);
                        state=DS_READ_CHUNK_ID;  
                        break;
                }
			    break;
            case DS_READ_OBJECT_NAME:
                strcpy(ptr, model->name);
                ptr+=(strlen(ptr)+1);
                state=DS_READ_CHUNK_ID;                 
			    break;
            case DS_SKIP_CHUNK:
			    break;
            case DS_READ_POINT_COUNT:
                model->vertexnb=*((u16*) ptr);
                state=DS_READ_POINTS;  
                ptr+=2;
			    break;
            case DS_READ_POINTS:
                i=0;
                listfloat=ptr;
                model->vertexlist=0x00300000;
                while(i<model->vertexnb)
                {
                    model->vertexlist[i].x=*(listfloat++);
                    model->vertexlist[i].y=*(listfloat++);
                    model->vertexlist[i].z=*(listfloat++);
                    model->vertexlist[i++].w=1.0;               
                }
                ptr=listfloat;                
                state=DS_READ_CHUNK_ID;  
			    break;
            case DS_READ_FACE_COUNT:
                model->facenb=*((u16*) ptr);
                state=DS_READ_FACES;  
                ptr+=2;
			    break;
            case DS_READ_FACES:
                i=0;
                listunsigned=ptr;
                model->facelist=0x00400000;
                while(i<model->facenb*3)
                {
                    model->facelist[i++]=*(listunsigned++);
                    model->facelist[i++]=*(listunsigned++);
                    model->facelist[i++]=*(listunsigned++);
                    listunsigned++;
                }
                ptr=listunsigned;                
                state=DS_READ_CHUNK_ID;  
			    break;
            case DS_READ_MATRIX:
                i=0;
                listfloat=ptr;
                while(i<4)
                {
                    model->view.V[i].x=*(listfloat++);
                    model->view.V[i].y=*(listfloat++);
                    model->view.V[i].z=*(listfloat++);    
                    model->view.V[i++].w=0.0f;         
                }
                model->view.V[3].w=1.0f;
                ptr=listfloat;       
                state=DS_READ_CHUNK_ID;       
			    break;
            case DS_READ_DONE:
			    break;
                
        }
            
    }
}

