//==============================================================
//==============================================================
//= math_ops.h =================================================
//= Original coders: Trent Polack (trent@voxelsoft.com) and	   =
//-					 Evan Pipho	  (evan@codershq.com)		   =
//==============================================================
//= A math library (3D vectors, 4x4 matrices)				   =
//==============================================================
//==============================================================

//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "math_ops.h"

#include <math.h>
#include <memory.h>

//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:		 CVECTOR::Normalize - public
// Description:	 Normalize a vector's information
// Arguments:	 None
// Return Value: None
//--------------------------------------------------------------
void CVECTOR::Normalize( void )
{
	float fTemp= 1/Length( );

	m_fVec[0]*= fTemp;
	m_fVec[1]*= fTemp;
	m_fVec[2]*= fTemp;
}

//--------------------------------------------------------------
// Name:		 CVECTOR::Transform - public
// Description:	 Transform a vector based on the information from
//				 a 4x4 matrix
// Arguments:	 -pMatrix: the matrix to use in the calculations
// Return Value: None
//--------------------------------------------------------------
void CVECTOR::Transform( CMATRIX* pMatrix )
{
	float* fpMat= pMatrix->Get( );
	float  fpVec[3];

	fpVec[0]= m_fVec[0]*fpMat[0]  +
			  m_fVec[1]*fpMat[4]  +
			  m_fVec[2]*fpMat[8]  + fpMat[12];

	fpVec[1]= m_fVec[0]*fpMat[1]  +
			  m_fVec[1]*fpMat[5]  +
			  m_fVec[2]*fpMat[9]  + fpMat[13];

	fpVec[2]= m_fVec[0]*fpMat[2]  +
			  m_fVec[1]*fpMat[6]  +
			  m_fVec[2]*fpMat[10] + fpMat[14];

	//copy the results from the calculations
	Set( fpVec );
}

//--------------------------------------------------------------
// Name:		 CVECTOR::Transform3 - public
// Description:	 Transform a vector based on the information from
//				 a 4x4 matrix
// Arguments:	 -pMatrix: the matrix to use in the calculations
// Return Value: None
//--------------------------------------------------------------
void CVECTOR::Transform3( CMATRIX* pMatrix )
{
	float* fpMat= pMatrix->Get( );
	float  fpVec[3];

	fpVec[0]= m_fVec[0]*fpMat[0] +
		      m_fVec[1]*fpMat[4] +
			  m_fVec[2]*fpMat[8];

	fpVec[1]= m_fVec[0]*fpMat[1] +
			  m_fVec[1]*fpMat[5] +
			  m_fVec[2]*fpMat[9];

	fpVec[2]= m_fVec[0]*fpMat[2] +
			  m_fVec[1]*fpMat[6] +
			  m_fVec[2]*fpMat[10];

	//copy the results from the calculations
	Set( fpVec );
}

//--------------------------------------------------------------
// Name:		 CVECTOR::Length - public
// Description:	 Calculate the vector's length/magnitude
// Arguments:	 None
// Return Value: A floating point length value
//--------------------------------------------------------------
float CVECTOR::Length(void)
{
	return ( ( float )sqrt( m_fVec[0]*m_fVec[0] +
							m_fVec[1]*m_fVec[1] +
							m_fVec[2]*m_fVec[2] ) );
}

//--------------------------------------------------------------
// Name:		 CVECTOR::DotProduct - public
// Description:	 Calculate the dot product of two vectors
// Arguments:	 -vec: second vector in the calculation
// Return Value: A floating point dot product value
//--------------------------------------------------------------
float CVECTOR::DotProduct( CVECTOR& vec )
{
	return ( m_fVec[0]*vec.m_fVec[0] + 
			 m_fVec[1]*vec.m_fVec[1] + 
			 m_fVec[2]*vec.m_fVec[2] );
}

//--------------------------------------------------------------
// Name:		 CVECTOR::CrossProduct - public
// Description:	 Calculate the cross product of two vectors
// Arguments:	 -vec: second vector in the calculation
// Return Value: A vector representing the cross product
//--------------------------------------------------------------
CVECTOR CVECTOR::CrossProduct(CVECTOR& vec)
{
	return CVECTOR( m_fVec[1]*vec.m_fVec[2] - m_fVec[2]*vec.m_fVec[1],
				    m_fVec[2]*vec.m_fVec[0] - m_fVec[0]*vec.m_fVec[2],
				    m_fVec[0]*vec.m_fVec[1] - m_fVec[1]*vec.m_fVec[0] );
}

//--------------------------------------------------------------
// Name:		 CMATRIX::Set - public
// Description:	 Set the values of the matrix to that of the
//				 provided 16 value array
// Arguments:	 -fpMatrix: a 16 value 4x4 matrix array
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::Set( float* fpMatrix )
{
	memcpy( fpMatrix, fpMatrix, sizeof( float[16] ) );
}

//--------------------------------------------------------------
// Name:		 CMATRIX::SetIdentity - public
// Description:	 Set the matrix's information to that of the
//				 4x4 identity matrix
// Arguments:	 None
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::SetIdentity( void )
{
	memset( m_fMatrix, 0, sizeof( float[16] ) );

	m_fMatrix[0]= m_fMatrix[5]= m_fMatrix[10]= m_fMatrix[15] = 1;
}

