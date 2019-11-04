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


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:		 CROAM::Init - public
// Description:	 Initialize the ROAM engine
// Arguments:	 -iMaxLevel: the maximum iLevel we can recurse down to
//				 -iPoolSize: size of the diamond pool
//				 -pCamera: pointer to a camera instance
// Return Value: None
//--------------------------------------------------------------
void CROAM::Init( int iMaxLevel, int iPoolSize, CCAMERA* pCamera )
{
	SROAM_DIAMOND* pPrevDmnd, *pNextDmnd;
	SROAM_DIAMOND* pDmnd;

	int iLevel;
	int di, dj, ix, jx;
	int i, j, k;

	m_iMaxLevel= iMaxLevel;
	m_fpLevelMDSize= new float [m_iMaxLevel+1];

	//generate table of displacement sizes versus levels (since this implementation
	//does not have crack prevention steps, we need to provide a quick hack to do it)
	for( iLevel=0; iLevel<=m_iMaxLevel; iLevel++ )
		m_fpLevelMDSize[iLevel]= 255.0f/( ( float )sqrt( ( float )( 1<<iLevel ) ) );

    //create diamond store, free list
	//allocate diamond and hash table arrays
	m_iPoolSize = iPoolSize;
	m_pDmndPool= new SROAM_DIAMOND [m_iPoolSize];

	//start all diamonds on free list
	for( i=0; ( i + 1 )<m_iPoolSize; i++ )
	{
		pPrevDmnd= m_pDmndPool + i;
		pNextDmnd= m_pDmndPool + ( i + 1 );

		pPrevDmnd->m_pNextDmnd= pNextDmnd;
		pNextDmnd->m_pPrevDmnd= pPrevDmnd;
	}

	//set the free diamonds in the pool
	m_pFreeDmnd[0]= m_pDmndPool;
	m_pFreeDmnd[1]= m_pDmndPool + ( m_iPoolSize - 1 );

	//clear the free diamond's links
	m_pFreeDmnd[0]->m_pPrevDmnd= ( SROAM_DIAMOND* )0;
	m_pFreeDmnd[1]->m_pNextDmnd= ( SROAM_DIAMOND* )0;

	//mark the diamonds as "free"
	for( k=0; k<m_iPoolSize; k++ )
	{
		pPrevDmnd= m_pDmndPool+k;
		
		pPrevDmnd->m_fBoundRad  = -1.0f;
		pPrevDmnd->m_ucLockCount= 0;
	}

    //allocate, position and link together the base-mesh diamonds
	for( k= 0; k<25; k++ )
	{
		if( k<9 )
		{
			j= k/3;
			i= k%3;

			m_pLevel0Dmnd[j][i]= pDmnd= Create( );
			pDmnd->m_fVert[0]= ( 2.0f*( float )( i - 1 ) );
			pDmnd->m_fVert[2]= ( 2.0f*( float )( j - 1 ) );
		}
		
		else
		{
			j= ( k - 9 )/4;
			i= ( k - 9 )%4;

			m_pLevel1Dmnd[j][i]= pDmnd= Create( );
			pDmnd->m_fVert[0]= ( 2.0f*( float )i - 3.0f );
			pDmnd->m_fVert[2]= ( 2.0f*( float )j - 3.0f );
		}
		ShiftCoords( &pDmnd->m_fVert[0], &pDmnd->m_fVert[2] );

		CLAMP( pDmnd->m_fVert[0], 0, ( m_iSize-1 ) );
		CLAMP( pDmnd->m_fVert[2], 0, ( m_iSize-1 ) );

		pDmnd->m_fVert[1]= GetTrueHeightAtPoint( ( int )fabs( pDmnd->m_fVert[0] ),
												 ( int )fabs( pDmnd->m_fVert[2] ) );

		pDmnd->m_cLevel= ( k<9 ? 0 : ( ( ( i^j ) & 1 ) ? -1 : -2 ) );

		pDmnd->m_fBoundRad= ( float )SQR( m_iSize );
		pDmnd->m_fErrorRad= ( float )m_iSize;

		//start the diamond's links off on a clean slate
		pDmnd->m_pParent[0]= pDmnd->m_pParent[1]= pDmnd->m_pParent[2]= pDmnd->m_pParent[3]= ( SROAM_DIAMOND* )0;
		pDmnd->m_pChild[0] = pDmnd->m_pChild[1] = pDmnd->m_pChild[2] = pDmnd->m_pChild[3] = ( SROAM_DIAMOND* )0;
	}

	//set the diamond's links
	for( k= 0; k<9; k++ )
	{
		j= k/3;
		i= k%3;

		pDmnd= m_pLevel0Dmnd[j][i];
		di= ( ( ( i^j ) & 1 ) ? 1 : -1 );
		dj= 1;

		ix= ( 2*i + 1 - di )>>1;
		jx= ( 2*j + 1 - dj )>>1;
		pDmnd->m_pParent[0]= m_pLevel1Dmnd[jx][ix];

		ix= ( 2*i + 1 + di )>>1;
		jx= ( 2*j + 1 + dj )>>1;
		pDmnd->m_pParent[1]= m_pLevel1Dmnd[jx][ix];

		ix= ( 2*i + 1 - dj )>>1;
		jx= ( 2*j + 1 + di )>>1;
		pDmnd->m_pParent[2]= m_pLevel1Dmnd[jx][ix];

		ix= ( 2*i + 1 + dj )>>1;
		jx= ( 2*j + 1 - di )>>1;
		pDmnd->m_pParent[3]= m_pLevel1Dmnd[jx][ix];

		ix= ( di<0 ? 0 : 3 );
		pDmnd->m_pParent[0]->m_pChild[ix]= pDmnd;
		pDmnd->m_cChildIndex[0]= ix;

		ix= ( di<0 ? 2 : 1 );
		pDmnd->m_pParent[1]->m_pChild[ix]= pDmnd;
		pDmnd->m_cChildIndex[1]= ix;
	}

	//configure the diamond nodes
	for( k=0; k<16; k++ )
	{
		j= k/4;
		i= k%4;

		pDmnd= m_pLevel1Dmnd[j][i];
		if( j>0 )
			pDmnd->m_pParent[3]= m_pLevel1Dmnd[j-1][i];
		if( j<3 )
			pDmnd->m_pParent[2]= m_pLevel1Dmnd[j+1][i];
		if( i>0 )
			pDmnd->m_pParent[0]= m_pLevel1Dmnd[j][i-1];
		if( i<3 )
			pDmnd->m_pParent[1]= m_pLevel1Dmnd[j][i+1];
	}

	//set the class's camera pointer
	m_pCamera= pCamera;
}

