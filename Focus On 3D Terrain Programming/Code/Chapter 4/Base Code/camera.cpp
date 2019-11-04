//==============================================================
//==============================================================
//= camera.cpp =================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= The routines for the CCAMERA class
//==============================================================
//==============================================================


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "camera.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:			CCAMERA::ComputeViewMatrix - public
// Description:		Compute the information for the camera
// Arguments:		-fTimeDelta: amount to interpolate from last frame
//								 (defaults to 1.0f)
// Return Value:	None
//--------------------------------------------------------------
void CCAMERA::ComputeViewMatrix( float fTimeDelta )
{
	if( ( m_fYaw>=360.0f) || ( m_fYaw<=-360.0f ) )
		m_fYaw= 0.0f;

	if( m_fPitch>60.0f )
		m_fPitch= 60.0f;
	if( m_fPitch<-60.0f )
		m_fPitch= -60.0f;
     
	float cosYaw  = cosf( DEG_TO_RAD( m_fYaw ) );
	float sinYaw  = sinf( DEG_TO_RAD( m_fYaw ) );
	float sinPitch= sinf( DEG_TO_RAD( m_fPitch ) );
	float cosPitch= cosf( DEG_TO_RAD( m_fPitch ) );

	m_vecForward[0]= sinYaw * cosPitch;
	m_vecForward[1]= sinPitch;
	m_vecForward[2]= cosPitch * -cosYaw;

	m_vecLookAt= m_vecEyePos + m_vecForward;

	m_vecSide= m_vecForward.CrossProduct( m_vecUp );
}

