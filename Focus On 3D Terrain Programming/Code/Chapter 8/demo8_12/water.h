//==============================================================
//==============================================================
//= water.h ====================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= 
//==============================================================
//==============================================================
#ifndef __WATER_H__
#define __WATER_H__

//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "../Base Code/math_ops.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CONSTANTS --------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#define WATER_RESOLUTION 60


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CWATER
{
	private:
		CVECTOR m_pVertArray[SQR( WATER_RESOLUTION )];
		CVECTOR m_pNormalArray[SQR( WATER_RESOLUTION )];
		float m_pForceArray[SQR( WATER_RESOLUTION )];
		float m_pVelArray[SQR( WATER_RESOLUTION )];
		int m_pPolyIndexArray[SQR( ( WATER_RESOLUTION-1 ) )*6];

		//Water surface geometry
		int m_iNumVertices;
		int m_iNumIndices;
		
		float m_fWorldSize;

		CVECTOR m_vecColor;
		float m_fTransparency;

		unsigned int m_refmapID;

	public:

	void Init( float fWorldSize );

	void Update( float fDelta );
	void CalcNormals( void );
	void Render( bool bUseCVA );

	void LoadReflectionMap( char* szFilename );

	//--------------------------------------------------------------
	// Name:		 CWATER::SetColor - public
	// Description:	 Set the color of the water mesh
	// Arguments:	 -fRed, fGreen, fBlue, fAlpha: RGBA color values
	// Return Value: None
	//--------------------------------------------------------------
	inline void SetColor( float fRed, float fGreen, float fBlue, float fAlpha )
	{
		m_vecColor.Set( fRed, fGreen, fBlue );
		m_fTransparency= fAlpha;
	}

	//--------------------------------------------------------------
	// Name:			CWATER::GetNumVertices - public
	// Description:		Get the number of vertices in the mesh
	// Arguments:		None
	// Return Value:	An integer value: number of vertices per frame
	//--------------------------------------------------------------
	inline int GetNumVertices( void )
	{	return m_iNumVertices;	}

	//--------------------------------------------------------------
	// Name:			CWATER::GetNumTriangles - public
	// Description:		Get the number of triangles in the mesh
	// Arguments:		None
	// Return Value:	An integer value: number of triangles per frame
	//--------------------------------------------------------------
	inline int GetNumTriangles( void )
	{	return m_iNumVertices*3;	}

	//--------------------------------------------------------------
	// Name:			CWATER::CWATER - public
	// Description:		Default constructor
	// Arguments:		None
	// Return Value:	None
	//--------------------------------------------------------------
	CWATER( void ) : m_vecColor( 1.0f, 1.0f, 1.0f ), m_fTransparency( 1.0f )
	{	}

	//--------------------------------------------------------------
	// Name:			CWATER::~CWATER - public
	// Description:		Default destructor
	// Arguments:		None
	// Return Value:	None
	//--------------------------------------------------------------
	~CWATER( void )
	{	}
};


#endif	//__WATER_H__