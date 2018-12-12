/*******************************************************************************/
/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
/*                                                                             */
#include "matrix.h"
#include "types.h"
#include "math.h"

/*******************************************************************************/
/* Affiche un vecteur de 4 composantes */

void vector4_show(vector4 src)
{
	printf("vecteur: X=%f Y=%f Z=%f W=%f \r\n", src.x, src.y, src.z,
	       src.w);
}

/*******************************************************************************/
/* Créé un vecteur de 4 composantes */

void vector4_create(float x, float y, float z, float w, vector4 * dst)
{
	dst->x = x;
	dst->y = y;
	dst->z = z;
	dst->w = w;
}

/*******************************************************************************/
/* Copie un vecteur de 4 composantes */

void vector4_copy(vector4 src, vector4 * dst)
{
	vector4_create(src.x, src.y, src.z, src.w, dst);
}

/*******************************************************************************/
/* Ajoute deux vecteurs de 4 composantes */

void vector4_add(vector4 v1, vector4 v2, vector4 * dst)
{
	dst->x = v1.x + v2.x;
	dst->y = v1.y + v2.y;
	dst->z = v1.z + v2.z;
}

/*******************************************************************************/
/* Soustrait un vecteur de 4 composantes depuis un autre*/

void vector4_sub(vector4 v1, vector4 v2, vector4 * dst)
{
	dst->x = v1.x - v2.x;
	dst->y = v1.y - v2.y;
	dst->z = v1.z - v2.z;
}

/*******************************************************************************/
/* Redimensionne un vecteur de 4 composantes */

void vector4_scale(vector4 * dst, float factor)
{
	dst->x *= factor;
	dst->y *= factor;
	dst->z *= factor;
	dst->w *= factor;
}

/*******************************************************************************/
/* Calcule le produit vectoriel de deux vecteurs de 4 composantes */

void vector4_crossproduct(vector4 v1, vector4 v2, vector4 * dst)
{
	dst->x = v1.y * v2.z - v1.z * v2.y;
	dst->y = v1.z * v2.x - v1.x * v2.z;
	dst->z = v1.x * v2.y - v1.y * v2.x;
}

/*******************************************************************************/
/* Normalise un vecteur de 4 composantes */

void vector4_normalize(vector4 * dst)
{
	float   len;
	float   norm;
	norm = vector4_norm(*dst);
	if (norm != 0)
	{
		len = 1 / norm;
		dst->x = dst->x * len;
		dst->y = dst->y * len;
		dst->z = dst->z * len;
		dst->w = 0;
	}
}

/*******************************************************************************/
/* Divise un vecteur de 4 composantes depuis un autre*/

void vector4_divide(vector4 * v1, vector4 v2, vector4 * dst)
{
	dst->x = v1->x / v2.x;
	dst->y = v1->y / v2.y;
	dst->z = v1->z / v2.z;
}

/*******************************************************************************/
/* Détermine le 3ème vecteur perpendiculaire au 2 autres */

void vector4_perpendicular(vector4 v1, vector4 v2, vector4 * dst)
{
	float   dot = vector4_dotproduct(v1, v2);
	dst->x = v1.x - dot * v2.x;
	dst->y = v1.y - dot * v2.y;
	dst->z = v1.z - dot * v2.z;
}

/*******************************************************************************/
/* Tourne un vecteur à 4 composantes autour de X */

void vector4_rotate_x(vector4 * dst, float angle)
{
	vector4 origin;
	float   sinus, cosinus;
	sinus = sinf(angle);
	cosinus = cosf(angle);
	origin.x = dst->x;
	origin.y = dst->y;
	origin.z = dst->z;
	dst->y = cosinus * origin.y + sinus * origin.z;
	dst->z = cosinus * origin.z - sinus * origin.y;
}

/*******************************************************************************/
/* Tourne un vecteur à 4 composantes autour de Y */

void vector4_rotate_y(vector4 * dst, float angle)
{
	vector4 origin;
	float   sinus, cosinus;
	sinus = sinf(angle);
	cosinus = cosf(angle);
	origin.x = dst->x;
	origin.y = dst->y;
	origin.z = dst->z;
	dst->x = cosinus * origin.x + sinus * origin.z;
	dst->z = cosinus * origin.z - sinus * origin.x;
}

