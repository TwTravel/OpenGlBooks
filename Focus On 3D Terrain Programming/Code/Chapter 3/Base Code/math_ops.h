//==============================================================
//==============================================================
//= math_ops.h =================================================
//= Original coders: Trent Polack (trent@voxelsoft.com) and	   =
//-					 Evan Pipho	  (evan@codershq.com)		   =
//==============================================================
//= A math library (3D vectors, 4x4 matrices)				   =
//==============================================================
//==============================================================
#ifndef __MATH_OPS_H__
#define __MATH_OPS_H__


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CONSTANTS --------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#define PI		   3.1415926535897932384626433832795f
#define PIOVER180  0.0174532925199432957692369076848861f
#define PIUNDER180 57.2957795130823208767981548141052f
#define EPSILON	   1.0e-8
#define SMALL	   1.0e-4f
#define BIG		   1.0e+10f


//--------------------------------------------------------------
//--------------------------------------------------------------
//- MACROS -----------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#define DEG_TO_RAD( angle )	  ( ( angle )*PIOVER180 )
#define RAD_TO_DEG( radians ) ( ( radians )*PIUNDER180 )

#define SQR( number )		( number*number )
#define CUBE( number )		( number*number*number )


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASSES ----------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CVECTOR;
class CMATRIX;

class CVECTOR
{
	public:
		float m_fVec[3];


	void Transform( CMATRIX* pMatrix );
	void Transform3( CMATRIX* pMatrix );
	void Normalize( void );
	float Length( void );
	float DotProduct( CVECTOR& vec );
	CVECTOR CrossProduct( CVECTOR& vec );
	void Negate( void );


	//----------------------------------------------------------
	// Name:		 CVECTOR::Get - public
	// Description:	 Get the vector's information
	// Arguments:	 None
	// Return Value: An array with 3 floating point values
	//----------------------------------------------------------
	inline float* Get( void )
	{	return m_fVec;	}

	//----------------------------------------------------------
	// Name:		 CVECTOR::Set - public
	// Description:	 Set the values of the vector's members
	// Arguments:	 -x, y, z: variables to initialize the vector with
	// Return Value: None
	//----------------------------------------------------------
	inline void Set( float x, float y, float z )
	{
		m_fVec[0]= x;
		m_fVec[1]= y;
		m_fVec[2]= z;
	}

	//----------------------------------------------------------
	// Name:		 CVECTOR::Set - public
	// Description:	 Initialization of a vector via a 3-float array of values
	// Arguments:	 -fpValues: array of three floating-point values
	// Return Value: None
	//----------------------------------------------------------
	inline void Set( float* fpValues )
	{
		m_fVec[0]= fpValues[0];
		m_fVec[1]= fpValues[1];
		m_fVec[2]= fpValues[2];
	}

	//overloaded '[]' operator for vector info access, vector[1] (vector.m_fVec[1])
	inline float& operator []( int iIdx )
	{	return m_fVec[iIdx];	}

	//overloaded '-' operator, vector - scalar
	inline const CVECTOR operator -  ( const float fScalar ) const
	{
		return CVECTOR( m_fVec[0] - fScalar,
						m_fVec[1] - fScalar,
						m_fVec[2] - fScalar );
	}

	//overloaded '-' operator, vector - vector
	inline const CVECTOR operator -  ( const CVECTOR& vec )  const
	{
		return CVECTOR( m_fVec[0] - vec.m_fVec[0],
						m_fVec[1] - vec.m_fVec[1],
						m_fVec[2] - vec.m_fVec[2] );
	}

	//overloaded '-' operator for vector negation, -vector
	inline const CVECTOR operator -  ( void )
	{
		m_fVec[0]= -m_fVec[0];
		m_fVec[1]= -m_fVec[1];
		m_fVec[2]= -m_fVec[2];

		return *this;
	}

	//overloaded '+' operator, vector + scalar
	inline const CVECTOR operator +  (const float fScalar ) const
	{
		return CVECTOR( m_fVec[0] + fScalar,
						m_fVec[1] + fScalar,
						m_fVec[2] + fScalar );

	}

	//overloaded '+' operator, vector + vector
	inline const CVECTOR operator +  (const CVECTOR& vec )  const
	{
		return CVECTOR( m_fVec[0] + vec.m_fVec[0],
						m_fVec[1] + vec.m_fVec[1],
						m_fVec[2] + vec.m_fVec[2] );
	}

	//overloaded '=' operator, vector= scalar
	inline const void operator =  ( const CVECTOR& vec )
	{
		m_fVec[0]= vec.m_fVec[0];
		m_fVec[1]= vec.m_fVec[1];
		m_fVec[2]= vec.m_fVec[2];
	}

	//overloaded '+=' operator, vector+= scalar
	inline const void operator += ( const float fScalar )
	{
		m_fVec[0]+= fScalar;
		m_fVec[1]+= fScalar;
		m_fVec[2]+= fScalar;
	}

	//overloaded '+=' operator, vector+= vector
	inline const void operator += ( const CVECTOR& vec )
	{
		m_fVec[0]+= vec.m_fVec[0];
		m_fVec[1]+= vec.m_fVec[1];
		m_fVec[2]+= vec.m_fVec[2];
	}

	//overloaded '-=' operator, vector-= scalar
	inline const void operator -= ( const float fScalar )
	{
		m_fVec[0]-= fScalar;
		m_fVec[1]-= fScalar;
		m_fVec[2]-= fScalar;
	}

	//overloaded '-=' operator, vector-= vector
	inline const void operator -= ( const CVECTOR& vec )
	{
		m_fVec[0]-= vec.m_fVec[0];
		m_fVec[1]-= vec.m_fVec[1];
		m_fVec[2]-= vec.m_fVec[2];
	}

