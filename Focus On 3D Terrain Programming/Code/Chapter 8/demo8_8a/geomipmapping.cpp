//==============================================================
//==============================================================
//= geomipmapping.cpp ==========================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= This file (along with geomipmapping.cpp) contains all of   =
//= the information for the geomipmapping terrain component.   =
//==============================================================
//==============================================================


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <stdio.h>

#include "../Base Code/gl_app.h"

#include "geomipmapping.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:			CGEOMIPMAPPING::Init - public
// Description:		Initiate the geomipmapping system
// Arguments:		- iPatchSize: the size of the patch (in vertices)
//								  a good size is usually around 17 (17x17 verts)
// Return Value:	A boolean value: -true: successful initiation
//									 -false: unsuccessful initiation
//--------------------------------------------------------------
bool CGEOMIPMAPPING::Init( int iPatchSize )
{
	int x, z;
	int iLOD;
	int iDivisor;
	int iPatch;

	if( m_iSize==0 )
		return false;

	if( m_pPatches )
		Shutdown( );

	//initiate the patch information
	m_iPatchSize= iPatchSize;
	m_iNumPatchesPerSide= m_iSize/m_iPatchSize;
	m_pPatches= new SGEOMM_PATCH [SQR( m_iNumPatchesPerSide )];
	if( m_pPatches==NULL )
	{
		Shutdown( );

		g_log.Write( LOG_SUCCESS, "Could not allocate memory for geomipmapping patch system" );
		return false;
	}

	//figure out the maximum level of detail for a patch
	iDivisor= m_iPatchSize-1;
	iLOD= 0;
	while( iDivisor>2 )
	{
		iDivisor= iDivisor>>1;
		iLOD++;
	}

	//the max amount of detail
	m_iMaxLOD= iLOD;

	//initialize the patch values
	for( z=0; z<m_iNumPatchesPerSide; z++ )
	{
		for( x=0; x<m_iNumPatchesPerSide; x++ )
		{
			iPatch= GetPatchNumber( x, z );

			//initialize the patches to the lowest level of detail
			m_pPatches[iPatch].m_iLOD= m_iMaxLOD;

			m_pPatches[iPatch].m_bVisible= true;
		}
	}

	g_log.Write( LOG_SUCCESS, "Geomipmapping system successfully initialized" );
	return true;
}

//--------------------------------------------------------------
// Name:			CGEOMIPMAPPING::Shutdown - public
// Description:		Shutdown the geomipmapping system
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CGEOMIPMAPPING::Shutdown( void )
{
	//delete the patch buffer
	if( m_pPatches )
		delete[] m_pPatches;

	//reset patch values
	m_iPatchSize= 0;
	m_iNumPatchesPerSide= 0;
}

//--------------------------------------------------------------
// Name:			CGEOMIPMAPPING::Update - public
// Description:		Update the geomipmapping system
// Arguments:		-camera: the camera object your demo is using
//					-bCullPatches: cull unseen patches (true by default)
// Return Value:	None
//--------------------------------------------------------------
void CGEOMIPMAPPING::Update( CCAMERA camera, bool bCullPatches )
{
	float fX, fY, fZ;
	float fScaledSize;
	int x, z;
	int iPatch;

	fScaledSize= m_iPatchSize*m_vecScale[0];

	for( z=0; z<m_iNumPatchesPerSide; z++ )
	{
		for( x=0; x<m_iNumPatchesPerSide; x++ )
		{
			iPatch= GetPatchNumber( x, z );

			//compute patch center (used for distance determination
			fX= ( x*m_iPatchSize )+( m_iPatchSize/2.0f );
			fZ= ( z*m_iPatchSize )+( m_iPatchSize/2.0f );
			fY= GetScaledHeightAtPoint( ( int )fX, ( int )fZ );

			//only scale the X and Z values, the Y value has already been scaled
			fX*= m_vecScale[0];
			fZ*= m_vecScale[2];

			//check to see if the user wanted to cull the non-visible patches
			if( bCullPatches )
			{
				//do a frustum test against the patch
				if( camera.CubeFrustumTest( fX, fY, fZ, m_iPatchSize*m_vecScale[0] ) )
					m_pPatches[iPatch].m_bVisible= true;

				//the patch is not visible
				else
					m_pPatches[iPatch].m_bVisible= false;
			}

			//make all patches visible
			else
				m_pPatches[iPatch].m_bVisible= true;

			//only finish updating if the patch is visible
			if( m_pPatches[iPatch].m_bVisible )
			{
				//get the distance from the camera to the patch
				m_pPatches[iPatch].m_fDistance= sqrtf( SQR( ( fX-camera.m_vecEyePos[0] ) )+
													   SQR( ( fY-camera.m_vecEyePos[1] ) )+
													   SQR( ( fZ-camera.m_vecEyePos[2] ) ) );

				//BAD way to determine patch LOD, we will be fixing this code a bit later in the chapter
				if( m_pPatches[iPatch].m_fDistance<100 )
					m_pPatches[iPatch].m_iLOD= 0;
			
				else if( m_pPatches[iPatch].m_fDistance<250 )
					m_pPatches[iPatch].m_iLOD= 1;

				else if( m_pPatches[iPatch].m_fDistance<750 )
					m_pPatches[iPatch].m_iLOD= 2;

				else if( m_pPatches[iPatch].m_fDistance>=750 )
					m_pPatches[iPatch].m_iLOD= 3;
			}
		}
	}
}