/*******************************************************************************/
/* Tourne un vecteur à 4 composantes autour de Z */

void vector4_rotate_z(vector4 * dst, float angle)
{
	vector4 origin;
	float   sinus, cosinus;
	sinus = sinf(angle);
	cosinus = cosf(angle);
	origin.x = dst->x;
	origin.y = dst->y;
	origin.z = dst->z;
	dst->x = cosinus * origin.x + sinus * origin.y;
	dst->y = cosinus * origin.y - sinus * origin.x;
}

/*******************************************************************************/
/* Donne la longueur d'un vecteur à 4 composantes */

float vector4_len(vector4 src)
{
	return sqrtf((src.x * src.x) + (src.y * src.y) + (src.z * src.z));
}

/*******************************************************************************/
/* Retourne le produit scalaire de deux vecteurs à 4 composantes */

float vector4_dotproduct(vector4 v1, vector4 v2)
{
	return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

/*******************************************************************************/
/* Retourne la norme d'un vecteur à 4 composantes */

float vector4_norm(vector4 src)
{
	return sqrtf((src.x * src.x) + (src.y * src.y) + (src.z * src.z));
}

/*******************************************************************************/
/* Retourne la distance de deux vecteurs à 4 composantes */

float vector4_distance(vector4 v1, vector4 v2)
{
	return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) +
		    pow(v2.z - v1.z, 2));
}

/*******************************************************************************/
/* Compare deux vecteurs à 4 composantes */

int vector4_isequals(vector4 v1, vector4 v2)
{
	float   x, y, z;
	x = fabsf(v1.x - v2.x);
	y = fabsf(v1.y - v2.y);
	z = fabsf(v1.z - v2.z);
	return (x < 0.000001 && y < 0.000001 && z < 0.000001);
}

/*******************************************************************************/
/* Définie le plan normal à 3 vecteurs à 4 composantes */

void vector4_planenormal(vector4 v1, vector4 v2, vector4 v3, vector4 * dst)
{
	vector4 temp1, temp2;
	vector4_sub(v2, v1, &temp1);
	vector4_sub(v3, v1, &temp2);
	vector4_crossproduct(temp1, temp2, dst);
	vector4_normalize(dst);
}

/*******************************************************************************/
/* Créé une matrice d'identité */

void matrix44_homogen(matrix44 * matrix)
{
	vector4_create(1, 0, 0, 0, &matrix->V[0]);
	vector4_create(0, 1, 0, 0, &matrix->V[1]);
	vector4_create(0, 0, 1, 0, &matrix->V[2]);
	vector4_create(0, 0, 0, 1, &matrix->V[3]);
}

/*******************************************************************************/
/* Créé une matrice vide */

void matrix44_empty(matrix44 * matrix)
{
	vector4_create(0, 0, 0, 0, &matrix->V[0]);
	vector4_create(0, 0, 0, 0, &matrix->V[1]);
	vector4_create(0, 0, 0, 0, &matrix->V[2]);
	vector4_create(0, 0, 0, 0, &matrix->V[3]);
}

/*******************************************************************************/
/* Créé une matrice de redimensionnement (par un vecteur) */

void matrix44_scaling(vector4 v, matrix44 * dst)
{
	matrix44_homogen(dst);
	dst->V[0].x = v.x;
	dst->V[1].y = v.y;
	dst->V[2].z = v.z;
}

/*******************************************************************************/
/* Créé une matrice de déplacement */

void matrix44_translation(vector4 v, matrix44 * dst)
{
	matrix44_homogen(dst);
	dst->V[0].x = v.x;
	dst->V[1].y = v.y;
	dst->V[2].z = v.z;
}

/*******************************************************************************/
/* Créé une matrice de redimensionnement (par un facteur) */

void matrix44_scale(matrix44 * dst, float factor)
{
	vector4_scale(&dst->V[0], factor);
	vector4_scale(&dst->V[1], factor);
	vector4_scale(&dst->V[2], factor);
	vector4_scale(&dst->V[3], factor);
}