	//overloaded '*' operator for evaluation, vector * scalar
	inline const CVECTOR operator *  ( const float fScalar ) const
	{
		return CVECTOR( m_fVec[0]*fScalar,
						m_fVec[1]*fScalar,
						m_fVec[2]*fScalar );
	}

	//overloaded '*' operator, vector * vector
	inline const CVECTOR operator *  ( const CVECTOR& vec )  const
	{
		return CVECTOR( m_fVec[0]*vec.m_fVec[0],
						m_fVec[1]*vec.m_fVec[1],
						m_fVec[2]*vec.m_fVec[2] );
	}

	//overloaded '/' operator, vector/scalar
	inline const CVECTOR operator /  ( const float fScalar ) const
	{
		float fTemp;

		fTemp= 1/fScalar;
		
		return CVECTOR( m_fVec[0]*fTemp,
						m_fVec[1]*fTemp,
						m_fVec[2]*fTemp );
	}

	//overloaded '*=' operator, vector*=scalar
	inline const void operator *= ( const float fScalar )
	{
		m_fVec[0]*= fScalar;
		m_fVec[1]*= fScalar;
		m_fVec[2]*= fScalar;
	}

	//overloaded '*=' operator, vector/vector
	inline const void operator *= ( const CVECTOR& vec )
	{
		m_fVec[0]*= vec.m_fVec[0];
		m_fVec[1]*= vec.m_fVec[1];
		m_fVec[2]*= vec.m_fVec[2];
	}

	//overloaded '/=' operator, vector/= scalar
	inline const void operator /= ( const float fScalar )
	{
		m_fVec[0]/= fScalar;
		m_fVec[1]/= fScalar;
		m_fVec[2]/= fScalar;
	}

	//overloaded '/=' operator, vector/vector
	inline const void operator /= ( const CVECTOR& vec )
	{
		m_fVec[0]/= vec.m_fVec[0];
		m_fVec[1]/= vec.m_fVec[1];
		m_fVec[2]/= vec.m_fVec[2];
	}

	//overloaded '==' operator for evaluation, vector == vector
	inline const bool operator == ( const CVECTOR& vec )  const
	{
		return ( ( m_fVec[0]==vec.m_fVec[0] ) &&
				 ( m_fVec[1]==vec.m_fVec[1] ) && 
				 ( m_fVec[2]==vec.m_fVec[2] ) );
	}

	//overloaded '!=' operator for evaluation, vector != vector
	inline const bool operator != ( const CVECTOR& vec )  const
	{	return !( *this==vec );	}


	//----------------------------------------------------------
	// Name:		 CVECTOR::CVECTOR - public
	// Description:	 Default class constructor
	// Arguments:	 None
	// Return Value: None
	//----------------------------------------------------------
	CVECTOR( void )
	{	Set( 0.0f, 0.0f, 0.0f );	}

	//----------------------------------------------------------
	// Name:		 CVECTOR::CVECTOR - public
	// Description:	 Class constructor with vector initialization
	//				 via a 3-float array of values
	// Arguments:	 -fpValues: array of three floating-point values
	// Return Value: None
	//----------------------------------------------------------
	CVECTOR( float* fpValues )
	{	Set( fpValues );	}

	//----------------------------------------------------------
	// Name:		 CVECTOR::CVECTOR - public
	// Description:	 Class constructor with vector initialization
	//				 via x, y and z values
	// Arguments:	 -x, y, z: variables to initialize the vector with
	// Return Value: None
	//----------------------------------------------------------
	CVECTOR( float x, float y, float z )
	{
		m_fVec[0]= x;
		m_fVec[1]= y;
		m_fVec[2]= z;
	}

	//----------------------------------------------------------
	// Name:		 CVECTOR::~CVECTOR - public
	// Description:	 Default class destructor
	// Arguments:	 None
	// Return Value: None
	//----------------------------------------------------------
	~CVECTOR( void )
	{	}
};

class CMATRIX
{
	public:
		float m_fMatrix[16];


	void Set( float* fpMatrix );

	void SetIdentity( void );

	void MultiplyByMatrix( CMATRIX& pMat );

	void SetTranslation( float* fpVec );
	void SetTranslation( CVECTOR vec );

	void SetInvTranslation( float* fpVec );
	void SetInvTranslation( CVECTOR vec );

	void InvTranslateVec( float* fpVec );
	void InvTranslateVec( CVECTOR* pVec );

	void SetRotation( float* fpVec );
	void SetRotation( CVECTOR vec );

	void SetInvRotation( float* fpVec );
	void SetInvRotation( CVECTOR vec );

	void InvRotateVec( float* fpVec );
	void InvRotateVec( CVECTOR* pVec );

	//----------------------------------------------------------
	// Name:		 CMATRIX::Get - public
	// Description:	 Get the matrix's information as a 16 value
	//				 floating point array
	// Arguments:	 None
	// Return Value: A 16 value floating point array
	//----------------------------------------------------------
	inline float* Get( void )
	{	return m_fMatrix;	}

	//----------------------------------------------------------
	// Name:		 CMATRIX::CMATRIX - public
	// Description:	 Default class constructor
	// Arguments:	 None
	// Return Value: None
	//----------------------------------------------------------
	CMATRIX( void )
	{	}

	//----------------------------------------------------------
	// Name:		 CMATRIX::~CMATRIX - public
	// Description:	 Default class destructor
	// Arguments:	 None
	// Return Value: None
	//----------------------------------------------------------
	~CMATRIX( void )
	{	}
};

#endif	//__MATH_OPS_H__