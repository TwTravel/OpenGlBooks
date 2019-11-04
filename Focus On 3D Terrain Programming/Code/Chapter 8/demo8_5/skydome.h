//==============================================================
//==============================================================
//= skydome.h ==================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= Routines for a skydome (half-sphere) system				   =
//==============================================================
//==============================================================
#ifndef __SKYDOME_H__
#define __SKYDOME_H__


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "../Base Code/math_ops.h"
#include "../Base Code/image.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CSKYDOME
{
	private:
		float* m_fVertices;
		float* m_fTexCoords;

		CVECTOR m_vecCenter;

		int m_iNumVertices;

		unsigned int m_uiTexID;

	float CosineInterpolation( float fNum1, float fNum2, float x );
	float RangedRandom( int x, int y );
	float RangedSmoothRandom( int x, int y );
	float Noise( float x, float y );
	float FBM( float x, float y, float fOctaves, float fAmplitude, float fFrequency, float fH );

	void NormalizeFractal( float* fpData, int iSize );
	void BlurBand( float* fpBand, int iStride, int iCount, float fFilter );
	void Blur( float* fpData, int iSize, float fFilter  );

	public:

	void Init( float fTheta, float fPhi, float fRadius );
	void Shutdown( void );

	void Render( float fDelta, bool bRotate );

	void GenCloudTexture( int size, float fBlur, float fOctaves, float fAmplitude, float fFrequency, float fH );

	//--------------------------------------------------------------
	// Name:		 CSKYDOME::LoadTexture - public
	// Description:	 Load the skydome's texture
	// Arguments:	 -szFilename: filename of the texture
	// Return Value: None
	//--------------------------------------------------------------
	void LoadTexture( char* szFilename )
	{
		CIMAGE texture;
		
		texture.Load( szFilename, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true );
	
		m_uiTexID= texture.GetID( );	
	}

	//--------------------------------------------------------------
	// Name:		 CSKYDOME::Set - public
	// Description:	 Set the center of the skydome
	// Arguments:	 -fCX, fCY, fCZ: center location
	// Return Value: None
	//--------------------------------------------------------------
	void Set( float fCX, float fCY, float fCZ )
	{	m_vecCenter.Set( fCX, fCY, fCZ );	}

	//--------------------------------------------------------------
	// Name:		 CSKYDOME::GetNumVertices - public
	// Description:	 Get the number of vertices being rendered per frame
	// Arguments:	 None
	// Return Value: An integer value representing the number of vertices rendered per frame
	//--------------------------------------------------------------
	int GetNumVertices( void )
	{	return m_iNumVertices;	}

	//--------------------------------------------------------------
	// Name:		 CSKYDOME::GetNumTriangles - public
	// Description:	 Get the number of triangles being rendered per frame
	// Arguments:	 None
	// Return Value: An integer value representing the number of triangles rendered per frame
	//--------------------------------------------------------------
	int GetNumTriangles( void )
	{	return m_iNumVertices-2;	}

	CSKYDOME( void )
	{	}
	~CSKYDOME( void )
	{	}
};


#endif	//__SKYDOME_H__