
#include "main.h"
#include "image.h"


/** 载入位图文件 */
tImage *LoadBMP(const char *strFileName)
{
	AUX_RGBImageRec *pBitmap = NULL;
	FILE *pFile = NULL;

	if((pFile = fopen(strFileName, "rb")) == NULL) 
	{
		MessageBox(g_hWnd, "Unable to load BMP File!", "Error", MB_OK);
		return NULL;
	}

	pBitmap = auxDIBImageLoad(strFileName);				

	tImage *pImage = (tImage *)malloc(sizeof(tImage));

	pImage->channels = 3;
	pImage->sizeX = pBitmap->sizeX;
	pImage->sizeY = pBitmap->sizeY;
	pImage->data  = pBitmap->data;

	free(pBitmap);

	return pImage;
}

/** 载入TGA文件 */
tImage *LoadTGA(const char *strFileName)
{
	tImage *pImageData = NULL;			
	WORD width = 0, height = 0;			
	byte length = 0;					
	byte imageType = 0;					
	byte bits = 0;						
	FILE *pFile = NULL;					
	int channels = 0;					
	int stride = 0;						
	int i = 0;							

	if((pFile = fopen(strFileName, "rb")) == NULL) 
	{
		MessageBox(g_hWnd, "Unable to load TGA File!", "Error", MB_OK);
		return NULL;
	}
		
	pImageData = (tImage*)malloc(sizeof(tImage));

	fread(&length, sizeof(byte), 1, pFile);
	
	fseek(pFile,1,SEEK_CUR); 

	fread(&imageType, sizeof(byte), 1, pFile);
	
	fseek(pFile, 9, SEEK_CUR); 

	fread(&width,  sizeof(WORD), 1, pFile);
	fread(&height, sizeof(WORD), 1, pFile);
	fread(&bits,   sizeof(byte), 1, pFile);
	
	fseek(pFile, length + 1, SEEK_CUR); 

	if(imageType != TGA_RLE)
	{
		if(bits == 24 || bits == 32)
		{
			channels = bits / 8;
			stride = channels * width;
			pImageData->data = ((unsigned char*)malloc(sizeof(unsigned char)*stride*height));

			for(int y = 0; y < height; y++)
			{
				unsigned char *pLine = &(pImageData->data[stride * y]);

				fread(pLine, stride, 1, pFile);

				for(i = 0; i < stride; i += channels)
				{
					int temp     = pLine[i];
					pLine[i]     = pLine[i + 2];
					pLine[i + 2] = temp;
				}
			}
		}
		else if(bits == 16)
		{
			unsigned short pixels = 0;
			int r=0, g=0, b=0;

			channels = 3;
			stride = channels * width;
			pImageData->data = ((unsigned char*)malloc(sizeof(unsigned char)*stride*height));

			for(int i = 0; i < width*height; i++)
			{
				fread(&pixels, sizeof(unsigned short), 1, pFile);
				
				b = (pixels & 0x1f) << 3;
				g = ((pixels >> 5) & 0x1f) << 3;
				r = ((pixels >> 10) & 0x1f) << 3;
				
				pImageData->data[i * 3 + 0] = r;
				pImageData->data[i * 3 + 1] = g;
				pImageData->data[i * 3 + 2] = b;
			}
		}	
		else
			return NULL;
	}
	else
	{
		byte rleID = 0;
		int colorsRead = 0;
		channels = bits / 8;
		stride = channels * width;

		pImageData->data = ((unsigned char*)malloc(sizeof(unsigned char)*stride*height));
		byte *pColors = ((byte*)malloc(sizeof(byte)*channels));

		while(i < width*height)
		{
			fread(&rleID, sizeof(byte), 1, pFile);
			
			if(rleID < 128)
			{
				rleID++;

				while(rleID)
				{
					fread(pColors, sizeof(byte) * channels, 1, pFile);

					pImageData->data[colorsRead + 0] = pColors[2];
					pImageData->data[colorsRead + 1] = pColors[1];
					pImageData->data[colorsRead + 2] = pColors[0];

					if(bits == 32)
						pImageData->data[colorsRead + 3] = pColors[3];

					i++;
					rleID--;
					colorsRead += channels;
				}
			}
			else
			{
				rleID -= 127;

				fread(pColors, sizeof(byte) * channels, 1, pFile);

				while(rleID)
				{
					pImageData->data[colorsRead + 0] = pColors[2];
					pImageData->data[colorsRead + 1] = pColors[1];
					pImageData->data[colorsRead + 2] = pColors[0];

					if(bits == 32)
						pImageData->data[colorsRead + 3] = pColors[3];

					i++;
					rleID--;
					colorsRead += channels;
				}
				
			}
				
		}
	}

	fclose(pFile);

	pImageData->channels = channels;
	pImageData->sizeX    = width;
	pImageData->sizeY    = height;

	return pImageData;
}



/** 解码JPG */
void DecodeJPG(jpeg_decompress_struct* cinfo, tImage *pImageData)
{
	jpeg_read_header(cinfo, TRUE);
	
	jpeg_start_decompress(cinfo);

	pImageData->channels = cinfo->num_components;
	pImageData->sizeX    = cinfo->image_width;
	pImageData->sizeY    = cinfo->image_height;

	int rowSpan = cinfo->image_width * cinfo->num_components;
	
	pImageData->data = ((unsigned char*)malloc(sizeof(unsigned char)*rowSpan*pImageData->sizeY));
			
	unsigned char** rowPtr = new unsigned char*[pImageData->sizeY];

	for (int i = 0; i < pImageData->sizeY; i++)
		rowPtr[i] = &(pImageData->data[i * rowSpan]);

	int rowsRead = 0;
	while (cinfo->output_scanline < cinfo->output_height) 
	{
		rowsRead += jpeg_read_scanlines(cinfo, 
										&rowPtr[rowsRead], cinfo->output_height - rowsRead);
	}
	
	delete [] rowPtr;

	jpeg_finish_decompress(cinfo);
}


/** 载入JPG文件 */
tImage *LoadJPG(const char *strFileName)
{
	struct jpeg_decompress_struct cinfo;
	tImage *pImageData = NULL;
	FILE *pFile;
		
	if((pFile = fopen(strFileName, "rb")) == NULL) 
	{
		MessageBox(g_hWnd, "Unable to load JPG File!", "Error", MB_OK);
		return NULL;
	}
	
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	
	jpeg_create_decompress(&cinfo);
	
	jpeg_stdio_src(&cinfo, pFile);
	
	pImageData = (tImage*)malloc(sizeof(tImage));

	DecodeJPG(&cinfo, pImageData);
	
	jpeg_destroy_decompress(&cinfo);
	
	fclose(pFile);

	return pImageData;
}

