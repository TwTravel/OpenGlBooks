#ifndef _IMAGE_H
#define _IMAGE_H

#include "jpeglib.h"



#define TGA_RGB		 2		
#define TGA_A		 3		
#define TGA_RLE		10		

struct tImage
{
	int channels;			
	int sizeX;				
	int sizeY;				
	unsigned char *data;	
};

/** 载入位图 */
tImage *LoadBMP(const char *strFileName);

/** 载入TGA文件 */
tImage *LoadTGA(const char *strFileName);

/** 载入JPG文件 */
tImage *LoadJPG(const char *strFileName);

/** 解码JPG文件 */
void DecodeJPG(jpeg_decompress_struct* cinfo, tImage *pImageData);


#endif