//--------------------------------------------------------------
// Name:			CCAMERA::CalculateViewFrustum - public
// Description:		Calculate the planes that make-up the viewing m_viewFrustum
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CCAMERA::CalculateViewFrustum( void )
{
	float fPMtrx[16];	//projection matrix
	float fMMtrx[16];	//modelview matrix
	float clip[16];
	float fNorm;

	/* Get the current PROJECTION matrix from OpenGL */
	glGetFloatv( GL_PROJECTION_MATRIX, fPMtrx );

	/* Get the current MODELVIEW matrix from OpenGL */
	glGetFloatv( GL_MODELVIEW_MATRIX, fMMtrx );

	/* Combine the two matrices (multiply projection by modelview) */
	clip[ 0]= fMMtrx[ 0]*fPMtrx[ 0] + fMMtrx[ 1]*fPMtrx[ 4] + fMMtrx[ 2]*fPMtrx[ 8] + fMMtrx[ 3]*fPMtrx[12];
	clip[ 1]= fMMtrx[ 0]*fPMtrx[ 1] + fMMtrx[ 1]*fPMtrx[ 5] + fMMtrx[ 2]*fPMtrx[ 9] + fMMtrx[ 3]*fPMtrx[13];
	clip[ 2]= fMMtrx[ 0]*fPMtrx[ 2] + fMMtrx[ 1]*fPMtrx[ 6] + fMMtrx[ 2]*fPMtrx[10] + fMMtrx[ 3]*fPMtrx[14];
	clip[ 3]= fMMtrx[ 0]*fPMtrx[ 3] + fMMtrx[ 1]*fPMtrx[ 7] + fMMtrx[ 2]*fPMtrx[11] + fMMtrx[ 3]*fPMtrx[15];

	clip[ 4]= fMMtrx[ 4]*fPMtrx[ 0] + fMMtrx[ 5]*fPMtrx[ 4] + fMMtrx[ 6]*fPMtrx[ 8] + fMMtrx[ 7]*fPMtrx[12];
	clip[ 5]= fMMtrx[ 4]*fPMtrx[ 1] + fMMtrx[ 5]*fPMtrx[ 5] + fMMtrx[ 6]*fPMtrx[ 9] + fMMtrx[ 7]*fPMtrx[13];
	clip[ 6]= fMMtrx[ 4]*fPMtrx[ 2] + fMMtrx[ 5]*fPMtrx[ 6] + fMMtrx[ 6]*fPMtrx[10] + fMMtrx[ 7]*fPMtrx[14];
	clip[ 7]= fMMtrx[ 4]*fPMtrx[ 3] + fMMtrx[ 5]*fPMtrx[ 7] + fMMtrx[ 6]*fPMtrx[11] + fMMtrx[ 7]*fPMtrx[15];

	clip[ 8]= fMMtrx[ 8]*fPMtrx[ 0] + fMMtrx[ 9]*fPMtrx[ 4] + fMMtrx[10]*fPMtrx[ 8] + fMMtrx[11]*fPMtrx[12];
	clip[ 9]= fMMtrx[ 8]*fPMtrx[ 1] + fMMtrx[ 9]*fPMtrx[ 5] + fMMtrx[10]*fPMtrx[ 9] + fMMtrx[11]*fPMtrx[13];
	clip[10]= fMMtrx[ 8]*fPMtrx[ 2] + fMMtrx[ 9]*fPMtrx[ 6] + fMMtrx[10]*fPMtrx[10] + fMMtrx[11]*fPMtrx[14];
	clip[11]= fMMtrx[ 8]*fPMtrx[ 3] + fMMtrx[ 9]*fPMtrx[ 7] + fMMtrx[10]*fPMtrx[11] + fMMtrx[11]*fPMtrx[15];

	clip[12]= fMMtrx[12]*fPMtrx[ 0] + fMMtrx[13]*fPMtrx[ 4] + fMMtrx[14]*fPMtrx[ 8] + fMMtrx[15]*fPMtrx[12];
	clip[13]= fMMtrx[12]*fPMtrx[ 1] + fMMtrx[13]*fPMtrx[ 5] + fMMtrx[14]*fPMtrx[ 9] + fMMtrx[15]*fPMtrx[13];
	clip[14]= fMMtrx[12]*fPMtrx[ 2] + fMMtrx[13]*fPMtrx[ 6] + fMMtrx[14]*fPMtrx[10] + fMMtrx[15]*fPMtrx[14];
	clip[15]= fMMtrx[12]*fPMtrx[ 3] + fMMtrx[13]*fPMtrx[ 7] + fMMtrx[14]*fPMtrx[11] + fMMtrx[15]*fPMtrx[15];

	/* Extract the numbers for the RIGHT plane */
	m_viewFrustum[FRUSTUM_RIGHT][0]= clip[ 3] - clip[ 0];
	m_viewFrustum[FRUSTUM_RIGHT][1]= clip[ 7] - clip[ 4];
	m_viewFrustum[FRUSTUM_RIGHT][2]= clip[11] - clip[ 8];
	m_viewFrustum[FRUSTUM_RIGHT][3]= clip[15] - clip[12];

	//normalize the plane
	fNorm= sqrtf( SQR( m_viewFrustum[FRUSTUM_RIGHT][0] )+
				  SQR( m_viewFrustum[FRUSTUM_RIGHT][1] )+
				  SQR( m_viewFrustum[FRUSTUM_RIGHT][2] ) );
	m_viewFrustum[FRUSTUM_RIGHT][0]/= fNorm;
	m_viewFrustum[FRUSTUM_RIGHT][1]/= fNorm;
	m_viewFrustum[FRUSTUM_RIGHT][2]/= fNorm;
	m_viewFrustum[FRUSTUM_RIGHT][3]/= fNorm;

	/* Extract the numbers for the LEFT plane */
	m_viewFrustum[FRUSTUM_LEFT][0]= clip[ 3] + clip[ 0];
	m_viewFrustum[FRUSTUM_LEFT][1]= clip[ 7] + clip[ 4];
	m_viewFrustum[FRUSTUM_LEFT][2]= clip[11] + clip[ 8];
	m_viewFrustum[FRUSTUM_LEFT][3]= clip[15] + clip[12];

	//normalize the plane
	fNorm= sqrtf( SQR( m_viewFrustum[FRUSTUM_LEFT][0] )+
				  SQR( m_viewFrustum[FRUSTUM_LEFT][1] )+
				  SQR( m_viewFrustum[FRUSTUM_LEFT][2] ) );
	m_viewFrustum[FRUSTUM_LEFT][0]/= fNorm;
	m_viewFrustum[FRUSTUM_LEFT][1]/= fNorm;
	m_viewFrustum[FRUSTUM_LEFT][2]/= fNorm;
	m_viewFrustum[FRUSTUM_LEFT][3]/= fNorm;

	/* Extract the BOTTOM plane */
	m_viewFrustum[FRUSTUM_BOTTOM][0]= clip[ 3] + clip[ 1];
	m_viewFrustum[FRUSTUM_BOTTOM][1]= clip[ 7] + clip[ 5];
	m_viewFrustum[FRUSTUM_BOTTOM][2]= clip[11] + clip[ 9];
	m_viewFrustum[FRUSTUM_BOTTOM][3]= clip[15] + clip[13];

	//normalize the plane
	fNorm= sqrtf( SQR( m_viewFrustum[FRUSTUM_BOTTOM][0] )+
				  SQR( m_viewFrustum[FRUSTUM_BOTTOM][1] )+
				  SQR( m_viewFrustum[FRUSTUM_BOTTOM][2] ) );
	m_viewFrustum[FRUSTUM_BOTTOM][0]/= fNorm;
	m_viewFrustum[FRUSTUM_BOTTOM][1]/= fNorm;
	m_viewFrustum[FRUSTUM_BOTTOM][2]/= fNorm;
	m_viewFrustum[FRUSTUM_BOTTOM][3]/= fNorm;

	/* Extract the TOP plane */
	m_viewFrustum[FRUSTUM_TOP][0]= clip[ 3] - clip[ 1];
	m_viewFrustum[FRUSTUM_TOP][1]= clip[ 7] - clip[ 5];
	m_viewFrustum[FRUSTUM_TOP][2]= clip[11] - clip[ 9];
	m_viewFrustum[FRUSTUM_TOP][3]= clip[15] - clip[13];

	//normalize the plane
	fNorm= sqrtf( SQR( m_viewFrustum[FRUSTUM_TOP][0] )+
				  SQR( m_viewFrustum[FRUSTUM_TOP][1] )+
				  SQR( m_viewFrustum[FRUSTUM_TOP][2] ) );
	m_viewFrustum[FRUSTUM_TOP][0]/= fNorm;
	m_viewFrustum[FRUSTUM_TOP][1]/= fNorm;
	m_viewFrustum[FRUSTUM_TOP][2]/= fNorm;
	m_viewFrustum[FRUSTUM_TOP][3]/= fNorm;

	/* Extract the FAR plane */
	m_viewFrustum[FRUSTUM_FAR][0]= clip[ 3] - clip[ 2];
	m_viewFrustum[FRUSTUM_FAR][1]= clip[ 7] - clip[ 6];
	m_viewFrustum[FRUSTUM_FAR][2]= clip[11] - clip[10];
	m_viewFrustum[FRUSTUM_FAR][3]= clip[15] - clip[14];

	//normalize the plane
	fNorm= sqrtf( SQR( m_viewFrustum[FRUSTUM_FAR][0] )+
				  SQR( m_viewFrustum[FRUSTUM_FAR][1] )+
				  SQR( m_viewFrustum[FRUSTUM_FAR][2] ) );
	m_viewFrustum[FRUSTUM_FAR][0]/= fNorm;
	m_viewFrustum[FRUSTUM_FAR][1]/= fNorm;
	m_viewFrustum[FRUSTUM_FAR][2]/= fNorm;
	m_viewFrustum[FRUSTUM_FAR][3]/= fNorm;

	/* Extract the NEAR plane */
	m_viewFrustum[FRUSTUM_NEAR][0]= clip[ 3] + clip[ 2];
	m_viewFrustum[FRUSTUM_NEAR][1]= clip[ 7] + clip[ 6];
	m_viewFrustum[FRUSTUM_NEAR][2]= clip[11] + clip[10];
	m_viewFrustum[FRUSTUM_NEAR][3]= clip[15] + clip[14];

	//normalize the plane
	fNorm= sqrtf( SQR( m_viewFrustum[FRUSTUM_NEAR][0] )+
				  SQR( m_viewFrustum[FRUSTUM_NEAR][1] )+
				  SQR( m_viewFrustum[FRUSTUM_NEAR][2] ) );
	m_viewFrustum[FRUSTUM_NEAR][0]/= fNorm;
	m_viewFrustum[FRUSTUM_NEAR][1]/= fNorm;
	m_viewFrustum[FRUSTUM_NEAR][2]/= fNorm;
	m_viewFrustum[FRUSTUM_NEAR][3]/= fNorm;
}

