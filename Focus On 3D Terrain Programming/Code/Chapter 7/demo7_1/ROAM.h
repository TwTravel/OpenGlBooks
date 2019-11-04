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

#include "terrain.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CROAM : public CTERRAIN
{
	private:
		CCAMERA* m_pCamera;

		float* m_fpLevelMDSize;				//max midpoint displacement per level
		int m_iMaxLevel;

		unsigned int m_uiGridID;			//id from glGenTextures
		float m_fGridTexCoords[3][3];		//texture coordinates for three verts

	void RenderSub( int iLevel, float* fpVert1, float* fpVert2, float* fpVert3 );

	public:


	void Init( int iMaxLevel, CCAMERA* pCamera );
	void Shutdown( void );
	void Update( void );
	void Render( void );

	CROAM( void ) { }
	~CROAM( void ) { }
};


#endif	//__ROAM_H__