/*******************************************************************************/
/* Créé une matrice de redimensionnement et de déplacement */

void matrix44_scale_translation(vector4 scale, vector4 translation,
				matrix44 * dst)
{
	matrix44_homogen(dst);
	dst->V[0].x = scale.x;
	dst->V[1].y = scale.y;
	dst->V[2].z = scale.z;
	dst->V[3].x = translation.x;
	dst->V[3].y = translation.y;
	dst->V[3].z = translation.z;
}

/*******************************************************************************/
/* Créé une matrice de rotation autour de X */

void matrix44_rotation_x(float angle, matrix44 * dst)
{
	float   sinus, cosinus;
	cosinus = cosf(angle);
	sinus = sinf(angle);
	matrix44_empty(dst);
	dst->V[0].x = 1;
	dst->V[1].y = cosinus;
	dst->V[1].z = sinus;
	dst->V[2].y = -1 * sinus;
	dst->V[2].z = cosinus;
	dst->V[3].w = 1;
}

/*******************************************************************************/
/* Créé une matrice de rotation autour de Y */

void matrix44_rotation_y(float angle, matrix44 * dst)
{
	float   sinus, cosinus;
	cosinus = cosf(angle);
	sinus = sinf(angle);
	matrix44_empty(dst);
	dst->V[0].x = cosinus;
	dst->V[0].z = -1 * sinus;
	dst->V[1].y = 1;
	dst->V[2].x = sinus;
	dst->V[2].z = cosinus;
	dst->V[3].w = 1;
}

/*******************************************************************************/
/* Créé une matrice de rotation autour de Z */

void matrix44_rotation_z(float angle, matrix44 * dst)
{
	float   sinus, cosinus;
	cosinus = cosf(angle);
	sinus = sinf(angle);
	matrix44_empty(dst);
	dst->V[0].x = cosinus;
	dst->V[0].y = sinus;
	dst->V[1].x = -1 * sinus;
	dst->V[1].y = cosinus;
	dst->V[2].z = 1;
	dst->V[3].w = 1;
}

/*******************************************************************************/
/* Créé une matrice de rotation multiple */

void matrix44_rotation(vector4 axis, float angle, matrix44 * dst)
{
	float   cosinus, sinus, minuscos;
	sinus = sinf(angle);
	cosinus = cosf(angle);
	vector4_normalize(&axis);
	minuscos = 1 - cosinus;

	dst->V[0].x = (minuscos * axis.x * axis.x) + cosinus;
	dst->V[0].y = (minuscos * axis.x * axis.y) - (axis.z * sinus);
	dst->V[0].z = (minuscos * axis.z * axis.x) + (axis.y * sinus);
	dst->V[0].w = 0;

	dst->V[1].x = (minuscos * axis.x * axis.y) + (axis.z * sinus);
	dst->V[1].y = (minuscos * axis.y * axis.y) + cosinus;
	dst->V[1].z = (minuscos * axis.y * axis.z) - (axis.x * sinus);
	dst->V[1].w = 0;

	dst->V[2].x = (minuscos * axis.z * axis.x) - (axis.y * sinus);
	dst->V[2].y = (minuscos * axis.y * axis.z) + (axis.x * sinus);
	dst->V[2].z = (minuscos * axis.z * axis.z) + cosinus;
	dst->V[2].w = 0;

	dst->V[3].x = 0;
	dst->V[3].y = 0;
	dst->V[3].z = 0;
	dst->V[3].w = 1;
}

/*******************************************************************************/
/* Multiplie deux matrices */

