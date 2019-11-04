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
	SROAM_DIAMOND* pDmnd, *pTempDmnd0, *pTempDmnd1;
	float f;
	int iLevel, i, j, k, di, dj, ix, jx, *pInt;

	//initialize the ROAM engine
    m_iQueueCoarse= 1990;			//1990 happened to be the "magic number" for the queue "fineness"
    m_iFrameCount = 0;

	m_iMaxLevel    = iMaxLevel;
	m_fpLevelMDSize= new float [m_iMaxLevel+1];

	//generate table of displacement sizes versus levels
	for( iLevel=0; iLevel <= m_iMaxLevel; iLevel++ )
		m_fpLevelMDSize[iLevel]= 255.0f/( ( float )sqrt( ( float )( ( __int64 )1<<iLevel ) ) );

	//create diamond store, free list
	//allocate diamond and hash table arrays
	m_iPoolSize = iPoolSize;
	m_pDmndPool= new SROAM_DIAMOND [m_iPoolSize];

	m_iMaxTriChunks= TRI_IMAX;
	m_ipPDmndIS= new int [m_iMaxTriChunks];

	//allocate memory for the vertex/texture coordinates
	m_fVertTexBuffer= new float [m_iMaxTriChunks*15];

	//start all diamonds on the free list
	for( i=0; i + 1 < m_iPoolSize; i++ )
	{
		pTempDmnd0= m_pDmndPool + i;
		pTempDmnd1= m_pDmndPool + ( i + 1 );

		pTempDmnd0->m_pNextDmnd= pTempDmnd1;
		pTempDmnd1->m_pPrevDmnd= pTempDmnd0;
	}
	m_pFreeDmnd[0]= m_pDmndPool;
	m_pFreeDmnd[1]= m_pDmndPool + ( m_iPoolSize - 1 );
	m_pFreeDmnd[0]->m_pPrevDmnd= ( SROAM_DIAMOND* )0;
	m_pFreeDmnd[1]->m_pNextDmnd= ( SROAM_DIAMOND* )0;
	m_iFreeElements= m_iPoolSize;

	//initialize diamonds to be NEW and FREE
	for( i=0; i < m_iPoolSize; i++ )
	{
		pTempDmnd0= m_pDmndPool + i;

		pTempDmnd0->m_fBoundRad= -1;		//indicated a NEW diamond
		pTempDmnd0->m_ucLockCount= 0;
		pTempDmnd0->m_ucFlags= 0;
		pTempDmnd0->m_ucFrameCount= 255;

		pTempDmnd0->m_pParent[2]= pTempDmnd0->m_pParent[3]= ( SROAM_DIAMOND* )0;
		
		pTempDmnd0->m_ucCull= 0;
		pTempDmnd0->m_cChildIndex[0]= pTempDmnd0->m_cChildIndex[1]= 0;
		pTempDmnd0->m_sQueueIndex= IQMAX/2;
			
		pTempDmnd0->m_pChild[0]= pTempDmnd0->m_pChild[1]= pTempDmnd0->m_pChild[2]= pTempDmnd0->m_pChild[3]= ( SROAM_DIAMOND* )0;
			
		pTempDmnd0->m_cLevel= -100;
			
		pTempDmnd0->m_pParent[0]= pTempDmnd0->m_pParent[1]= ( SROAM_DIAMOND* )0;
			
		pTempDmnd0->m_fErrorRad= 10.0f;
	}

	//clear the split/merge priority queues
	for( i= 0; i < IQMAX; i++ )
	{
		m_pSplitQueue[i]= m_pMergeQueue[i]= ( SROAM_DIAMOND* )0;
	}
	m_iPQMax= -1;
	m_iPQMin= IQMAX;

	//clear the triangle render-list
	for( i=0; i<m_iMaxTriChunks; i++ )
		m_ipPDmndIS[i]= -1;
	m_iFreeTri= 1;
	m_iTrisPerFrame= 0;
	m_iFreeTriCount= m_iMaxTriChunks - 1;
	m_iMaxTris= 30000;			//set the default maximum triangle count (max triangles visible per frame)

	//generate a LUT for float->int conversations (huge speed-saver later on)
	pInt= ( int * )( &f );
	for( i=0; i<256; i++ )
	{
		*pInt= 0x3f800000 + ( i<<15 );
		m_iLog2Table[i]= ( int )floor( m_iSize*( log( f )/log( 2.0 ) - ( float )i/256.0 ) + 0.5f )<<12;
	}

	//allocate base diamonds, and set their information (minus link info)
	for( k=0; k<32; k++ )
	{
		//initialization code for dimaonds 0-15
		if( k<16 )
		{
			j= k/4;
			i= k%4;
				
			m_pLevel0Dmnd[j][i]= pDmnd= Create( );
			pDmnd->m_fVert[0]= ( 2.0f*( float )( i - 1 ) );
			pDmnd->m_fVert[2]= ( 2.0f*( float )( j - 1 ) );
		}
		//initialization for diamonds 15-31
		else
		{
			j= ( k - 16 )/4;
			i= ( k - 16 )%4;
				
			m_pLevel1Dmnd[j][i]= pDmnd= Create( );
			pDmnd->m_fVert[0]= ( 2.0f*( float )i - 3.0f );
			pDmnd->m_fVert[2]= ( 2.0f*( float )j - 3.0f );
		}

		//shift the previous coords from [-3, 3] to [0, m_iSize-1]
		ShiftCoords( &pDmnd->m_fVert[0], &pDmnd->m_fVert[2] );

		//clamp the coordinates to the map size (prevent a buffer overflow)
		CLAMP( pDmnd->m_fVert[0], 0, ( m_iSize-1 ) );
		CLAMP( pDmnd->m_fVert[2], 0, ( m_iSize-1 ) );

		pDmnd->m_fVert[1]= ( float )GetTrueHeightAtPoint( ( int )( fabs( pDmnd->m_fVert[0] ) ),
														  ( int )( fabs( pDmnd->m_fVert[2] ) ) );
		pDmnd->m_usTriIndex[0]= pDmnd->m_usTriIndex[1]= 0;

		pDmnd->m_fBoundRad= ( float )SQR( m_iSize );
		pDmnd->m_fErrorRad= ( float )m_iSize;

		//reset all links
		pDmnd->m_pParent[0]= pDmnd->m_pParent[1]= pDmnd->m_pParent[2]= pDmnd->m_pParent[3]= ( SROAM_DIAMOND* )0;
		pDmnd->m_pChild[0] = pDmnd->m_pChild[1] = pDmnd->m_pChild[2] = pDmnd->m_pChild[3] = ( SROAM_DIAMOND* )0;

		pDmnd->m_cLevel= ( k < 16 ? 0 : ( ( ( i^j ) & 1 ) ? -1 : -2 ) );
		pDmnd->m_ucCull = 0;
		pDmnd->m_ucFlags= 0;
		pDmnd->m_ucSplitFlags= 0;
		pDmnd->m_sQueueIndex= IQMAX - 1;

		if( k<16 && k!=5 )
			pDmnd->m_ucFlags|= ROAM_CLIPPED;
		if( pDmnd->m_cLevel<0 )
			pDmnd->m_ucFlags|= ROAM_SPLIT;
	}

	//now that all of the diamonds exist, we need to set their links
	for( k=0; k<16; k++ )
	{
		j= k/4;
		i= k%4;
		
		//links for the iLevel 0 diamonds
		pDmnd=m_pLevel0Dmnd[j][i];
		di= ( ( ( i^j ) & 1 ) ? 1 : -1 );
		dj= 1;
		ix= ( ( 2*i + 1 - di )>>1 )%4;
		jx= ( ( 2*j + 1 - dj )>>1 )%4;
		pDmnd->m_pParent[0]= m_pLevel1Dmnd[jx][ix];
			
		ix= ( ( 2*i + 1 + di )>>1 )%4;
		jx= ( ( 2*j + 1 + dj )>>1 )%4;
		pDmnd->m_pParent[1]= m_pLevel1Dmnd[jx][ix];
			
		ix= ( ( 2*i + 1 - dj )>>1 )%4;
		jx= ( ( 2*j + 1 + di )>>1 )%4;
		pDmnd->m_pParent[2]= m_pLevel1Dmnd[jx][ix];
			
		ix= ( ( 2*i + 1 + dj )>>1 )%4;
		jx= ( ( 2*j + 1 - di )>>1 )%4;
		pDmnd->m_pParent[3]= m_pLevel1Dmnd[jx][ix];
			
		ix= ( di < 0 ? 0 : 3 );
		pDmnd->m_pParent[0]->m_pChild[ix]= pDmnd;
		pDmnd->m_cChildIndex[0]= ix;
			
		ix= ( di < 0 ? 2 : 1 );
		pDmnd->m_pParent[1]->m_pChild[ix]= pDmnd;
		pDmnd->m_cChildIndex[1]= ix;
	}

	for( k=0; k<16; k++ )
	{
		j= k/4;
		i= k%4;
		
		//links for the iLevel -1 diamonds
		pDmnd=m_pLevel1Dmnd[j][i];
		pDmnd->m_pParent[3]= m_pLevel1Dmnd[( j+3 )%4][i];
		pDmnd->m_pParent[2]= m_pLevel1Dmnd[( j+1 )%4][i];
		pDmnd->m_pParent[0]= m_pLevel1Dmnd[j][( i+3 )%4];
		pDmnd->m_pParent[1]= m_pLevel1Dmnd[j][( i+1 )%4];
	}

	//put the top-iLevel diamond on the split queue (and we can work from there
	pDmnd= m_pLevel0Dmnd[1][1];
	Enqueue( pDmnd, ROAM_SPLITQ, IQMAX-1 );

	//get the base tris
	AllocateTri( pDmnd, 0 );
	AllocateTri( pDmnd, 1 );

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
	delete[] m_fVertTexBuffer;
	delete[] m_ipPDmndIS;
	delete[] m_pDmndPool;
	delete[] m_fpLevelMDSize;
}

