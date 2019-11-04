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
#include "randtab.h"


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
	char cGridData[128*128*4];	//texture for showing mesh
	float a0, a1, a2;
	float f;
	int x, y;
	int k, iLevel;

	m_iMaxLevel= iMaxLevel;
	m_fpLevelMDSize= new float [m_iMaxLevel+1];

	//generate table of displacement sizes versus levels (since this implementation
	//does not have crack prevention steps, we need to provide a quick hack to do it)
	for( iLevel=0; iLevel<=m_iMaxLevel; iLevel++ )
		m_fpLevelMDSize[iLevel]= 0.3f/( ( float )sqrt( ( float )( 1<<iLevel ) ) );

    //generate grid texture
	for( y=0; y < 128; y++ ) {
		for( x=0; x < 128; x++ ) {
			//create bump-shaped function f that is zero on each edge
			a0= ( float )y/127.0f;
			a1= ( float )( 127 - x )/127.0f;
			a2= ( float )( x - y )/127.0f;

			f= a0*a1*a2;
			f= ( float )sqrt( f );

			//quantize the function value and make into color
			//store in rgb components of texture entry
			k= ( int )floor( 1400.0f*f );

			if( k < 0 )
				k= 0;
			if( k > 255 )
				k= 255;

			cGridData[4*( y*128+x )+0]= 30+( SQR( k ) )/290;
			cGridData[4*( y*128+x )+1]= k;
			cGridData[4*( y*128+x )+2]= ( k<128 ? 0 : ( k-128 )*2 );
			cGridData[4*( y*128+x )+3]= 0;
		}
	}

	//make texture coordinates for three triangle vertices
	//all triangles use same tex coords when showing grid
	//suck in by half a texel to be correct for bilinear textures
	a0= 0.5f/128.0f;
	a1= 1.0f-a0;

	//set the texture coordinates for three vertices
	m_fGridTexCoords[0][0]= a0;	m_fGridTexCoords[0][1]= a0;
	m_fGridTexCoords[1][0]= a1;	m_fGridTexCoords[1][1]= a0;
	m_fGridTexCoords[2][0]= a1;	m_fGridTexCoords[2][1]= a1;

	//set up the gridview texture
	glGenTextures( 1,&m_uiGridID );
	glBindTexture( GL_TEXTURE_2D, m_uiGridID );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGBA,
				  GL_UNSIGNED_BYTE, 0 );

	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 128, 128,
					 GL_RGBA, GL_UNSIGNED_BYTE, cGridData );

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
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, m_uiGridID );

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	//render the roam mesh
	//compute four corners of the base square 
	for( i=0; i<4; i++ )
	{
		fVerts[i][0]= ( ( i & 1 ) ? 1.0f : -1.0f );
		fVerts[i][1]= 0.0f;
		fVerts[i][2]= ( ( i & 2 ) ? 1.0f : -1.0f );
	}

	//recurse on the two base triangles
	glBegin( GL_TRIANGLES );
		RenderSub( 0, fVerts[0], fVerts[1], fVerts[3] );
		RenderSub( 0, fVerts[3], fVerts[2], fVerts[0] );
	glEnd( );

	//end texturing
	glDisable( GL_TEXTURE_2D );
}

//--------------------------------------------------------------
// Name:			CROAM::RenderSub - private
// Description:		Render a subdivision (child) triangle
// Arguments:		-iLevel: current iLevel in the tessellation
//					-fpVert1, fpVert2, fpVert3: pointers to the vertex information
//												for the triangle
// Return Value:	None
//--------------------------------------------------------------
void CROAM::RenderSub( int iLevel, float* fpVert1, float* fpVert2, float* fpVert3 )
{
	unsigned char *pC;
	unsigned int s;
    float fNewVert[3];	// new (split) vertex
	float fMD, fDist;
	float fRandHash;
	int *pInt;
	int  i;

	//max midpoint-displacement size (CRACK-FIX HACK)
	fMD= m_fpLevelMDSize[iLevel];

    //compute split point of base edge
    fNewVert[0]= ( fpVert1[0] + fpVert3[0] )/2.0f;
	fNewVert[1]= 0.0f;
    fNewVert[2]= ( fpVert1[2] + fpVert3[2] )/2.0f;

	//determine random perturbation of center z using hash of x,y
	//random number lookup per byte of (x, z) data, all added
	pC= ( unsigned char* )fNewVert;
	for( i=0, s=0; i < 8; i++ )
		s+= randtab[( i<<8 ) | pC[i]];

	//stuff random hash value bits from s into float (float viewed
	//as an int, IEEE float tricks here...)
	pInt= ( int* )( &fRandHash );

	*pInt	  = 0x40000000+( s & 0x007fffff );
	fRandHash-= 3.0f;

	//the random height value for the vertex
    fNewVert[1]= ( ( fpVert1[1] + fpVert3[1] )/2.0f )+fRandHash*fMD;

    //distance calculation
    fDist= SQR( ( fNewVert[0] - m_pCamera->m_vecEyePos[0] ) )+
		   SQR( ( fNewVert[1] - m_pCamera->m_vecEyePos[1] ) )+
		   SQR( ( fNewVert[2] - m_pCamera->m_vecEyePos[2] ) );

    if( iLevel<m_iMaxLevel && SQR( fMD )>fDist*0.00001f )
	{
		//render the children
        RenderSub( iLevel+1, fpVert1, fNewVert, fpVert2 );
		RenderSub( iLevel+1, fpVert2, fNewVert, fpVert3 );
        
		//the current node doesn't need to be rendered,
		//since both of its children are
		return;
    }

	//send the vertices to the rendering API
	glTexCoord2fv( m_fGridTexCoords[0] ); glVertex3fv( fpVert1 );
	glTexCoord2fv( m_fGridTexCoords[1] ); glVertex3fv( fpVert2 );
	glTexCoord2fv( m_fGridTexCoords[2] ); glVertex3fv( fpVert3 );

	m_iVertsPerFrame+= 3;
	m_iTrisPerFrame++;
}