/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */

#ifndef MATRIX
# define MATRIX

typedef struct vector4{
  union {
    struct {
        float x;	
	    float y;
	    float z;
	    float w;
   };
    float v[4];
  };
} vector4 __attribute__ ((packed));

typedef struct matrix44{
  union {
    struct {
        vector4 V[4];
   };
    float v[16];
  };
} matrix44 __attribute__ ((packed));

void vector4_show(vector4 src);
void vector4_create(float x, float y, float z, float w, vector4 *dst);
void vector4_copy(vector4 src, vector4 *dst);
void vector4_add(vector4 v1, vector4 v2, vector4 *dst);
void vector4_sub(vector4 v1, vector4 v2, vector4 *dst);
void vector4_scale(vector4 *dst, float factor);
void vector4_crossproduct(vector4 v1, vector4 v2, vector4 *dst);
void vector4_normalize(vector4 *dst);
void vector4_divide(vector4 *v1, vector4 v2, vector4 *dst);
void vector4_perpendicular(vector4 v1, vector4 v2, vector4 *dst);
void vector4_rotate_x(vector4 *dst, float angle);
void vector4_rotate_y(vector4 *dst, float angle);
void vector4_rotate_z(vector4 *dst, float angle);
float vector4_len(vector4 src);
float vector4_dotproduct(vector4 v1, vector4 v2);
float vector4_norm(vector4 src);
float vector4_distance(vector4 v1, vector4 v2);
int vector4_isequals(vector4 v1, vector4 v2);
void vector4_planenormal(vector4 v1, vector4 v2, vector4 v3, vector4 *dst);

void matrix44_homogen(matrix44 *matrix);
void matrix44_empty(matrix44 *matrix);
void matrix44_scaling(vector4 v, matrix44 *dst);
void matrix44_translation(vector4 v, matrix44 *dst);
void matrix44_scale(matrix44 *dst, float factor);
void matrix44_scale_translation(vector4 scale, vector4 translation, matrix44 *dst);
void matrix44_rotation_x(float angle, matrix44 *dst);
void matrix44_rotation_y(float angle, matrix44 *dst);
void matrix44_rotation_z(float angle, matrix44 *dst);
void matrix44_rotation(vector4 axis, float angle, matrix44 *dst);
void matrix44_multiply(matrix44 *m1, matrix44 *m2, matrix44 *dst);
void matrix44_transform(matrix44 *matrix, vector4 *dst);
float matrix44_determinant(matrix44 *matrix);
float todeterminant(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3);
void matrix44_adjoint(matrix44 *matrix);
void matrix44_show(matrix44 *matrix);
void matrix44_invert(matrix44 *matrix);
void matrix44_transpose(matrix44 *matrix);
void matrix44_lookat(vector4 eye, vector4 dst, vector4 up, matrix44 *matrix);
int matrix44_isequals(matrix44 *m1, matrix44 *m2);
float *toarray(matrix44 *m);

#endif