//--------------------------------------------------------------
// Name:			CGEOMIPMAPPING::Render - public
// Description:		Render the geomipmapping system
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CGEOMIPMAPPING::Render( void )
{
	int	x, z;

	//reset the counting variables
	m_iPatchesPerFrame = 0;
	
	m_iVertsPerFrame= 0;
	m_iTrisPerFrame = 0;

	//enable back-face culling
	glEnable( GL_CULL_FACE );

	//render the multitexturing terrain
	if( m_bMultitexture && m_bDetailMapping && m_bTextureMapping )
	{
		glDisable( GL_BLEND );

		//bind the primary color texture to the first texture unit
		glActiveTextureARB( GL_TEXTURE0_ARB );
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, m_texture.GetID( ) );

		//bind the detail color texture to the second texture unit
		glActiveTextureARB( GL_TEXTURE1_ARB );
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, m_detailMap.GetID( ) );
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB );
		glTexEnvi( GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2 );

		//render the patches
		for( z=0; z<m_iNumPatchesPerSide; z++ )
		{
			for( x=0; x<m_iNumPatchesPerSide; x++ )
			{
				if( m_pPatches[GetPatchNumber( x, z )].m_bVisible )
				{
					RenderPatch( x, z, true, true );
					m_iPatchesPerFrame++;
				}
			}
		}
	}
	
	//no hardware multitexturing available, or the user only wants to render
	//the detail texture or the color texture
	else
	{
		if( m_bTextureMapping )
		{
			//bind the primary color texture (FOR THE PRIMARY TEXTURE PASS)
			glActiveTextureARB( GL_TEXTURE0_ARB );
			glEnable( GL_TEXTURE_2D );
			glBindTexture( GL_TEXTURE_2D, m_texture.GetID( ) );

			//render the color texture
			for( z=0; z<m_iNumPatchesPerSide; z++ )
			{
				for( x=0; x<m_iNumPatchesPerSide; x++ )
				{
					if( m_pPatches[GetPatchNumber( x, z )].m_bVisible )
					{
						RenderPatch( x, z, true, true );
						m_iPatchesPerFrame++;
					}
				}
			}
		}

		if( !( m_bTextureMapping && !m_bDetailMapping ) )
		{
			//if the user wants detail mapping, we need to set some things up
			if( m_bDetailMapping )
			{
				//bind the detail texture
				glActiveTextureARB( GL_TEXTURE0_ARB );
				glEnable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, m_detailMap.GetID( ) );
			
				//only use blending if a texture pass was made
				if( m_bTextureMapping )
				{
					glEnable( GL_BLEND );
					glBlendFunc( GL_ZERO, GL_SRC_COLOR );
				}
			}

			//render either the detail map on top of the texture,
			//only the detail map, or neither
			for( z=0; z<m_iNumPatchesPerSide; z++ )
			{
				for( x=0; x<m_iNumPatchesPerSide; x++ )
				{
					if( m_pPatches[GetPatchNumber( x, z )].m_bVisible )
					{
						RenderPatch( x, z, true, true );
						m_iPatchesPerFrame++;
					}
				}
			}
		}
	}

	glDisable( GL_BLEND );

	//unbind the texture occupying the second texture unit
	glActiveTextureARB( GL_TEXTURE1_ARB );
	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );

	//unbind the texture occupying the first texture unit
	glActiveTextureARB( GL_TEXTURE0_ARB );
	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