//--------------------------------------------------------------
// Name:		 CROAM::Update - public
// Description:	 Update the engine's mesh (process queues)
// Arguments:	 None
// Return Value: None
//--------------------------------------------------------------
void CROAM::Update( void )
{
 	static int i0= 0;
	SROAM_DIAMOND* pDmnd;
	int iSide, iOverlap, iOverlap0, iOptCount, iMaxOptCount, i1, i;

	//update all active diamonds with a recursive culling update
	pDmnd= m_pLevel0Dmnd[1][1];
	UpdateChildCull( pDmnd );
	for( i= 0; i < 4; i++ )
	{
		if( pDmnd->m_pChild[i] )
			UpdateChildCull( pDmnd->m_pChild[i] );
	}

	//update all queued diamonds' priority
	i1= i0+( m_iPoolSize+9 )/10;

	if( i1>=m_iPoolSize )
		i1= m_iPoolSize-1;

	for( i=i0; i<=i1; i++ )
	{
	    pDmnd= m_pDmndPool+i;
	    if( pDmnd->m_ucFlags & ROAM_ALLQ )
			UpdatePriority( pDmnd );
	}

	i0= ( i1+1 )%m_iPoolSize; 

    /*
     * keep splitting/merging until either
     *  (a) no split/merge priority iOverlap and:
     *      target tri count reached or accuracy target reached
     * or
     *  (b) time is up (limit optimization-loop iterations)
     * or
     *  (c) not enough free (unlocked) diamonds in cache
     *
     * Note: this implementation handles non-monotonic priorities,
     * i.e. where a child can have a higher priority than its parent.
     * Also, we are careful to be just one force-split away from being
     * beyond the target triangle/accuracy count.  As a iSide effect, this
     * eliminates one kind of oscillation that might occur if using
     * the suggested pseudocode from the original ROAM paper (see Vis 1997).
     *
     */
	iMaxOptCount= 2000; /* split/merge limit */
	iOptCount   = 0;
		
	//check to see if the mesh is too coarse
	if( ( m_iTrisPerFrame<=m_iMaxTris && m_iPQMax>=m_iQueueCoarse &&
		  m_iFreeElements>128	      && m_iFreeTriCount>128 ) )
		iSide= -1;
	else
		iSide= 1;
		
	iOverlap= iOverlap0= m_iPQMax-m_iPQMin;

	//loop through performing split/merge ops
	while( ( iSide!=0 || iOverlap0>1 ) && iOptCount<iMaxOptCount )
	{
		if( iSide<=0 )
		{
			if( m_iPQMax>0 )
			{
				Split( m_pSplitQueue[m_iPQMax] );

				//check to see if the mesh is too coarse
				if( !( m_iTrisPerFrame<=m_iMaxTris && m_iPQMax>=m_iQueueCoarse &&
					   m_iFreeElements>128	       && m_iFreeTriCount>128 ) )
					iSide= 1;
			}
			else
				iSide= 0;
		}
		else
		{
			Merge( m_pMergeQueue[m_iPQMin] );

			//check to see if the mesh is too coarse
			if( ( m_iTrisPerFrame<=m_iMaxTris && m_iPQMax>=m_iQueueCoarse && \
				  m_iFreeElements>128	      && m_iFreeTriCount>128 ) )
				iSide= 0;
		}

		iOverlap= m_iPQMax-m_iPQMin;
		if( iOverlap<iOverlap0 )
			iOverlap0= iOverlap;

		iOptCount++;
	}

	m_iFrameCount= ( m_iFrameCount+1 ) & 255;
}

