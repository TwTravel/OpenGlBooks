//==============================================================
//==============================================================
//= ROAM.cpp ===================================================
//= Original coders: Trent Polack (trent@voxelsoft.com) and	   =
//=				     Mark Duchaineau						   =
//==============================================================
//= The header file for the ROAM implementation				   =
//==============================================================
//==============================================================


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <windows.h>
#include <math.h>
#include <GL/gl.h>

#include "ROAM.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:			CROAM::Init - public
// Description:		Initialize the ROAM engine
// Arguments:		-iMaxLevel: the maximum iLevel we can recurse down to
//					-pCamera: pointer to a camera instance
// Return Value:	None
//--------------------------------------------------------------
void CROAM::Init( int iMaxLevel, CCAMERA* pCamera )
{
	int iLevel;

	m_iMaxLevel= iMaxLevel;
	m_fpLevelMDSize= new float [m_iMaxLevel+1];

	//generate table of displacement sizes versus levels (since this implementation
	//does not have crack prevention steps, we need to provide a quick hack to do it)
	for( iLevel=0; iLevel<=m_iMaxLevel; iLevel++ )
		m_fpLevelMDSize[iLevel]= 255.0f/( ( float )sqrt( ( float )( 1<<iLevel ) ) );

	//set the class's camera pointer
	m_pCamera= pCamera;
}

//--------------------------------------------------------------
// Name:			CROAM::Shutdown - public
// Description:		Shutdown the ROAM engine
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CROAM::Shutdown( void )
{
	delete[] m_fpLevelMDSize;
}

//--------------------------------------------------------------
// Name:			CROAM::Update - public
// Description:		Update the ROAM engine
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CROAM::Update( )
{
}

//--------------------------------------------------------------
// Name:			CROAM::Render - public
// Description:		Render the ROAM engine
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CROAM::Render( void )
{
	float fVerts[4][3];
	int i;    

	//reset the debug counters
	m_iVertsPerFrame= 0;
	m_iTrisPerFrame = 0;

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

	//render the roam mesh
	//compute four corners of the base square 
	for( i=0; i<4; i++ )
	{
		fVerts[i][0]= ( ( i & 1 ) ? m_iSize-1 : 0.0f );
		fVerts[i][1]= 0.0f;
		fVerts[i][2]= ( ( i & 2 ) ? m_iSize-1 : 0.0f );
	}

	//recurse on the two base triangles
	glBegin( GL_TRIANGLES );
		RenderSub( 0, fVerts[0], fVerts[1], fVerts[3], 0 );
		RenderSub( 0, fVerts[3], fVerts[2], fVerts[0], 0 );
	glEnd( );

	//end texturing
	glActiveTextureARB( GL_TEXTURE0_ARB );
	glDisable( GL_TEXTURE_2D );

	glActiveTextureARB( GL_TEXTURE1_ARB );
	glDisable( GL_TEXTURE_2D );
}

