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
//- CONSTANTS --------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#define TRN_NUM_TILES 5


//--------------------------------------------------------------
//--------------------------------------------------------------
//- STRUCTURES -------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
enum ETILE_TYPES
{
	LOWEST_TILE= 0,		//sand, dirt, etc.
	LOW_TILE,			//grass
	HIGH_TILE,			//mountainside
	HIGHEST_TILE		//tip of mountain
};

enum ELIGHTING_TYPES
{
	HEIGHT_BASED= 0
};

struct STRN_HEIGHT_DATA
{
	unsigned char* m_ucpData;	//the height data
	int m_iSize;				//the height size (must be a power of 2)
};

struct STRN_TEXTURE_REGIONS
{
	int m_iLowHeight;			//lowest possible height (0%)
	int m_iOptimalHeight;		//optimal height (100%)
	int m_iHighHeight;			//highest possible height (0%)
};

struct STRN_TEXTURE_TILES
{
	STRN_TEXTURE_REGIONS m_regions[TRN_NUM_TILES];	//texture regions
	CIMAGE textureTiles[TRN_NUM_TILES];				//texture tiles
	int iNumTiles;
};


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CTERRAIN
{
	protected:
		STRN_HEIGHT_DATA m_heightData;	//the height data

		float m_fHeightScale;		//scaling variable

		//texture information
		STRN_TEXTURE_TILES m_tiles;
		CIMAGE m_texture;
		CIMAGE m_detailMap;
		int	   m_iRepeatDetailMap;
		bool   m_bMultitexture;
		bool   m_bTextureMapping;
		bool   m_bDetailMapping;

		//lighting information
		ELIGHTING_TYPES m_lightingType;
		CVECTOR m_vecLightColor;

		int m_iVertsPerFrame;		//stat variables
		int m_iTrisPerFrame;

	//fractal terrain generation
	void NormalizeTerrain( float* fpHeightData );
	void FilterHeightBand( float* fpBand, int iStride, int iCount, float fFilter );
	void FilterHeightField( float* fpHeightData, float fFilter );

	//texture map generation functions
	float RegionPercent( int tileType, unsigned char ucHeight );
	void GetTexCoords( CIMAGE texture, unsigned int* x, unsigned int* y );
	unsigned char InterpolateHeight( int x, int z, float fHeightToTexRatio );

	//lighting functions
	unsigned char CalculateLightingAtPoint( int x, int z );

	//--------------------------------------------------------------
	// Name:			CTERRAIN::Limit - private
	// Description:		Limit the given unsigned char value to 0-255
	// Arguments:		-ucValue: Value to check
	// Return Value:	An unsigned char value: the limited value
	//--------------------------------------------------------------
	unsigned char Limit( unsigned char ucValue )
	{
		if( ucValue>255 )
			return 255;
		else if( ucValue<0 )
			return 0;

		return ucValue;
	}


	public:
		int m_iSize;	//the size of the heightmap, must be a power of two


	virtual void Render( void )= 0;

	bool LoadHeightMap( char* szFilename, int iSize );
	bool SaveHeightMap( char* szFilename );
	bool UnloadHeightMap( void );

	bool MakeTerrainFault( int iSize, int iIterations, int iMinDelta, int iMaxDelta, float fFilter );
	bool MakeTerrainPlasma( int iSize, float fRoughness );

	//texture map generation
	void GenerateTextureMap( unsigned int uiSize );

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
	inline void SetHeightAtPoint( unsigned char ucHeight, int x, int z)
	{	m_heightData.m_ucpData[( z*m_iSize )+x]= ucHeight;	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::GetTrueHeightAtPoint - public
	// Description:		A function to get the true height value (0-255) at a point
	// Arguments:		-iX, iZ: which height value to retrieve
	// Return Value:	An float value: the true height at
	//					the given point
	//--------------------------------------------------------------
	inline unsigned char GetTrueHeightAtPoint( int x, int z )
	{	return ( m_heightData.m_ucpData[( z*m_iSize )+x] );	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::GetScaledHeightAtPoint - public
	// Description:		Retrieve the scaled height at a given point
	// Arguments:		-iX, iZ: which height value to retrieve
	// Return Value:	A float value: the scaled height at the given
	//					point.
	//--------------------------------------------------------------
	inline float GetScaledHeightAtPoint( int x, int z )
	{	return ( ( float )( m_heightData.m_ucpData[( z*m_iSize )+x] )*m_fHeightScale );	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::SetLightingType - public
	// Description:		Set the lighting technique to be used
	// Arguments:		-lightingType: which lighting technique to use
	// Return Value:	None
	//--------------------------------------------------------------
	inline void SetLightingType( ELIGHTING_TYPES lightingType )
	{	m_lightingType= lightingType;	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::SaveTextureMap - public
	// Description:		Save the current texture map to a file
	// Arguments:		-szFilename: Name of the file to save to
	// Return Value:	A boolean value: -true: successful save
	//									 -false: unsuccessful save
	//--------------------------------------------------------------
	inline bool SaveTextureMap( char* szFilename )
	{
		//first check to see if a texture is loaded, if so, save it!
		if( m_texture.IsLoaded( ) )
			return ( m_texture.Save( szFilename ) );

		return false;
	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::LoadTexture - public
	// Description:		Load a texture map for the terrain
	// Arguments:		-szFilename: Name of the file to load
	// Return Value:	A boolean value: -true: successful load
	//									 -false: unsuccessful load
	//--------------------------------------------------------------
	inline bool LoadTexture( char* szFilename )
	{	return m_texture.Load( szFilename, GL_LINEAR, GL_LINEAR, false );	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::UnloadTexture - public
	// Description:		Unload the terrain's texture
	// Arguments:		None
	// Return Value:	None
	//--------------------------------------------------------------
	inline void UnloadTexture( void )
	{	m_texture.Unload( );	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::LoadDetailMap - public
	// Description:		Load a detail map to add realism to the terrain
	// Arguments:		-szFilename: Name of the file to load
	// Return Value:	A boolean value: -true: successful load
	//									 -false: unsuccessful load
	//--------------------------------------------------------------
	inline bool LoadDetailMap( char* szFilename )
	{	return m_detailMap.Load( szFilename, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true );	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::UnloadDetailMap - public
	// Description:		Unload the terrain's detail map
	// Arguments:		None
	// Return Value:	None
	//--------------------------------------------------------------
	inline void UnloadDetailMap( void )
	{	m_detailMap.Unload( );	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::DoDetailMapping - public
	// Description:		Do a detail map texture pass (which is much more
	//					economical if we can use hardware multitexturing)
	// Arguments:		-bDo: Do detail mapping or not
	// Return Value:	None
	//--------------------------------------------------------------
	inline void DoDetailMapping( bool bDo, int iRepeatNum= 0 )
	{
		m_bDetailMapping  = bDo;
		m_iRepeatDetailMap= iRepeatNum;
	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::DoTextureMapping - public
	// Description:		Do texturing (large color map)
	// Arguments:		-bDo: Do texture mapping or not
	// Return Value:	None
	//--------------------------------------------------------------
	inline void DoTextureMapping( bool bDo )
	{	m_bTextureMapping= bDo;	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::DoMultitexturing - public
	// Description:		Do hardware multitexturing
	// Arguments:		-bDo: Use hardware multitexturing or not
	// Return Value:	None
	//--------------------------------------------------------------
	inline void DoMultitexturing( bool bDo )
	{	m_bMultitexture= bDo;	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::LoadTile - public
	// Description:		Load a single tile for the texture generation
	// Arguments:		-tileType: the tile type to load
	//					-szFilename: filename of the tile to load in
	// Return Value:	A boolean value: -true: successful load
	//									 -false: unsuccessful load
	//--------------------------------------------------------------
	inline bool LoadTile( ETILE_TYPES tileType, char* szFilename )
	{	return m_tiles.textureTiles[tileType].LoadData( szFilename );	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::UnloadTile - public
	// Description:		Unload a single tile
	// Arguments:		-tileType: the texture which is currently loaded
	//							   for this tile type
	// Return Value:	None
	//--------------------------------------------------------------
	inline void UnloadTile( ETILE_TYPES tileType )
	{	m_tiles.textureTiles[tileType].Unload( );	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::UnloadAllTiles - public
	// Description:		Unload all tiles for the texture generation
	// Arguments:		None
	// Return Value:	None
	//--------------------------------------------------------------
	inline void UnloadAllTiles( void )
	{
		UnloadTile( LOWEST_TILE );
		UnloadTile( LOW_TILE );
		UnloadTile( HIGH_TILE );
		UnloadTile( HIGHEST_TILE );
	}

	//--------------------------------------------------------------
	// Name:			CTERRAIN::SetLightColor - public
	// Description:		Set the color of the terrain's lighting system
	// Arguments:		-vecColor: the color of the light
	// Return Value:	None
	//--------------------------------------------------------------
	inline void SetLightColor( CVECTOR vecColor )
	{	m_vecLightColor= vecColor;	}

	CTERRAIN( void ) : m_vecLightColor( 1.0f, 1.0f, 1.0f )
	{	}
	~CTERRAIN( void )
	{	}
};

#endif	//__TERRAIN_H__
