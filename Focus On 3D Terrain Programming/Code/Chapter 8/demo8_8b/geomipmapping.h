//==============================================================
//==============================================================
//= geomipmapping.h ============================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= This file (along with geomipmapping.cpp) contains all of   =
//= the information for the geomipmapping terrain component.   =
//==============================================================
//==============================================================
#ifndef __GEOMIPMAPPING_H__
#define __GEOMIPMAPPING_H__


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "terrain.h"

#include "../Base Code/camera.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DATA STRUCTURES --------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
struct SGEOMM_PATCH
{
	float m_fDistance;

	int  m_iLOD;
	bool m_bVisible;
};

struct SGEOMM_NEIGHBOR
{
	bool m_bLeft;
	bool m_bUp;
	bool m_bRight;
	bool m_bDown;
};


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CGEOMIPMAPPING : public CTERRAIN
{
	private:
		SGEOMM_PATCH* m_pPatches;
		float m_fFogDepth;

		int			  m_iPatchSize;
		int			  m_iNumPatchesPerSide;

		int	m_iMaxLOD;

		int m_iPatchesPerFrame;	//the number of rendered patches per second

	void RenderFan( float cX, float cZ, float iSize, SGEOMM_NEIGHBOR neighbor, bool bMultiTex, bool bDetail );
	void RenderPatch( int PX, int PZ, bool bMultiTex= false, bool bDetail= false );

	//--------------------------------------------------------------
	// Name:		 CGEOMIPMAPPING::RenderVertex - private
	// Description:	 Set the volumetric fog coordinate for the vertex in question
	// Arguments:	 -fDepth: depth of the fog
	//				 -fHeight: height for the current fog coordinate
	// Return Value: None
	//--------------------------------------------------------------
	inline void SetFogCoord( float fHeight )
	{
		float fFogY= 0;

		//check to ensure that the height is not higher than the fog depth
		if( fHeight>m_fFogDepth )
			fFogY= 0;
		//calculate the fog depth
		else
			fFogY= -( fHeight-m_fFogDepth );

		//send GL the fog depth
		glFogCoordfEXT( fFogY );
	}

	//--------------------------------------------------------------
	// Name:			CGEOMIPMAPPING::RenderVertex - private
	// Description:		Render a vertex, mostly used for saving space (in the code)
	// Arguments:		- x, z: vertex to render
	//					- u, v: texture coordinates
	//					- bMultiTex: use multitexturing or not
	// Return Value:	None
	//--------------------------------------------------------------
	inline void RenderVertex( float x, float z, float u, float v, bool bMultiTex )
	{
		unsigned char ucColor;
		int iX, iZ;

		iX= ( int )x;
		iZ= ( int )z;
		ucColor= GetBrightnessAtPoint( iX, iZ );

		//send the shaded color to the rendering API
		glColor3ub( ( unsigned char )( ucColor*m_vecLightColor[0] ),
				    ( unsigned char )( ucColor*m_vecLightColor[1] ),
				    ( unsigned char )( ucColor*m_vecLightColor[2] ) );

		//send the texture coordinates to the rendering API	
		glMultiTexCoord2fARB( GL_TEXTURE0_ARB, u, v );
		if( bMultiTex )
			glMultiTexCoord2fARB( GL_TEXTURE1_ARB, u*m_iRepeatDetailMap, v*m_iRepeatDetailMap );

		SetFogCoord( GetScaledHeightAtPoint( iX, iZ ) );

		//output the vertex to the rendering API
		glVertex3f( x*m_vecScale[0],
					GetScaledHeightAtPoint( iX, iZ ),
					z*m_vecScale[2] );

		//increase the number of vertices rendered
		m_iVertsPerFrame++;
	}

	public:

	bool Init( int iPatchSize );
	void Shutdown( void );
	
	void Update( CCAMERA camera, bool bCullPatches= true );
	void Render( void );

	//--------------------------------------------------------------
	// Name:		 CGEOMIPMAPPING::SetFogDepth - public
	// Description:	 Set the depth of the volumetric fog
	// Arguments:	 -fDepth: depth of the fog
	// Return Value: None
	//--------------------------------------------------------------
	inline void SetFogDepth( float fDepth )
	{	m_fFogDepth= fDepth;	}

	//--------------------------------------------------------------
	// Name:			CGEOMIPMAPPING::GetNumPatchesPerFrame - public
	// Description:		Get the number of patches being rendered per frame
	// Arguments:		None
	// Return Value:	A integer value: number of rendered patches per frame
	//--------------------------------------------------------------
	inline int GetNumPatchesPerFrame( void )
	{	return m_iPatchesPerFrame;	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::GetPatchNumber - public
	// Description:		Calculate the current patch number
	// Arguments:		-PX, PZ: the patch number
	// Return Value:	An integer value: the patch number (for array access)
	//--------------------------------------------------------------
	inline int GetPatchNumber( int PX, int PZ )
	{	return ( ( PZ*m_iNumPatchesPerSide )+PX );	}

	CGEOMIPMAPPING( void )
	{	}
	~CGEOMIPMAPPING( void )
	{	}
};

#endif	//__GEOMIPMAPPING_H__