//--------------------------------------------------------------
// Name:		 CROAM::Render - public
// Description:	 Render the ROAM engine
// Arguments:	 None
// Return Value: None
//--------------------------------------------------------------
void CROAM::Render( void )
{
	float* fpVB;

	//bind the primary color texture to the first texture unit
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, m_texture.GetID( ) );

	//render the mesh using vertex/texture arrays
	fpVB= m_fVertTexBuffer + 15;

	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	//render using a base texture and vertex info
	//NOTE: lighting/detail map has been eliminated from this demo for simplicity's sake
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	glVertexPointer( 3,   GL_FLOAT, 20, fpVB+2 );
	glTexCoordPointer( 2, GL_FLOAT, 20, fpVB );

	//lock the arrays (provides a speed-up)
	glLockArraysEXT( 0, 3*( m_iFreeTri-1 ) );

	//draw the water patch
	glDrawArrays( GL_TRIANGLES, 0, 3*( m_iFreeTri-1 ) );

	//unlock the arrays
	glUnlockArraysEXT( );

	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glDisableClientState( GL_VERTEX_ARRAY );

	//end texturing
	glDisable( GL_TEXTURE_2D );
}

//--------------------------------------------------------------
// Name:		 CROAM::AllocateTri - private
// Description:	 Allocate a triangle for the triangle render list
// Arguments:	 -pDmnd: pointer to a diamond
//				 -j: side index
// Return Value: None
//--------------------------------------------------------------
void CROAM::AllocateTri( SROAM_DIAMOND* pDmnd, int j )
{
    int iFlags;

	//CLIPPED diamonds never have triangles
	if( pDmnd->m_ucFlags & ROAM_CLIPPED )
		return;

	iFlags= pDmnd->m_pParent[j]->m_ucFlags;

	//CLIPPED parent j means no triangle on iSide j
	if( iFlags & ROAM_CLIPPED )
		return;

	//indicate that the triangle on side j is active
	pDmnd->m_ucFlags|= ROAM_TRI0<<j;

	//if not IN, take the triangle off of the OUT list
	if( !( pDmnd->m_ucCull & CULL_OUT ) )
		AddTri( pDmnd, j );
}

