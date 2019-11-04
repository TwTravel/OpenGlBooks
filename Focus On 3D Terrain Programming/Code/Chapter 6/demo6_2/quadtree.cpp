//==============================================================
//==============================================================
//= quadtree.cpp ===============================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= This file (along with quadtree.cpp) contains all of the    =
//= information for the quadtree terrain component.            =
//==============================================================
//==============================================================


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <stdio.h>

#include "../Base Code/gl_app.h"

#include "quadtree.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- GLOBALS ----------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//fan iFanCode table
char g_cQTFanCode[] = { 10, 8, 8, 12, 8, 0, 12, 14, 8, 12, 0, 14, 12, 14, 14, 0 };
char g_cQTFanStart[]= { 3,  3, 0,  3, 1, 0,  0,  3, 2,  2, 0,  2,  1,  1,  0, 0 };


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:			CQUADTREE::Init - public
// Description:		Initialize the quadtree engine
// Arguments:		None
// Return Value:	A boolean value: -true: successful init
//									 -false: unsuccessful init
//--------------------------------------------------------------
bool CQUADTREE::Init( void )
{
	int x, z;

	//create memory for the quadtree matrix
	m_ucpQuadMtrx= new unsigned char [SQR( m_iSize )];
 	if( m_ucpQuadMtrx==NULL )
	{
		g_log.Write( LOG_FAILURE, "Could not initialize memory for the quadtree matrix" );
		return false;
	}

	//initialize the quadtree matrix to 'true' (for all possible nodes)
	for( z=0; z<m_iSize; z++ )
	{
		for( x=0; x<m_iSize; x++ )
		{
			m_ucpQuadMtrx[GetMatrixIndex( x, z )]= 1;
		}
	}

	//propagate the roughness in the height map (so we can apply more triangles in rough spots of terrain)
	PropagateRoughness( );

	//intialization was a success
	g_log.Write( LOG_SUCCESS, "The quadtree terrain engine has been successfully initialized" );
	return true;
}

//--------------------------------------------------------------
// Name:			CQUADTREE::Shutdown - public
// Description:		Shutdown the quadtree engine
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CQUADTREE::Shutdown( void )
{
	//free the memory stored in the quadtree matrix
	if( m_ucpQuadMtrx )
		delete[] m_ucpQuadMtrx;
}

//--------------------------------------------------------------
// Name:			CQUADTREE::Update - public
// Description:		Update the quadtree engine
// Arguments:		-camera: a camera object
// Return Value:	None
//--------------------------------------------------------------
void CQUADTREE::Update( CCAMERA* pCamera )
{
	float fCenter;

	m_pCamera= pCamera;

	//calculate the center of the terrain mesh
	fCenter= ( m_iSize-1 )/2.0f;

	//build the mesh through top-down quadtree traversal
	RefineNode( fCenter, fCenter, m_iSize );
}

//--------------------------------------------------------------
// Name:			CQUADTREE::Render - public
// Description:		Render the quadtree engine
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CQUADTREE::Render( void )
{
	float fCenter;

	//reset the counting variables
	m_iVertsPerFrame= 0;
	m_iTrisPerFrame = 0;

	//calculate the center of the mesh
	fCenter= ( m_iSize-1 )/2.0f;

	//enable back-face culling
	glDisable( GL_CULL_FACE );

	//use hardware multitexturing for the texture map and the detail map
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

		//render the main node (which will recurse down to the other nodes)
		RenderNode( fCenter, fCenter, m_iSize, true, true );
	}
	
	//no hardware multitexturing available, or the user only wants to render
	//the detail texture or the color texture
	else
	{
		if( m_bTextureMapping )
		{
			//bind the primary color texture (FOR THE PRIMARY TEXTURE PASS)
			glActiveTextureARB( GL_TEXTURE0_ARB );
			glEnable( GL_TEXTURE_2D );
			glBindTexture( GL_TEXTURE_2D, m_texture.GetID( ) );

			//render the main node (which will recurse down to the other nodes)
			RenderNode( fCenter, fCenter, m_iSize, false, false );
		}

		if( !( m_bTextureMapping && !m_bDetailMapping ) )
		{
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
			//render the main node (which will recurse down to the other nodes)
			RenderNode( fCenter, fCenter, m_iSize, false, m_bDetailMapping );
		}
	}

	glDisable( GL_BLEND );

	//unbind the texture occupying the second texture unit
	glActiveTextureARB( GL_TEXTURE1_ARB );
	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );

	//unbind the texture occupying the first texture unit
	glActiveTextureARB( GL_TEXTURE0_ARB );
	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

