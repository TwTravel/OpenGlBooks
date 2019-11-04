//==============================================================
//==============================================================
//= water.cpp ==================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= The routines for the CCAMERA class
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
// Name:		 CWATER::Init - public
// Description:	 Initialize the water mesh
// Arguments:	 -fWorldSize: length of the mesh in world-space
// Return Value: None
//--------------------------------------------------------------
void CWATER::Init( float fWorldSize )
{
	CVECTOR dx, dy;
	int j, k, x, z, *indexPtr;

	m_fWorldSize= fWorldSize;

	m_iNumVertices= SQR( WATER_RESOLUTION );
	m_iNumIndices = SQR( ( WATER_RESOLUTION-1 ) )*6;

	//calculate vertex location
	dx[0]= m_fWorldSize/( WATER_RESOLUTION-1 );
	dx[1]= 0.0f;
	dx[2]= 0.0f/( WATER_RESOLUTION-1 );
	dy[0]= 0.0f/( WATER_RESOLUTION-1 );
	dy[1]= 0.0f;
	dy[2]= m_fWorldSize/( WATER_RESOLUTION-1 );
	for( j=0; j<WATER_RESOLUTION; j++ )
	{
		for( k=0; k<WATER_RESOLUTION; k++ )
		{
			//set the vertex location
			m_pVertArray[( j*WATER_RESOLUTION )+k].Set( -1.0f + dx[0]*k + dy[0]*j,
														 0.0f + dx[1]*k + dy[1]*j,
														-1.0f + dx[2]*k + dy[2]*j );
		}
	}

	//calculate polygon indices
	x= 0;
	z= WATER_RESOLUTION;
	indexPtr= m_pPolyIndexArray;
	for( j=0; j<WATER_RESOLUTION-1; j++ )
	{
		for( k=0; k<WATER_RESOLUTION-1; k++ )
		{
			indexPtr[0]= x;
			indexPtr[1]= x+1;
			indexPtr[2]= z;
			indexPtr[3]= z;
			indexPtr[4]= x+1;
			indexPtr[5]= z+1;

			indexPtr+= 6;
			x++;
			z++;
		}
		x++;
		z++;
	}

	//start a water ripple at a random spot in the water field
	m_pVertArray[rand( )%( SQR( WATER_RESOLUTION ) )][1]= 200.0f;
}

//--------------------------------------------------------------
// Name:		 CWATER::Update - public
// Description:	 Update the vertices of the water mesh
// Arguments:	 -fDelta: timestep between the last update
// Return Value: None
//--------------------------------------------------------------
void CWATER::Update( float fDelta )
{
	float d, fTempF, fVert;
	int x, z;

	//calculate the current forces acting upon the water
	for( z=1; z<WATER_RESOLUTION-1; z++ )
	{
		for( x=1; x<WATER_RESOLUTION-1; x++ ) 
		{
			//how we are updating the vertices (the forces at 'X' apply to '0')
			//
			//    X | X | X
			//   -----------
			//    X | 0 | X
			//   -----------
			//    X | X | X
			//
			//set the temp variables (space-savers :))
			fTempF= m_pForceArray[( z*WATER_RESOLUTION )+x];
			fVert= m_pVertArray[( z*WATER_RESOLUTION )+x][1];

			//bottom
			d= fVert - m_pVertArray[( ( z-1 )*WATER_RESOLUTION )+x][1];
			m_pForceArray[( ( z-1 )*WATER_RESOLUTION )+x]+= d;
			fTempF-= d;

			//left
			d= fVert- m_pVertArray[( z*WATER_RESOLUTION )+( x-1 )][1];
			m_pForceArray[( z*WATER_RESOLUTION )+( x-1 )]+= d;
			fTempF-= d;

			//top
			d= ( fVert- m_pVertArray[( ( z+1 )*WATER_RESOLUTION )+x][1] );
			m_pForceArray[( ( z+1 )*WATER_RESOLUTION )+x]+= d;
			fTempF-= d;

			//right
			d= ( fVert- m_pVertArray[( z*WATER_RESOLUTION )+( x+1 )][1] );
			m_pForceArray[( z*WATER_RESOLUTION )+( x+1 )]+= d;
			fTempF-= d;

			//upper right
			d= ( fVert- m_pVertArray[( ( z+1 )*WATER_RESOLUTION )+( x+1 )][1] )*4.94974747f;
			m_pForceArray[( ( z+1 )*WATER_RESOLUTION )+( x+1 )]+= d;
			fTempF-= d;

			//lower left
			d= ( fVert- m_pVertArray[( ( z-1 )*WATER_RESOLUTION )+( x-1 )][1] )*4.94974747f;
			m_pForceArray[( ( z-1 )*WATER_RESOLUTION )+( x-1 )]+= d;
			fTempF-= d;

			//lower right
			d= ( fVert- m_pVertArray[( ( z-1 )*WATER_RESOLUTION )+( x+1 )][1] )*4.94974747f;
			m_pForceArray[( ( z-1 )*WATER_RESOLUTION )+( x+1 )]+= d;
			fTempF-= d;

			//upper left
			d= ( fVert- m_pVertArray[( ( z+1 )*WATER_RESOLUTION )+( x-1 )][1] )*4.94974747f;
			m_pForceArray[( ( z+1 )*WATER_RESOLUTION )+( x-1 )]+= d;
			fTempF-= d;

			m_pForceArray[( z*WATER_RESOLUTION )+x]= fTempF;
		}
	}

	//calculate velocity, and update the poly field
	for (x=0; x<m_iNumVertices; x++)
	{
		m_pVelArray[x]+= ( m_pForceArray[x]*fDelta );
		m_pVertArray[x][1]+= m_pVelArray[x];

		m_pForceArray[x]= 0.0f;
	}
}

