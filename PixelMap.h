
#pragma once

//#############################################################################
//	File:		PixelMap.h
//	Module:		CPSC 453 - Assignment #3 
// 	Description: Simple class definition for a pixel map 
//               with support for 2D textures	
//
//#############################################################################

#include "Pixel.h"

#include <string>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/freeglut.h>
#endif

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned long  ulong;

class PixelMap
{
private: 
	// dimensions of the pixel map
	int m_rows, m_cols; 

	// array of pixels
	Pixel* m_pixel; 

	// Table of color quantization
	Component *m_quantize;

	std::fstream inf;  // global in this file for convenience
	std::fstream outf; // ditto
	
public:

	// constructors
	PixelMap();
	// destructor
	~PixelMap();
	// read BMP file into this pixmap
	int readBMPFile(std::string fname); 

	ushort getShort();
	ulong getLong();

	//
	void makeCheckerBoard();
	//
	void setTexture(GLuint textureName);
}; 
