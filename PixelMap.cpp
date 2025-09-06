//#############################################################################
//	File:		PixelMap.cpp
//	Module:		
// 	Description:	
//
//#############################################################################

#include "PixelMap.h"

PixelMap::PixelMap() { m_rows = m_cols = 0; m_pixel = 0; m_quantize = 0; }

PixelMap::~PixelMap()
{
	delete []m_pixel;
	m_pixel = 0;
	m_rows = m_cols = 0;
	delete []m_quantize;
	m_quantize = 0;
}

//#############################################################################
//	Read BMP file into this pixelmap
//#############################################################################

ushort PixelMap::getShort()
{// read a short in little-endian form
	char ic;
	ushort ip;
	inf.get(ic); ip = ic;  //first byte is little one 
	inf.get(ic);  ip |= ((ushort)ic << 8); // or in high order byte
	return ip;
}
ulong PixelMap::getLong()
{  // get little-endian 4-byte value from file, compose along portably
	ulong ip = 0;
	char ic = 0;
	unsigned char uc = ic;
	inf.get(ic); uc = ic; ip = uc;
	inf.get(ic); uc = ic; ip |=((ulong)uc << 8);
	inf.get(ic); uc = ic; ip |=((ulong)uc << 16);
	inf.get(ic); uc = ic; ip |=((ulong)uc << 24);
	return ip;
}

int PixelMap:: readBMPFile(std::string fname) 
{  // Read into memory an Pixel image from an uncompressed BMP file.
	// return 0 on failure, 1 on success
	inf.open(fname.c_str(), std::ios::in| std::ios::binary); //must read raw binary char's.
	if(!inf){ std::cout << " can't open file: " << fname << std::endl; return 0;}
	int k, row, col, numPadBytes, nBytesInRow;
	// read header information
	char ch1, ch2;
	inf.get(ch1); inf.get(ch2); // type is always 'BM'
	//cout << "file type = " << ch1 << ch2 << endl;
	ulong fileSize = getLong();
	ushort reserved1 =  getShort();     // always 0
	ushort reserved2= 	getShort();     // always 0 
	ulong offBits =		getLong();	    // offset to image - unreliable
	ulong headerSize =   getLong();      // always 40
	ulong numCols =		getLong();	    // number of columns in image
	ulong numRows = 		getLong();	    // number of rows in image
	ushort planes=    	getShort();     // always 1 
	ushort bitsPerPix = getShort();    // 8 or 24;only 24 bit case done 
	ulong compression = getLong();     // must be 0 for umcompressed 
	ulong imageSize = 	getLong();      // total bytes in image 
	ulong xPels =    	getLong();      // always 0 
	ulong yPels =    	getLong();      // always 0 
	ulong numLUTentries = getLong();   // 256 for 8 bit, otherwise 0 
	ulong impColors = 	getLong();      // always 0 

	if(bitsPerPix != 24) { std::cout << "not a 24 bit/pixelimage!\n"; inf.close(); return 0;}; // error!
	// in BMP file, pad bytes inserted at end of each row so total number is a mult. of 4
	nBytesInRow = ((3 * numCols + 3)/4) * 4; // round up 3*numCols to next mult. of 4
	numPadBytes = nBytesInRow - 3 * numCols; // need this many
	m_rows = numRows; // set class's data members
    m_cols = numCols;
	std::cout << "numRows,numCols = " << numRows << "," << numCols << std::endl;
	std::cout.flush();
	m_pixel = new Pixel[m_rows * m_cols]; //space for array in memory
	if(!m_pixel) return 0; // out of memory!
	long count = 0;
	char dum,r,g,b;
	
	for(row = 0; row < m_rows; row++) // read pixel values
	{
		for(col = 0; col < m_cols; col++)
		{
			inf.get(b); inf.get(g); inf.get(r); // funny color order in BMP file
			m_pixel[count].set_r(r); m_pixel[count].set_g(g); m_pixel[count++].set_b(b);
		}
      for(k = 0; k < numPadBytes ; k++) //skip over padding bytes at row's end
			inf >> dum;
	}
	inf.close();
	return 1; // success
}

void PixelMap::makeCheckerBoard()
{
	m_rows = m_cols = 64;
	m_pixel = new Pixel[3*m_rows*m_cols];  
	long count = 0;
	for (int i = 0; i< m_rows; i++)
		for (int j = 0; j < m_cols; j++)
		{
			int c = (((i/8) + (j/8)) %2) * 255;
			m_pixel[count].set_r(c);
			m_pixel[count].set_g(c);
			m_pixel[count++].set_b(0);
		}
}

void PixelMap::setTexture(GLuint textureName)
{
	glBindTexture(GL_TEXTURE_2D, textureName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 
		         0, GL_RGB, m_cols, m_rows, 
				 0, GL_RGB, GL_UNSIGNED_BYTE, m_pixel);
}
