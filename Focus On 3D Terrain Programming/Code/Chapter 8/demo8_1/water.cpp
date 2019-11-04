//==============================================================
//==============================================================
//= water.cpp ==================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= A simple water system
//==============================================================
//==============================================================


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "../Base Code/math_ops.h"
#include "../Base Code/gl_app.h"
#include "../Base Code/image.h"

#include "water.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:		 CWATER::Render - public
// Description:	 Render the water mesh
// Arguments:	 -fWorldSize: the size of the water patch in worldspace
// Return Value: None
//--------------------------------------------------------------
void CWATER::Render( float fWorldSize )
{
	static float fU= 0.0f;
	static float fV= 0.0f;

	//bind the water's "water map"
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );

	glColor4f( m_vecColor[0], m_vecColor[1], m_vecColor[2], m_fTransparency );

	//render the first water patch
	glBindTexture( GL_TEXTURE_2D, m_texID );
	glEnable( GL_TEXTURE_2D );
	glBegin( GL_TRIANGLE_STRIP );
		glTexCoord2f( fU,    fV );		glVertex3f( 0.0f, 0.0f, 0.0f );
		glTexCoord2f( fU+16, fV );		glVertex3f( fWorldSize, 0.0f, 0.0f );
		glTexCoord2f( fU,    fV+16 );	glVertex3f( 0.0f, 0.0f, fWorldSize);
		glTexCoord2f( fU+16, fV+16 );	glVertex3f( fWorldSize, 0.0f, fWorldSize );
	glEnd( );

	//"animate" the water
	fV+= 0.01f;

	glDisable( GL_TEXTURE_2D );
	glDisable( GL_BLEND );
}

//--------------------------------------------------------------
// Name:		 CWATER::LoadReflectionMap - public
// Description:	 Load the water's reflection map
// Arguments:	 -szFilename: filename of the reflection map image
// Return Value: None
//--------------------------------------------------------------
void CWATER::LoadTextureMaps( char* szFilename )
{
	CIMAGE image;

	//load the image
	image.Load( szFilename, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true );

	//save the image's texture ID
	m_texID= image.GetID( );
}