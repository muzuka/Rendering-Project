#pragma once

/* Point class header file
 * 
 * Represents a point
 * 
 * Written by: Sean Brown
 */

#include <math.h>

// Basically just a container
class Point {
	private:
		float x, y, z;
		float size;
		float distance;
		
	public:
		Point(float x, float y, float z, float size) {
			this->x = x;
			this->y = y;
			this->z = z;
			this->size = size;
			this->distance = 0.0;
		}
			
		float getX()			{return x;}
		float getY()			{return y;}
		float getZ()			{return z;}
		float getSize()			{return size;}
		float getDistance() 	{return distance;}
	
		void setDistance(float distance) {this->distance = distance;}
		
		Point operator*(float a) {
			return Point(x*a, y*a, z*a, size);
		}
		
		float length() {
			return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		}
		
};