//--------------------------------------------------------------
// Name:			CGEOMIPMAPPING::RenderPatch - private
// Description:		Render a patch of terrain
// Arguments:		-PX, PZ: the patch location
//					-bMultitex: use multitexturing or not
//					-bDetail: render with a detail map or not
// Return Value:	None
//--------------------------------------------------------------
void CGEOMIPMAPPING::RenderPatch( int PX, int PZ, bool bMultiTex, bool bDetail )
{
	SGEOMM_NEIGHBOR patchNeighbor;
	SGEOMM_NEIGHBOR fanNeighbor;
	float fSize;
	float fHalfSize;
	float x, z;
	int iPatch= GetPatchNumber( PX, PZ );
	int iDivisor;
	int iLOD;

	//find out information about the patch to the current patch's left, if the patch is of a
	//greater detail or there is no patch to the left, we can render the mid-left vertex
	if( m_pPatches[GetPatchNumber( PX-1, PZ )].m_iLOD<=m_pPatches[iPatch].m_iLOD || PX==0 )
		patchNeighbor.m_bLeft= true;
	else
		patchNeighbor.m_bLeft= false;

	//find out about the upper patch
	if( m_pPatches[GetPatchNumber( PX, PZ+1 )].m_iLOD<=m_pPatches[iPatch].m_iLOD || PZ==m_iNumPatchesPerSide )
		patchNeighbor.m_bUp= true;
	else
		patchNeighbor.m_bUp= false;

	//find out about the right patch
	if( m_pPatches[GetPatchNumber( PX+1, PZ )].m_iLOD<=m_pPatches[iPatch].m_iLOD || PX==m_iNumPatchesPerSide )
		patchNeighbor.m_bRight= true;
	else
		patchNeighbor.m_bRight= false;

	//find out about the lower patch
	if( m_pPatches[GetPatchNumber( PX, PZ-1 )].m_iLOD<=m_pPatches[iPatch].m_iLOD || PZ==0 )
		patchNeighbor.m_bDown= true;
	else
		patchNeighbor.m_bDown= false;

	//we need to determine the distance between each triangle-fan that
	//we will be rendering
	iLOD    = m_pPatches[GetPatchNumber( PX, PZ )].m_iLOD+1;
	fSize   = ( float )m_iPatchSize;
	iDivisor= m_iPatchSize-1;

	//find out how many fan divisions we are going to have
	while( --iLOD>-1 )
		iDivisor= iDivisor>>1;

	//the size between the center of each triangle fan
	fSize/= iDivisor;

	//half the size between the center of each triangle fan (this will be
	//the size between each vertex)
	fHalfSize= fSize/2.0f;
	for( z=fHalfSize; ( ( int )( z+fHalfSize ) )<m_iPatchSize+1; z+=fSize )
	{
		for( x=fHalfSize; ( ( int )( x+fHalfSize ))<m_iPatchSize+1; x+=fSize )
		{
			//if this fan is in the left row, we may need to adjust it's rendering to
			//prevent cracks
			if( x==fHalfSize )
				fanNeighbor.m_bLeft= patchNeighbor.m_bLeft;
			else
				fanNeighbor.m_bLeft= true;

			//if this fan is in the bottom row, we may need to adjust it's rendering to
			//prevent cracks
			if( z==fHalfSize )
				fanNeighbor.m_bDown= patchNeighbor.m_bDown;
			else
				fanNeighbor.m_bDown= true;

			//if this fan is in the right row, we may need to adjust it's rendering to
			//prevent cracks
			if( x>=( m_iPatchSize-fHalfSize ) )
				fanNeighbor.m_bRight= patchNeighbor.m_bRight;
			else
				fanNeighbor.m_bRight= true;

			//if this fan is in the top row, we may need to adjust it's rendering to
			//prevent cracks
			if( z>=( m_iPatchSize-fHalfSize ) )
				fanNeighbor.m_bUp= patchNeighbor.m_bUp;
			else
				fanNeighbor.m_bUp= true;

			//render the triangle fan
			RenderFan( ( PX*m_iPatchSize )+x, ( PZ*m_iPatchSize )+z,
					   fSize, fanNeighbor, bMultiTex, bDetail );
		}
	}
}

