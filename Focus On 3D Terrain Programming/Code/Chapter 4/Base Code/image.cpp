//==============================================================
//==============================================================
//= image.cpp ==================================================
//= Original coders: Trent Polack (trent@voxelsoft.com) and	   =
//-					 Evan Pipho	  (evan@codershq.com)		   =
//==============================================================
//= Image/texture loadings ops								   =
//==============================================================
//==============================================================


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <stdio.h>
#include <memory.h>

#include "image.h"
#include "log.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- GLOBALS ----------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
unsigned char g_ucUTGAcompare[12]= {	0, 0, 2,  0, 0, 0, 0, 0, 0, 0, 0, 0	};
unsigned char g_ucCTGAcompare[12]= {	0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0	};


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:			CIMAGE::Create - public
// Description:		Create space for use with a new texture
// Arguments:		-uiWidth, uiHeight: the dimensions of the new image
//					-uiBPP: the bits per pixel for the new image
// Return Value:	A boolean variable: -true: memory was successfully allocated
//									    -false: memory was not successfully allocated
//--------------------------------------------------------------
bool CIMAGE::Create( unsigned int uiWidth, unsigned int uiHeight, unsigned int uiBPP )
{
	//set the member variables
	m_uiWidth = uiWidth;
	m_uiHeight= uiHeight;
	m_uiBPP   = uiBPP;

	//allocate memory
	m_ucpData= new unsigned char [uiWidth*uiHeight*( m_uiBPP/8 )];
	if( !m_ucpData )
	{
		g_log.Write( LOG_FAILURE, "Out of memory for image memory allocation" );
		return false;
	}

	//set the loaded flag
	m_bIsLoaded= true;
	return true;
}

//--------------------------------------------------------------
// Name:			CIMAGE::LoadData - public
// Description:		Load only the data for a new image (do not create an
//					OpenGL texture)
// Arguments:		-szFilename: the file to load in
// Return Value:	A boolean variable: -true: data was successfully loaded
//									    -false: data was not successfully loaded
//--------------------------------------------------------------
bool CIMAGE::LoadData( char* szFilename )
{
	FILE* pFile;
	int iStart;
	int iEnd;
	int iSize;

	//open the file for reading (in binary mode)
	pFile= fopen( szFilename, "rb" );

	//check to see if we were able to open the file
	if( pFile==NULL )
	{
		g_log.Write( LOG_FAILURE, "Could not open file %s.", szFilename );
		return false;
	}
		
	//Get file length
	fseek( pFile, 0, SEEK_END );
	iEnd = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );
	iStart = ftell( pFile );
	iSize = iEnd - iStart;

	//allocate the data buffer (temporary)
	m_ucpData= new unsigned char [iSize];

	//read the image's data into the buffer
	if( fread( m_ucpData, sizeof( unsigned char ), iSize, pFile ) != ( unsigned )iSize )
	{
		//the file is corrupted
		g_log.Write( LOG_FAILURE, "%s is corrupted, could not read all data\n", szFilename );
		if( m_ucpData )
		{
			delete[] m_ucpData;
			m_ucpData = NULL;
		}
		return false;
	}

	//check to see if the file is in the BMP format
	if( memcmp( m_ucpData, "BM", 2 )==0 )
	{
		//load the BMP using the BMP-loading routine
		if( !LoadBMP( ) )
		{
			//could not load the BMP file
			g_log.Write( LOG_FAILURE, "Could not load BMP %s.", szFilename );
			if( m_ucpData )
			{
				delete[] m_ucpData;
				m_ucpData= NULL;
			}
			return false;
		}
	}

	//check to see if the file is an uncompressed TGA
	else if( memcmp( m_ucpData, g_ucUTGAcompare, 12 )==0 )
	{
		//load the file using the uncompressed TGA Loader
		if( !LoadUncompressedTGA( ) )
		{
			//could not load the file
			g_log.Write( LOG_FAILURE, "Could not load uncompressed TGA %s\n", szFilename );
			if( m_ucpData )
			{
				delete[] m_ucpData;
				m_ucpData= NULL;
			}
			return false;
		}
	}
	
	//check to see if the file is a compressed TGA
	else if( memcmp( m_ucpData, g_ucCTGAcompare, 12 )==0 )
	{
		//load the file using the compressed TGA loader
		if( !LoadCompressedTGA( ) )
		{
			//could not load the file
			if( m_ucpData )
			{
				delete[] m_ucpData;
				m_ucpData= NULL;
			}
			g_log.Write( LOG_FAILURE, "Could not load compressed TGA %s\n", szFilename );
			return false;
		}
	}

	//the file is not supported by our image loader
	else
	{
		g_log.Write( LOG_FAILURE, "%s is not a supported image type\n", szFilename );
		if( m_ucpData )
		{
			delete[] m_ucpData;
			m_ucpData= NULL;
		}
		return false;
	}

	//the file's data was successfully loaded
	m_bIsLoaded= true;
	g_log.Write( LOG_SUCCESS, "Loaded %s correctly\n", szFilename );
	return true;
}