//--------------------------------------------------------------
// Name:			CQUADTREE::PropagateRoughness - private
// Description:		Propagate the roughness of the height map (so more 
//					triangles will get applied to rougher areas of the map)
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CQUADTREE::PropagateRoughness( void )
{
	float fKUpperBound;
	int iDH, iD2, iLocalD2, iLocalH;
	int iEdgeLength, iEdgeOffset;
	int iChildOffset;
	int x, z;

	//set the iEdgeLength to 3 (lowest length possible)
	iEdgeLength= 3;

	//start off at the lowest level of detail, and traverse up to the highest node (lowest detail)
	while( iEdgeLength<=m_iSize )
	{
		//offset of node edges (since all edges are the same length
		iEdgeOffset= ( iEdgeLength-1 )>>1;

		//offset of the node's children's edges
		iChildOffset= ( iEdgeLength-1 )>>2;

		for( z=iEdgeOffset; z<m_iSize; z+=( iEdgeLength-1 ) )
		{
			for( x=iEdgeOffset; x<m_iSize; x+=( iEdgeLength-1 ) )
			{
				//compute "iLocalD2" values for this node
				//upper-mid
				iLocalD2= ( int )ceil( abs( ( ( GetTrueHeightAtPoint( x-iEdgeOffset, z+iEdgeOffset )+
											    GetTrueHeightAtPoint( x+iEdgeOffset, z+iEdgeOffset ) )>>1 )-
											    GetTrueHeightAtPoint( x,			z+iEdgeOffset ) ) );

				//right-mid
				iDH= ( int )ceil( abs( ( ( GetTrueHeightAtPoint( x+iEdgeOffset, z+iEdgeOffset )+
										   GetTrueHeightAtPoint( x+iEdgeOffset, z-iEdgeOffset ) )>>1 )-
										   GetTrueHeightAtPoint( x+iEdgeOffset, z ) ) );
				iLocalD2= MAX( iLocalD2, iDH );

				//bottom-mid
				iDH= ( int )ceil( abs( ( ( GetTrueHeightAtPoint( x-iEdgeOffset, z-iEdgeOffset )+
										   GetTrueHeightAtPoint( x+iEdgeOffset, z-iEdgeOffset ) )>>1 )-
										   GetTrueHeightAtPoint( x,		   z-iEdgeOffset ) ) );
				iLocalD2= MAX( iLocalD2, iDH );

				//left-mid
				iDH= ( int )ceil( abs( ( ( GetTrueHeightAtPoint( x-iEdgeOffset, z+iEdgeOffset )+
										   GetTrueHeightAtPoint( x-iEdgeOffset, z-iEdgeOffset ) )>>1 )-
										   GetTrueHeightAtPoint( x-iEdgeOffset, z ) ) );
				iLocalD2= MAX( iLocalD2, iDH );

	 			//bottom-left to top-right diagonal
				iDH= ( int )ceil( abs( ( ( GetTrueHeightAtPoint( x-iEdgeOffset, z-iEdgeOffset )+
										   GetTrueHeightAtPoint( x+iEdgeOffset, z+iEdgeOffset ) )>>1 )-
										   GetTrueHeightAtPoint( x,		   z ) ) );
				iLocalD2= MAX( iLocalD2, iDH );

				//bottom-right to top-left diagonal
				iDH= ( int )ceil( abs( ( ( GetTrueHeightAtPoint( x+iEdgeOffset, z-iEdgeOffset )+
										   GetTrueHeightAtPoint( x-iEdgeOffset, z+iEdgeOffset ) )>>1 )-
										   GetTrueHeightAtPoint( x, z ) ) );
				iLocalD2= MAX( iLocalD2, iDH );

				//make iLocalD2 a value between 0-255
				iLocalD2= ( int )ceil( ( iLocalD2*3.0f )/iEdgeLength );

				//test minimally sized block
				if( iEdgeLength==3 )
				{
					iD2= iLocalD2;

					//compute the "iLocalH" value
					//upper right
					iLocalH= GetTrueHeightAtPoint( x+iEdgeOffset, z+iEdgeOffset );

					//right mid
					iLocalH= MAX( iLocalH, GetTrueHeightAtPoint( x+iEdgeOffset, z ) );

					//lower right
					iLocalH= MAX( iLocalH, GetTrueHeightAtPoint( x+iEdgeOffset, z-iEdgeOffset ) );

					//bottom mid
					iLocalH= MAX( iLocalH, GetTrueHeightAtPoint( x,	z-iEdgeOffset ) );

					//lower left
					iLocalH= MAX( iLocalH, GetTrueHeightAtPoint( x-iEdgeOffset, z-iEdgeOffset ) );

					//left mid
					iLocalH= MAX( iLocalH, GetTrueHeightAtPoint( x-iEdgeOffset, z ) );

					//upper left
					iLocalH= MAX( iLocalH, GetTrueHeightAtPoint( x-iEdgeOffset, z+iEdgeOffset ) );

					//upper mid
					iLocalH= MAX( iLocalH, GetTrueHeightAtPoint( x, z+iEdgeOffset ) );

					//center
					iLocalH= MAX( iLocalH, GetTrueHeightAtPoint( x,	z ) );

					//store the maximum iLocalH value in the matrix
					m_ucpQuadMtrx[GetMatrixIndex( x+1, z )]= iLocalH;
				}
				
				else
				{
					fKUpperBound= 1.0f*m_fMinResolution/( 2.0f*( m_fMinResolution-1.0f ) );

					//use d2 values from farther up on the quadtree
					iD2= ( int )ceil( MAX( fKUpperBound*( float )GetQuadMatrixData( x, z ),			    ( float )iLocalD2 ) );
					iD2= ( int )ceil( MAX( fKUpperBound*( float )GetQuadMatrixData( x-iEdgeOffset, z ), ( float )iD2 ) );
					iD2= ( int )ceil( MAX( fKUpperBound*( float )GetQuadMatrixData( x+iEdgeOffset, z ),	( float )iD2 ) );
					iD2= ( int )ceil( MAX( fKUpperBound*( float )GetQuadMatrixData( x, z+iEdgeOffset ), ( float )iD2 ) );
					iD2= ( int )ceil( MAX( fKUpperBound*( float )GetQuadMatrixData( x, z-iEdgeOffset ), ( float )iD2 ) );

					//get the max local height values of the 4 nodes (LL, LR, UL, UR)
					iLocalH= GetTrueHeightAtPoint( x+iChildOffset, z+iChildOffset );
					iLocalH= MAX( iLocalH, GetTrueHeightAtPoint( x+iChildOffset, z-iChildOffset ) );
					iLocalH= MAX( iLocalH, GetTrueHeightAtPoint( x-iChildOffset, z-iChildOffset ) );
					iLocalH= MAX( iLocalH, GetTrueHeightAtPoint( x-iChildOffset, z+iChildOffset ) );

					//store the max value in the quadtree matrix
					m_ucpQuadMtrx[GetMatrixIndex( x+1, z )]= iLocalH;
				}

				//store the values we calculated for iD2 into the quadtree matrix
				m_ucpQuadMtrx[GetMatrixIndex( x, z )]  = iD2;
				m_ucpQuadMtrx[GetMatrixIndex( x-1, z )]= iD2;

				//propogate the value up the quadtree
				m_ucpQuadMtrx[GetMatrixIndex( x-iEdgeOffset, z-iEdgeOffset )]= MAX( GetQuadMatrixData( x-iEdgeOffset, z-iEdgeOffset ), iD2 );
				m_ucpQuadMtrx[GetMatrixIndex( x-iEdgeOffset, z+iEdgeOffset )]= MAX( GetQuadMatrixData( x-iEdgeOffset, z+iEdgeOffset ), iD2 );
				m_ucpQuadMtrx[GetMatrixIndex( x+iEdgeOffset, z+iEdgeOffset )]= MAX( GetQuadMatrixData( x+iEdgeOffset, z+iEdgeOffset ), iD2 );
				m_ucpQuadMtrx[GetMatrixIndex( x+iEdgeOffset, z-iEdgeOffset )]= MAX( GetQuadMatrixData( x+iEdgeOffset, z-iEdgeOffset ), iD2 );
			}
		}

		//move up to the next quadtree level (lower level of detail)
		iEdgeLength= ( iEdgeLength<<1 )-1;
	}
}