void matrix44_multiply(matrix44 * m1, matrix44 * m2, matrix44 * dst)
{
	dst->V[0].x =
		(m1->V[0].x * m2->V[0].x + m1->V[0].y * m2->V[1].x +
		 m1->V[0].z * m2->V[2].x + m1->V[0].w * m2->V[3].x);
	dst->V[0].y =
		(m1->V[0].x * m2->V[0].y + m1->V[0].y * m2->V[1].y +
		 m1->V[0].z * m2->V[2].y + m1->V[0].w * m2->V[3].y);
	dst->V[0].z =
		(m1->V[0].x * m2->V[0].z + m1->V[0].y * m2->V[1].z +
		 m1->V[0].z * m2->V[2].z + m1->V[0].w * m2->V[3].z);
	dst->V[0].w =
		(m1->V[0].x * m2->V[0].w + m1->V[0].y * m2->V[1].w +
		 m1->V[0].z * m2->V[2].w + m1->V[0].w * m2->V[3].w);
	dst->V[1].x =
		(m1->V[1].x * m2->V[0].x + m1->V[1].y * m2->V[1].x +
		 m1->V[1].z * m2->V[2].x + m1->V[1].w * m2->V[3].x);
	dst->V[1].y =
		(m1->V[1].x * m2->V[0].y + m1->V[1].y * m2->V[1].y +
		 m1->V[1].z * m2->V[2].y + m1->V[1].w * m2->V[3].y);
	dst->V[1].z =
		(m1->V[1].x * m2->V[0].z + m1->V[1].y * m2->V[1].z +
		 m1->V[1].z * m2->V[2].z + m1->V[1].w * m2->V[3].z);
	dst->V[1].w =
		(m1->V[1].x * m2->V[0].w + m1->V[1].y * m2->V[1].w +
		 m1->V[1].z * m2->V[2].w + m1->V[1].w * m2->V[3].w);
	dst->V[2].x =
		(m1->V[2].x * m2->V[0].x + m1->V[2].y * m2->V[1].x +
		 m1->V[2].z * m2->V[2].x + m1->V[2].w * m2->V[3].x);
	dst->V[2].y =
		(m1->V[2].x * m2->V[0].y + m1->V[2].y * m2->V[1].y +
		 m1->V[2].z * m2->V[2].y + m1->V[2].w * m2->V[3].y);
	dst->V[2].z =
		(m1->V[2].x * m2->V[0].z + m1->V[2].y * m2->V[1].z +
		 m1->V[2].z * m2->V[2].z + m1->V[2].w * m2->V[3].z);
	dst->V[2].w =
		(m1->V[2].x * m2->V[0].w + m1->V[2].y * m2->V[1].w +
		 m1->V[2].z * m2->V[2].w + m1->V[2].w * m2->V[3].w);
	dst->V[3].x =
		(m1->V[3].x * m2->V[0].x + m1->V[3].y * m2->V[1].x +
		 m1->V[3].z * m2->V[2].x + m1->V[3].w * m2->V[3].x);
	dst->V[3].y =
		(m1->V[3].x * m2->V[0].y + m1->V[3].y * m2->V[1].y +
		 m1->V[3].z * m2->V[2].y + m1->V[3].w * m2->V[3].y);
	dst->V[3].z =
		(m1->V[3].x * m2->V[0].z + m1->V[3].y * m2->V[1].z +
		 m1->V[3].z * m2->V[2].z + m1->V[3].w * m2->V[3].z);
	dst->V[3].w =
		(m1->V[3].x * m2->V[0].w + m1->V[3].y * m2->V[1].w +
		 m1->V[3].z * m2->V[2].w + m1->V[3].w * m2->V[3].w);
}

/*******************************************************************************/
/* Transforme une matrice avec un vecteur à 4 composantes */

void matrix44_transform(matrix44 * matrix, vector4 * dst)
{
	vector4 origin;
	origin.x = dst->x;
	origin.y = dst->y;
	origin.z = dst->z;
	origin.w = dst->w;
	dst->x = origin.x * matrix->V[0].x + origin.y * matrix->V[1].x +
		origin.z * matrix->V[2].x + origin.w * matrix->V[3].x;
	dst->y = origin.x * matrix->V[0].y + origin.y * matrix->V[1].y +
		origin.z * matrix->V[2].y + origin.w * matrix->V[3].y;
	dst->z = origin.x * matrix->V[0].z + origin.y * matrix->V[1].z +
		origin.z * matrix->V[2].z + origin.w * matrix->V[3].z;
	dst->w = origin.x * matrix->V[0].w + origin.y * matrix->V[1].w +
		origin.z * matrix->V[2].w + origin.w * matrix->V[3].w;
}