//--------------------------------------------------------------
// Name:		 CWATER::CalcNormals - public
// Description:	 Calculate the mesh's polygon normals
// Arguments:	 None
// Return Value: None
//--------------------------------------------------------------
void CWATER::CalcNormals( void )
{
	CVECTOR* vert, *normal;
	float tmpf;
	int i, j;

	for( i=0; i<WATER_RESOLUTION; i++ )
	{
		for( j=0; j<WATER_RESOLUTION; j++ )
		{
			vert  = &m_pVertArray[( i*WATER_RESOLUTION )+j];
			normal= &m_pNormalArray[( i*WATER_RESOLUTION )+j];

			//calculate the normal value for the current normal
			normal->m_fVec[0]= 0.0f;
			normal->m_fVec[1]= 1.0f;
			normal->m_fVec[2]= 0.0f;

			//above
			if( i!=0 )
            {
				if( j!=0 )
				{
					normal->m_fVec[0]+= -vert[-WATER_RESOLUTION-1][1];
					normal->m_fVec[2]+= -vert[-WATER_RESOLUTION-1][1];
				}
				else
				{
					normal->m_fVec[0]+= -vert[-WATER_RESOLUTION][1];
					normal->m_fVec[2]+= -vert[-WATER_RESOLUTION][1];
				}

				normal->m_fVec[0] += -vert[-WATER_RESOLUTION][1]*2.0f;

				if( j!=( WATER_RESOLUTION-1 ) )
				{
					normal->m_fVec[0]+= -vert[-WATER_RESOLUTION+1][1];
					normal->m_fVec[2]+= vert[-WATER_RESOLUTION+1][1];
				}
				else
				{
					normal->m_fVec[0]+= -vert[-WATER_RESOLUTION][1];
					normal->m_fVec[2]+= vert[-WATER_RESOLUTION][1];
				}
			}
			else
			{
				normal->m_fVec[0]+= -vert[0][1];
				normal->m_fVec[0]+= -vert[0][1]*2.0f;
				normal->m_fVec[0]+= -vert[0][1];

				normal->m_fVec[2]+= -vert[0][1];
				normal->m_fVec[2]+= vert[0][1];
			}

			//current line
			if( j!=0 )
				normal->m_fVec[2]+= -vert[-1][1]*2.0f;
			else
				normal->m_fVec[2]+= -vert[0][1]*2.0f;

			if( j!=( WATER_RESOLUTION-1 ) )
				normal->m_fVec[2]+= vert[1][1]*2.0f;
			else
				normal->m_fVec[2]+= vert[0][1]*2.0f;

			//below
			if( i!=( WATER_RESOLUTION-1 ) )
			{
				if( j!=0 )
				{
					normal->m_fVec[0]+=  vert[WATER_RESOLUTION-1][1];
					normal->m_fVec[2]+= -vert[WATER_RESOLUTION-1][1];
				}
				else
				{
					normal->m_fVec[0]+=  vert[WATER_RESOLUTION][1];
					normal->m_fVec[2]+= -vert[WATER_RESOLUTION][1];
				}

				normal->m_fVec[0] += vert[WATER_RESOLUTION][1]*2.0f;

				if( j!=WATER_RESOLUTION-1 )
				{
					normal->m_fVec[0] += vert[WATER_RESOLUTION+1][1];
					normal->m_fVec[2] += vert[WATER_RESOLUTION+1][1];
				}
				else
				{
					normal->m_fVec[0] += vert[WATER_RESOLUTION][1];
					normal->m_fVec[2] += vert[WATER_RESOLUTION][1];
				}
			}
			else
			{
				normal->m_fVec[0]+= vert[0][1];
				normal->m_fVec[0]+= vert[0][1]*2.0f;
				normal->m_fVec[0]+= vert[0][1];

				normal->m_fVec[2]+= -vert[0][1];
				normal->m_fVec[2]+= vert[0][1];
			}

			//normalize the normal (sounds crazy, eh?)
			tmpf= 1.0f/( float )sqrt( normal->m_fVec[0]*normal->m_fVec[0] + normal->m_fVec[2]*normal->m_fVec[2] + 1.0f );
			normal->m_fVec[0]*= tmpf;
			normal->m_fVec[1]*= tmpf;
			normal->m_fVec[2]*= tmpf;
		}
	}
}