//--------------------------------------------------------------
// Name:			CIMAGE::Load - public
// Description:		Completely setup a new texture, first load the data in,
//					and then setup the texture for use with OpenGL.  This
//					function supports both the BMP and the TGA formats
// Arguments:		-szFilename: the file to load in
//					-fMinFilter/fMaxFilter: OpenGL filter (GL_LINEAR is most common)
//					-bMipmap: create mipmaps for the texture being created
// Return Value:	A boolean variable: -true: texture was successfully loaded
//									    -false: texture was not successfully loaded
//--------------------------------------------------------------
bool CIMAGE::Load( char* szFilename, float fMinFilter, float fMaxFilter, bool bMipmap )
{
	int	iType;

	//load the file's data in
	if( !LoadData( szFilename ) )
		return false;

	//set the image's OpenGL BPP type
	if( m_uiBPP==24 )
		iType= GL_RGB;
	else
		iType= GL_RGBA;

	//build the texture for use with OpenGL
	glGenTextures( 1, &m_ID );
	glBindTexture( GL_TEXTURE_2D, m_ID );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, fMinFilter );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, fMaxFilter );						

	//create the texture normally
	if( !bMipmap )
		glTexImage2D( GL_TEXTURE_2D, 0, iType, m_uiWidth, m_uiHeight, 
					  0, iType, GL_UNSIGNED_BYTE, m_ucpData );

	//create a mipmapped texture
	else
		gluBuild2DMipmaps( GL_TEXTURE_2D, iType, m_uiWidth, m_uiHeight, 
						   iType, GL_UNSIGNED_BYTE, m_ucpData );

	//the image has been successfully loaded
	m_bIsLoaded= true;
	return true;
}

//--------------------------------------------------------------
// Name:			CIMAGE::Unload - public
// Description:		Unload the texture that is currently loaded
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CIMAGE::Unload( void )
{
	if( m_bIsLoaded )
	{
		delete[] m_ucpData;

		m_uiWidth = 0;
		m_uiHeight= 0;
		m_uiBPP   = 0;

		m_bIsLoaded= false;
	}
}

//--------------------------------------------------------------
// Name:			CIMAGE::Save - public
// Description:		Save the current image information to a file.
//					This function only supports the BMP format.
// Arguments:		-szFilename: the filename of the file to be saved
// Return Value:	A boolean variable: -true: image was saved
//									    -false: image was not saved
//--------------------------------------------------------------
bool CIMAGE::Save( char* szFilename )
{
	//save the file as a windows bitmap (.BMP)
	if( !SaveBMP( szFilename ) )
	{
		g_log.Write( LOG_FAILURE, "Could not save %s.", szFilename );
		return false;
	}

	//the file has been successfully saved
	g_log.Write( LOG_SUCCESS, "%s has been successfully saved", szFilename );
	return true;
}

