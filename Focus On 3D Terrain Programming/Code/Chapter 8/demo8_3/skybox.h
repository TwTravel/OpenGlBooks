//==============================================================
//==============================================================
//= skybox.h ===================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= Code routines for a skybox class						   =
//==============================================================
//==============================================================
#ifndef __SKYBOX_H__
#define __SKYBOX_H__


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "../Base Code/math_ops.h"
#include "../Base Code/image.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#define SBX_NUMSIDES 6

#define SBX_FRONT  0
#define SBX_BACK   1
#define SBX_RIGHT  2
#define SBX_LEFT   3
#define SBX_TOP    4
#define SBX_BOTTOM 5


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CSKYBOX
{
	private:
		CIMAGE m_textures[SBX_NUMSIDES];

		CVECTOR m_vecMin, m_vecMax, m_vecCenter;

	public:

	bool LoadTexture( int iSide, char* szFilename );

	void Render( void );

	//--------------------------------------------------------------
	// Name:		 CSKYBOX::Set - private
	// Description:	 Set the skybox's parameters
	// Arguments:	 -fCX, fCY, fCZ: Center vertex
	//				 -fSize: size of the skybox
	// Return Value: None
	//--------------------------------------------------------------
	void Set( float fCX, float fCY, float fCZ, float fSize )
	{
		float fHalfSize= fSize/2;

		m_vecCenter.Set( fCX, fCY, fCZ );

		m_vecMin.Set( -fHalfSize, -fHalfSize, -fHalfSize );
		m_vecMax.Set( fHalfSize, fHalfSize, fHalfSize );
	}

	CSKYBOX( void )
	{	}
	~CSKYBOX( void )
	{	}
};


#endif	//__SKYBOX_H__