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

		CIMAGE m_texture;

	public:

	void Init( float fTheta, float fPhi, float fRadius );
	void Shutdown( void );

	void Render( float fDelta, bool bRotate );

	void LoadTexture( char* szFilename )
	{	m_texture.Load( szFilename, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true );	}

	void Set( float fCX, float fCY, float fCZ )
	{	m_vecCenter.Set( fCX, fCY, fCZ );	}

	int GetNumVertices( void )
	{	return m_iNumVertices;	}

	int GetNumTriangles( void )
	{	return m_iNumVertices-1;	}

	CSKYDOME( void )
	{	}
	~CSKYDOME( void )
	{	}
};


#endif	//__SKYDOME_H__