//--------------------------------------------------------------
// Name:			CIMAGE::LoadBMP - private
// Description:		The routine to load a windows bitmap (BMP)
// Arguments:		None
// Return Value:	A boolean variable: -true: BMP was loaded
//									    -false: BMP was not loaded
//--------------------------------------------------------------
bool CIMAGE::LoadBMP( void )
{
	BMPFileHeader* fileHeader;
	BMPInfoHeader* infoHeader;
	unsigned int iImageSize;
	unsigned char uiTempRGB;
	unsigned int i;
	unsigned char* ucpFile= m_ucpData;

	//load in the file header
	fileHeader= ( BMPFileHeader* )ucpFile;

	//advance the buffer, and load in the file information header
	ucpFile  += sizeof( BMPFileHeader );
	infoHeader= ( BMPInfoHeader* )ucpFile;
	
	//advance the buffer to load int he actual image data
	ucpFile  += sizeof( BMPInfoHeader );
	ucpFile  += fileHeader->uiOffBits;

	//initialize the image memory
	iImageSize= fileHeader->uiSize - fileHeader->uiOffBits;
	Create( infoHeader->lWidth, infoHeader->lWidth, infoHeader->usBitCount );

	//copy the data to the class's data buffer
	memcpy( m_ucpData, ucpFile, iImageSize );

	//swap the R and B values to get RGB since the bitmap color format is in BGR
	for( i=0; i<infoHeader->uiSizeImage; i+=3 )
	{
		uiTempRGB	  = m_ucpData[i];
		m_ucpData[i]  = m_ucpData[i+2];
		m_ucpData[i+2]= uiTempRGB;
	}

	//the BMP has been successfully loaded
	return true;
}

//--------------------------------------------------------------
// Name:			CIMAGE::SaveBMP - private
// Description:		The routine to save a windows bitmap (BMP)
// Arguments:		-szFilename: Filename to save the image to
// Return Value:	A boolean variable: -true: BMP was saved
//									    -false: BMP was not saved
//--------------------------------------------------------------
bool CIMAGE::SaveBMP( char* szFilename )
{
	FILE*		   pFile;
	BMPFileHeader bitmapFileHeader;
	BMPInfoHeader bitmapInfoHeader;
	unsigned char  ucTempRGB;
	unsigned int   i;

	//open a file that we can write to
	pFile= fopen( szFilename, "wb" );
	if( !pFile )
	{
		g_log.Write( LOG_FAILURE, "Could not open a file to save %s in", szFilename );
		return false;
	}

	//define the BMP file header
	bitmapFileHeader.uiSize	    = m_uiWidth*m_uiHeight*3+sizeof( BMPFileHeader )+sizeof( BMPInfoHeader );
	bitmapFileHeader.usType	    = BITMAP_ID;
	bitmapFileHeader.usReserved1= 0;
	bitmapFileHeader.usReserved2= 0;
	bitmapFileHeader.uiOffBits  = sizeof( BMPFileHeader )+sizeof( BMPInfoHeader );

	//write the .bmp file header to the file
	fwrite( &bitmapFileHeader, 1, sizeof( BMPFileHeader ), pFile );

	//define the BMP information header
	bitmapInfoHeader.uiSize		   = sizeof( BMPInfoHeader );
	bitmapInfoHeader.usPlanes	   = 1;
	bitmapInfoHeader.usBitCount	   = 24;
	bitmapInfoHeader.uiCompression = BI_RGB;
	bitmapInfoHeader.uiSizeImage   = m_uiWidth*m_uiHeight*3;
	bitmapInfoHeader.lXPelsPerMeter= 0;
	bitmapInfoHeader.lYPelsPerMeter= 0;
	bitmapInfoHeader.uiClrUsed	   = 0;
	bitmapInfoHeader.uiClrImportant= 0;
	bitmapInfoHeader.lWidth		   = m_uiWidth;
	bitmapInfoHeader.lHeight	   = m_uiHeight;

	//write the .bmp file information header to the file
	fwrite( &bitmapInfoHeader, 1, sizeof( BMPInfoHeader ), pFile );

	//swap the image bit order (RGB to BGR)
	for( i=0; i<bitmapInfoHeader.uiSizeImage; i+=3 )
	{
		ucTempRGB	  = m_ucpData[i];
		m_ucpData[i]  = m_ucpData[i+2];
		m_ucpData[i+2]= ucTempRGB;
	}

	//now write the actual image data
	fwrite( m_ucpData, 1, bitmapInfoHeader.uiSizeImage, pFile );

	//the file has been successfully saved
	fclose( pFile );
	return true;
}

