
#ifndef _pixelH
#define _pixelH

//#############################################################################
//	File:		Pixel.h
//	Module:		CPSC 453
// 	Description: Implementation of a single pixel 
//               with RGB components in the range [0..255]	
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
using namespace std;
#endif

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif

//#############################################################################
//	Global definitions
//#############################################################################

// Color component data type
typedef unsigned char Component;

//#############################################################################
//	Global declarations
//#############################################################################

// Confines a component in the range [0..255]
inline Component clamp(int i)
{ return (i<0) ? 0 : (i>255) ? 255 : i; }


class Pixel
{
private:
	Component m_r, m_g, m_b;
public: 
	Pixel(){m_r = m_g = m_b = 0;}
	Pixel(Component rr, Component gg, Component bb){m_r = rr; m_g = gg; m_b = bb;}

	Component inq_r() { return m_r; }
	Component inq_g() { return m_g; }
	Component inq_b() { return m_b; }

	void set_r(Component r) { m_r = r; }
	void set_g(Component g) { m_g = g; }
	void set_b(Component b) { m_b = b; }

	void set_pixel(Component r, Component g, Component b){m_r = r; m_g = g; m_b = b;}

	Component luminance()
	{
		float value = floor( (m_r*0.3086) + (m_g*0.6094) + (m_b*0.0820) );
        Component lum = clamp( (int) value );
		return lum;
	}
	
	void interpolate(Pixel c, Pixel d, float t)
	{
		m_r = clamp( (int) floor((1.0 - t) * c.m_r + t * d.m_r + 0.5) );
		m_g = clamp( (int) floor((1.0 - t) * c.m_g + t * d.m_g + 0.5) );
		m_b = clamp( (int) floor((1.0 - t) * c.m_b + t * d.m_b + 0.5) );
	} 
};


//#############################################################################
//	End
//#############################################################################

#endif /* pixelH */