/*******************************************************************************/
/* Calcule le déterminant d'une matrice */

float matrix44_determinant(matrix44 * matrix)
{
	float   a, b, c, d;
	a = matrix->V[0].x * todeterminant(matrix->V[1].y, matrix->V[2].y,
					   matrix->V[3].y, matrix->V[1].z,
					   matrix->V[2].z, matrix->V[3].z,
					   matrix->V[1].w, matrix->V[2].w,
					   matrix->V[3].w);
	b = matrix->V[0].y * todeterminant(matrix->V[1].x, matrix->V[2].x,
					   matrix->V[3].x, matrix->V[1].z,
					   matrix->V[2].z, matrix->V[3].z,
					   matrix->V[1].w, matrix->V[2].w,
					   matrix->V[3].w);
	c = matrix->V[0].z * todeterminant(matrix->V[1].x, matrix->V[2].x,
					   matrix->V[3].x, matrix->V[1].y,
					   matrix->V[2].y, matrix->V[3].y,
					   matrix->V[1].w, matrix->V[2].w,
					   matrix->V[3].w);
	d = matrix->V[0].w * todeterminant(matrix->V[1].x, matrix->V[2].x,
					   matrix->V[3].x, matrix->V[1].y,
					   matrix->V[2].y, matrix->V[3].y,
					   matrix->V[1].z, matrix->V[2].z,
					   matrix->V[3].z);
	return a - b + c - d;
}

float todeterminant(float a1, float a2, float a3, float b1, float b2,
		    float b3, float c1, float c2, float c3)
{
	return (a1 * ((b2 * c3) - (b3 * c2))) -
		(b1 * ((a2 * c3) - (a3 * c2))) +
		(c1 * ((a2 * b3) - (a3 * b2)));
}

/*******************************************************************************/
/* Crée une matrice adjointe */

