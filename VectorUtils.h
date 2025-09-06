// Vector class
// Provides basic vector math!
//
// Written by: Sean Brown

#include <stdio.h>
#include <math.h>

class VectorUtils {
public:
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
