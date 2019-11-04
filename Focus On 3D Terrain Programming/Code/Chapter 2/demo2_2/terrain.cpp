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

//--------------------------------------------------------------
// Name:			CTERRAIN::NormalizeTerrain - private
// Description:		Scale the terrain height values to a range of
//					0-255
// Arguments:		-fpHeightData: the height data buffer
// Return Value:	None
//--------------------------------------------------------------
void CTERRAIN::NormalizeTerrain( float* fpHeightData )
{
	float fMin, fMax;
	float fHeight;
	int i;

	fMin= fpHeightData[0];
	fMax= fpHeightData[0];

	//find the min/max values of the height fTempBuffer
	for( i=1; i<m_iSize*m_iSize; i++ )
	{
		if( fpHeightData[i]>fMax ) 
			fMax= fpHeightData[i];

		else if( fpHeightData[i]<fMin ) 
			fMin= fpHeightData[i];
	}

	//find the range of the altitude
	if( fMax<=fMin )
		return;

	fHeight= fMax-fMin;

	//scale the values to a range of 0-255 (because I like things that way)
	for( i=0; i<m_iSize*m_iSize; i++ )
		fpHeightData[i]= ( ( fpHeightData[i]-fMin )/fHeight )*255.0f;
}

//--------------------------------------------------------------
// Name:			CTERRAIN::FilterHeightBand - private
// Description:		Apply the erosion filter to an individual 
//					band of height values
// Arguments:		-fpBand: the band to be filtered
//					-iStride: how far to advance per pass
//					-iCount: Number of passes to make
//					-fFilter: the filter strength
// Return Value:	None
//--------------------------------------------------------------
void CTERRAIN::FilterHeightBand( float* fpBand, int iStride, int iCount, float fFilter )
{
	float v= fpBand[0];
	int j  = iStride;
	int i;

	//go through the height band and apply the erosion filter
	for( i=0; i<iCount-1; i++ )
	{
		fpBand[j]= fFilter*v + ( 1-fFilter )*fpBand[j];
		
		v = fpBand[j];
		j+= iStride;
	}
}

//--------------------------------------------------------------
// Name:			CTERRAIN::FilterHeightfTempBuffer - private
// Description:		Apply the erosion filter to an entire buffer
//					of height values
// Arguments:		-fpHeightData: the height values to be filtered
//					-fFilter: the filter strength
// Return Value:	None
//--------------------------------------------------------------
void CTERRAIN::FilterHeightField( float* fpHeightData, float fFilter )
{
	int i;

	//erode left to right
	for( i=0; i<m_iSize; i++ )
		FilterHeightBand( &fpHeightData[m_iSize*i], 1, m_iSize, fFilter );
	
	//erode right to left
	for( i=0; i<m_iSize; i++ )
		FilterHeightBand( &fpHeightData[m_iSize*i+m_iSize-1], -1, m_iSize, fFilter );

	//erode top to bottom
	for( i=0; i<m_iSize; i++ )
		FilterHeightBand( &fpHeightData[i], m_iSize, m_iSize, fFilter);

	//erode from bottom to top
	for( i=0; i<m_iSize; i++ )
		FilterHeightBand( &fpHeightData[m_iSize*(m_iSize-1)+i], -m_iSize, m_iSize, fFilter );
}