//--------------------------------------------------------------
// Name:			CGEOMIPMAPPING::RenderFan - private
// Description:		Update the geomipmapping system
// Arguments:		-cX, cZ: center of the triangle fan to render
//					-fSize: half of the fan's entire size
//					-neightbor: the fan's neighbor structure (used to avoid cracking)
//					-bMultitex: use multitexturing or not
//					-bDetail: render with a detail map or not
// Return Value:	None
//--------------------------------------------------------------
void CGEOMIPMAPPING::RenderFan( float cX, float cZ, float fSize, SGEOMM_NEIGHBOR neighbor, bool bMultiTex, bool bDetail )
{
	float fTexLeft, fTexBottom, fMidX, fMidZ, fTexRight, fTexTop;
	float fHalfSize= fSize/2.0f;

	//calculate the texture coordinates if we're not doing multitexturing, but still detail mapping
	if( bDetail && !bMultiTex )
	{
		//calculate the texture coordinates
		fTexLeft  = ( ( float )fabs( cX-fHalfSize )/m_iSize )*m_iRepeatDetailMap;
		fTexBottom= ( ( float )fabs( cZ-fHalfSize )/m_iSize )*m_iRepeatDetailMap;
		fTexRight = ( ( float )fabs( cX+fHalfSize )/m_iSize )*m_iRepeatDetailMap;
		fTexTop	  = ( ( float )fabs( cZ+fHalfSize )/m_iSize )*m_iRepeatDetailMap;

		fMidX= ( ( fTexLeft+fTexRight )/2 );
		fMidZ= ( ( fTexBottom+fTexTop )/2 );
	}

	//calculate the texture coordinates otherwise
	else
	{
		//calculate the texture coordinates
		fTexLeft  = ( ( float )fabs( cX-fHalfSize )/m_iSize );
		fTexBottom= ( ( float )fabs( cZ-fHalfSize )/m_iSize );
		fTexRight = ( ( float )fabs( cX+fHalfSize )/m_iSize );
		fTexTop	  = ( ( float )fabs( cZ+fHalfSize )/m_iSize );

		fMidX= ( ( fTexLeft+fTexRight )/2 );
		fMidZ= ( ( fTexBottom+fTexTop )/2 );
	}


	//begin a new triangle strip
	glBegin( GL_TRIANGLE_FAN );
		//render the CENTER vertex
		RenderVertex( cX, cZ, fMidX, fMidZ, bMultiTex );

		//render the LOWER-LEFT vertex
		RenderVertex( cX-fHalfSize, cZ-fHalfSize, fTexLeft, fTexBottom, bMultiTex );		

		//only render the next vertex if the left patch is NOT of a lower LOD
		if( neighbor.m_bLeft )
		{
			RenderVertex( cX-fHalfSize, cZ, fTexLeft, fMidZ, bMultiTex );
			m_iTrisPerFrame++;
		}
	
		//render the UPPER-LEFT vertex
		RenderVertex( cX-fHalfSize, cZ+fHalfSize, fTexLeft, fTexTop, bMultiTex );
		m_iTrisPerFrame++;

		//only render the next vertex if the upper patch is NOT of a lower LOD
		if( neighbor.m_bUp )
		{
			RenderVertex( cX, cZ+fHalfSize, fMidX, fTexTop, bMultiTex );
			m_iTrisPerFrame++;
		}

		//render the UPPER-RIGHT vertex
		RenderVertex( cX+fHalfSize, cZ+fHalfSize, fTexRight, fTexTop, bMultiTex );
		m_iTrisPerFrame++;

		//only render the next vertex if the right patch is NOT of a lower LOD
		if( neighbor.m_bRight )
		{
			//render the MID-RIGHT vertex
			RenderVertex( cX+fHalfSize, cZ, fTexRight, fMidZ, bMultiTex );
			m_iTrisPerFrame++;
		}

		//render the LOWER-RIGHT vertex
		RenderVertex( cX+fHalfSize, cZ-fHalfSize, fTexRight, fTexBottom, bMultiTex );
		m_iTrisPerFrame++;	

		//only render the next vertex if the bottom patch is NOT of a lower LOD
		if( neighbor.m_bDown )
		{
			//render the LOWER-MID vertex
			RenderVertex( cX, cZ-fHalfSize, fMidX, fTexBottom, bMultiTex );	
			m_iTrisPerFrame++;
		}

		//render the LOWER-LEFT vertex
		RenderVertex( cX-fHalfSize, cZ-fHalfSize, fTexLeft, fTexBottom, bMultiTex );
		m_iTrisPerFrame++;	

	//end the triangle strip
	glEnd( );
}