//--------------------------------------------------------------
// Name:		 CROAM::FreeTri - private
// Description:	 Free a triangle from the triangle render list
// Arguments:	 -pDmnd: pointer to a diamond
//				 -j: side index
// Return Value: None
//--------------------------------------------------------------
void CROAM::FreeTri( SROAM_DIAMOND* pDmnd, int j )
{
	int iFlags;

	//CLIPPED diamonds never have triangles
	if( pDmnd->m_ucFlags & ROAM_CLIPPED )
		return;

	iFlags= pDmnd->m_pParent[j]->m_ucFlags;

	//CLIPPED parent j means no triangle on iSide j
	if( iFlags & ROAM_CLIPPED )
		return;

	//indicate that the triangle on iSide j is not active
	pDmnd->m_ucFlags&= ~( ROAM_TRI0<<j );

	//if not OUT, take the triangle off of the IN list
	if( !( pDmnd->m_ucCull & CULL_OUT ) )
		RemoveTri( pDmnd, j );
}

//--------------------------------------------------------------
// Name:		 CROAM::AddTri - private
// Description:	 Add a triangle to the render list
// Arguments:	 -pDmnd: pointer to a diamond
//				 -j: side index
// Return Value: None
//--------------------------------------------------------------
void CROAM::AddTri( SROAM_DIAMOND* pDmnd, int j )
{
	SROAM_DIAMOND* pDmndTable[3];
	float* fpVB;
	int i, vi;

	/* grab free tri and fill in */
	//get a free triangle and "fill" it in 
	i= m_iFreeTri++;
	if( i>=m_iMaxTriChunks )
	{
		fprintf( stderr,"roam: out of triangle memory\n" );
		/* should deal with this more gracefully... */
		exit( 1 );
	}
	m_iFreeTriCount--;
	pDmnd->m_usTriIndex[j]= i;
	m_ipPDmndIS[i]= ( ( pDmnd-m_pDmndPool )<<1 ) | j;

	//fill in the information for the triangle
	pDmndTable[1]= pDmnd->m_pParent[j];
	if( j )
	{
		pDmndTable[0]= pDmnd->m_pParent[3];
		pDmndTable[2]= pDmnd->m_pParent[2];
	}
	else
	{
		pDmndTable[0]= pDmnd->m_pParent[2];
		pDmndTable[2]= pDmnd->m_pParent[3];
	}

	//fill the vertex buffer with the information
	fpVB= ( float * )m_fVertTexBuffer+15*i;
	for( vi=0; vi<3; vi++, fpVB+=5 )
	{
		fpVB[2]= pDmndTable[vi]->m_fVert[0];
		fpVB[3]= pDmndTable[vi]->m_fVert[1];
		fpVB[4]= pDmndTable[vi]->m_fVert[2];

		fpVB[0]= fpVB[2]/m_iSize;
		fpVB[1]= fpVB[4]/m_iSize;
	}

	m_iVertsPerFrame+= 3;
	m_iTrisPerFrame++;
}

//--------------------------------------------------------------
// Name:		 CROAM::RemoveTri - private
// Description:	 Remove a triangle from the render list
// Arguments:	 -pDmnd: pointer to a diamond
//				 -j: side index
// Return Value: None
//--------------------------------------------------------------
void CROAM::RemoveTri( SROAM_DIAMOND* pDmnd, int j )
{
	SROAM_DIAMOND *pDmndX;
	float *fpVB, *fpVBIndex;
	int iDmndIS, ix, jx, i;

	i= pDmnd->m_usTriIndex[j];

	//put the triangle back on the "free" list for use
	pDmnd->m_usTriIndex[j]=0;
	m_iFreeTri--;
	m_iFreeTriCount++;

	//copy the last triangle on the list (non-free) to the freed triangle
	ix  = m_iFreeTri;
	iDmndIS= m_ipPDmndIS[ix];
	jx  = iDmndIS & 1;
	pDmndX = m_pDmndPool+( iDmndIS>>1 );
		
	pDmndX->m_usTriIndex[jx]= i;
	m_ipPDmndIS[i]= iDmndIS;
		
	fpVB = ( float * )m_fVertTexBuffer;
	fpVBIndex= fpVB+15*ix;
	fpVB+= 15*i;
	memcpy( ( void* )fpVB, ( void* )fpVBIndex, 15*sizeof( float ) );

	m_iVertsPerFrame-= 3;
	m_iTrisPerFrame--;
}

//--------------------------------------------------------------
// Name:		 CROAM::Create - private
// Description:	 Create a new diamond
// Arguments:	 None
// Return Value: Pointer to the created diamond
//--------------------------------------------------------------
SROAM_DIAMOND* CROAM::Create( void )
{
	SROAM_DIAMOND* pDmnd;

	//recycle the least recently used diamond
	pDmnd= m_pFreeDmnd[0];
	if( !pDmnd )
	{
		g_log.Write( LOG_FAILURE, "Out of ROAM diamond storage" );
		exit( 1 );
	}

	//if the diamond is not NEW, reset its links
	if( pDmnd->m_fBoundRad>=0.0f )
	{
		pDmnd->m_pParent[0]->m_pChild[pDmnd->m_cChildIndex[0]]= ( SROAM_DIAMOND* )0;
		Unlock( pDmnd->m_pParent[0] );
		
		pDmnd->m_pParent[1]->m_pChild[pDmnd->m_cChildIndex[1]]= ( SROAM_DIAMOND* )0;
		Unlock( pDmnd->m_pParent[1] );
		pDmnd->m_sQueueIndex= IQMAX>>1;
	}
	else
		pDmnd->m_fBoundRad= 0.0f;	//set diamond to NEW

	//make sure that the frame count is old (so we can perform updates and such)
	pDmnd->m_ucFrameCount= ( m_iFrameCount-1 ) & 255;

	//lock the gathered diamond and return it for use
	Lock( pDmnd );
	return pDmnd;
}

