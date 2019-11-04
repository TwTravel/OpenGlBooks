//==============================================================
//==============================================================
//= camera.h ===================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= This file contains the code routines for a simple camera.  =
//==============================================================
//==============================================================
#ifndef __CAMERA_H__
#define __CAMERA_H__


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <math.h>

#include "math_ops.h"
#include "gl_app.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CONSTANTS --------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#define FRUSTUM_RIGHT  0
#define FRUSTUM_LEFT   1
#define FRUSTUM_BOTTOM 2
#define FRUSTUM_TOP    3
#define FRUSTUM_FAR    4
#define FRUSTUM_NEAR   5


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CCAMERA
{
	public:
		CVECTOR m_vecEyePos;			//position of camera
		CVECTOR m_vecLookAt;			//lookat vector

		//up, forward, right vectors
		CVECTOR m_vecUp;
		CVECTOR m_vecForward;
		CVECTOR m_vecSide;

		//the camera's yaw and pitch
		float m_fYaw;
		float m_fPitch;

		float m_viewFrustum[6][4];

	void ComputeViewMatrix( float fTimeDelta= 1.0f );
	void CalculateViewFrustum( void );

	//--------------------------------------------------------------
	// Name:			CCAMERA::ComputeInfo - public
	// Description:		Compute the information for the camera
	// Arguments:		-fTimeDelta: amount to interpolate from last frame
	// Return Value:	None
	//--------------------------------------------------------------
	inline void SetPosition( float fX, float fY, float fZ )
	{	m_vecEyePos.Set( fX, fY, fZ );	}

	//--------------------------------------------------------------
	// Name:			CCAMERA::SetViewMatrix - public
	// Description:		Set the view matrix using the gluLookAt
	//					utility function, and the calculations from
	//					ComputeViewMatrix
	// Arguments:		None
	// Return Value:	None
	//--------------------------------------------------------------
	inline void SetViewMatrix( void )
	{
		//have OpenGL calculate the viewing matrix
		gluLookAt(m_vecEyePos[0], m_vecEyePos[1], m_vecEyePos[2],
				  m_vecLookAt[0], m_vecLookAt[1], m_vecLookAt[2],
				  m_vecUp[0], m_vecUp[1], m_vecUp[2]);
	}

	bool VertexFrustumTest( float x, float y, float z, bool bTestLR= true, bool bTestTB= true, bool bTestNF= true );
	bool CubeFrustumTest( float x, float y, float z, float size );
	bool SphereInFrustum( float x, float y, float z, float fRadius );

	//--------------------------------------------------------------
	// Name:			CCAMERA::CCAMERA - public
	// Description:		Default constructor
	// Arguments:		None
	// Return Value:	None
	//--------------------------------------------------------------
	CCAMERA( void )
	{
		m_vecUp.Set( 0.0f, 1.0f, 0.0f );
		m_vecSide.Set( 1.0f, 0.0f, 0.0f );
	}

	//--------------------------------------------------------------
	// Name:			CCAMERA::~CCAMERA - public
	// Description:		Default destructor
	// Arguments:		None
	// Return Value:	None
	//--------------------------------------------------------------
	~CCAMERA( void )
	{	}
};


#endif	//__CAMERA_H__