//--------------------------------------------------------------
// Name:			CQUADTREE::RefineNode - private
// Description:		Refine a quadtree node (update the quadtree matrix)
// Arguments:		-x, z: center of current node
//					-iEdgeLength: length of the current node's edge
//					-camera: camera object (for calculating distance)
// Return Value:	None
//--------------------------------------------------------------
void CQUADTREE::RefineNode( float x, float z, int iEdgeLength )
{
	float fViewDistance, f;
	float fChildOffset;
	int iChildEdgeLength;
	int iBlend;

	//calculate the distance from the current point (L1 NORM, which, essentially, is a faster version of the 
	//normal distance equation you may be used to... yet again, thanks to Chris Cookson)
	fViewDistance= ( float )( fabs( m_pCamera->m_vecEyePos[0]-( x*m_vecScale[0] ) )+
							  fabs( m_pCamera->m_vecEyePos[1]-GetQuadMatrixData( ( int )x+1, ( int )z ) )+
							  fabs( m_pCamera->m_vecEyePos[2]-( z*m_vecScale[2] ) ) );


	//compute the 'f' value (as stated in Rottger's whitepaper of this algorithm)
	f= fViewDistance/( ( float )iEdgeLength*m_fMinResolution*
					       MAX( m_fDetailLevel*( float )GetQuadMatrixData( ( int )x-1, ( int )z )/3, 1.0f ) );

	if( f<1.0f )
		iBlend= 255;		
	else
		iBlend= 0;

	//store the blend factor in the quadtree matrix
	m_ucpQuadMtrx[GetMatrixIndex( ( int )x, ( int )z )]= iBlend;

	if( iBlend )
	{
		//else, we need to recurse down farther into the quadtree
		if( !( iEdgeLength<=3 ) )
		{
			fChildOffset    = ( float )( ( iEdgeLength-1 ) >> 2 );
			iChildEdgeLength= ( iEdgeLength+1 ) >> 1;

			//refine the various child nodes
			//lower left
			RefineNode( x-fChildOffset, z-fChildOffset, iChildEdgeLength );

			//lower right
			RefineNode( x+fChildOffset, z-fChildOffset, iChildEdgeLength );

			//upper left
			RefineNode( x-fChildOffset, z+fChildOffset, iChildEdgeLength );

			//upper right
			RefineNode( x+fChildOffset, z+fChildOffset, iChildEdgeLength );
		}
	} 
}