//--------------------------------------------------------------
// Name:		 CMATRIX::MultiplyByMatrix - public
// Description:	 Multiply the current matrix by another
// Arguments:	 -pMat: matrix to multiply the current by
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::MultiplyByMatrix( CMATRIX& pMat )
{
	const float* m1= m_fMatrix;
	const float* m2= pMat.Get( );
	float newMatrix[16];

	newMatrix[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
	newMatrix[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
	newMatrix[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
	newMatrix[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];

	newMatrix[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
	newMatrix[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
	newMatrix[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
	newMatrix[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];

	newMatrix[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
	newMatrix[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
	newMatrix[10]= m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
	newMatrix[11]= m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];

	newMatrix[12]= m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
	newMatrix[13]= m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
	newMatrix[14]= m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
	newMatrix[15]= m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];

	Set( newMatrix );
}

//--------------------------------------------------------------
// Name:		 CMATRIX::SetTranslation - public
// Description:	 Set the matrix's translation based off of a
//				 3 value array
// Arguments:	 -fpVec: 3 value array with translation info
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::SetTranslation( float* fpVec )
{
	m_fMatrix[12]= fpVec[0];
	m_fMatrix[13]= fpVec[1];
	m_fMatrix[14]= fpVec[2];
	m_fMatrix[15]= 1;
}

//--------------------------------------------------------------
// Name:		 CMATRIX::SetTranslation - public
// Description:	 Set the matrix's translation based off of 
//				 vector
// Arguments:	 -vec: translation vector
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::SetTranslation( CVECTOR vec )
{
	m_fMatrix[12]= vec[0];
	m_fMatrix[13]= vec[1];
	m_fMatrix[14]= vec[2];
	m_fMatrix[15]= 1;
}

//--------------------------------------------------------------
// Name:		 CMATRIX::SetInvTranslation - public
// Description:	 Inversely translate the matrix based off of a
//				 3 value array
// Arguments:	 -fpVec: 3 value array with inverse translation info
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::SetInvTranslation( float* fpVec )
{
	m_fMatrix[12]= -fpVec[0];
	m_fMatrix[13]= -fpVec[1];
	m_fMatrix[14]= -fpVec[2];
	m_fMatrix[15]= 1;
}

//--------------------------------------------------------------
// Name:		 CMATRIX::SetInvTranslation - public
// Description:	 Inversely translate the matrix based off of a
//				 vector
// Arguments:	 -vec: inverse translation vector
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::SetInvTranslation( CVECTOR vec )
{
	m_fMatrix[12]= -vec[0];
	m_fMatrix[13]= -vec[1];
	m_fMatrix[14]= -vec[2];
	m_fMatrix[15]= 1;
}

//--------------------------------------------------------------
// Name:		 CMATRIX::InvTranslateVec - public
// Description:	 Fill a vector with inverse translation info
// Arguments:	 -fpVec: 3 value array to fill
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::InvTranslateVec( float* fpVec )
{
	fpVec[0]= fpVec[0] - m_fMatrix[12];
	fpVec[1]= fpVec[1] - m_fMatrix[13];
	fpVec[2]= fpVec[2] - m_fMatrix[14];
}

//--------------------------------------------------------------
// Name:		 CMATRIX::InvTranslateVec - public
// Description:	 Inversely translate the matrix based off of a
//				 vector
// Arguments:	 -pVec: inverse translation vector
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::InvTranslateVec( CVECTOR* pVec )
{
	pVec[0]= pVec[0] - m_fMatrix[12];
	pVec[1]= pVec[1] - m_fMatrix[13];
	pVec[2]= pVec[2] - m_fMatrix[14];
}

//--------------------------------------------------------------
// Name:		 CMATRIX::SetRotation - public
// Description:	 Set the matrix's rotation based off of a
//				 3 value array
// Arguments:	 -fpVec: 3 value array with rotation info
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::SetRotation( float* fpVec )
{
	float CX= cosf( fpVec[0] );
	float SX= sinf( fpVec[0] );
	float CY= cosf( fpVec[1] );
	float SY= sinf( fpVec[1] );
	float CZ= cosf( fpVec[2] );
	float SZ= sinf( fpVec[2] );

	m_fMatrix[0] = CY * CZ;
	m_fMatrix[1] = CY * SZ;
	m_fMatrix[2] =-SY;

	m_fMatrix[4] = SX * SY * CZ - CX * SZ;
	m_fMatrix[5] = SX * SY * SZ + CX * CZ;
	m_fMatrix[6] = SX * CY;

	m_fMatrix[8] = CX * SY * CZ + SX * SZ;
	m_fMatrix[9] = CX * SY * SZ - SX * CZ;
	m_fMatrix[10]= CX * CY;

	m_fMatrix[15]= 1;
}

//--------------------------------------------------------------
// Name:		 CMATRIX::SetRotation - public
// Description:	 Set the matrix's rotation based off of 
//				 vector
// Arguments:	 -vec: rotation vector
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::SetRotation( CVECTOR vec )
{
	float CX= cosf( vec[0] );
	float SX= sinf( vec[0] );
	float CY= cosf( vec[1] );
	float SY= sinf( vec[1] );
	float CZ= cosf( vec[2] );
	float SZ= sinf( vec[2] );

	m_fMatrix[0] = CY * CZ;
	m_fMatrix[1] = CY * SZ;
	m_fMatrix[2] =-SY;

	m_fMatrix[4] = SX * SY * CZ - CX * SZ;
	m_fMatrix[5] = SX * SY * SZ + CX * CZ;
	m_fMatrix[6] = SX * CY;

	m_fMatrix[8] = CX * SY * CZ + SX * SZ;
	m_fMatrix[9] = CX * SY * SZ - SX * CZ;
	m_fMatrix[10]= CX * CY;

	m_fMatrix[15]= 1;
}

//--------------------------------------------------------------
// Name:		 CMATRIX::SetInvRotation - public
// Description:	 Inversely rotate the matrix based off of a
//				 3 value array
// Arguments:	 -fpVec: 3 value array with inverse rotation info
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::SetInvRotation( float* fpVec )
{
	float CX= cosf( fpVec[0] );
	float SX= sinf( fpVec[0] );
	float CY= cosf( fpVec[1] );
	float SY= sinf( fpVec[1] );
	float CZ= cosf( fpVec[2] );
	float SZ= sinf( fpVec[2] );

	m_fMatrix[0] = CY * CZ;
	m_fMatrix[4] = CY * SZ;
	m_fMatrix[8] =-SY;

	m_fMatrix[1] = SX * SY * CZ - CX * SZ;
	m_fMatrix[5] = SX * SY * SZ + CX * CZ;
	m_fMatrix[9] = SX * CY;

	m_fMatrix[2] = CX * SY * CZ + SX * SZ;
	m_fMatrix[6] = CX * SY * SZ - SX * CZ;
	m_fMatrix[10]= CX * CY;

	m_fMatrix[15]= 1;
}

//--------------------------------------------------------------
// Name:		 CMATRIX::SetInvRotation - public
// Description:	 Inversely rotate the matrix based off of a
//				 vector
// Arguments:	 -vec: inverse rotation vector
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::SetInvRotation( CVECTOR vec )
{
	float CX= cosf( vec[0] );
	float SX= sinf( vec[0] );
	float CY= cosf( vec[1] );
	float SY= sinf( vec[1] );
	float CZ= cosf( vec[2] );
	float SZ= sinf( vec[2] );

	m_fMatrix[0] = CY * CZ;
	m_fMatrix[4] = CY * SZ;
	m_fMatrix[8] =-SY;

	m_fMatrix[1] = SX * SY * CZ - CX * SZ;
	m_fMatrix[5] = SX * SY * SZ + CX * CZ;
	m_fMatrix[9] = SX * CY;

	m_fMatrix[2] = CX * SY * CZ + SX * SZ;
	m_fMatrix[6] = CX * SY * SZ - SX * CZ;
	m_fMatrix[10]= CX * CY;

	m_fMatrix[15]= 1;
}

//--------------------------------------------------------------
// Name:		 CMATRIX::InvRotateVec - public
// Description:	 Fill a vector with inverse rotation info
// Arguments:	 -fpVec: 3 value array to fill
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::InvRotateVec( float* fpVec )
{
	float fVec[3];
	
	fVec[0]= fpVec[0]*m_fMatrix[0] +
			 fpVec[1]*m_fMatrix[1] +
			 fpVec[2]*m_fMatrix[2];

	fVec[1]= fpVec[0]*m_fMatrix[4] +
			 fpVec[1]*m_fMatrix[5] +
			 fpVec[2]*m_fMatrix[6];

	fVec[2]= fpVec[0]*m_fMatrix[8] +
			 fpVec[1]*m_fMatrix[9] +
			 fpVec[2]*m_fMatrix[10];

	//copy the results from the calculations
	memcpy( fpVec, fVec, sizeof( float[3] ) );
}

//--------------------------------------------------------------
// Name:		 CMATRIX::InvRotateVec - public
// Description:	 Inversely rotate the matrix based off of a
//				 vector
// Arguments:	 -pVec: inverse rotation vector
// Return Value: None
//--------------------------------------------------------------
void CMATRIX::InvRotateVec( CVECTOR* pVec )
{
	float* fpVec;
	float fVec[3];

	//copy the information from the vector into a local buffer
	fpVec= pVec->Get( );
	
	fVec[0]= fpVec[0]*m_fMatrix[0] +
			 fpVec[1]*m_fMatrix[1] +
			 fpVec[2]*m_fMatrix[2];

	fVec[1]= fpVec[0]*m_fMatrix[4] +
			 fpVec[1]*m_fMatrix[5] +
			 fpVec[2]*m_fMatrix[6];

	fVec[2]= fpVec[0]*m_fMatrix[8] +
			 fpVec[1]*m_fMatrix[9] +
			 fpVec[2]*m_fMatrix[10];

	//copy the results from the calculations
	pVec->Set( fVec );
}