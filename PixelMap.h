
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