//--------------------------------------------------------------
// Name:			CIMAGE::LoadCompressedTGA - private
// Description:		Load a compressed targa (TGA)
// Arguments:		None
// Return Value:	A boolean variable: -true: TGA was loaded
//									    -false: TGA was not loaded
//--------------------------------------------------------------
bool CIMAGE::LoadCompressedTGA( void )
{
	TGAInformationHeader pTGAinfo;
	unsigned char* ucpFile;
	unsigned char* ucpColorBuffer;
	unsigned char  ucChunkHeader;	
	unsigned int   uiPixelCount;
	unsigned int   uiCurrentPixel;
	unsigned int   uiCurrentByte;
	short		   i;

	ucpFile= m_ucpData;

	//skip past the image header
	ucpFile+= 12;

	memcpy( pTGAinfo.m_ucHeader, ucpFile, sizeof( unsigned char[6] ) );
	
	//allocate memory for the image data buffer
	Create( pTGAinfo.m_ucHeader[1] * 256 + pTGAinfo.m_ucHeader[0],
			pTGAinfo.m_ucHeader[3] * 256 + pTGAinfo.m_ucHeader[2],
			pTGAinfo.m_ucHeader[4] );
	
	//set the class's member variables
	pTGAinfo.m_uiBPP   = m_uiBPP;
	pTGAinfo.m_uiHeight= m_uiHeight;
	pTGAinfo.m_uiWidth = m_uiWidth;

	//advance the file buffer
	ucpFile+= 6;

	//make sure that the image's dimensions are supported by this loaded
	if( ( m_uiWidth<=0 ) || ( m_uiHeight<=0 ) || ( ( m_uiBPP!=24 ) && ( m_uiBPP!=32 ) ) )
		return false;

	pTGAinfo.m_uiBytesPerPixel= pTGAinfo.m_uiBPP/8;
	pTGAinfo.m_uiImageSize	  = ( pTGAinfo.m_uiBytesPerPixel*pTGAinfo.m_uiWidth*pTGAinfo.m_uiHeight );

	uiPixelCount  = pTGAinfo.m_uiHeight * pTGAinfo.m_uiWidth;
	uiCurrentPixel= 0;
	uiCurrentByte = 0;
	ucpColorBuffer= new unsigned char [pTGAinfo.m_uiBytesPerPixel];

	do
	{
		ucChunkHeader= 0;

		ucChunkHeader= *( unsigned char* )ucpFile;
		ucpFile++;

		if( ucChunkHeader<128 )
		{
			ucChunkHeader++;

			//read RAW color values
			for( i=0; i<ucChunkHeader; i++ )
			{
				memcpy( ucpColorBuffer, ucpFile, pTGAinfo.m_uiBytesPerPixel );
				ucpFile += pTGAinfo.m_uiBytesPerPixel;
				
				//flip R and B color values around
				m_ucpData[uiCurrentByte]  = ucpColorBuffer[2];
				m_ucpData[uiCurrentByte+1]= ucpColorBuffer[1];
				m_ucpData[uiCurrentByte+2]= ucpColorBuffer[0];

				if( pTGAinfo.m_uiBytesPerPixel==4 )
					m_ucpData[uiCurrentByte+3]= ucpColorBuffer[3];

				uiCurrentByte += pTGAinfo.m_uiBytesPerPixel;
				uiCurrentPixel++;

				//make sure too many pixels have not been read in
				if( uiCurrentPixel>uiPixelCount )
				{
					if( ucpColorBuffer!=NULL )
						delete[] ucpColorBuffer;
					if( m_ucpData!=NULL )
						delete[] m_ucpData;

					return false;
				}
			}
		}

		//the chunk header is greater than 128 RLE data
		else
		{
			ucChunkHeader-= 127;

			memcpy( ucpColorBuffer, ucpFile, pTGAinfo.m_uiBytesPerPixel );
			ucpFile+= pTGAinfo.m_uiBytesPerPixel;

			//copy the color into the image data as many times as needed
			for( i=0; i<ucChunkHeader; i++ )					
			{
				//switch R and B bytes around while copying
				m_ucpData[uiCurrentByte]  = ucpColorBuffer[2];					
				m_ucpData[uiCurrentByte+1]= ucpColorBuffer[1];
				m_ucpData[uiCurrentByte+2]= ucpColorBuffer[0];

				if( pTGAinfo.m_uiBytesPerPixel==4 )
					m_ucpData[uiCurrentByte+3]= ucpColorBuffer[3];

				uiCurrentByte += pTGAinfo.m_uiBytesPerPixel;
				uiCurrentPixel++;

				//make sure that we have not written too many pixels
				if( uiCurrentPixel>uiPixelCount )
				{
					if( ucpColorBuffer!=NULL )
						delete[] ucpColorBuffer;
					if( m_ucpData!=NULL )
						delete[] m_ucpData;

					return false;
				}
			}
		}
	}

	//loop while there are still pixels left
	while( uiCurrentPixel<uiPixelCount );

	//the compressed TGA has been successfully loaded
	return true;					
}

