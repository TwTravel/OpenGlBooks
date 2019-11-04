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
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CROAM::Init( void )
{
	char cGridTexData[128*128*4];	//texture for showing mesh

	float a0, a1, a2;
	float f;
	int x, y;
	int k, iLevel;

	m_fpLevel2dzsize= new float [LEVEL_MAX+1];

	//generate table of displacement sizes versus levels
	for( iLevel=0; iLevel<=LEVEL_MAX; iLevel++ )
		m_fpLevel2dzsize[iLevel]= 3.0f/( ( float )sqrt( ( float )( 1<<iLevel ) ) );

    //generate grid texture
	for( y=0; y<128; y++ )
	{
		for( x=0; x<128; x++ )
		{
			//create bump-shaped function f that is zero on each edge
			a0= ( float )y/127.0f;
			a1= ( float )( 127-x )/127.0f;
			a2= ( float )( x-y )/127.0f;

			f= a0*a1*a2;
			f= ( float )sqrt( f );

			//quantize the function value and make into color
			//store in rgb components of texture entry
			k= ( int )floor( 1400.0f*f );

			if( k<0 )
				k= 0;
			if( k>255 )
				k= 255;

			cGridTexData[4*( y*128+x )+0]= 30+( SQR( k ) )/290;
			cGridTexData[4*( y*128+x )+1]= k;
			cGridTexData[4*( y*128+x )+2]= ( k<128 ? 0 : ( k-128 )*2 );
			cGridTexData[4*( y*128+x )+3]= 0;
		}
	}

	//make texture coordinates for three triangle vertices
	//all triangles use same tex coords when showing grid
	//suck in by half a texel to be correct for bilinear textures
	a0= 0.5f/128.0f;
	a1= 1.0f-a0;

	//set the texture coordinates for three vertices
	m_fGridtex_t[0][0]= a0;	m_fGridtex_t[0][1]= a0;
	m_fGridtex_t[1][0]= a1;	m_fGridtex_t[1][1]= a0;
	m_fGridtex_t[2][0]= a1;	m_fGridtex_t[2][1]= a1;

	//set up the gridview texture
	glGenTextures( 1,&m_uiGridTexID );
	glBindTexture( GL_TEXTURE_2D, m_uiGridTexID );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGBA,
				  GL_UNSIGNED_BYTE, 0 );

	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 128, 128,
					 GL_RGBA, GL_UNSIGNED_BYTE, cGridTexData );
}

//--------------------------------------------------------------
// Name:			CROAM::Shutdown - public
// Description:		Shutdown the ROAM engine
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CROAM::Shutdown( void )
{
	delete[] m_fpLevel2dzsize;
}

//--------------------------------------------------------------
// Name:			CROAM::Update - public
// Description:		Update the ROAM engine
// Arguments:		-vecCameraEye: The camera'uiS eye position
// Return Value:	None
//--------------------------------------------------------------
void CROAM::Update( CVECTOR vecCameraEye )
{
	m_vecCameraEye= vecCameraEye;
}

//--------------------------------------------------------------
// Name:			CROAM::Render - public
// Description:		Render the ROAM engine
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CROAM::Render( void )
{
	float verts[4][3];
	int i;    

	//reset the debug counters
	m_iVertsPerFrame= 0;
	m_iTrisPerFrame = 0;

	//turn on textured drawing (with the grid texture for now)
	glBindTexture( GL_TEXTURE_2D, m_uiGridTexID );
	glEnable( GL_TEXTURE_2D );

	//render the roam mesh
	//compute four corners of the base square 
	for( i=0; i<4; i++ )
	{
		verts[i][0]= ( ( i & 1 ) ? 1.0f : -1.0f );
		verts[i][1]= 0.0f;
		verts[i][2]= ( ( i & 2 ) ? 1.0f : -1.0f );
	}

	glColor3f( 1.0f, 1.0f, 1.0f );

	//recurse on the two base triangles
	RenderSub( 0, verts[0], verts[1], verts[3] );
	RenderSub( 0, verts[3], verts[2], verts[0] );

	//end texturing
	glDisable( GL_TEXTURE_2D );
}

//--------------------------------------------------------------
// Name:			CROAM::RenderSub - private
// Description:		Render a subdivision (child) triangle
// Arguments:		-iLevel: current level in the tessellation
//					-fpVert1, fpVert2, fpVert3: pointers to the vertex information
//								 for the triangle
// Return Value:	None
//--------------------------------------------------------------
void CROAM::RenderSub( int iLevel, float* fpVert1, float* fpVert2, float* fpVert3 )
{
	unsigned char* pC;
	unsigned int uiS;
    float fNewVert[3];	// new (split) vertex
	float fSqrEdge, fDistance;
	float fRandHeight;
	int* pInt;
	int  i;

    //squared length of edge (fpVert1-fpVert3)
    fSqrEdge= SQR( ( fpVert3[0]-fpVert1[0] ) )+
			  SQR( ( fpVert3[1]-fpVert1[1] ) )+
			  SQR( ( fpVert3[2]-fpVert1[2] ) );

    //compute split point of base edge
    fNewVert[0]= ( fpVert1[0]+fpVert3[0] )/2.0f;
	fNewVert[1]= 0.0f;
    fNewVert[2]= ( fpVert1[2]+fpVert3[2] )/2.0f;

	//determine random perturbation of center z using hash of x,y
	//random number lookup per byte of (x, z) data, all added
	pC= ( unsigned char* )fNewVert;
	for( i=0, uiS=0; i<8; i++ )
		uiS+= randtab[( i<<8 ) | pC[i]];

	//stuff random hash value bits from uiS into float (float viewed
	//as an int, IEEE float tricks here...)
	pInt= ( int* )( &fRandHeight );
        
	*pInt	    = 0x40000000+( uiS & 0x007fffff );
	fRandHeight-= 3.0f;
	fRandHeight	= 0.10f*fRandHeight*m_fpLevel2dzsize[iLevel];

	//the random height value for the vertex
    fNewVert[1]= ( ( fpVert1[1]+fpVert3[1] )/2.0f )+fRandHeight;

    //distance calculation
    fDistance= SQR( ( fNewVert[0]-m_vecCameraEye[0] ) )+
			   SQR( ( fNewVert[1]-m_vecCameraEye[1] ) )+
			   SQR( ( fNewVert[2]-m_vecCameraEye[2] ) );

    if( iLevel<LEVEL_MAX && fSqrEdge>fDistance*0.005f )
	{
		//render the children
        RenderSub( iLevel+1, fpVert1, fNewVert, fpVert2 );
		RenderSub( iLevel+1, fpVert2, fNewVert, fpVert3 );
        
		//the current node doesn't need to be rendered,
		//since both of its children are
		return;
    }

	//send the vertices to the rendering API
    glBegin( GL_TRIANGLE_STRIP );
		glTexCoord2fv( m_fGridtex_t[0] ); glVertex3fv( fpVert1 );
		glTexCoord2fv( m_fGridtex_t[1] ); glVertex3fv( fpVert2 );
		glTexCoord2fv( m_fGridtex_t[2] ); glVertex3fv( fpVert3 );
    glEnd( );

	m_iVertsPerFrame+= 3;
	m_iTrisPerFrame++;
}