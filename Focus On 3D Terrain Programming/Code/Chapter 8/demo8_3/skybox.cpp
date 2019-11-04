//==============================================================
//==============================================================
//= skybox.cpp =================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= Code routines for a skybox class						   =
//==============================================================
//==============================================================


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "../Base Code/math_ops.h"
#include "../Base Code/gl_app.h"
#include "../Base Code/glext.h"
#include "../Base Code/image.h"

#include "skybox.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:		 CSKYBOX::LoadTexture - private
// Description:	 Load the skybox textures, side-by-side
// Arguments:	 -fCX, fCY, fCZ: Center vertex
//				 -fSize: size of the skybox
// Return Value: None
//--------------------------------------------------------------
bool CSKYBOX::LoadTexture( int iSide, char* szFilename )
{
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	if( !m_textures[iSide].Load( szFilename, GL_LINEAR, GL_LINEAR, false ) )
		return false;

	return true;
}

//--------------------------------------------------------------
// Name:		 CSKYBOX::Render - private
// Description:	 Render the skybox
// Arguments:	 None
// Return Value: None
//--------------------------------------------------------------
void CSKYBOX::Render( void )
{
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glEnable( GL_TEXTURE_2D );

	glPushMatrix( );
		glTranslatef( m_vecCenter[0], m_vecCenter[1], m_vecCenter[2] );

		//front face
		glBindTexture( GL_TEXTURE_2D, m_textures[SBX_FRONT].GetID( ) );
		glBegin( GL_TRIANGLE_FAN );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f( m_vecMax[0], m_vecMax[1], m_vecMax[2] );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f( m_vecMax[0], m_vecMin[1], m_vecMax[2] );
			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( m_vecMin[0], m_vecMin[1], m_vecMax[2] );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( m_vecMin[0], m_vecMax[1], m_vecMax[2] );
		glEnd( );

		//back face
		glBindTexture( GL_TEXTURE_2D, m_textures[SBX_BACK].GetID( ) );
		glBegin( GL_TRIANGLE_FAN );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f( m_vecMin[0], m_vecMax[1], m_vecMin[2] );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f( m_vecMin[0], m_vecMin[1], m_vecMin[2] );
			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( m_vecMax[0], m_vecMin[1], m_vecMin[2] );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( m_vecMax[0], m_vecMax[1], m_vecMin[2] );
		glEnd( );

		//right face
		glBindTexture( GL_TEXTURE_2D, m_textures[SBX_RIGHT].GetID( ) );
		glBegin( GL_TRIANGLE_FAN );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f( m_vecMax[0], m_vecMax[1], m_vecMin[2] );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f( m_vecMax[0], m_vecMin[1], m_vecMin[2] );
			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( m_vecMax[0], m_vecMin[1], m_vecMax[2] );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( m_vecMax[0], m_vecMax[1], m_vecMax[2] );
		glEnd( );

		//left face
		glBindTexture( GL_TEXTURE_2D, m_textures[SBX_LEFT].GetID( ) );
		glBegin( GL_TRIANGLE_FAN );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f( m_vecMin[0], m_vecMax[1], m_vecMax[2] );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f( m_vecMin[0], m_vecMin[1], m_vecMax[2] );
			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( m_vecMin[0], m_vecMin[1], m_vecMin[2] );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( m_vecMin[0], m_vecMax[1], m_vecMin[2] );
		glEnd( );
			
		//top face
		glBindTexture( GL_TEXTURE_2D, m_textures[SBX_TOP].GetID( ) );
		glBegin( GL_TRIANGLE_FAN );
			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( m_vecMin[0], m_vecMax[1], m_vecMax[2] );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( m_vecMin[0], m_vecMax[1], m_vecMin[2] );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f( m_vecMax[0], m_vecMax[1], m_vecMin[2] );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f( m_vecMax[0], m_vecMax[1], m_vecMax[2] );
		glEnd( );

		//bottom face
		glBindTexture( GL_TEXTURE_2D, m_textures[SBX_BOTTOM].GetID( ) );
		glBegin( GL_TRIANGLE_FAN );
			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( m_vecMin[0], m_vecMin[1], m_vecMin[2] );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( m_vecMin[0], m_vecMin[1], m_vecMax[2] );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f( m_vecMax[0], m_vecMin[1], m_vecMax[2] );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f( m_vecMax[0], m_vecMin[1], m_vecMin[2] );
		glEnd( );
	glPopMatrix( );
}