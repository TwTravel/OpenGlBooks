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
	unsigned char ucColor;
	int	iZ;
	int iX;

	//reset the stat variables
	m_iVertsPerFrame= 0;
	m_iTrisPerFrame = 0;

	//cull non camera-facing polygons
	glEnable( GL_CULL_FACE );

	//loop through the Z-axis of the terrain
	for( iZ=0; iZ<m_iSize-1; iZ++ )
	{
		//begin a new triangle strip
		glBegin( GL_TRIANGLE_STRIP );

			//loop through the X-axis of the terrain
			//this is where the triangle strip is constructed
			for( iX=0; iX<m_iSize-1; iX++ )
			{
				//use height-based coloring (high-points are
				//light, low points are dark)
				ucColor= GetTrueHeightAtPoint( iX, iZ );

				//set the color with OpenGL, and render the point
				glColor3ub( ucColor, ucColor, ucColor );
				glVertex3f( ( float )iX, GetScaledHeightAtPoint( iX, iZ ), ( float )iZ );

				//use height-based coloring (high-points are
				//light, low points are dark)
				ucColor= GetTrueHeightAtPoint( iX, iZ+1 );

				//set the color with OpenGL, and render the point
				glColor3ub( ucColor, ucColor, ucColor );
				glVertex3f( ( float )iX, GetScaledHeightAtPoint( iX, iZ+1 ), ( float )iZ+1 );

				//increase the vertex count by two (which is how many we sent to the API)
				m_iVertsPerFrame+= 2;

				//there are no triangles being rendered on the first X-loop, they just start the
				//triangle strip off
				if( iX!= 0 )
					m_iTrisPerFrame+= 2;
			}

		//end the triangle strip
		glEnd( );
	}
}