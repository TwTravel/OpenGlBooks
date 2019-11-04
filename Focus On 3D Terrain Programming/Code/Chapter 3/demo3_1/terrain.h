//==============================================================
//==============================================================
//= terrain.h ==================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= This header file contains the information for the abstract =
//= terrain class, which all specific implementations are	   =
//= derived from, also contains other general data structures  =
//= and constants.											   =
//==============================================================
//==============================================================
#ifndef __TERRAIN_H__
#define __TERRAIN_H__


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <stdlib.h>

#include "../Base Code/image.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- STRUCTURES -------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
struct SHEIGHT_DATA
{
	unsigned char* m_ucpData;	//the height data
	int m_iSize;				//the height size (must be a power of 2)
};


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CTERRAIN
{
	protected:
		SHEIGHT_DATA m_heightData;	//the height data

		float m_fHeightScale;		//scaling variable

		//texturing variables
		CIMAGE m_texture;
		bool   m_bTextureMapping;

		int m_iVertsPerFrame;		//stat variables
		int m_iTrisPerFrame;

	//fractal terrain generation
	void NormalizeTerrain( float* fpHeightData );
	void FilterHeightBand( float* fpBand, int iStride, int iCount, float fFilter );
	void FilterHeightField( float* fpHeightData, float fFilter );


	public:
		int m_iSize;	//the size of the heightmap, must be a power of two


	virtual void Render( void )= 0;

	bool LoadHeightMap( char* szFilename, int iSize );
	bool SaveHeightMap( char* szFilename );
	bool UnloadHeightMap( void );

	bool MakeTerrainFault( int iSize, int iIterations, int iMinDelta, int iMaxDelta, float fFilter );
	bool MakeTerrainPlasma( int iSize, float fRoughness );

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

	//--------------------------------------------------------------
	// Name:			CTERRAIN::RangedRandom - public
	// Description:		Get a random value between the two arguments
	// Arguments:		-f1, f2: Random boundaries
	// Return Value:	A floating point value: the random number
	//--------------------------------------------------------------
	inline float RangedRandom( float f1, float f2 )
	{	return ( f1+( f2-f1 )*( ( float )rand( ) )/( ( float )RAND_MAX ) );	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::SetHeightScale - public
	// Description:		Set the height scaling variable
	// Arguments:		-fScale: how much to scale the terrain
	// Return Value:	None
	//--------------------------------------------------------------
	inline void SetHeightScale( float fScale )
	{	m_fHeightScale= fScale;	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::SetHeightAtPoint - public
	// Description:		Set the true height value at the given point
	// Arguments:		-ucHeight: the new height value for the point
	//					-iX, iZ: which height value to retrieve
	// Return Value:	None
	//--------------------------------------------------------------
	inline void SetHeightAtPoint( unsigned char ucHeight, int iX, int iZ)
	{	m_heightData.m_ucpData[( iZ*m_iSize )+iX]= ucHeight;	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::GetTrueHeightAtPoint - public
	// Description:		A function to get the true height value (0-255) at a point
	// Arguments:		-iX, iZ: which height value to retrieve
	// Return Value:	An float value: the true height at
	//					the given point
	//--------------------------------------------------------------
	inline unsigned char GetTrueHeightAtPoint( int iX, int iZ )
	{	return ( m_heightData.m_ucpData[( iZ*m_iSize )+iX] );	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::GetScaledHeightAtPoint - public
	// Description:		Retrieve the scaled height at a given point
	// Arguments:		-iX, iZ: which height value to retrieve
	// Return Value:	A float value: the scaled height at the given
	//					point.
	//--------------------------------------------------------------
	inline float GetScaledHeightAtPoint( int iX, int iZ )
	{	return ( ( float )( m_heightData.m_ucpData[( iZ*m_iSize )+iX] )*m_fHeightScale );	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::DoTextureMapping - public
	// Description:		Do texturing (large color map)
	// Arguments:		-bDo: Do texture mapping or not
	// Return Value:	None
	//--------------------------------------------------------------
	inline void DoTextureMapping( bool bDo )
	{	m_bTextureMapping= bDo;	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::LoadTexture - public
	// Description:		Load a texture that will be stretched over the landscape
	// Arguments:		-szFilename: the texture to be loaded in
	// Return Value:	A boolean value: -true: successful load
	//									 -false: unsuccessful load
	//--------------------------------------------------------------
	inline bool LoadTexture( char* szFilename )
	{	return ( m_texture.Load( szFilename, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true ) );	}

	CTERRAIN( void )
	{	}
	~CTERRAIN( void )
	{	}
};

#endif	//__TERRAIN_H__