//--------------------------------------------------------------
// Name:			CQUADTREE::RenderNode - private
// Description:		Render leaf (no children) quadtree nodes
// Arguments:		-x, z: center of current node
//					-iEdgeLength: length of the current node's edge
//					-bMultiTex: use multitexturing for rendering
//					-bDetail: use a detail map when rendering
// Return Value:	None
//--------------------------------------------------------------
void CQUADTREE::RenderNode( float x, float z, int iEdgeLength, bool bMultiTex, bool bDetail )
{
	float fTexLeft, fTexBottom, fMidX, fMidZ, fTexRight, fTexTop;
	float fChildOffset;
	float fEdgeOffset;
	int iStart, iFanCode;
	int iChildEdgeLength;
	int iChildOffset;
	int iEdgeOffset;
	int iAdjOffset;
	int iFanLength;
	int iFanPosition;
	int iBlend;
	int iX, iZ;

	iX= ( int )x;
	iZ= ( int )z;

	//compute the edge offset of the current node
	iEdgeOffset= ( iEdgeLength-1 )/2;
	fEdgeOffset= ( iEdgeLength-1 )/2.0f;

	//compute the offset to the nodes near the current node
	iAdjOffset= iEdgeLength-1;

	if( bDetail && !bMultiTex )
	{
		//calculate the texture coordinates
		fTexLeft  = ( ( float )fabs( x-fEdgeOffset )/m_iSize )*m_iRepeatDetailMap;
		fTexBottom= ( ( float )fabs( z-fEdgeOffset )/m_iSize )*m_iRepeatDetailMap;
		fTexRight = ( ( float )fabs( x+fEdgeOffset )/m_iSize )*m_iRepeatDetailMap;
		fTexTop	  = ( ( float )fabs( z+fEdgeOffset )/m_iSize )*m_iRepeatDetailMap;

		fMidX= ( ( fTexLeft+fTexRight )/2.0f );
		fMidZ= ( ( fTexBottom+fTexTop )/2.0f );
	}

	else
	{
		//calculate the texture coordinates
		fTexLeft  = ( ( float )fabs( x-fEdgeOffset )/m_iSize );
		fTexBottom= ( ( float )fabs( z-fEdgeOffset )/m_iSize );
		fTexRight = ( ( float )fabs( x+fEdgeOffset )/m_iSize );
		fTexTop	  = ( ( float )fabs( z+fEdgeOffset )/m_iSize );

		fMidX= ( ( fTexLeft+fTexRight )/2.0f );
		fMidZ= ( ( fTexBottom+fTexTop )/2.0f );
	}

	//get the blend factor from the current quadtree value
	iBlend= GetQuadMatrixData( iX, iZ );

	if( iBlend>0 )
	{
		//is this the smallest node?
		if( iEdgeLength<=3 )
		{
			//render a triangle fan to represent the node
			glBegin( GL_TRIANGLE_FAN );

				//center vertex
				RenderVertex( x, z, fMidX, fMidZ, bMultiTex );

				//lower left vertex
				RenderVertex( x-fEdgeOffset, z-fEdgeOffset, fTexLeft, fTexBottom, bMultiTex );

				//lower mid, skip if the adjacent node is of a lower detail level
				if( ( ( iZ-iAdjOffset )<0 ) || GetQuadMatrixData( iX, iZ-iAdjOffset )!=0 )
				{
					RenderVertex( x, z-fEdgeOffset, fMidX, fTexBottom, bMultiTex );
					m_iTrisPerFrame++;
				}

				//bottom right vertex
				RenderVertex( x+fEdgeOffset, z-fEdgeOffset, fTexRight, fTexBottom, bMultiTex );
				m_iTrisPerFrame++;

				//right mid, skip if the adjacent node is of a lower detail level
				if( ( ( iX+iAdjOffset )>=m_iSize ) || GetQuadMatrixData( iX+iAdjOffset, iZ )!=0 )
				{
					RenderVertex( x+fEdgeOffset, z, fTexRight, fMidZ, bMultiTex );
					m_iTrisPerFrame++;
				}

				//upper right vertex
				RenderVertex( x+fEdgeOffset, z+fEdgeOffset, fTexRight, fTexTop, bMultiTex );
				m_iTrisPerFrame++;

				//upper mid, skip if the adjacent node is of a lower detail level
				if( ( ( iZ+iAdjOffset )>=m_iSize ) || GetQuadMatrixData( iX, iZ+iAdjOffset )!=0 )
				{
					RenderVertex( x, z+fEdgeOffset, fMidX, fTexTop, bMultiTex );
					m_iTrisPerFrame++;
				}

				//upper left vertex
				RenderVertex( x-fEdgeOffset, z+fEdgeOffset, fTexLeft, fTexTop, bMultiTex );
				m_iTrisPerFrame++;

				//left mid, skip if the adjacent node is of a lower detail level
				if( ( ( iX-iAdjOffset )<0 ) || GetQuadMatrixData( iX-iAdjOffset, iZ )!=0 )
				{
					RenderVertex( x-fEdgeOffset, z, fTexLeft, fMidZ, bMultiTex );
					m_iTrisPerFrame++;
				}

				//bottom left vertex again
				RenderVertex( x-fEdgeOffset, z-fEdgeOffset, fTexLeft, fTexBottom, bMultiTex );
				m_iTrisPerFrame++;
			glEnd( );
			return;

		}
		
		else
		{
			//calculate the child node's offset values
			iChildOffset= ( iEdgeLength-1 )/4;
			fChildOffset= ( float )iChildOffset;

			//calculate the edge length of the child nodes
			iChildEdgeLength= ( iEdgeLength+1 )/2;

			//calculate the bit-iFanCode for the fan arrangement (which fans need to be rendered)
			//upper right
			iFanCode = ( GetQuadMatrixData( iX+iChildOffset, iZ+iChildOffset )!=0 )*8;

			//upper left
			iFanCode|= ( GetQuadMatrixData( iX-iChildOffset, iZ+iChildOffset )!=0 )*4;

			//lower left
			iFanCode|= ( GetQuadMatrixData( iX-iChildOffset, iZ-iChildOffset )!=0 )*2;

			//lower right
			iFanCode|= ( GetQuadMatrixData( iX+iChildOffset, iZ-iChildOffset )!=0 );

			//now, use the previously calculate codes, and render some tri-fans :)
			//this node has four children, no rendering is needed (for this node at least), but
			//we need to recurse down to this node's children
			if( iFanCode==QT_NO_FAN )
			{
				//lower left
				RenderNode( x-fChildOffset, z-fChildOffset, iChildEdgeLength );

				//lower right
				RenderNode( x+fChildOffset, z-fChildOffset, iChildEdgeLength );

				//upper left
				RenderNode( x-fChildOffset, z+fChildOffset, iChildEdgeLength );

				//upper right
				RenderNode( x+fChildOffset, z+fChildOffset, iChildEdgeLength );
				return;
			}

			//render the lower left and upper right fans
			if( iFanCode==QT_LL_UR )
			{
				//the upper right fan
				glBegin( GL_TRIANGLE_FAN );
					//center vertex
					RenderVertex( x, z, fMidX, fMidZ, bMultiTex );

					//right mid vertex
					RenderVertex( x+fEdgeOffset, z, fTexRight, fMidZ, bMultiTex );

					//upper right vertex
					RenderVertex( x+fEdgeOffset, z+fEdgeOffset, fTexRight, fTexTop, bMultiTex );
					m_iTrisPerFrame++;

					//upper mid vertex
					RenderVertex( x, z+fEdgeOffset, fMidX, fTexTop, bMultiTex );
					m_iTrisPerFrame++;
				glEnd( );

				//lower left fan
				glBegin( GL_TRIANGLE_FAN );
					//center vertex
					RenderVertex( x, z, fMidX, fMidZ, bMultiTex );

					//left mid
					RenderVertex( x-fEdgeOffset, z, fTexLeft, fMidZ, bMultiTex );

					//bottom left
					RenderVertex( x-fEdgeOffset, z-fEdgeOffset, fTexLeft, fTexBottom, bMultiTex );
					m_iTrisPerFrame++;

					//bottom mid
					RenderVertex( x, z-fEdgeOffset, fMidX, fTexBottom, bMultiTex );
					m_iTrisPerFrame++;
				glEnd( );

				//recurse further down to the upper left and lower right nodes
				RenderNode( x-fChildOffset, z+fChildOffset, iChildEdgeLength );
				RenderNode( x+fChildOffset, z-fChildOffset, iChildEdgeLength );
				return;

			}

			//render the lower-right and upper-left triangles fans
			if( iFanCode==QT_LR_UL )
			{
				//upper left fan
				glBegin( GL_TRIANGLE_FAN );
					//center vertex
					RenderVertex( x, z, fMidX, fMidZ, bMultiTex );

					//upper mid vertex
					RenderVertex( x, z+fEdgeOffset, fMidX, fTexTop, bMultiTex );

					//upper left vertex
					RenderVertex( x-fEdgeOffset, z+fEdgeOffset, fTexLeft, fTexTop, bMultiTex );
					m_iTrisPerFrame++;

					//left mid vertex
					RenderVertex( x-fEdgeOffset, z, fTexLeft, fMidZ, bMultiTex );
					m_iTrisPerFrame++;
				glEnd( );

				//lower right fan
				glBegin( GL_TRIANGLE_FAN );
					//center vertex
					RenderVertex( x, z, fMidX, fMidZ, bMultiTex );

					//lower mid vertex
					RenderVertex( x, z-fEdgeOffset, fMidX, fTexBottom, bMultiTex );

					//lower right vertex
					RenderVertex( x+fEdgeOffset, z-fEdgeOffset, fTexRight, fTexBottom, bMultiTex );
					m_iTrisPerFrame++;

					//right mid vertex
					RenderVertex( x+fEdgeOffset, z, fTexRight, fMidZ, bMultiTex );
					m_iTrisPerFrame++;
				glEnd( );

				//recurse further down to the upper right and lower left nodes
				RenderNode( x+fChildOffset, z+fChildOffset, iChildEdgeLength );
				RenderNode( x-fChildOffset, z-fChildOffset, iChildEdgeLength );
				return;
			}

			//this node is a leaf-node, render a complete fan
			if( iFanCode==QT_COMPLETE_FAN )
			{
				glBegin( GL_TRIANGLE_FAN );
					//center vertex
					RenderVertex( x, z, fMidX, fMidZ, bMultiTex );

					//render the lower left vertex
					RenderVertex( x-fEdgeOffset, z-fEdgeOffset, fTexLeft, fTexBottom, bMultiTex );

					//lower mid, skip if the adjacent node is of a lower detail level
					if( ( ( iZ-iAdjOffset )<0 ) || GetQuadMatrixData( iX, iZ-iAdjOffset )!=0 )
					{
						RenderVertex( x, z-fEdgeOffset, fMidX, fTexBottom, bMultiTex );
						m_iTrisPerFrame++;
					}

					//lower right vertex
					RenderVertex( x+fEdgeOffset, z-fEdgeOffset, fTexRight, fTexBottom, bMultiTex );
					m_iTrisPerFrame++;

					//right mid, skip if the adjacent node is of a lower detail level
					if( ( ( iX+iAdjOffset )>=m_iSize ) || GetQuadMatrixData( iX+iAdjOffset, iZ )!=0 )
					{
						RenderVertex( x+fEdgeOffset, z, fTexRight, fMidZ, bMultiTex );
						m_iTrisPerFrame++;
					}

					//upper right vertex
					RenderVertex( x+fEdgeOffset, z+fEdgeOffset, fTexRight, fTexTop, bMultiTex );
					m_iTrisPerFrame++;

					//upper mid, skip if the adjacent node is of a lower detail level
					if( ( ( iZ+iAdjOffset )>=m_iSize ) || GetQuadMatrixData( iX, iZ+iAdjOffset )!=0 )
					{
						RenderVertex( x, z+fEdgeOffset, fMidX, fTexTop, bMultiTex );
						m_iTrisPerFrame++;
					}

					//upper left
					RenderVertex( x-fEdgeOffset, z+fEdgeOffset, fTexLeft, fTexTop, bMultiTex );
					m_iTrisPerFrame++;

					//left mid, skip if the adjacent node is of a lower detail level
					if( ( ( iX-iAdjOffset )<0 ) || GetQuadMatrixData( iX-iAdjOffset, iZ )!=0 )
					{
						RenderVertex( x-fEdgeOffset, z, fTexLeft, fMidZ, bMultiTex );
						m_iTrisPerFrame++;
					}

					//lower left vertex
					RenderVertex( x-fEdgeOffset, z-fEdgeOffset, fTexLeft, fTexBottom, bMultiTex );
					m_iTrisPerFrame++;
				glEnd( );
				return;
			}

			//the remaining cases are only partial fans, so we need to figure out what to render
			//(thanks to Chris Cookson for this idea)
			iStart= g_cQTFanStart[iFanCode];

			iFanLength= 0;

			//calculate the fan length by computing the index of the first non-zero bit in g_cQTFanCode
			while( !( ( ( long )g_cQTFanCode[iFanCode] )&( 1<<iFanLength ) ) && iFanLength<8 )
				iFanLength++;

			//render a triangle fan
			glBegin( GL_TRIANGLE_FAN );
				//center vertex
				RenderVertex( x, z, fMidX, fMidZ, bMultiTex );

				//render a triangle fan
				for( iFanPosition=iFanLength; iFanPosition>0; iFanPosition-- )
				{
					switch( iStart )
					{
						//lower right node
						case QT_LR_NODE:
							//lower mid, skip if the adjacent node is of a lower detail level
							if( ( ( iZ-iAdjOffset )<0 ) || GetQuadMatrixData( iX, iZ-iAdjOffset )!=0 || iFanPosition==iFanLength )
							{
								RenderVertex( x, z-fEdgeOffset, fMidX, fTexBottom, bMultiTex );
								m_iTrisPerFrame++;
							}

							//lower right vertex
							RenderVertex( x+fEdgeOffset, z-fEdgeOffset, fTexRight, fTexBottom, bMultiTex );
							m_iTrisPerFrame++;

							//finish off the fan with a right mid vertex
							if( iFanPosition==1 )
							{
								RenderVertex( x+fEdgeOffset, z, fTexRight, fMidZ, bMultiTex );
								m_iTrisPerFrame++;
							}
							break;

						//lower left node
						case QT_LL_NODE:
							//left mid, skip if the adjacent node is of a lower detail level
							if( ( ( x-iAdjOffset )<0 ) || GetQuadMatrixData( iX-iAdjOffset, iZ )!=0 || iFanPosition==iFanLength )
							{
								RenderVertex( x-fEdgeOffset, z, fTexLeft, fMidZ, bMultiTex );
								m_iTrisPerFrame++;
							}

							//lower left vertex
							RenderVertex( x-fEdgeOffset, z-fEdgeOffset, fTexLeft, fTexBottom, bMultiTex );
							m_iTrisPerFrame++;

							//finish off the fan with a lower mid vertex
							if( iFanPosition==1 )
							{
								RenderVertex( x, z-fEdgeOffset, fMidX, fTexBottom, bMultiTex );
								m_iTrisPerFrame++;
							}
							break;

						//upper left node
						case QT_UL_NODE:
							//upper mid, skip if the adjacent node is of a lower detail level
							if( ( ( iZ+iAdjOffset )>=m_iSize ) || GetQuadMatrixData( iX, iZ+iAdjOffset )!=0 || iFanPosition==iFanLength )
							{
								RenderVertex( x, z+fEdgeOffset, fMidX, fTexTop, bMultiTex );
								m_iTrisPerFrame++;
							}

							//upper left vertex
							RenderVertex( x-fEdgeOffset, z+fEdgeOffset, fTexLeft, fTexTop, bMultiTex );

							//finish off the fan with a left mid vertex
							if( iFanPosition==1 )
							{
								RenderVertex( x-fEdgeOffset, z, fTexLeft, fMidZ, bMultiTex );
								m_iTrisPerFrame++;
							}
							break;

						//upper right node
						case QT_UR_NODE:
							//right mid, skip if the adjacent node is of a lower detail level
							if( ( ( iX+iAdjOffset )>=m_iSize ) || GetQuadMatrixData( iX+iAdjOffset, iZ )!=0 || iFanPosition==iFanLength )
							{
								RenderVertex( x+fEdgeOffset, z, fTexRight, fMidZ, bMultiTex );
								m_iTrisPerFrame++;
							}

							//upper right vertex
							RenderVertex( x+fEdgeOffset, z+fEdgeOffset, fTexRight, fTexTop, bMultiTex );
							m_iTrisPerFrame++;

							//finish off the fan with a top mid vertex
							if( iFanPosition==1 )
							{
								RenderVertex( x, z+fEdgeOffset, fMidX, fTexTop, bMultiTex );
								m_iTrisPerFrame++;
							}
							break;
					}

					iStart--;
					iStart&= 3;
				}
			glEnd( );

			//now, recurse down to children (special cases that weren't handled earlier)
			for( iFanPosition=( 4-iFanLength ); iFanPosition>0; iFanPosition-- )
			{
				switch( iStart )
				{
					//lower right node
					case QT_LR_NODE:
						RenderNode( x+fChildOffset, z-fChildOffset, iChildEdgeLength );
						break;

					//lower left node
					case QT_LL_NODE:
						RenderNode( x-fChildOffset, z-fChildOffset, iChildEdgeLength );
						break;

					//upper left node
					case QT_UL_NODE:
						RenderNode( x-fChildOffset, z+fChildOffset, iChildEdgeLength );
						break;

					//upper right node
					case QT_UR_NODE:
						RenderNode( x+fChildOffset, z+fChildOffset, iChildEdgeLength );
						break;
				}

				iStart--;
				iStart&= 3;
			}

			return;
		}
	}
}