void matrix44_adjoint(matrix44 * matrix)
{
	float   a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3,
		d4;
	a1 = matrix->V[0].x;
	b1 = matrix->V[0].y;
	c1 = matrix->V[0].z;
	d1 = matrix->V[0].w;

	a2 = matrix->V[1].x;
	b2 = matrix->V[1].y;
	c2 = matrix->V[1].z;
	d2 = matrix->V[1].w;

	a3 = matrix->V[2].x;
	b3 = matrix->V[2].y;
	c3 = matrix->V[2].z;
	d3 = matrix->V[2].w;

	a4 = matrix->V[3].x;
	b4 = matrix->V[3].y;
	c4 = matrix->V[3].z;
	d4 = matrix->V[3].w;

	matrix->V[0].x = todeterminant(b2, b3, b4, c2, c3, c4, d2, d3, d4);
	matrix->V[1].x =
		-todeterminant(a2, a3, a4, c2, c3, c4, d2, d3, d4);
	matrix->V[2].x = todeterminant(a2, a3, a4, b2, b3, b4, d2, d3, d4);
	matrix->V[3].x =
		-todeterminant(a2, a3, a4, b2, b3, b4, c2, c3, c4);

	matrix->V[0].y =
		-todeterminant(b1, b3, b4, c1, c3, c4, d1, d3, d4);
	matrix->V[1].y = todeterminant(a1, a3, a4, c1, c3, c4, d1, d3, d4);
	matrix->V[2].y =
		-todeterminant(a1, a3, a4, b1, b3, b4, d1, d3, d4);
	matrix->V[3].y = todeterminant(a1, a3, a4, b1, b3, b4, c1, c3, c4);

	matrix->V[0].z = todeterminant(b1, b2, b4, c1, c2, c4, d1, d2, d4);
	matrix->V[1].z =
		-todeterminant(a1, a2, a4, c1, c2, c4, d1, d2, d4);
	matrix->V[2].z = todeterminant(a1, a2, a4, b1, b2, b4, d1, d2, d4);
	matrix->V[3].z =
		-todeterminant(a1, a2, a4, b1, b2, b4, c1, c2, c4);

	matrix->V[0].w =
		-todeterminant(b1, b2, b3, c1, c2, c3, d1, d2, d3);
	matrix->V[1].w = todeterminant(a1, a2, a3, c1, c2, c3, d1, d2, d3);
	matrix->V[2].w =
		-todeterminant(a1, a2, a3, b1, b2, b3, d1, d2, d3);
	matrix->V[3].w = todeterminant(a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

/*******************************************************************************/
/* Affiche une matrice */

void matrix44_show(matrix44 * matrix)
{
	printf("Matrice: X=%f Y=%f Z=%f W=%f \r\n", matrix->V[0].x,
	       matrix->V[1].y, matrix->V[2].z, matrix->V[3].w);
	printf("         X=%f Y=%f Z=%f W=%f \r\n", matrix->V[0].x,
	       matrix->V[1].y, matrix->V[2].z, matrix->V[3].w);
	printf("         X=%f Y=%f Z=%f W=%f \r\n", matrix->V[0].x,
	       matrix->V[1].y, matrix->V[2].z, matrix->V[3].w);
}

/*******************************************************************************/
/* Inverse une matrice */

void matrix44_invert(matrix44 * matrix)
{
	float   det;
	det = matrix44_determinant(matrix);
	if (fabs(det) < EPSILON)
	{
		matrix44_homogen(matrix);
	}
	else
	{
		matrix44_adjoint(matrix);
		matrix44_scale(matrix, 1.0 / det);
	}
}

/*******************************************************************************/
/* Transpose une matrice */

void matrix44_transpose(matrix44 * matrix)
{
	float   f;
	f = matrix->V[0].y;
	matrix->V[0].y = matrix->V[1].x;
	matrix->V[1].x = f;

	f = matrix->V[0].z;
	matrix->V[0].z = matrix->V[2].x;
	matrix->V[2].x = f;

	f = matrix->V[0].w;
	matrix->V[0].w = matrix->V[3].x;
	matrix->V[3].x = f;

	f = matrix->V[1].z;
	matrix->V[1].z = matrix->V[2].y;
	matrix->V[2].y = f;

	f = matrix->V[1].w;
	matrix->V[1].w = matrix->V[3].y;
	matrix->V[3].y = f;

	f = matrix->V[2].w;
	matrix->V[2].w = matrix->V[3].z;
	matrix->V[3].z = f;
}

/*******************************************************************************/
/* Crée une matrice de camera */

void matrix44_lookat(vector4 eye, vector4 dst, vector4 up,
		     matrix44 * matrix)
{
	vector4 xaxis, yaxis, zaxis, negeye;
	vector4_sub(dst, eye, &zaxis);
	vector4_normalize(&zaxis);

	vector4_crossproduct(zaxis, up, &xaxis);
	vector4_normalize(&xaxis);

	vector4_crossproduct(xaxis, zaxis, &yaxis);

	vector4_copy(xaxis, &matrix->V[0]);
	vector4_copy(yaxis, &matrix->V[1]);
	vector4_copy(zaxis, &matrix->V[2]);

	matrix->V[2].x = -matrix->V[2].x;
	matrix->V[2].y = -matrix->V[2].y;
	matrix->V[2].z = -matrix->V[2].z;

	vector4_create(0, 0, 0, 1, &matrix->V[3]);

	matrix44_transpose(matrix);
	vector4_create(-eye.x, -eye.y, -eye.z, 1, &negeye);

	matrix44_transform(matrix, &negeye);
	vector4_copy(negeye, &matrix->V[3]);
}

/*******************************************************************************/
/* Vérifie que deux matrices sont égales */

int matrix44_isequals(matrix44 * m1, matrix44 * m2)
{
	return vector4_isequals(m1->V[0], m2->V[0])
		&& vector4_isequals(m1->V[1], m2->V[1])
		&& vector4_isequals(m1->V[2], m2->V[2])
		&& vector4_isequals(m1->V[3], m2->V[3]);
}

/*******************************************************************************/
/* Transforme une matrice en tableau */

float  *toarray(matrix44 * m)
{
	return &m->v;
}
