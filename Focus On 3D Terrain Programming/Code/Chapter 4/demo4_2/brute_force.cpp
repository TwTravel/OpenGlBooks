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
	unsigned char ucShadeLow, ucShadeHigh;	
	int	x, z;

	//reset the counting variables
	m_iVertsPerFrame= 0;
	m_iTrisPerFrame = 0;

	//cull non camera-facing polygons
	glEnable( GL_CULL_FACE );

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

					//get the brightness for the two points to be rendered
					ucShadeLow = GetBrightnessAtPoint( x, z );
					ucShadeHigh= GetBrightnessAtPoint( x, z+1 );

					//set the color with OpenGL, and render the point
					glColor4ub( ( unsigned char )( ucShadeLow*m_vecLightColor[0] ),
							    ( unsigned char )( ucShadeLow*m_vecLightColor[1] ),
							    ( unsigned char )( ucShadeLow*m_vecLightColor[2] ), 255 );
					glMultiTexCoord2fARB( GL_TEXTURE0_ARB, fTexLeft, fTexBottom );
					glMultiTexCoord2fARB( GL_TEXTURE1_ARB, fTexLeft*m_iRepeatDetailMap, fTexBottom*m_iRepeatDetailMap );
					glVertex3f( ( float )x, GetScaledHeightAtPoint( x, z ), ( float )z );		

					//set the color with OpenGL, and render the point
					glColor4ub( ( unsigned char )( ucShadeHigh*m_vecLightColor[0] ),
							    ( unsigned char )( ucShadeHigh*m_vecLightColor[1] ),
							    ( unsigned char )( ucShadeHigh*m_vecLightColor[2] ), 255 );
					glMultiTexCoord2fARB( GL_TEXTURE0_ARB, fTexLeft, fTexTop );
					glMultiTexCoord2fARB( GL_TEXTURE1_ARB, fTexLeft*m_iRepeatDetailMap, fTexTop*m_iRepeatDetailMap );
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

		//unbind the texture occupying the second texture unit
		glActiveTextureARB( GL_TEXTURE1_ARB );
		glDisable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, 0 );

		//unbind the texture occupying the first texture unit
		glActiveTextureARB( GL_TEXTURE0_ARB );
		glDisable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	//multitexturing is not enabled, which means we'll have to do a seperate texture
	//pass if detail mapping is needed
	else
	{
		if( m_bTextureMapping )
		{
			//bind the primary color texture (FOR THE PRIMARY TEXTURE PASS)
			glActiveTextureARB( GL_TEXTURE0_ARB );
			glEnable( GL_TEXTURE_2D );
			glBindTexture( GL_TEXTURE_2D, m_texture.GetID( ) );

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
							
						//get the brightness for the two points to be rendered
						ucShadeLow = GetBrightnessAtPoint( x, z );
						ucShadeHigh= GetBrightnessAtPoint( x, z+1 );

						//set the color with OpenGL, and render the point
						glColor4ub( ( unsigned char )( ucShadeLow*m_vecLightColor[0] ),
									( unsigned char )( ucShadeLow*m_vecLightColor[1] ),
									( unsigned char )( ucShadeLow*m_vecLightColor[2] ), 255 );
						glMultiTexCoord2fARB( GL_TEXTURE0_ARB, fTexLeft, fTexBottom );
						glVertex3f( ( float )x, GetScaledHeightAtPoint( x, z ), ( float )z );

						//set the color with OpenGL, and render the point
						glColor4ub( ( unsigned char )( ucShadeHigh*m_vecLightColor[0] ),
									( unsigned char )( ucShadeHigh*m_vecLightColor[1] ),
									( unsigned char )( ucShadeHigh*m_vecLightColor[2] ), 255 );
						glMultiTexCoord2fARB( GL_TEXTURE0_ARB, fTexLeft, fTexTop );
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

		//if there is no detail mapping, but a texture pass was made
		//then we don't need to continue
		else if( m_bTextureMapping )
			return;

		//the user didn't want detail texturing nor color texturing,
		//but we still need to render the terrain
		else
		{
			//unbind the first texture unit
			glActiveTextureARB( GL_TEXTURE0_ARB );
			glDisable( GL_TEXTURE_2D );
			glBindTexture( GL_TEXTURE_2D, 0 );
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
					
					//get the brightness for the two points to be rendered
					ucShadeLow = GetBrightnessAtPoint( x, z );
					ucShadeHigh= GetBrightnessAtPoint( x, z+1 );

					//set the color with OpenGL, and render the point
					glColor4ub( ( unsigned char )( ucShadeLow*m_vecLightColor[0] ),
							    ( unsigned char )( ucShadeLow*m_vecLightColor[1] ),
							    ( unsigned char )( ucShadeLow*m_vecLightColor[2] ), 255 );
					glMultiTexCoord2fARB( GL_TEXTURE0_ARB, fTexLeft*m_iRepeatDetailMap, fTexBottom*m_iRepeatDetailMap );
					glVertex3f( ( float )x, GetScaledHeightAtPoint( x, z ), ( float )z );

					//set the color with OpenGL, and render the point
					glColor4ub( ( unsigned char )( ucShadeHigh*m_vecLightColor[0] ),
							    ( unsigned char )( ucShadeHigh*m_vecLightColor[1] ),
							    ( unsigned char )( ucShadeHigh*m_vecLightColor[2] ), 255 );
					glMultiTexCoord2fARB( GL_TEXTURE0_ARB, fTexLeft*m_iRepeatDetailMap, fTexTop*m_iRepeatDetailMap );
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

		glDisable( GL_BLEND );
	}
}