//--------------------------------------------------------------
// Name:			CIMAGE::LoadUncompressedTGA - private
// Description:		Load an uncompressed targa (TGA)
// Arguments:		None
// Return Value:	A boolean variable: -true: TGA was loaded
//									    -false: TGA was not loaded
//--------------------------------------------------------------
bool CIMAGE::LoadUncompressedTGA( void )
{
	TGAInformationHeader pTGAinfo;
	unsigned int uiCSwap;
	unsigned char* ucpFile= m_ucpData;

	//skip past the TGA header in the data buffer
	ucpFile+= 12;

	//copy the header data
	memcpy( pTGAinfo.m_ucHeader, ucpFile, sizeof( unsigned char[6] ) );

	//allocate memory for the image's data buffer
	Create( pTGAinfo.m_ucHeader[1]*256+pTGAinfo.m_ucHeader[0],
			pTGAinfo.m_ucHeader[3]*256+pTGAinfo.m_ucHeader[2],
			pTGAinfo.m_ucHeader[4] );

	//set the class's member variables
	pTGAinfo.m_uiBPP   = m_uiBPP;
	pTGAinfo.m_uiWidth = m_uiWidth;
	pTGAinfo.m_uiHeight= m_uiHeight;

	ucpFile+= 6;

	if( ( m_uiWidth<=0 ) || ( m_uiHeight<=0 ) || ( ( m_uiBPP!=24 ) && ( m_uiBPP!=32 ) ) )
		return false;

	pTGAinfo.m_uiBytesPerPixel= m_uiBPP/8;
	pTGAinfo.m_uiImageSize	  = ( pTGAinfo.m_uiBytesPerPixel*m_uiWidth*m_uiHeight );

	//copy the image data
	memcpy( m_ucpData, ucpFile, pTGAinfo.m_uiImageSize );

	//byte swapping ( optimized by Steve Thomas )
	for( uiCSwap=0; uiCSwap<( int )pTGAinfo.m_uiImageSize; uiCSwap+=pTGAinfo.m_uiBytesPerPixel )
	{
		m_ucpData[uiCSwap]^= m_ucpData[uiCSwap+2]^=
		m_ucpData[uiCSwap]^= m_ucpData[uiCSwap+2];
	}

	//the uncompressed TGA has been successfully loaded
	return true;
}
