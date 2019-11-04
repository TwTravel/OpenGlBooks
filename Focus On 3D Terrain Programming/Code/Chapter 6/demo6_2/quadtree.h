//==============================================================
//==============================================================
//= quadtree.h =================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= This file (along with quadtree.cpp) contains all of the    =
//= information for the quadtree terrain component.            =
//==============================================================
//==============================================================
#ifndef __QUADTREE_H__
#define __QUADTREE_H__


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "terrain.h"

#include "../Base Code/camera.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CONSTANTS --------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#define QT_LR_NODE 0
#define QT_LL_NODE 1
#define QT_UL_NODE 2
#define QT_UR_NODE 3

//bit codes
#define QT_COMPLETE_FAN 0
#define QT_LL_UR		5
#define QT_LR_UL		10
#define QT_NO_FAN       15


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DATA STRUCTURES --------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
struct SQT_VERTEX
{
	float m_fHeight;
};

//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CQUADTREE : public CTERRAIN
{
	private:
		//the quadtree matrix (the underlying power of the engine)
		unsigned char* m_ucpQuadMtrx;

		//pointer to a camera object
		CCAMERA* m_pCamera;

		//detail level variables
		float m_fDetailLevel;
		float m_fMinResolution;

	void PropagateRoughness( void );
	void RefineNode( float x, float z, int iEdgeLength );
	void RenderNode( float x, float z, int iEdgeLength, bool bMultiTex= true, bool bDetail= true );

	//--------------------------------------------------------------
	// Name:			CQUADTREE::RenderVertex - private
	// Description:		Render a single vertex
	// Arguments:		-x, z: vertex to render
	//					-u, v: texture coordinates for the vertex
	//					-bMultiTex: use multitexturing or not
	// Return Value:	None
	//--------------------------------------------------------------
	inline void RenderVertex( float x, float z, float u, float v, bool bMultiTex )
	{
		unsigned char ucColor= GetBrightnessAtPoint( ( int )x, ( int )z );

		glColor3ub( ( unsigned char )( ucColor*m_vecLightColor[0] ),
				    ( unsigned char )( ucColor*m_vecLightColor[1] ),
				    ( unsigned char )( ucColor*m_vecLightColor[2] ) );
		
		glMultiTexCoord2fARB( GL_TEXTURE0_ARB, u, v );
		if( bMultiTex )
			glMultiTexCoord2fARB( GL_TEXTURE1_ARB, u*m_iRepeatDetailMap, v*m_iRepeatDetailMap );

		glVertex3f( x*m_vecScale[0],
					GetScaledHeightAtPoint( ( int )x, ( int )z ),
					z*m_vecScale[2] );

		m_iVertsPerFrame++;
	}

	//--------------------------------------------------------------
	// Name:			CQUADTREE::GetMatrixIndex - private
	// Description:		Calculate the index value to access the quadtree matrix
	// Arguments:		-iX, iZ: vertex to calculate the index
	// Return Value:	An integer value: index to access information in
	//									  the quadtree matrix
	//--------------------------------------------------------------
	inline int GetMatrixIndex( int iX, int iZ )
	{	return ( ( iZ*m_iSize )+iX );	}

	public:

	bool Init( void );
	void Shutdown( void );

	void Update( CCAMERA* pCamera );
	void Render( void );

	//--------------------------------------------------------------
	// Name:			CQUADTREE::SetDetailLevel - public
	// Description:		Set the engine's detail level
	// Arguments:		-fDetail: detail level to set
	// Return Value:	None
	//--------------------------------------------------------------
	inline void SetDetailLevel( float fDetail )
	{	m_fDetailLevel= fDetail;	}

	//--------------------------------------------------------------
	// Name:			CQUADTREE::SetMinResolution - public
	// Description:		Set the engine's global resolution value
	// Arguments:		-fRes: minimum global resolution to set
	// Return Value:	None
	//--------------------------------------------------------------
	inline void SetMinResolution( float fRes )
	{	m_fMinResolution= fRes;	}

	//--------------------------------------------------------------
	// Name:			CQUADTREE::GetQuadMatrixData - public
	// Description:		Retrieve a value from the quadtree matrix
	// Arguments:		-iX, iZ: vertex to get data from
	// Return Value:	An unsigned char value: the value stored in the
	//					                        quadtree matrix
	//--------------------------------------------------------------
	inline unsigned char GetQuadMatrixData( int iX, int iZ )
	{	return m_ucpQuadMtrx[ ( iZ*m_iSize )+iX];	}

	CQUADTREE( void ) : m_fDetailLevel( 50.0f ), m_fMinResolution( 10.0f )
	{	}
	~CQUADTREE( void )
	{	}
};

#endif	//__QUADTREE_H__