//--------------------------------------------------------------
// Name:		 CROAM::GetChild - private
// Description:	 Get a new child and set it's information
// Arguments:	 pDmnd: pointer to a diamond
//				 i: child index of given diamond
// Return Value: Pointer to the created child
//--------------------------------------------------------------
SROAM_DIAMOND* CROAM::GetChild( SROAM_DIAMOND* pDmnd, int i )
{
 	SROAM_DIAMOND* k, *pParentX, *pChildX;
	float* pDmndCenter, *pParentVert0, *pParentVert1;
	float fSqrBound, fSqrBoundTemp, *fTempVert;
	int ix;

	//if the diamond is already alive, return it
	if( ( k= pDmnd->m_pChild[i] ) )
	{
		Lock( k );
		return k;
	}

	//lock the center diamond to prevent early recycling
	Lock( pDmnd );

	//recursively create other parent for the child (i)
	if( i<2 )
	{
		pParentX= pDmnd->m_pParent[0];
		ix= ( pDmnd->m_cChildIndex[0] + ( i==0 ? 1 : -1 ) ) & 3;
	}
	else
	{
		pParentX= pDmnd->m_pParent[1];
		ix= ( pDmnd->m_cChildIndex[1] + ( i==2 ? 1 : -1 ) ) & 3;
	}

	pChildX= GetChild( pParentX, ix ); //lock other child parent

	//create a new child, and lock it
	k= Create( );

	//set all of the child's links
	pDmnd->m_pChild[i]= k;
	ix= ( i & 1 )^1;
	if( pChildX->m_pParent[1]==pParentX )
		ix|= 2;

	pChildX->m_pChild[ix]= k;
	if( i & 1 )
	{
		k->m_pParent[0]    = pChildX;
		k->m_cChildIndex[0]= ix;
		k->m_pParent[1]    = pDmnd;
		k->m_cChildIndex[1]= i;
	}
	else
	{
		k->m_pParent[0]    = pDmnd;
		k->m_cChildIndex[0]= i;
		k->m_pParent[1]    = pChildX;
		k->m_cChildIndex[1]= ix;
	}
	k->m_pParent[2]= pDmnd->m_pParent[i>>1];
	k->m_pParent[3]= pDmnd->m_pParent[( ( ( i + 1 ) & 2 )>>1 ) + 2];
	k->m_pChild[0] = k->m_pChild[1]= k->m_pChild[2]= k->m_pChild[3]= ( SROAM_DIAMOND* )0;

	//set child information (iLevel, vertex info, flags, etc.)
	k->m_ucCull = 0;
	k->m_ucFlags= 0;
	k->m_ucSplitFlags= 0;
	if( ( k->m_pParent[2]->m_ucFlags & ROAM_CLIPPED ) ||
		( ( pDmnd->m_ucFlags & ROAM_CLIPPED ) && ( pChildX->m_ucFlags & ROAM_CLIPPED ) ) )
		k->m_ucFlags|= ROAM_CLIPPED;
	k->m_usTriIndex[0]= k->m_usTriIndex[1]= 0;
	k->m_sQueueIndex= -10;
	k->m_cLevel= pDmnd->m_cLevel+1;

	pDmndCenter= k->m_fVert;

	pParentVert0= k->m_pParent[2]->m_fVert;
	pParentVert1= k->m_pParent[3]->m_fVert;
	k->m_fVert[0]= ( float )fabs( ( pParentVert0[0] + pParentVert1[0] )/2.0f );
	k->m_fVert[2]= ( float )fabs( ( pParentVert0[2] + pParentVert1[2] )/2.0f );
	k->m_fVert[1]= GetTrueHeightAtPoint( ( int )k->m_fVert[0],
										 ( int )k->m_fVert[2] );

    //compute radius of diamond bounding sphere (squared)
	//calculate the bounding sphere for the current triangle
	//calculation 1
	fTempVert= k->m_pParent[0]->m_fVert;
	fSqrBound= SQR( ( fTempVert[0]-pDmndCenter[0] ) ) +
	 		   SQR( ( fTempVert[1]-pDmndCenter[1] ) ) +
			   SQR( ( fTempVert[2]-pDmndCenter[2] ) );

	fTempVert= k->m_pParent[1]->m_fVert;
	//calculation 2
	fSqrBoundTemp= SQR( ( fTempVert[0]-pDmndCenter[0] ) ) +
				   SQR( ( fTempVert[1]-pDmndCenter[1] ) ) +
				   SQR( ( fTempVert[2]-pDmndCenter[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( fSqrBoundTemp>fSqrBound )
		fSqrBound= fSqrBoundTemp;

	//calculation 3
	fTempVert= k->m_pParent[2]->m_fVert;
	fSqrBoundTemp= SQR( ( fTempVert[0]-pDmndCenter[0] ) ) +
				   SQR( ( fTempVert[1]-pDmndCenter[1] ) ) +
				   SQR( ( fTempVert[2]-pDmndCenter[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( fSqrBoundTemp>fSqrBound )
		fSqrBound= fSqrBoundTemp;

	//calculation 4
	fTempVert= k->m_pParent[3]->m_fVert;
	fSqrBoundTemp= SQR( ( fTempVert[0]-pDmndCenter[0] ) ) +
				   SQR( ( fTempVert[1]-pDmndCenter[1] ) ) +
				   SQR( ( fTempVert[2]-pDmndCenter[2] ) );
	
	//check to see if this is the largest distance we've calculated so far
	if( fSqrBoundTemp>fSqrBound )
		fSqrBound= fSqrBoundTemp;

	k->m_fBoundRad= fSqrBound;
	k->m_fErrorRad= SQR( m_fpLevelMDSize[k->m_cLevel] );

    return k;
}

//--------------------------------------------------------------
// Name:		 CROAM::Lock - private
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

		m_iFreeElements--;
	}
	
	pDmnd->m_ucLockCount++;
}

//--------------------------------------------------------------
// Name:		 CROAM::Unlock - private
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

		m_iFreeElements++;
	}
}

//--------------------------------------------------------------
// Name:		 CROAM::UpdateChildCull - private
// Description:	 Update a child's culling flag
// Arguments:	 -pDmnd: pointer to a diamond
// Return Value: None
//--------------------------------------------------------------
void CROAM::UpdateChildCull( SROAM_DIAMOND* pDmnd )
{
    SROAM_DIAMOND* pChild;
    int iCull, i;

	//CLIPPED diamonds have no interest here, back out
	if( pDmnd->m_ucFlags & ROAM_CLIPPED )
		return;

	iCull= pDmnd->m_ucCull; //save old culling flag for comparison

	//update the diamond's culling flags
	UpdateCull( pDmnd );

	//skip subtree if nothing has really changed
	if( iCull==pDmnd->m_ucCull && ( iCull==CULL_OUT || iCull==CULL_ALLIN ) )
		return;

	//update diamond priority if culling OUT state has changed
	if( ( iCull^pDmnd->m_ucCull ) & CULL_OUT )
		UpdatePriority( pDmnd );

	//if diamond is split, recurse down to it's four children if they exist
	if( pDmnd->m_ucFlags & ROAM_SPLIT )
	{
		for( i=0; i<4; i+=2 )
		{
			if( pChild= pDmnd->m_pChild[i] )
			{
				if( pChild->m_pParent[0]==pDmnd )
				{
					//update culling for the first child
					if( pChild->m_pChild[0] )
						UpdateChildCull( pChild->m_pChild[0] );

					//update culling for the second child
					if( pChild->m_pChild[1] )
						UpdateChildCull( pChild->m_pChild[1] );
				}
				else
				{
					//update culling for the third child
					if( pChild->m_pChild[2] )
						UpdateChildCull( pChild->m_pChild[2] );

					//update culling for the fourth child
					if( pChild->m_pChild[3] )
						UpdateChildCull( pChild->m_pChild[3] );
				}
			}
		}
	}
}

//--------------------------------------------------------------
// Name:		 CROAM::UpdateCull - private
// Description:	 Update a given diamond's culling flag
// Arguments:	 -pDmnd: pointer to a diamond
// Return Value: None
//--------------------------------------------------------------
void CROAM::UpdateCull( SROAM_DIAMOND* pDmnd )
{
    float r;
    int iCull, j, m;

	//get the diamond's parent's culling flag
	iCull= pDmnd->m_pParent[2]->m_ucCull;

	//if needed, update for all non-IN halfspaces
	if( iCull!=CULL_ALLIN && iCull!=CULL_OUT )
	{
		for( j=0, m=1; j<6; j++, m<<= 1 )
		{
			if( !( iCull & m ) )
			{
				r= m_pCamera->m_viewFrustum[j][0]*pDmnd->m_fVert[0] +
				   m_pCamera->m_viewFrustum[j][1]*pDmnd->m_fVert[1] +
				   m_pCamera->m_viewFrustum[j][2]*pDmnd->m_fVert[2] +
				   m_pCamera->m_viewFrustum[j][3];

				//cull the diamond
				if( SQR( r )>pDmnd->m_fBoundRad )
				{
					if( r<0.0f )
						iCull= CULL_OUT;
					else
						iCull|= m; //IN
				} //else still overlaps this frustum plane
			}
		} 
	}

    //if OUT state changes, update in/out listing on any draw tris
	if( ( pDmnd->m_ucCull^iCull ) & CULL_OUT )
	{
		for( j=0; j<2; j++ )
		{
			if( pDmnd->m_ucFlags & ( ROAM_TRI0<<j ) )
			{
				if( iCull & CULL_OUT )
					RemoveTri( pDmnd, j );
				else
					AddTri( pDmnd, j );
			}
		}
	}

	//store the updated cull flags
	pDmnd->m_ucCull= iCull;
}

//--------------------------------------------------------------
// Name:		 CROAM::UpdatePriority - private
// Description:	 Update a diamond's priority in the split/merge queues
// Arguments:	 -pDmnd: pointer to a diamond
// Return Value: None
//--------------------------------------------------------------
void CROAM::UpdatePriority( SROAM_DIAMOND* pDmnd )
{
	float d;
	int j, k, *pInt;

	//skip the update procedure if it has already been done
	if( m_iFrameCount==pDmnd->m_ucFrameCount )
		return;
	pDmnd->m_ucFrameCount= m_iFrameCount;

	if( ( pDmnd->m_ucFlags & ROAM_CLIPPED ) || pDmnd->m_cLevel >= m_iMaxLevel )
		k= 0;
	else
	{
		pInt= ( int* )( &d );		//set the local integer pointer (for the IEEE floating-point tricks)

		d= pDmnd->m_fErrorRad;

		//compute the fixed-point log_2 value (based on the error metric)
		k = *pInt;
		k+= m_iLog2Table[( k>>15 ) & 0xff];
    
		//distance calculation
		d= SQR( ( pDmnd->m_fVert[0]-m_pCamera->m_vecEyePos[0] ) ) +
		   SQR( ( pDmnd->m_fVert[1]-m_pCamera->m_vecEyePos[1] ) ) +
		   SQR( ( pDmnd->m_fVert[2]-m_pCamera->m_vecEyePos[2] ) );

		//compute the fixed-point log_2 value (based on the distance to the camera)
		j = *pInt;
		j+= m_iLog2Table[( j>>15 ) & 0xff];

		//compute the fixed-point log_2(error/distance)
		k= ( k-j )+0x10000000;

		//scale and clamp the priority index to [1, IQMAX-1]
		if( k<0 )
			k= 0;
			
		k= ( k>>16 )+1;
		if( k>=IQMAX )
			k= IQMAX-1;

		//for OUT diamonds, reduce priority (but leave them ordered)
		if( pDmnd->m_ucCull & CULL_OUT )
		{
			if( k>m_iSize )
				k-= ( m_iSize/2 );
			else
				k= ( k+1 )>>1;
		}
	}

	//update the queue index
	Enqueue( pDmnd, pDmnd->m_ucFlags & ROAM_ALLQ, k );
}

//--------------------------------------------------------------
// Name:		 CROAM::Enqueue - private
// Description:	 "Re-queue" a diamond in the appropriate queue
// Arguments:	 -pDmnd: pointer to a diamond
// Return Value: None
//--------------------------------------------------------------
void CROAM::Enqueue( SROAM_DIAMOND* pDmnd, int iQueueFlags, int iNewPQIndex )
{
	SROAM_DIAMOND** pQueue, *pDmndX;
  	int i, iDmndLock;

	//return early if the diamond is already in the queue
	if( ( pDmnd->m_ucFlags & ROAM_ALLQ )==iQueueFlags && pDmnd->m_sQueueIndex==iNewPQIndex )
		return;

	//determine the overall change in the diamond lock count
	iDmndLock= 0;
	if( pDmnd->m_ucFlags & ROAM_ALLQ )
		iDmndLock--;
	if( iQueueFlags & ROAM_ALLQ )
		iDmndLock++;

	//remove the diamond from the old queue (if needed)
	if( pDmnd->m_ucFlags & ROAM_ALLQ )
	{
		pQueue= ( ( pDmnd->m_ucFlags & ROAM_SPLITQ ) ? m_pSplitQueue : m_pMergeQueue );
		if( pDmnd->m_pPrevDmnd )
			pDmnd->m_pPrevDmnd->m_pNextDmnd= pDmnd->m_pNextDmnd;
		else
		{
			pQueue[pDmnd->m_sQueueIndex]= pDmnd->m_pNextDmnd;
			if( !pDmnd->m_pNextDmnd )
			{
				if( pDmnd->m_ucFlags & ROAM_SPLITQ )
				{
					if( pDmnd->m_sQueueIndex==m_iPQMax )
					{
						pDmndX= pQueue[0];
						pQueue[0]= ( SROAM_DIAMOND* )1;

						//loop until we get 'i' where we want it
						for( i=pDmnd->m_sQueueIndex; !pQueue[i]; i-- )
							;
						if( !( pQueue[0]= pDmndX ) && i==0 )
							i--;
						m_iPQMax= i;
					}
				}
				else
				{
					if( pDmnd->m_sQueueIndex==m_iPQMin )
					{
						pDmndX= pQueue[IQMAX-1];
						pQueue[IQMAX-1]= ( SROAM_DIAMOND* )1;

						//loop until we get 'i' where we want it
						for( i= pDmnd->m_sQueueIndex; !pQueue[i]; i++ )
							;
						if( !( pQueue[IQMAX-1]= pDmndX ) && i==IQMAX-1 )
							i++;
						m_iPQMin= i;
					}
				}
			}
		}
		if( pDmnd->m_pNextDmnd )
			pDmnd->m_pNextDmnd->m_pPrevDmnd= pDmnd->m_pPrevDmnd;
		pDmnd->m_ucFlags&= ~ROAM_ALLQ;
	}

	//update the diamond's priority
	pDmnd->m_sQueueIndex= iNewPQIndex;

	//insert the diamond into the new queue if needed
	if( iQueueFlags & ROAM_ALLQ )
	{
		//insert into the necessary queue bucket (split or merge)
		pQueue= ( ( iQueueFlags & ROAM_SPLITQ ) ? m_pSplitQueue : m_pMergeQueue );
		pDmnd->m_pPrevDmnd= ( SROAM_DIAMOND* )0;
		pDmnd->m_pNextDmnd= pQueue[pDmnd->m_sQueueIndex];

		pQueue[pDmnd->m_sQueueIndex]= pDmnd;
		if( pDmnd->m_pNextDmnd )
			pDmnd->m_pNextDmnd->m_pPrevDmnd= pDmnd;
		else
		{
			if( iQueueFlags & ROAM_SPLITQ )
			{
				if( pDmnd->m_sQueueIndex>m_iPQMax )
					m_iPQMax= pDmnd->m_sQueueIndex;
			}
			else
			{
				if( pDmnd->m_sQueueIndex<m_iPQMin )
					m_iPQMin= pDmnd->m_sQueueIndex;
			}
		}

		//specify which queue the current diamond is in
		pDmnd->m_ucFlags|= iQueueFlags;
	}

	//perform any required locking/unlocking
	if( iDmndLock!=0 )
	{
		if( iDmndLock<0 )
			Unlock( pDmnd );
		else
			Lock( pDmnd );
	}
}

//--------------------------------------------------------------
// Name:		 CROAM::Split - private
// Description:	 Split a diamond (finer detail)
// Arguments:	 -pDmnd: pointer to a diamond
// Return Value: None
//--------------------------------------------------------------
void CROAM::Split( SROAM_DIAMOND* pDmnd )
{
	SROAM_DIAMOND* pChild, *pParent;
	int i, s;

	//if the diamond has already been split, then skip it! And skip it good!
	if( pDmnd->m_ucFlags & ROAM_SPLIT )
		return;

	//split parents recursively (as needed)
	for( i=0; i<2; i++ )
	{
		pParent= pDmnd->m_pParent[i];
		Split( pParent );

		//if the diamond is pParent's first split child, take pParent off of the merge queue
		if( !( pParent->m_ucSplitFlags & SPLIT_K ) )
			Enqueue( pParent, ROAM_UNQ, pParent->m_sQueueIndex );
		pParent->m_ucSplitFlags|= SPLIT_K0<<pDmnd->m_cChildIndex[i];
	}

	//get the children, update flags, and put on the split queue
	for( i=0; i<4; i++ )
	{
		pChild= GetChild( pDmnd, i );
		UpdateCull( pChild );
		UpdatePriority( pChild );

		//children of the newly split diamond now go on the split queue
		Enqueue( pChild, ROAM_SPLITQ, pChild->m_sQueueIndex );
		s= ( pChild->m_pParent[1]==pDmnd ? 1 :0 );
		pChild->m_ucSplitFlags|= SPLIT_P0<<s;
		Unlock( pChild );

		//put the child triangles on the render list
		AllocateTri( pChild, s );
	}

	//diamond is split, update it's queue, and add to "check list"
	pDmnd->m_ucFlags|= ROAM_SPLIT;
	Enqueue( pDmnd, ROAM_MERGEQ, pDmnd->m_sQueueIndex );	//newly split diamond goes on merge queue

	//put parent tris back on the free list
	FreeTri( pDmnd, 0 );
	FreeTri( pDmnd, 1 );
}

//--------------------------------------------------------------
// Name:		 CROAM::Merge - private
// Description:	 Merge a diamond (coarser detail)
// Arguments:	 -pDmnd: pointer to a diamond
// Return Value: None
//--------------------------------------------------------------
void CROAM::Merge( SROAM_DIAMOND* pDmnd )
{
	SROAM_DIAMOND* k, *pParent;
	int i, s;

	//if this diamond has already been merged, then skip
	if( !( pDmnd->m_ucFlags & ROAM_SPLIT ) )
		return;

	//children off split queue if their other parent is not split
	for( i=0; i<4; i++ )
	{
		k= pDmnd->m_pChild[i];
		s= ( k->m_pParent[1]==pDmnd ? 1 : 0 );

		k->m_ucSplitFlags&= ~( SPLIT_P0<<s );
		if( !( k->m_ucSplitFlags & SPLIT_P ) )
			Enqueue( k, ROAM_UNQ, k->m_sQueueIndex );

		//put the tris back on the free list
		FreeTri( k, s );
	}

	//diamond is not split, update it's queue, and add to "check list"
	pDmnd->m_ucFlags&= ~ROAM_SPLIT;
	Enqueue( pDmnd, ROAM_SPLITQ, pDmnd->m_sQueueIndex );

	//update the diamond's parents, only if it is needed
	for( i=0; i<2; i++ )
	{
		pParent= pDmnd->m_pParent[i];

		pParent->m_ucSplitFlags&= ~( SPLIT_K0<<pDmnd->m_cChildIndex[i] );
		if( !( pParent->m_ucSplitFlags & SPLIT_K ) )
		{
			UpdatePriority( pParent );
			Enqueue( pParent, ROAM_MERGEQ, pParent->m_sQueueIndex );
		}
	}

	//put the parent tris on the triangle render list
	AllocateTri( pDmnd, 0 );
	AllocateTri( pDmnd, 1 );
}
