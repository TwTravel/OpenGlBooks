//==============================================================
//==============================================================
//= image.h ====================================================
//= Original coders: Trent Polack (trent@voxelsoft.com) and	   =
//-					 Evan Pipho	  (evan@codershq.com)		   =
//==============================================================
//= Image/texture loadings ops								   =
//==============================================================
//==============================================================
#ifndef __IMAGE_H__
#define __IMAGE_H__


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <windows.h>

#include <gl/gl.h>
#include <gl/glu.h>


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CONSTANTS --------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#define BITMAP_ID 0x4D42


//--------------------------------------------------------------
//--------------------------------------------------------------
//- STRUCTURES -------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
struct TGAInformationHeader
{
	unsigned char m_ucHeader[6];
	unsigned int  m_uiBytesPerPixel;
	unsigned int  m_uiImageSize;
	unsigned int  m_uiTemp;
	unsigned int  m_uiType;	
	unsigned int  m_uiHeight;
	unsigned int  m_uiWidth;
	unsigned int  m_uiBPP;
};

#pragma pack( push, 1 )

struct BMPFileHeader
{
	unsigned short usType; 
    unsigned int uiSize; 
    unsigned short usReserved1; 
    unsigned short usReserved2; 
    unsigned int uiOffBits; 
};

#pragma pack( pop )

struct BMPInfoHeader
{
	unsigned int  uiSize; 
    long   lWidth; 
    long   lHeight; 
    unsigned short usPlanes; 
    unsigned short usBitCount; 
    unsigned int uiCompression; 
    unsigned int uiSizeImage; 
    long lXPelsPerMeter; 
    long lYPelsPerMeter; 
    unsigned int uiClrUsed; 
    unsigned int uiClrImportant; 
};

//--------------------------------------------------------------
//--------------------------------------------------------------
//- GLOBALS ----------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
extern unsigned char g_ucUTGAcompare[12];
extern unsigned char g_ucCTGAcompare[12];


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CIMAGE
{
	private:
		unsigned char* m_ucpData;
		unsigned int   m_uiWidth;
		unsigned int   m_uiHeight;
		unsigned int   m_uiBPP;
		unsigned int   m_ID;

		bool m_bIsLoaded;

	bool LoadBMP( void );
	bool SaveBMP( char* szFilename );

	bool LoadCompressedTGA( void );
	bool LoadUncompressedTGA( void );

	public:


	bool Create( unsigned int uiWidth, unsigned int uiHeight, unsigned int uiBPP );

	bool LoadData( char* a_szFilename );

	bool Load( char* a_szFilename, float a_fMinFilter, float a_fMaxFilter, bool a_bMipmap= false );
	void Unload( void );
	bool Save( char* szFilename );

	//--------------------------------------------------------------
	// Name:			CIMAGE::GetColor - public
	// Description:		Get the color (RGB triplet) from a texture pixel
	// Arguments:		-x, y: position to get color from
	//					-ucpRed, ucpGreen, ucpBlue: place to store the RGB
	//					 values that are extracted from the texture
	// Return Value:	None
	//--------------------------------------------------------------
	inline void GetColor( unsigned int x, unsigned  int y, unsigned char* ucpRed, unsigned char* ucpGreen, unsigned char* ucpBlue )
	{
		unsigned int uiBPP= m_uiBPP/8;

		if( ( x<m_uiWidth ) && ( y<m_uiHeight ) )
		{
			*ucpRed  = m_ucpData[( ( y*m_uiHeight )+x )*uiBPP];
			*ucpGreen= m_ucpData[( ( y*m_uiHeight )+x )*uiBPP+1];
			*ucpBlue = m_ucpData[( ( y*m_uiHeight )+x )*uiBPP+2];
		}
	}

	//--------------------------------------------------------------
	// Name:			CIMAGE::SetColor - public
	// Description:		Set the color (RGB triplet) for a texture pixel
	// Arguments:		-x, y: position to set color at
	//					-ucpRed, ucpGreen, ucpBlue: color to set at pixel
	// Return Value:	None
	//--------------------------------------------------------------
	inline void SetColor( unsigned int x, unsigned  int y, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue )
	{
		unsigned int uiBPP= m_uiBPP/8;

		if( ( x<m_uiWidth ) && ( y<m_uiHeight ) )
		{
			m_ucpData[( ( y*m_uiHeight )+x )*uiBPP]  = ucRed;
			m_ucpData[( ( y*m_uiHeight )+x )*uiBPP+1]= ucGreen;
			m_ucpData[( ( y*m_uiHeight )+x )*uiBPP+2]= ucBlue;
		}
	}

	//--------------------------------------------------------------
	// Name:			CIMAGE::GetData - public
	// Description:		Get a pointer to the image's data buffer
	// Arguments:		None
	// Return Value:	An unsigned char buffer (the texture's data)
	//--------------------------------------------------------------
	inline unsigned char* GetData( void )
	{	return m_ucpData;	}

	//--------------------------------------------------------------
	// Name:			CIMAGE::GetWidth - public
	// Description:		Get the texture's width (size along the x-axis)
	// Arguments:		None
	// Return Value:	An unsigned int value: the texture's size along the x-axis
	//--------------------------------------------------------------
	inline unsigned int GetWidth( void )
	{	return m_uiWidth;	}

	//--------------------------------------------------------------
	// Name:			CIMAGE::GetHeight - public
	// Description:		Get the texture's height (size along the y-axis)
	// Arguments:		None
	// Return Value:	An unsigned int value: the texture's size along the y-axis
	//--------------------------------------------------------------
	inline unsigned int GetHeight( void )
	{	return m_uiHeight;	}

	//--------------------------------------------------------------
	// Name:			CIMAGE::GetBPP - public
	// Description:		Get the texture's number of bits per pixel
	// Arguments:		None
	// Return Value:	An unsigned int value: the texture's bits per pixel
	//--------------------------------------------------------------
	inline unsigned int GetBPP( void )
	{	return m_uiBPP;	}

	//--------------------------------------------------------------
	// Name:			CIMAGE::GetID - public
	// Description:		Get the texture's identifying ID (use with OpenGL)
	// Arguments:		None
	// Return Value:	An unsigned int value: the texture's ID
	//--------------------------------------------------------------
	inline unsigned int GetID( void )
	{	return m_ID;	}

	//--------------------------------------------------------------
	// Name:			CIMAGE::SetID - public
	// Description:		Set the texture's identifying ID (use with OpenGL)
	// Arguments:		-uiID: the texture's soon-to-be ID
	// Return Value:	None
	//--------------------------------------------------------------
	inline void SetID( unsigned int uiID )
	{	m_ID= uiID;	}

	//--------------------------------------------------------------
	// Name:			CIMAGE::IsLoaded - public
	// Description:		Find out if the texture has been loaded or not
	// Arguments:		None
	// Return Value:	A boolean value: -true: a texture has been loaded
	//									 -false: a texture hasn't been loaded
	//--------------------------------------------------------------
	inline bool IsLoaded( void )
	{	return m_bIsLoaded;	}
};


#endif	//__IMAGE_H__
