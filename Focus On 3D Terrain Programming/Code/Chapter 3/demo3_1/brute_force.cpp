//==============================================================
//==============================================================
//= brute_force.cpp ============================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= This file (along with brute_force.h) contains the		   =
//= information for a brute force terrain implementation.	   =
//==============================================================
//==============================================================


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <stdio.h>

#include "../Base Code/gl_app.h"

#include "brute_force.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:			CBRUTE_FORCE::Render - public
// Description:		Render the terrain height field
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CBRUTE_FORCE::Render( void )
{
	float fTexLeft, fTexBottom, fTexTop;
	int	x, z;

	//reset the counting variables
	m_iVertsPerFrame= 0;
	m_iTrisPerFrame = 0;

	//cull non camera-facing polygons
	glEnable( GL_CULL_FACE );

	//if the texture is loaded then enable texturing and bind our texture
	if( m_bTextureMapping )
	{
		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, m_texture.GetID( ) );
	}

	//loop through the Z-axis of the terrain
	for( z=0; z<m_iSize-1; z++ )
	{
		//begin a new triangle strip
		glBegin( GL_TRIANGLE_STRIP );

			//loop through the X-axis of the terrain
			//this is where the triangle strip is constructed
			for( x=0; x<m_iSize-1; x++ )
			{
				//calculate the texture coordinates
				fTexLeft  = ( float )x/m_iSize;
				fTexBottom= ( float )z/m_iSize;
				fTexTop	  = ( float )( z+1 )/m_iSize;

				//set the color with OpenGL, and render the point
				glColor3ub( 255, 255, 255 );
				glTexCoord2f( fTexLeft, fTexBottom );
				glVertex3f( ( float )x, GetScaledHeightAtPoint( x, z ), ( float )z );

				//set the color with OpenGL, and render the point
				glColor3ub( 255, 255, 255 );
				glTexCoord2f( fTexLeft, fTexTop );
				glVertex3f( ( float )x, GetScaledHeightAtPoint( x, z+1 ), ( float )z+1 );

				//increase the vertex count by two (which is how many we sent to the API)
				m_iVertsPerFrame+= 2;

				//there are no triangles being rendered on the first X-loop, they just start the
				//triangle strip off
				if( x!= 0 )
					m_iTrisPerFrame+= 2;
			}

		//end the triangle strip
		glEnd( );
	}
}