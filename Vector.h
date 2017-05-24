#pragma once

// Vector class
// Provides basic vector math!
//
// Written by: Sean Brown

#include <stdio.h>
#include <math.h>

class Vector {
	public:
	// matrix operations
	
	// Algorithm from: http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
	// Taken from the glu implementation
	static bool inverse4x4(GLfloat m[16], GLfloat inverse[16]) {

    GLfloat inv[16], det;
    GLuint i;

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        inverse[i] = inv[i] * det;

    return true;

	}
	
	// multiplies 4x4 matrix
	static GLvoid multiply4Matrix_4Matrix(GLfloat mat1[16], GLfloat mat2[16], GLfloat (&product)[16]) {
		product[0] = (mat1[0]*mat2[0]) + (mat1[1]*mat2[4]) + (mat1[2]*mat2[8]) + (mat1[3]*mat2[12]);
		product[1] = (mat1[0]*mat2[1]) + (mat1[1]*mat2[5]) + (mat1[2]*mat2[9]) + (mat1[3]*mat2[13]);
		product[2] = (mat1[0]*mat2[2]) + (mat1[1]*mat2[6]) + (mat1[2]*mat2[10]) + (mat1[3]*mat2[14]);
		product[3] = (mat1[0]*mat2[3]) + (mat1[1]*mat2[7]) + (mat1[2]*mat2[11]) + (mat1[3]*mat2[15]);
		
		product[4] = (mat1[4]*mat2[0]) + (mat1[5]*mat2[4]) + (mat1[6]*mat2[8]) + (mat1[7]*mat2[12]);
		product[5] = (mat1[4]*mat2[1]) + (mat1[5]*mat2[5]) + (mat1[6]*mat2[9]) + (mat1[7]*mat2[13]);
		product[6] = (mat1[4]*mat2[2]) + (mat1[5]*mat2[6]) + (mat1[6]*mat2[10]) + (mat1[7]*mat2[14]);
		product[7] = (mat1[4]*mat2[3]) + (mat1[5]*mat2[7]) + (mat1[6]*mat2[11]) + (mat1[7]*mat2[15]);
		
		product[8] = (mat1[8]*mat2[0]) + (mat1[9]*mat2[4]) + (mat1[10]*mat2[8]) + (mat1[11]*mat2[12]);
		product[9] = (mat1[8]*mat2[1]) + (mat1[9]*mat2[5]) + (mat1[10]*mat2[9]) + (mat1[11]*mat2[13]);
		product[10] = (mat1[8]*mat2[2]) + (mat1[9]*mat2[6]) + (mat1[10]*mat2[10]) + (mat1[11]*mat2[14]);
		product[11] = (mat1[8]*mat2[3]) + (mat1[9]*mat2[7]) + (mat1[10]*mat2[11]) + (mat1[11]*mat2[15]);
		
		product[12] = (mat1[12]*mat2[0]) + (mat1[13]*mat2[4]) + (mat1[14]*mat2[8]) + (mat1[15]*mat2[12]);
		product[13] = (mat1[12]*mat2[1]) + (mat1[13]*mat2[5]) + (mat1[14]*mat2[9]) + (mat1[15]*mat2[13]);
		product[14] = (mat1[12]*mat2[2]) + (mat1[13]*mat2[6]) + (mat1[14]*mat2[10]) + (mat1[15]*mat2[14]);
		product[15] = (mat1[12]*mat2[3]) + (mat1[13]*mat2[7]) + (mat1[14]*mat2[11]) + (mat1[15]*mat2[15]);
	}
	
	// multiplies 3x3 matrix with a vector
	static GLvoid multiply3Matrix_3Vector(GLfloat mat[9], GLfloat vec[3], GLfloat (&product)[3]) {
		product[0] = (mat[0]*vec[0]) + (mat[1]*vec[1]) + (mat[2]*vec[2]);
		product[1] = (mat[3]*vec[0]) + (mat[4]*vec[1]) + (mat[5]*vec[2]);
		product[2] = (mat[6]*vec[0]) + (mat[7]*vec[1]) + (mat[8]*vec[2]);
	}
	
	// multiplies 4x4 matrix with a vector
	static GLvoid multiply4Matrix_4Vector(GLfloat mat[16], GLfloat vec[4], GLfloat (&product)[4]) {
		product[0] = (mat[0]*vec[0]) + (mat[1]*vec[1]) + (mat[2]*vec[2]) + (mat[3]*vec[3]);
		product[1] = (mat[4]*vec[0]) + (mat[5]*vec[1]) + (mat[6]*vec[2]) + (mat[7]*vec[3]);
		product[2] = (mat[8]*vec[0]) + (mat[9]*vec[1]) + (mat[10]*vec[2]) + (mat[11]*vec[3]);
		product[3] = (mat[12]*vec[0]) + (mat[13]*vec[1]) + (mat[14]*vec[2]) + (mat[15]*vec[3]);
	}
	
	// adds two vectors together
	static GLvoid add3(GLfloat vec1[3], GLfloat vec2[3], GLfloat (&sum)[3]) {
		sum[0] = vec1[0] + vec2[0];
		sum[1] = vec1[1] + vec2[1];
		sum[2] = vec1[2] + vec2[2];
	}
	
	// adds two vectors together
	static GLvoid add4(GLfloat vec1[4], GLfloat vec2[4], GLfloat (&sum)[4]) {
		sum[0] = vec1[0] + vec2[0];
		sum[1] = vec1[1] + vec2[1];
		sum[2] = vec1[2] + vec2[2];
		sum[3] = vec1[3] + vec2[3];
	}
	
	// subtracts vectors of length 3
	static GLvoid subtract3(GLfloat vec1[3], GLfloat vec2[3], GLfloat (&difference)[3]) {
		difference[0] = vec1[0] - vec2[0];
		difference[1] = vec1[1] - vec2[1];
		difference[2] = vec1[2] - vec2[2];
	}
	
	// subtracts vectors of length 4
	static GLvoid subtract4(GLfloat vec1[4], GLfloat vec2[4], GLfloat (&difference)[4]) {
		difference[0] = vec1[0] - vec2[0];
		difference[1] = vec1[1] - vec2[1];
		difference[2] = vec1[2] - vec2[2];
		difference[3] = vec1[3] - vec2[3];
	}
	
	static GLfloat dotProduct(GLfloat vec1[3], GLfloat vec2[3]) {
		return vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2];
	}
	
	static GLfloat length(GLfloat vec[3]) {
		return sqrt((vec[0]*vec[0]) + (vec[1]*vec[1]) + (vec[2]*vec[2]));
	}
	
	static GLvoid  normalize(GLfloat (&vec)[3]) {
		GLfloat len = length(vec);
		vec[0] = vec[0]/len;
		vec[1] = vec[1]/len;
		vec[2] = vec[2]/len;
	}
	static GLvoid crossProduct(GLfloat vert1[3], GLfloat vert2[3], GLfloat (&cross)[3]) {
		cross[0] = vert1[1]*vert2[2] - vert1[2]*vert2[1];
    cross[1] = vert1[2]*vert2[0] - vert1[0]*vert2[2];
    cross[2] = vert1[0]*vert2[1] - vert1[1]*vert2[0];
	} 


};