//--------------------------------------------------------------
// Name:			CTERRAIN::MakeTerrainFault - public
// Description:		Create a height data set using the "Fault Formation"
//					algorithm.  Thanks a lot to Jason Shankel for this code!
// Arguments:		-iSize: Desired size of the height map
//					-iIterations: Number of detail passes to make
//					-iMinDelta, iMaxDelta: the desired min/max heights
//					-iIterationsPerFilter: Number of passes per filter
//					-fFilter: Strength of the filter
// Return Value:	A boolean value: -true: successful creation
//									 -false: unsuccessful creation
//--------------------------------------------------------------
bool CTERRAIN::MakeTerrainFault( int iSize, int iIterations, int iMinDelta, int iMaxDelta, float fFilter )
{
	float* fTempBuffer;
	int iCurrentIteration;
	int iHeight;
	int iRandX1, iRandZ1;
	int iRandX2, iRandZ2;
	int iDirX1, iDirZ1;
	int iDirX2, iDirZ2;
	int x, z;
	int i;

	if( m_heightData.m_ucpData )
		UnloadHeightMap( );

	m_iSize= iSize;

	//allocate the memory for our height data
	m_heightData.m_ucpData= new unsigned char [m_iSize*m_iSize];
	fTempBuffer= new float [m_iSize*m_iSize];

	//check to see if memory was successfully allocated
	if( m_heightData.m_ucpData==NULL )
	{
		//something is seriously wrong here
		g_log.Write( LOG_FAILURE, "Could not allocate memory for height map" );
		return false;
	}

	//check to see if memory was successfully allocated
	if( fTempBuffer==NULL )
	{
		//something is seriously wrong here
		g_log.Write( LOG_FAILURE, "Could not allocate memory for height map" );
		return false;
	}

	//clear the height fTempBuffer
	for( i=0; i<m_iSize*m_iSize; i++ )
		fTempBuffer[i]= 0;

	for( iCurrentIteration=0; iCurrentIteration<iIterations; iCurrentIteration++ )
	{
		//calculate the height range (linear interpolation from iMaxDelta to
		//iMinDelta) for this fault-pass
		iHeight= iMaxDelta - ( ( iMaxDelta-iMinDelta )*iCurrentIteration )/iIterations;
		
		//pick two points at random from the entire height map
		iRandX1= rand( )%m_iSize;
		iRandZ1= rand( )%m_iSize;
		
		//check to make sure that the points are not the same
		do
		{
			iRandX2= rand( )%m_iSize;
			iRandZ2= rand( )%m_iSize;
		} while ( iRandX2==iRandX1 && iRandZ2==iRandZ1 );

		
		//iDirX1, iDirZ1 is a vector going the same direction as the line
		iDirX1= iRandX2-iRandX1;
		iDirZ1= iRandZ2-iRandZ1;
		
		for( z=0; z<m_iSize; z++ )
		{
			for( x=0; x<m_iSize; x++ )
			{
				//iDirX2, iDirZ2 is a vector from iRandX1, iRandZ1 to the current point (in the loop)
				iDirX2= x-iRandX1;
				iDirZ2= z-iRandZ1;
				
				//if the result of ( iDirX2*iDirZ1 - iDirX1*iDirZ2 ) is "up" (above 0), 
				//then raise this point by iHeight
				if( ( iDirX2*iDirZ1 - iDirX1*iDirZ2 )>0 )
					fTempBuffer[( z*m_iSize )+x]+= ( float )iHeight;
			}
		}

		//erode terrain
		FilterHeightField( fTempBuffer, fFilter );
	}

	//normalize the terrain for our purposes
	NormalizeTerrain( fTempBuffer );

	//transfer the terrain into our class's unsigned char height buffer
	for( z=0; z<m_iSize; z++ )
	{
		for( x=0; x<m_iSize; x++ )
			SetHeightAtPoint( ( unsigned char )fTempBuffer[( z*m_iSize )+x], x, z );
	}

	//delete temporary buffer
	if( fTempBuffer )
	{
		//delete the data
		delete[] fTempBuffer;
	}

	return true;
}