//--------------------------------------------------------------
// Name:		 CROAM::Shutdown - public
// Description:	 Shutdown the ROAM engine
// Arguments:	 None
// Return Value: None
//--------------------------------------------------------------
void CROAM::Shutdown( void )
{
	delete[] m_pDmndPool;
	delete[] m_fpLevelMDSize;
}

//--------------------------------------------------------------
// Name:		 CROAM::Render - public
// Description:	 Render the ROAM engine
// Arguments:	 None
// Return Value: None
//--------------------------------------------------------------
void CROAM::Render( void )
{
	//reset the debug counters
	m_iVertsPerFrame= 0;
	m_iTrisPerFrame = 0;

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

	glBegin( GL_TRIANGLES );
		//render the mesh
		RenderChild( m_pLevel1Dmnd[1][2], 0, 0 );
		RenderChild( m_pLevel1Dmnd[2][1], 2, 0 );
	glEnd( );

	//end texturing
	glActiveTextureARB( GL_TEXTURE0_ARB );
	glDisable( GL_TEXTURE_2D );

	glActiveTextureARB( GL_TEXTURE1_ARB );
	glDisable( GL_TEXTURE_2D );
}

//--------------------------------------------------------------
// Name:		 CROAM::Render - public
// Description:	 Render the ROAM engine
// Arguments:	 -pDmnd: pointer to base diamond to be rendered
//				 -iIndex: child index to be rendered
// Return Value: None
//--------------------------------------------------------------
void CROAM::RenderChild( SROAM_DIAMOND* pDmnd, int iIndex, int iCull )
{
	SROAM_DIAMOND* pPrevDmnd, *pNextDmnd; //diamonds of other two triangle vertices
    SROAM_DIAMOND* pCenterDmnd;			//center, split-edge diamond
	unsigned char ucShade;
    float* pDmndCenter;					//center vertex
	float fTexCoords[2];
	float fDist;

	//get the kid diamond that needs to be rendered
	pCenterDmnd= GetChild( pDmnd, iIndex );
	pDmndCenter= pCenterDmnd->m_fVert;

	//check the triangle's bounding sphere against the view frustum
	if( iCull!=CULL_ALLIN )
	{
		float r;
		int j, m;

		//perform culling against the view frustum
		for( j=0, m=1; j<6; j++, m<<= 1 )
		{
			if( !( iCull & m ) )
			{
				r= m_pCamera->m_viewFrustum[j][0]*pDmndCenter[0] +
				   m_pCamera->m_viewFrustum[j][1]*pDmndCenter[1] +
				   m_pCamera->m_viewFrustum[j][2]*pDmndCenter[2] +
				   m_pCamera->m_viewFrustum[j][3];

				//check for frustum inclusion
				if( SQR( r )>pCenterDmnd->m_fBoundRad )
				{
					//check to see if the triangle is actually within the viewing frustum
					if( r<0.0f )
					{
						Unlock( pCenterDmnd );
						return;
					}
					
					//triangle is within view
					iCull|= m;
				}
			}
		} 
	}

    //calculate the distance from the camera to the tri's center
    fDist= SQR( ( pDmndCenter[0] - m_pCamera->m_vecEyePos[0] ) )+
		   SQR( ( pDmndCenter[1] - m_pCamera->m_vecEyePos[1] ) )+
		   SQR( ( pDmndCenter[2] - m_pCamera->m_vecEyePos[2] ) );

	//if not max level and error large on screen, recursively split
	if( pCenterDmnd->m_cLevel<m_iMaxLevel && pCenterDmnd->m_fErrorRad>fDist*0.00001f )
	{
		if( pCenterDmnd->m_pParent[0]==pDmnd )
		{
			RenderChild( pCenterDmnd, 0, iCull );
			RenderChild( pCenterDmnd, 1, iCull );
		}
		
		else
		{
			RenderChild( pCenterDmnd, 2, iCull );
			RenderChild( pCenterDmnd, 3, iCull );
		}
	}

	//draw the current triangle (leaf node)
	else
	{
		//calculate the triangle vertices
		if( pCenterDmnd->m_pParent[0]==pDmnd )
		{
			pPrevDmnd= pCenterDmnd->m_pParent[2];
			pNextDmnd= pCenterDmnd->m_pParent[3];
		}
		else
		{
			pPrevDmnd= pCenterDmnd->m_pParent[3];
			pNextDmnd= pCenterDmnd->m_pParent[2];
		}

		//send the information for the first vertex (color->texture->vertex)
		ucShade= GetBrightnessAtPoint( ( int )pPrevDmnd->m_fVert[0], ( int )pPrevDmnd->m_fVert[2] );
		glColor3ub( ucShade, ucShade, ucShade );
		fTexCoords[0]= pPrevDmnd->m_fVert[0]/m_iSize;	fTexCoords[1]= pPrevDmnd->m_fVert[2]/m_iSize;

		if( m_bTextureMapping )
			glMultiTexCoord2fARB( GL_TEXTURE0_ARB, fTexCoords[0], fTexCoords[1] );
		if( m_bDetailMapping )
			glMultiTexCoord2fARB( GL_TEXTURE1_ARB, fTexCoords[0]*m_iRepeatDetailMap, fTexCoords[1]*m_iRepeatDetailMap );

		glVertex3fv( pPrevDmnd->m_fVert );

		//send the information for the second vertex (color->texture->vertex)
		ucShade= GetBrightnessAtPoint( ( int )pDmnd->m_fVert[0], ( int )pDmnd->m_fVert[2] );
		glColor3ub( ucShade, ucShade, ucShade );
		fTexCoords[0]= pDmnd->m_fVert[0]/m_iSize;	fTexCoords[1]= pDmnd->m_fVert[2]/m_iSize;

		if( m_bTextureMapping )
			glMultiTexCoord2fARB( GL_TEXTURE0_ARB, fTexCoords[0], fTexCoords[1] );
		if( m_bDetailMapping )
			glMultiTexCoord2fARB( GL_TEXTURE1_ARB, fTexCoords[0]*m_iRepeatDetailMap, fTexCoords[1]*m_iRepeatDetailMap );

		glVertex3fv( pDmnd->m_fVert );

		//send the information for the third vertex (color->texture->vertex)
		ucShade= GetBrightnessAtPoint( ( int )pNextDmnd->m_fVert[0], ( int )pNextDmnd->m_fVert[2] );
		glColor3ub( ucShade, ucShade, ucShade );
		fTexCoords[0]= pNextDmnd->m_fVert[0]/m_iSize;	fTexCoords[1]= pNextDmnd->m_fVert[2]/m_iSize;

		if( m_bTextureMapping )
			glMultiTexCoord2fARB( GL_TEXTURE0_ARB, fTexCoords[0], fTexCoords[1] );
		if( m_bDetailMapping )
			glMultiTexCoord2fARB( GL_TEXTURE1_ARB, fTexCoords[0]*m_iRepeatDetailMap, fTexCoords[1]*m_iRepeatDetailMap );

		glVertex3fv( pNextDmnd->m_fVert );

		m_iVertsPerFrame+= 3;
		m_iTrisPerFrame++;
	}

	//unlock the diamond, we are done with it
	Unlock( pCenterDmnd );
}

