//==============================================================
//==============================================================
//= ROAM.h =====================================================
//= Original coders: Trent Polack (trent@voxelsoft.com) and	   =
//=				     Mark Duchaineau						   =
//==============================================================
//= The header file for the ROAM implementation				   =
//==============================================================
//==============================================================
#ifndef __ROAM_H__
#define __ROAM_H__


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "../Base Code/camera.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CONSTANTS --------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#define LEVEL_MAX 30


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CROAM
{
	private:
		CVECTOR m_vecCameraEye;

		float* m_fpLevel2dzsize;			//max midpoint displacement per level

		unsigned int m_uiGridTexID;			//id from glGenTextures
		float m_fGridtex_t[3][3];			//texture coordinates for three verts

		int m_iVertsPerFrame;		//stat variables
		int m_iTrisPerFrame;

	void RenderSub( int iLevel, float* fpVert1, float* fpVert2, float* fpVert3 );

	public:


	void Init( void );
	void Shutdown( void );
	void Update( CVECTOR vecCameraEye );
	void Render( void );


	//--------------------------------------------------------------
	// Name:			CTERRAIN::GetNumVertsPerFrame - public
	// Description:		Get the number of vertices being sent to the
	//					API every frame
	// Arguments:		None
	// Return Value:	An integer value: number of vertices rendered per frame
	//--------------------------------------------------------------
	inline int GetNumVertsPerFrame( void )
	{	return m_iVertsPerFrame;	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::GetNumTrisPerSec - public
	// Description:		Get the number of triangles being rendered every frame
	// Arguments:		None
	// Return Value:	An integer value: number of triangles rendered every frame
	//--------------------------------------------------------------
	inline int GetNumTrisPerFrame( void )
	{	return m_iTrisPerFrame;	}

	CROAM( void )
	{	}
	
	~CROAM( void )
	{	}
};


#endif	//__ROAM_H__