//--------------------------------------------------------------
// Name:			CTERRAIN::MakeTerrainPlasma - public
// Description:		Create a height data set using the "Midpoint
//					Displacement" algorithm.  Thanks a lot to 
//					Jason Shankel for this code!
//					Note: this algorithm has limited use, since
//					CLOD algorithms usually require a height map 
//					size of (n^2)+1 x (n^2)+1, and this algorithm
//					can only generate (n^2) x (n^2) maps
// Arguments:		-iSize: Desired size of the height map
//					-fRoughness: Desired roughness of the created map
// Return Value:	A boolean value: -true: successful creation
//									 -false: unsuccessful creation
//--------------------------------------------------------------
bool CTERRAIN::MakeTerrainPlasma( int iSize, float fRoughness )
{
	float* fTempBuffer;
	float fHeight, fHeightReducer;
	int iRectSize= iSize;
	int ni, nj;
	int mi, mj;
	int pmi, pmj;
	int i, j;
	int x, z;

	if( m_heightData.m_ucpData )
		UnloadHeightMap( );

	if( fRoughness<0 )
		fRoughness*= -1;

	fHeight		  = ( float )iRectSize/2;
	fHeightReducer= ( float )pow(2, -1*fRoughness);

	m_iSize= iSize;

	//allocate the memory for our height data
	m_heightData.m_ucpData= new unsigned char [m_iSize*m_iSize];
	fTempBuffer= new float [m_iSize*m_iSize];

	//check to see if memory was successfully allocated
	if( m_heightData.m_ucpData==NULL )
	{
		//something is seriously wrong here
		g_log.Write( LOG_FAILURE, "Could not allocate memory for height map" );
		return false;
	}

	//check to see if memory was successfully allocated
	if( fTempBuffer==NULL )
	{
		//something is seriously wrong here
		g_log.Write( LOG_FAILURE, "Could not allocate memory for height map" );
		return false;
	}

	//set the first value in the height field
	fTempBuffer[0]= 0.0f;

	//being the displacement process
	while( iRectSize>0 )
	{
		/*Diamond step -

		Find the values at the center of the retangles by averaging the values at 
		the corners and adding a random offset:


		a.....b
		.     .  
		.  e  .
		.     .
		c.....d   

		e  = (a+b+c+d)/4 + random

		In the code below:
		a = (i,j)
		b = (ni,j)
		c = (i,nj)
		d = (ni,nj)
		e = (mi,mj)   */
		for( i=0; i<m_iSize; i+=iRectSize )
		{
			for( j=0; j<m_iSize; j+=iRectSize )
			{
				ni= ( i+iRectSize )%m_iSize;
				nj= ( j+iRectSize )%m_iSize;

				mi= ( i+iRectSize/2 );
				mj= ( j+iRectSize/2 );

				fTempBuffer[mi+mj*m_iSize]= ( float )( ( fTempBuffer[i+j*m_iSize] + fTempBuffer[ni+j*m_iSize] + fTempBuffer[i+nj*m_iSize] + fTempBuffer[ni+nj*m_iSize] )/4 + RangedRandom( -fHeight/2, fHeight/2 ) );
			}
		}

		/*Square step -

		Find the values on the left and top sides of each rectangle
		The right and bottom sides are the left and top sides of the neighboring rectangles,
		  so we don't need to calculate them

		The height m_heightData.m_ucpData wraps, so we're never left hanging.  The right side of the last
			rectangle in a row is the left side of the first rectangle in the row.  The bottom
			side of the last rectangle in a column is the top side of the first rectangle in
			the column

              .......
		      .     .
		      .     .
		      .  d  .
		      .     .
		      .     .
		......a..g..b
		.     .     .
		.     .     .
		.  e  h  f  .
		.     .     .
		.     .     .
		......c......

		g = (d+f+a+b)/4 + random
		h = (a+c+e+f)/4 + random
		
		In the code below:
			a = (i,j) 
			b = (ni,j) 
			c = (i,nj) 
			d = (mi,pmj) 
			e = (pmi,mj) 
			f = (mi,mj) 
			g = (mi,j)
			h = (i,mj)*/
		for( i=0; i<m_iSize; i+=iRectSize )
		{
			for( j=0; j<m_iSize; j+=iRectSize )
			{

				ni= (i+iRectSize)%m_iSize;
				nj= (j+iRectSize)%m_iSize;

				mi= (i+iRectSize/2);
				mj= (j+iRectSize/2);

				pmi= (i-iRectSize/2+m_iSize)%m_iSize;
				pmj= (j-iRectSize/2+m_iSize)%m_iSize;
				
				//Calculate the square value for the top side of the rectangle
				fTempBuffer[mi+j*m_iSize]= ( float )( ( fTempBuffer[i+j*m_iSize]	  +
														fTempBuffer[ni+j*m_iSize]	  +
														fTempBuffer[mi+pmj*m_iSize]	  +
														fTempBuffer[mi+mj*m_iSize] )/4+
														RangedRandom( -fHeight/2, fHeight/2 ) );

				//Calculate the square value for the left side of the rectangle
				fTempBuffer[i+mj*m_iSize]= ( float )( ( fTempBuffer[i+j*m_iSize]	  +
														fTempBuffer[i+nj*m_iSize]	  +
														fTempBuffer[pmi+mj*m_iSize]	  +
														fTempBuffer[mi+mj*m_iSize] )/4+ 
														RangedRandom( -fHeight/2, fHeight/2 ) );
			}
		}

		//reduce the rectangle size by two to prepare for the next
		//displacement stage
		iRectSize/= 2;

		//reduce the height by the height reducer
		fHeight*= fHeightReducer;
	}

	//normalize the terrain for our purposes
	NormalizeTerrain( fTempBuffer );

	//transfer the terrain into our class's unsigned char height buffer
	for( z=0; z<m_iSize; z++ )
	{
		for( x=0; x<m_iSize; x++ )
			SetHeightAtPoint( ( unsigned char )fTempBuffer[( z*m_iSize )+x], x, z );
	}

	//delete temporary buffer
	if( fTempBuffer )
	{
		//delete the data
		delete[] fTempBuffer;
	}

	return true;
}