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

void cube(model3d *model, vector4 *origin, u16 size)
{
    strcpy("cube",model->name);
    model->vertexnb=8;
    model->vertexlist=0x00300000;
    model->vertexlist[0].x=origin->x;
    model->vertexlist[0].y=origin->y;
    model->vertexlist[0].z=origin->z;
    model->vertexlist[0].w=1.0f;
    model->vertexlist[1].x=origin->x+size;
    model->vertexlist[1].y=origin->y;
    model->vertexlist[1].z=origin->z;
    model->vertexlist[1].w=1.0f;
    model->vertexlist[2].x=origin->x;
    model->vertexlist[2].y=origin->y+size;
    model->vertexlist[2].z=origin->z;
    model->vertexlist[2].w=1.0f;
    model->vertexlist[3].x=origin->x+size;
    model->vertexlist[3].y=origin->y+size;
    model->vertexlist[3].z=origin->z;
    model->vertexlist[3].w=1.0f;
    model->vertexlist[4].x=origin->x;
    model->vertexlist[4].y=origin->y;
    model->vertexlist[4].z=origin->z+size;
    model->vertexlist[4].w=1.0f;
    model->vertexlist[5].x=origin->x+size;
    model->vertexlist[5].y=origin->y;
    model->vertexlist[5].z=origin->z+size;
    model->vertexlist[5].w=1.0f;
    model->vertexlist[6].x=origin->x;
    model->vertexlist[6].y=origin->y+size;
    model->vertexlist[6].z=origin->z+size;
    model->vertexlist[6].w=1.0f;
    model->vertexlist[7].x=origin->x+size;
    model->vertexlist[7].y=origin->y+size;
    model->vertexlist[7].z=origin->z+size;    
    model->vertexlist[7].w=1.0f;
    model->facelist=0x00310000;
    model->facelist[0].V1=0;
    model->facelist[0].V2=1;
    model->facelist[0].V3=3;
    model->facelist[1].V1=0;
    model->facelist[1].V2=2;
    model->facelist[1].V3=3;
    model->facelist[2].V1=4;
    model->facelist[2].V2=5;
    model->facelist[2].V3=7;
    model->facelist[3].V1=4;
    model->facelist[3].V2=6;
    model->facelist[3].V3=7;
    model->facelist[4].V1=0;
    model->facelist[4].V2=1;
    model->facelist[4].V3=5;
    model->facelist[5].V1=0;
    model->facelist[5].V2=1;
    model->facelist[5].V3=4;
    model->facelist[6].V1=0;
    model->facelist[6].V2=0;
    model->facelist[6].V3=0;
    model->facelist[7].V1=0;
    model->facelist[7].V2=0;
    model->facelist[7].V3=0;
    model->facelist[8].V1=0;
    model->facelist[8].V2=0;
    model->facelist[8].V3=0;
    model->facelist[9].V1=0;
    model->facelist[9].V2=0;
    model->facelist[9].V3=0;
    model->facelist[10].V1=0;
    model->facelist[10].V2=0;
    model->facelist[10].V3=0;
    model->facelist[11].V1=0;
    model->facelist[11].V2=0;
    model->facelist[11].V3=0;
}

/*******************************************************************************/
/* Affiche un modèle 3D */

void show3dmodel(model3d *model, matrix44 *transformation, vector4 origin[], float factor, type3D type)
{
    u16 i;
    vertex2d *plane=0x00250000;
    for (i = 0; i < model->vertexnb; i++) 
    {
        matrix44_transform(transformation, &model->vertexlist[i]);
    }
    proj(model->vertexlist, plane, origin, model->vertexnb, factor);
    switch (type) {
	    case TYPE3D_POINTS:
            for(i=0;i<model->vertexnb;i++) {
                v_writepxl(&plane[i], egatorgb(4));
            }
            break;
	    case TYPE3D_LINES:
            for(i=0;i<model->facenb;i++) {
                v_line(&plane[model->facelist[i].V1], &plane[model->facelist[i].V2], egatorgb(4));
                v_line(&plane[model->facelist[i].V1], &plane[model->facelist[i].V3], egatorgb(4));
                v_line(&plane[model->facelist[i].V2], &plane[model->facelist[i].V3], egatorgb(4));
            }
            break;
	    case TYPE3D_FACES:
            break;
	    case TYPE3D_FLAT:
            break;
	    case TYPE3D_TEXTURE:
            break;
    }
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
                while(i<model->facenb)
                {
                    model->facelist[i].V1=*(listunsigned++);
                    model->facelist[i].V2=*(listunsigned++);
                    model->facelist[i++].V3=*(listunsigned++);
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

