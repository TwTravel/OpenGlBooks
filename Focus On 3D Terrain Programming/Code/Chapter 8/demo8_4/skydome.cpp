//==============================================================
//==============================================================
//= skydome.cpp ================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= Routines for a skydome (half-sphere) system				   =
//==============================================================
//==============================================================


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <math.h>

#include "../Base Code/math_ops.h"
#include "../Base Code/gl_app.h"
#include "../Base Code/image.h"

#include "skydome.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:		 CSKYDOME::Init - public
// Description:	 Create the skydome
// Arguments:	 -fTheta/fPhi: "latitude" and "longitude" of the dome
//				 -fRadius: radius of the dome in world space
// Return Value: None
//--------------------------------------------------------------
void CSKYDOME::Init( float fTheta, float fPhi, float fRadius )
{
	CVECTOR vecUV;
	const float fDTOR= PI/180.0f;
	int iTheta, iPhi, i, n= 0;

	//calculate the number of vertices for the skydome
	m_iNumVertices= ( int )( ( 360/fTheta )*( 90/fPhi )*4 );

	//allocate memory for the vertex and texture coordinate buffers
	m_fVertices  = new float [m_iNumVertices*3];
	m_fTexCoords= new float [m_iNumVertices*2];

	//initialize the values for the buffers
	memset( m_fVertices, 0, sizeof( float )*m_iNumVertices*3 );
	memset( m_fTexCoords, 0, sizeof( float )*m_iNumVertices*2 );

	//procedurally generate the dome
	for( iPhi=0; iPhi<=( 90-fPhi ); iPhi+=( int )fPhi )
	{
		for( iTheta=0; iTheta<=( 360-fTheta ); iTheta+=( int )fTheta )
		{
			//compute the vertex at phi, theta
			m_fVertices[( n*3 )+0]= fRadius*sinf( iPhi*fDTOR )*cosf( iTheta*fDTOR );
			m_fVertices[( n*3 )+1]= fRadius*sinf( iPhi*fDTOR )*sinf( iTheta*fDTOR );
			m_fVertices[( n*3 )+2]= fRadius*cosf( iPhi*fDTOR );

			//create a vector from the origin to this vertex and normalize it
			vecUV.Set( m_fVertices[( n*3 )+0], m_fVertices[( n*3 )+1], m_fVertices[( n*3 )+2] );
			vecUV.Normalize( );

			//compute the texture coordinates for the current vertex
			m_fTexCoords[( n*2 )+0]= ( float )( atan2( vecUV[0], vecUV[2] )/( PI*2 ) )+0.5f;
			m_fTexCoords[( n*2 )+1]= ( float )( asinf( vecUV[1] )/PI )+0.5f;		
			n++;

			//compute the vertex at phi+phi, theta
			m_fVertices[( n*3 )+0]= fRadius*sinf( ( iPhi+fPhi )*fDTOR )*cosf( iTheta*fDTOR );
			m_fVertices[( n*3 )+1]= fRadius*sinf( ( iPhi+fPhi )*fDTOR )*sinf( iTheta*fDTOR );
			m_fVertices[( n*3 )+2]= fRadius*cosf( ( iPhi+fPhi )*fDTOR );
			
			//create a vector from the origin to this vertex and normalize it
			vecUV.Set( m_fVertices[( n*3 )+0], m_fVertices[( n*3 )+1], m_fVertices[( n*3 )+2] );
			vecUV.Normalize( );

			//compute the texture coordinates for the current vertex
			m_fTexCoords[( n*2 )+0]= ( float )( atan2( vecUV[0], vecUV[2] )/( PI*2 ) )+0.5f;
			m_fTexCoords[( n*2 )+1]= ( float )( asinf( vecUV[1] )/PI )+0.5f;		
			n++;

			//compute the vertex at phi, theta+theta
			m_fVertices[( n*3 )+0]= fRadius*sinf( fDTOR*iPhi )*cosf( fDTOR*( iTheta+fTheta ) );
			m_fVertices[( n*3 )+1]= fRadius*sinf( fDTOR*iPhi )*sinf( fDTOR*( iTheta+fTheta ) );
			m_fVertices[( n*3 )+2]= fRadius*cosf( fDTOR*iPhi );
			
			//create a vector from the origin to this vertex and normalize it
			vecUV.Set( m_fVertices[( n*3 )+0], m_fVertices[( n*3 )+1], m_fVertices[( n*3 )+2] );
			vecUV.Normalize( );

			//compute the texture coordinates for the current vertex
			m_fTexCoords[( n*2 )+0]= ( float )( atan2( vecUV[0], vecUV[2] )/( PI*2 ) )+0.5f;
			m_fTexCoords[( n*2 )+1]= ( float )( asinf( vecUV[1] )/PI )+0.5f;		
			n++;

			if( iPhi>-90 && iPhi<90 )
			{
				//compute the vertex at phi+phi, theta+theta
				m_fVertices[( n*3 )+0]= fRadius*sinf( ( iPhi+fPhi )*fDTOR )*cosf( fDTOR*( iTheta+fTheta ) );
				m_fVertices[( n*3 )+1]= fRadius*sinf( ( iPhi+fPhi )*fDTOR )*sinf( fDTOR*( iTheta+fTheta ) );
				m_fVertices[( n*3 )+2]= fRadius*cosf( ( iPhi+fPhi )*fDTOR );
				
				//create a vector from the origin to this vertex and normalize it
				vecUV.Set( m_fVertices[( n*3 )+0], m_fVertices[( n*3 )+1], m_fVertices[( n*3 )+2] );
				vecUV.Normalize( );

				//compute the texture coordinates for the current vertex
				m_fTexCoords[( n*2 )+0]= ( float )( atan2( vecUV[0], vecUV[2] )/( PI*2 ) )+0.5f;
				m_fTexCoords[( n*2 )+1]= ( float )( asinf( vecUV[1] )/PI )+0.5f;		
				n++;
			}
		}
	}

	//fix the texture-seam problem
	for( i=0; i<m_iNumVertices-3; i++ )
	{
		int i0= ( i*2 );
		int i1= ( ( i+1 )*2 );
		int i2= ( ( i+2 )*2 );

		if( ( m_fTexCoords[i0+0]-m_fTexCoords[i1+0] )>0.9f )
			m_fTexCoords[i1+0]+= 1.0f;

		if( ( m_fTexCoords[i1+0]-m_fTexCoords[i0+0] )>0.9f )
			m_fTexCoords[i0+0]+= 1.0f;

		if( ( m_fTexCoords[i0+0]-m_fTexCoords[i2+0] )>0.9f )
			m_fTexCoords[i2+0]+= 1.0f;

		if( ( m_fTexCoords[i2+0]-m_fTexCoords[i0+0] )>0.9f )
			m_fTexCoords[i0+0]+= 1.0f;

		if( ( m_fTexCoords[i1+0]-m_fTexCoords[i2+0] )>0.9f )
			m_fTexCoords[i2+0]+= 1.0f;

		if( ( m_fTexCoords[i2+0]-m_fTexCoords[i1+0] )>0.9f )
			m_fTexCoords[i1+0]+= 1.0f;

		if( ( m_fTexCoords[i0+1]-m_fTexCoords[i1+1] )>0.8f )
			m_fTexCoords[i1+1]+= 1.0f;

		if( ( m_fTexCoords[i1+1]-m_fTexCoords[i0+1] )>0.8f )
			m_fTexCoords[i0+1]+= 1.0f;

		if( ( m_fTexCoords[i0+1]-m_fTexCoords[i2+1] )>0.8f )
			m_fTexCoords[i2+1]+= 1.0f;

		if( ( m_fTexCoords[i2+1]-m_fTexCoords[i0+1] )>0.8f )
			m_fTexCoords[i0+1]+= 1.0f;

		if( ( m_fTexCoords[i1+1]-m_fTexCoords[i2+1] )>0.8f )
			m_fTexCoords[i2+1]+= 1.0f;

		if( ( m_fTexCoords[i2+1]-m_fTexCoords[i1+1] )>0.8f )
			m_fTexCoords[i1+1]+= 1.0f;
	}
}

