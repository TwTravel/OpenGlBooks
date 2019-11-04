#include "Graph.h"
#include "fstream.h"
#include "windows.h"
#include "ijl.h"							 
#include <stdlib.h>

float random(float center, float range, int doublesided)
{
	return (float(rand()%10000)/10000.0)*range+center-(range/2.0)*doublesided;
}

/* 获得位图数据*/
unsigned char* GetBitmap(char bmpfile[],int &sizex,int &sizey)
{

	BITMAPINFO header;
	long height,width;
	fstream file;
	file.open(bmpfile,ios::app|ios::in|ios::binary|ios::nocreate);
	if(!file)
		{
			sizex=0;
			sizey=0;
			return NULL;
		}

	file.seekg(0xE);
	file.read((char*)&header,sizeof(header));
	sizex=width=header.bmiHeader.biWidth;
	sizey=height=header.bmiHeader.biHeight;

	file.seekg(0x36);

	unsigned char* buffer=new unsigned char[width*height*3];
	file.read((char*)buffer,height*width*3);
	file.close();
	return buffer;
}

/** 获得Jpeg数据 */
unsigned char* GetJPEG(char bmpfile[],int &sizex,int &sizey)
{

   JPEG_CORE_PROPERTIES		image;
	unsigned char			*imageBits = NULL,	 // 交换前	
							*imageBits2 = NULL;	 // 交换后	
	int						imageSize = 0,
							imageWidth = 0,
							imageHeight = 0;


   ZeroMemory( &image, sizeof( JPEG_CORE_PROPERTIES ) );

   if( ijlInit( &image ) != IJL_OK ) {

		return 0;
	}
   
	image.JPGFile = const_cast<char*>(bmpfile);

   if( ijlRead( &image, IJL_JFILE_READPARAMS ) != IJL_OK )  {
	
		return 0;
   }

	switch(image.JPGChannels) {

		case 1:
		{
			image.JPGColor = IJL_G;
			break;
		}

		case 3:
		{
          image.JPGColor = IJL_YCBCR;
          break;
		}

      default:
		{

			image.DIBColor = (IJL_COLOR)IJL_OTHER;
			image.JPGColor = (IJL_COLOR)IJL_OTHER;
			break;
		}
	}

   image.DIBWidth    = image.JPGWidth;
   image.DIBHeight   = image.JPGHeight;
   image.DIBChannels = 3;
   image.DIBPadBytes = IJL_DIB_PAD_BYTES(image.DIBWidth,image.DIBChannels);

   imageSize = (image.DIBWidth * image.DIBChannels + image.DIBPadBytes) * image.DIBHeight;

   // 分配内存
   imageBits = new byte[ imageSize ];
	imageBits2 = new byte[ imageSize ];

   if( (imageBits == NULL) || (imageBits2 == NULL) ) {
            
		return 0;
   }
        
   image.DIBBytes = imageBits;

   // 读取图像数据
   if( ijlRead( &image, IJL_JFILE_READWHOLEIMAGE ) != IJL_OK ) {

		delete [] imageBits;
		delete [] imageBits2;
   }

   if( ijlFree( &image ) != IJL_OK ) {
				
		return 0;			
   }

	imageWidth	= image.DIBWidth;
	imageHeight = image.DIBHeight;


    // 交换数据顺序
	for (int j = 0;j < imageHeight; j++) {

		for (int i = 0;i < imageWidth; i++) {

			imageBits2[(j * imageWidth+i)* 3] = imageBits[((imageHeight-1-j) * imageWidth+i)* 3+2];

			imageBits2[(j * imageWidth+i)* 3+1] = imageBits[((imageHeight-1-j) * imageWidth+i)* 3+1];

			imageBits2[(j * imageWidth+i)* 3+2] = imageBits[((imageHeight-1-j) * imageWidth+i)* 3];

		}
	}


	// 释放空间
	delete [] imageBits;
//	delete [] imageBits2;

	sizex=imageWidth;
	sizey=imageHeight;
   return imageBits2;
}