//==============================================================
//==============================================================
//= terrain.cpp ================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= This file contains the information for the "parent" terrain=
//= class, which all specific implementations are derived from,=
//= also contains other general data structures and constants. =
//==============================================================
//==============================================================


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <stdio.h>
#include <math.h>

#include "../Base Code/gl_app.h"

#include "terrain.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:			CTERRAIN::LoadHeightMap - public
// Description:		Load a grayscale RAW height map
// Arguments:		-szFilename: the file name of the height map
//					-im_iSize: the m_iSize (power of 2) of the map
// Return Value:	A boolean value: -true: successful load
//									 -false: unsuccessful load
//--------------------------------------------------------------
bool CTERRAIN::LoadHeightMap( char* szFilename, int iSize )
{
	FILE* pFile;

	//check to see if the data has been set
	if( m_heightData.m_ucpData )
		UnloadHeightMap( );

	//open the RAW height map dataset
	pFile= fopen( szFilename, "rb" );
	if( pFile==NULL )
	{
		//bad filename
		g_log.Write( LOG_FAILURE, "Could not load %s\n", szFilename );
		return false;
	}

	//allocate the memory for our height data
	m_heightData.m_ucpData= new unsigned char [iSize*iSize];

	//check to see if memory was successfully allocated
	if( m_heightData.m_ucpData==NULL )
	{
		//the memory could not be allocated something is seriously wrong here
		g_log.Write( LOG_FAILURE, "Could not allocate memory for%s\n", szFilename );
		return false;
	}

	//read the heightmap into context
	fread( m_heightData.m_ucpData, 1, iSize*iSize, pFile );
	
	//Close the file
	fclose( pFile );

	//set the m_iSize data
	m_iSize= iSize;

	//yahoo! The heightmap has been successfully loaded
	g_log.Write( LOG_SUCCESS, "Loaded %s\n", szFilename );
	return true;
}

//--------------------------------------------------------------
// Name:			CTERRAIN::SaveHeightMap - public
// Description:		Save a grayscale RAW height map
// Arguments:		-szFilename: the file name of the height map
// Return Value:	A boolean value: -true: successful save
//									 -false: unsuccessful save
//--------------------------------------------------------------
bool CTERRAIN::SaveHeightMap( char* szFilename )
{
	FILE* pFile;

	//open a file to write to
	pFile= fopen( szFilename, "wb" );
	if( pFile==NULL )
	{
		//bad filename
		g_log.Write( LOG_FAILURE, "Could not create %s\n", szFilename );
		return false;
	}

	//check to see if our height map actually has data in it
	if( m_heightData.m_ucpData==NULL )
	{
		//something is seriously wrong here
		g_log.Write( LOG_FAILURE, "The height data buffer for %s is empty\n", szFilename );
		return false;
	}

	//write the data to the file
	fwrite( m_heightData.m_ucpData, 1, m_iSize*m_iSize, pFile );
	
	//Close the file
	fclose( pFile );

	//w00t w00t! The heightmap has been successfully saved
	g_log.Write( LOG_SUCCESS, "Saved %s\n", szFilename );
	return true;
}

//--------------------------------------------------------------
// Name:			CTERRAIN::UnloadHeightMap - public
// Description:		Unload the class's height map (if there is one)
// Arguments:		None
// Return Value:	A boolean value: -true: successful load
//									 -false: unsuccessful load
//--------------------------------------------------------------
bool CTERRAIN::UnloadHeightMap( void )
{
	//check to see if the data has been set
	if( m_heightData.m_ucpData )
	{
		//delete the data
		delete[] m_heightData.m_ucpData;

		//reset the map dimensions also
		m_iSize= 0;
	}

	//the height map has been unloaded
	g_log.Write( LOG_SUCCESS, "Successfully unloaded the height map\n" );
	return true;
}