//--------------------------------------------------------------
// Name:			CROAM::RenderSub - private
// Description:		Render a subdivision (child) triangle
// Arguments:		-iLevel: current iLevel in the tessellation
//					-fpVert1, fpVert2, fpVert3: pointers to the vertex information
//								 for the triangle
//					-iCull: culling flag
// Return Value:	None
//--------------------------------------------------------------
void CROAM::RenderSub( int iLevel, float* fpVert1, float* fpVert2, float* fpVert3, int iCull )
{
	unsigned char ucShade;
	float fNewVert[3];	// new (split) vertex
	float fTexCoords[2];
	float fDist;
	float fSqrBound, fSqrBoundTemp;

    //compute split point of base edge
    fNewVert[0]= ( fpVert1[0] + fpVert3[0] )/2.0f;
    fNewVert[2]= ( fpVert1[2] + fpVert3[2] )/2.0f;
	fNewVert[1]= GetTrueHeightAtPoint( ( int )fNewVert[0], ( int )fNewVert[2] );

	//calculate the bounding sphere for the current triangle
	//calculation 1
	fSqrBound= SQR( ( fpVert1[0]-fNewVert[0] ) )+
			   SQR( ( fpVert1[1]-fNewVert[1] ) )+
			   SQR( ( fpVert1[2]-fNewVert[2] ) );

	//calculation 2
	fSqrBoundTemp= SQR( ( fpVert2[0]-fNewVert[0] ) )+
				   SQR( ( fpVert2[1]-fNewVert[1] ) )+
				   SQR( ( fpVert2[2]-fNewVert[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( fSqrBoundTemp>fSqrBound )
		fSqrBound= fSqrBoundTemp;

	//calculation 3
	fSqrBoundTemp= SQR( ( fpVert3[0]-fNewVert[0] ) )+
				   SQR( ( fpVert3[1]-fNewVert[1] ) )+
				   SQR( ( fpVert3[2]-fNewVert[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( fSqrBoundTemp>fSqrBound )
		fSqrBound= fSqrBoundTemp;

	//check the triangle's bounding sphere against the view frustum
	if( iCull!=CULL_ALLIN )
	{
		float r;
		int j, m;

		//perform culling against the view frustum
		for( j=0, m=1; j<6; j++, m<<= 1 )
		{
			if( !( iCull & m ) )
			{
				r= m_pCamera->m_viewFrustum[j][0]*fNewVert[0] +
				   m_pCamera->m_viewFrustum[j][1]*fNewVert[1] +
				   m_pCamera->m_viewFrustum[j][2]*fNewVert[2] +
				   m_pCamera->m_viewFrustum[j][3];

				//check for frustum inclusion
				if( SQR( r )>fSqrBound )
				{
					//check to see if the triangle is actually within the viewing frustum
					if( r<0.0f )
						return;
					
					//triangle is within view
					iCull|= m;
				}
			}
		} 
	}

    //distance calculation
    fDist= SQR( ( fNewVert[0]-m_pCamera->m_vecEyePos[0] ) )+
		   SQR( ( fNewVert[1]-m_pCamera->m_vecEyePos[1] ) )+
		   SQR( ( fNewVert[2]-m_pCamera->m_vecEyePos[2] ) );

    if( iLevel<m_iMaxLevel && SQR( m_fpLevelMDSize[iLevel] )>fDist*0.00001f )
	{
		//render the children
        RenderSub( iLevel+1, fpVert1, fNewVert, fpVert2, iCull );
		RenderSub( iLevel+1, fpVert2, fNewVert, fpVert3, iCull );
        
		//the current triangle doesn't need to be rendered,
		//since both of its children are
		return;
    }

	//send the vertices to the rendering API
	//send the information for the first vertex (color->texture->vertex)
	ucShade= GetBrightnessAtPoint( ( int )fpVert1[0], ( int )fpVert1[2] );
	glColor3ub( ucShade, ucShade, ucShade );
	fTexCoords[0]= fpVert1[0]/m_iSize;	fTexCoords[1]= fpVert1[2]/m_iSize;

	if( m_bTextureMapping )
		glMultiTexCoord2fARB( GL_TEXTURE0_ARB, fTexCoords[0], fTexCoords[1] );
	if( m_bDetailMapping )
		glMultiTexCoord2fARB( GL_TEXTURE1_ARB, fTexCoords[0]*m_iRepeatDetailMap, fTexCoords[1]*m_iRepeatDetailMap );

	glVertex3fv( fpVert1 );

	//send the information for the second vertex (color->texture->vertex)
	ucShade= GetBrightnessAtPoint( ( int )fpVert2[0], ( int )fpVert2[2] );
	glColor3ub( ucShade, ucShade, ucShade );
	fTexCoords[0]= fpVert2[0]/m_iSize;	fTexCoords[1]= fpVert2[2]/m_iSize;

	if( m_bTextureMapping )
		glMultiTexCoord2fARB( GL_TEXTURE0_ARB, fTexCoords[0], fTexCoords[1] );
	if( m_bDetailMapping )
		glMultiTexCoord2fARB( GL_TEXTURE1_ARB, fTexCoords[0]*m_iRepeatDetailMap, fTexCoords[1]*m_iRepeatDetailMap );

	glVertex3fv( fpVert2 );

	//send the information for the third vertex (color->texture->vertex)
	ucShade= GetBrightnessAtPoint( ( int )fpVert3[0], ( int )fpVert3[2] );
	glColor3ub( ucShade, ucShade, ucShade );
	fTexCoords[0]= fpVert3[0]/m_iSize;	fTexCoords[1]= fpVert3[2]/m_iSize;

	if( m_bTextureMapping )
		glMultiTexCoord2fARB( GL_TEXTURE0_ARB, fTexCoords[0], fTexCoords[1] );
	if( m_bDetailMapping )
		glMultiTexCoord2fARB( GL_TEXTURE1_ARB, fTexCoords[0]*m_iRepeatDetailMap, fTexCoords[1]*m_iRepeatDetailMap );

	glVertex3fv( fpVert3 );

	m_iVertsPerFrame+= 3;
	m_iTrisPerFrame++;
}