//--------------------------------------------------------------
// Name:			CCAMERA::VertexFrustumTest - public
// Description:		Test a vertex for inclusion in the viewing frustum
// Arguments:		- x, y, z: the vertex to test
// Return Value:	A boolean value: -true: the vertex is visible
//									 -false: the vertex is not visible
//--------------------------------------------------------------
bool CCAMERA::VertexFrustumTest( float x, float y, float z, bool bTestLR, bool bTestTB, bool bTestNF )
{
	int i;

	//loop through the frustum planes
	for( i=0; i<6; i++ )
	{
		if( ( i==FRUSTUM_RIGHT || i==FRUSTUM_LEFT ) && bTestLR )
		{
			if( m_viewFrustum[i][0]*x + m_viewFrustum[i][1]*y + m_viewFrustum[i][2]*z + m_viewFrustum[i][3] <= 0 )
				return false;
		}
		
		if( ( i==FRUSTUM_TOP || i==FRUSTUM_BOTTOM ) && bTestTB )
		{
			if( m_viewFrustum[i][0]*x + m_viewFrustum[i][1]*y + m_viewFrustum[i][2]*z + m_viewFrustum[i][3] <= 0 )
				return false;
		}

		if( ( i==FRUSTUM_NEAR || i==FRUSTUM_FAR ) && bTestNF )
		{
			if( m_viewFrustum[i][0]*x + m_viewFrustum[i][1]*y + m_viewFrustum[i][2]*z + m_viewFrustum[i][3] <= 0 )
				return false;
		}
	}

	return true;
}