//--------------------------------------------------------------
// Name:		 CWATER::Render - public
// Description:	 Render the water mesh
// Arguments:	 -bUseCVA: use compiled vertex arrays or not
// Return Value: None
//--------------------------------------------------------------
void CWATER::Render( bool bUseCVA )
{
	//bind the water's "water map"
	glBindTexture( GL_TEXTURE_2D, m_refmapID );
	glEnable( GL_TEXTURE_2D );

	//use sphere mapping to auto-gen texture coordinates
	glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
	glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
	glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );

	glColor4f( m_vecColor[0], m_vecColor[1], m_vecColor[2], m_fTransparency );

	//enable vertex arrays and send OpenGL the vertex buffer pointer
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, sizeof( CVECTOR ), &m_pVertArray );

	//enable normal arrays and send OpenGL the normal buffer pointer
	glEnableClientState( GL_NORMAL_ARRAY );
	glNormalPointer( GL_FLOAT, sizeof( CVECTOR ), &m_pNormalArray );

	glEnable( GL_TEXTURE_GEN_S );
	glEnable( GL_TEXTURE_GEN_T );
	glEnable( GL_TEXTURE_GEN_R );

	//lock the arrays
	if( bUseCVA )
		glLockArraysEXT( 0, m_iNumVertices );

	//draw the water patch
	glDrawElements( GL_TRIANGLES, m_iNumIndices, GL_UNSIGNED_INT, m_pPolyIndexArray );

	//unlock the arrays
	if( bUseCVA )
		glUnlockArraysEXT( );

	//disable vertex arrays
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );

	glDisable( GL_TEXTURE_2D );
	glDisable( GL_BLEND );

	//disable automatic tex-coordinate generation
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_TEXTURE_GEN_R );
}

//--------------------------------------------------------------
// Name:		 CWATER::LoadReflectionMap - public
// Description:	 Load the water's reflection map
// Arguments:	 -szFilename: filename of the reflection map image
// Return Value: None
//--------------------------------------------------------------
void CWATER::LoadReflectionMap( char* szFilename )
{
	CIMAGE image;

	//load the image
	image.Load( szFilename, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true );

	//save the image's texture ID
	m_refmapID= image.GetID( );
}