//--------------------------------------------------------------
// Name:		 CSKYDOME::Shutdown - public
// Description:	 Shutdown the skydome system
// Arguments:	 None
// Return Value: None Render the water mesh
//--------------------------------------------------------------
void CSKYDOME::Shutdown( void )
{
	delete[] m_fTexCoords;
	delete[] m_fVertices;
}

//--------------------------------------------------------------
// Name:		 CSKYDOME::Render - public
// Description:	 Render the skydome
// Arguments:	 -fDelta: time passed since the previous frame (for rotation purposes)
//				 -bRotate: rotate the skydome or not to simulate cloud movement
// Return Value: None
//--------------------------------------------------------------
void CSKYDOME::Render( float fDelta, bool bRotate )
{
	static float fRot= 0.0f;

	//bind the dome texture
	glBindTexture( GL_TEXTURE_2D, m_texture.GetID( ) );
	glEnable( GL_TEXTURE_2D );

	glPushMatrix( );
		glTranslatef( m_vecCenter[0], m_vecCenter[1], m_vecCenter[2] );

		//rotate the dome to simulate cloud movement
		if( bRotate )
		{
			fRot+= fDelta;
			glRotatef( fRot, 0.0f, 1.0f, 0.0f );
		}
		//orient the dome correctly
		glRotatef( 270, 1.0f, 0.0f, 0.0f );

		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

		//enable vertex arrays and send OpenGL the vertex buffer pointer
		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, m_fVertices );

		//enable normal arrays and send OpenGL the normal buffer pointer
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );	
		glTexCoordPointer( 2, GL_FLOAT, 0, m_fTexCoords );

		//lock the arrays
		glLockArraysEXT( 0, m_iNumVertices );

		//render the skydome
		glDrawArrays( GL_TRIANGLE_STRIP, 0, m_iNumVertices );

		//unlock the arrays
		glUnlockArraysEXT( );

		//disable vertex arrays
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glPopMatrix( );
}