//--------------------------------------------------------------
// Name:		 CROAM::GetChild - public
// Description:	 Get a new child and set it's information
// Arguments:	 None
// Return Value: Pointer to the created child
//--------------------------------------------------------------
SROAM_DIAMOND* CROAM::GetChild( SROAM_DIAMOND* pDmnd, int iIndex )
{
	SROAM_DIAMOND* pChild, *px, *cx;
	float* pDmndCenter, *pParentVert0, *pParentVert1;
	float sqrBound, sqrBoundTemp, *tempVert;
	int ix;

	//we don't need to create the child if it already exists
	if( ( pDmnd->m_pChild[iIndex] ) )
	{
		Lock( pDmnd->m_pChild[iIndex] );
		return pDmnd->m_pChild[iIndex];
	}

	//lock center diamond early to avoid an automatic "diamond discharge"
	Lock( pDmnd );

	//allocate new kid
	pChild= Create( );

	//recursively create other parent to the current child
	if( iIndex<2 )
	{
		px= pDmnd->m_pParent[0];
		
		ix= ( pDmnd->m_cChildIndex[0] + ( iIndex==0 ? 1 : -1 ) ) & 3;
	}
	else
	{
		px= pDmnd->m_pParent[1];

		ix= ( pDmnd->m_cChildIndex[1] + ( iIndex==2 ? 1 : -1 ) ) & 3;
	}

	//get the other child
	cx= GetChild( px, ix );

	//set all the links
	pDmnd->m_pChild[iIndex]= pChild;
	ix= ( iIndex & 1 )^1;
	if( cx->m_pParent[1]==px )
		ix|= 2;
		
	cx->m_pChild[ix]= pChild;
	if( iIndex & 1 )
	{
		pChild->m_pParent[0]    = cx;
		pChild->m_cChildIndex[0]= ix;

		pChild->m_pParent[1]    = pDmnd;
		pChild->m_cChildIndex[1]= iIndex;
	}
	else
	{
		pChild->m_pParent[0]    = pDmnd;
		pChild->m_cChildIndex[0]= iIndex;

		pChild->m_pParent[1]    = cx;
		pChild->m_cChildIndex[1]= ix;
	}

	pChild->m_pParent[2]= pDmnd->m_pParent[iIndex>>1];
	pChild->m_pParent[3]= pDmnd->m_pParent[( ( ( iIndex+1 ) & 2 )>>1 )+2];

	pChild->m_pChild[0]= pChild->m_pChild[1]= pChild->m_pChild[2]= pChild->m_pChild[3]= ( SROAM_DIAMOND* )0;

	//calculate the child's level and vertex information
	pChild->m_cLevel= pDmnd->m_cLevel+1;

	pParentVert0= pChild->m_pParent[2]->m_fVert;
	pParentVert1= pChild->m_pParent[3]->m_fVert;
	pChild->m_fVert[0]= ( float )fabs( ( pParentVert0[0] + pParentVert1[0] )/2.0f );
	pChild->m_fVert[2]= ( float )fabs( ( pParentVert0[2] + pParentVert1[2] )/2.0f );

	CLAMP( 	pChild->m_fVert[0], 0, ( m_iSize-1 ) );
	CLAMP( 	pChild->m_fVert[2], 0, ( m_iSize-1 ) );

	pChild->m_fVert[1]= GetTrueHeightAtPoint( ( int )pChild->m_fVert[0], ( int )pChild->m_fVert[2] );

	pDmndCenter= pChild->m_fVert;

    //compute radius of diamond bounding sphere (squared)
	//calculate the bounding sphere for the current triangle
	//calculation 1
	tempVert= pChild->m_pParent[0]->m_fVert;
	sqrBound= SQR( ( tempVert[0] - pDmndCenter[0] ) )+
			  SQR( ( tempVert[1] - pDmndCenter[1] ) )+
			  SQR( ( tempVert[2] - pDmndCenter[2] ) );

	tempVert= pChild->m_pParent[1]->m_fVert;
	//calculation 2
	sqrBoundTemp= SQR( ( tempVert[0] - pDmndCenter[0] ) )+
				  SQR( ( tempVert[1] - pDmndCenter[1] ) )+
				  SQR( ( tempVert[2] - pDmndCenter[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( sqrBoundTemp > sqrBound )
		sqrBound= sqrBoundTemp;

	//calculation 3
	tempVert= pChild->m_pParent[2]->m_fVert;
	sqrBoundTemp= SQR( ( tempVert[0] - pDmndCenter[0] ) )+
				  SQR( ( tempVert[1] - pDmndCenter[1] ) )+
				  SQR( ( tempVert[2] - pDmndCenter[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( sqrBoundTemp > sqrBound )
		sqrBound= sqrBoundTemp;

	//calculation 4
	tempVert= pChild->m_pParent[3]->m_fVert;
	sqrBoundTemp= SQR( ( tempVert[0] - pDmndCenter[0] ) )+
				  SQR( ( tempVert[1] - pDmndCenter[1] ) )+
				  SQR( ( tempVert[2] - pDmndCenter[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( sqrBoundTemp > sqrBound )
		sqrBound= sqrBoundTemp;

	pChild->m_fBoundRad= sqrBound;
	pChild->m_fErrorRad= SQR( m_fpLevelMDSize[pChild->m_cLevel] );

    return pChild;
}

//--------------------------------------------------------------
// Name:		 CROAM::Create - public
// Description:	 Create a new diamond
// Arguments:	 None
// Return Value: Pointer to the created diamond
//--------------------------------------------------------------
SROAM_DIAMOND* CROAM::Create( void )
{
	SROAM_DIAMOND* pDmnd;

	//use the most recent "freed" diamond
	pDmnd= m_pFreeDmnd[0];
	if( !pDmnd )
	{
		g_log.Write( LOG_FAILURE, "Out of memory!");

		return ( SROAM_DIAMOND* )0;
	}

	//if the diamond has been used before, delete it's previous parent links
	if( pDmnd->m_fBoundRad>=0.0f )
	{
		pDmnd->m_pParent[0]->m_pChild[pDmnd->m_cChildIndex[0]]= ( SROAM_DIAMOND *)0;
		Unlock( pDmnd->m_pParent[0] );
		
		pDmnd->m_pParent[1]->m_pChild[pDmnd->m_cChildIndex[1]]= ( SROAM_DIAMOND *)0;
		Unlock( pDmnd->m_pParent[1] );
	}

	//lock the new diamond
	Lock( pDmnd );
    return pDmnd;
}

//--------------------------------------------------------------
// Name:		 CROAM::Lock - public
// Description:	 Lock the child to prevent a "diamond discharge"
// Arguments:	 None
// Return Value: None
//--------------------------------------------------------------
void CROAM::Lock( SROAM_DIAMOND* pDmnd )
{
    SROAM_DIAMOND* pPrevDmnd, *pNextDmnd;

    //remove from free list if first reference
    if( pDmnd->m_ucLockCount==0 )
	{
		pPrevDmnd= pDmnd->m_pPrevDmnd;
		pNextDmnd= pDmnd->m_pNextDmnd;

		if( pPrevDmnd )
			pPrevDmnd->m_pNextDmnd= pNextDmnd;
		else
			m_pFreeDmnd[0]= pNextDmnd;

		if( pNextDmnd )
			pNextDmnd->m_pPrevDmnd= pPrevDmnd;
		else
			m_pFreeDmnd[1]= pPrevDmnd;
	}
	
	pDmnd->m_ucLockCount++;
}

//--------------------------------------------------------------
// Name:		 CROAM::Unlock - public
// Description:	 Unlock the child so it may be used by others
// Arguments:	 None
// Return Value: None
//--------------------------------------------------------------
void CROAM::Unlock( SROAM_DIAMOND* pDmnd )
{
	SROAM_DIAMOND* pPrevDmnd;

	pDmnd->m_ucLockCount--;

	//add to free list if no references left
	if( pDmnd->m_ucLockCount==0 )
	{
		pPrevDmnd= m_pFreeDmnd[1];

		pDmnd->m_pPrevDmnd= pPrevDmnd;
		pDmnd->m_pNextDmnd= ( SROAM_DIAMOND* )0;

		if( pPrevDmnd )
			pPrevDmnd->m_pNextDmnd= pDmnd;
		else
			m_pFreeDmnd[0]= pDmnd;

		m_pFreeDmnd[1]= pDmnd;
	}
}