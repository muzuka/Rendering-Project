
#pragma once

//#############################################################################
//	File:		PixelMap.h
//	Module:		CPSC 453 - Assignment #3 
// 	Description: Simple class definition for a pixel map 
//               with support for 2D textures	
//
//#############################################################################

//#############################################################################
//	Headers
//#############################################################################

#ifdef _MSC_VER
#include <string>
#include <iostream>
#include <fstream>
#include <strstream>
using namespace std;
#else
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <strstream>
#include <cstdio>
#include <algorithm>
#include <math.h>
using namespace std;
#endif

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif
#include "Pixel.h"

//#############################################################################
//	Global definitions
//#############################################################################

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned long  ulong;

//#############################################################################
//	Global declarations
//#############################################################################

class PixelMap
{
private: 
	// dimensions of the pixel map
	int m_rows, m_cols; 

	// array of pixels
	Pixel* m_pixel; 

	// Table of color quantization
	Component *m_quantize;
	
public:

	// constructors
	PixelMap();
	// destructor
	~PixelMap();
	// read BMP file into this pixmap
	int readBMPFile(string fname); 

	//
	void makeCheckerBoard();
	//
	void setTexture(GLuint textureName);
}; 

//#############################################################################
//	End
//#############################################################################