bool CCAMERA::CubeFrustumTest( float x, float y, float z, float size )
{
	int i;

	//loop through and test the six vertices of the bounding box against the viewing frustum
	for( i=0; i<6; i++ )
	{
		if( m_viewFrustum[i][0] * ( x-size )+m_viewFrustum[i][1] * 
								  ( y-size )+m_viewFrustum[i][2] * 
								  ( z-size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x+size )+m_viewFrustum[i][1] * 
								  ( y-size )+m_viewFrustum[i][2] * 
								  ( z-size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x-size )+m_viewFrustum[i][1] * 
								  ( y+size )+m_viewFrustum[i][2] * 
								  ( z-size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x+size )+m_viewFrustum[i][1] * 
								  ( y+size )+m_viewFrustum[i][2] * 
								  ( z-size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x-size )+m_viewFrustum[i][1] *
								  ( y-size )+m_viewFrustum[i][2] *
								  ( z+size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x+size )+m_viewFrustum[i][1] *
								  ( y-size )+m_viewFrustum[i][2] * 
								  ( z+size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x-size )+m_viewFrustum[i][1] *
								  ( y+size )+m_viewFrustum[i][2] * 
								  ( z+size )+m_viewFrustum[i][3] > 0 )
			continue;
		if( m_viewFrustum[i][0] * ( x+size )+m_viewFrustum[i][1] *
								  ( y+size )+m_viewFrustum[i][2] * 
								  ( z+size )+m_viewFrustum[i][3] > 0 )
			continue;

		return false;
	}
	
	return true;
}

bool CCAMERA::SphereInFrustum( float x, float y, float z, float fRadius )
{
	int i;

	for( i=0; i<6; i++ )
	{
		if( m_viewFrustum[i][0] * x + 
			m_viewFrustum[i][1] * y + 
			m_viewFrustum[i][2] * z + 
			m_viewFrustum[i][3] < -fRadius )
			return false;
	}

	return true;
}

