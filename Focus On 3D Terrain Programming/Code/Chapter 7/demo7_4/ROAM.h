//==============================================================
//==============================================================
//= ROAM.h =====================================================
//= Original coders: Trent Polack (trent@voxelsoft.com) and	   =
//=				     Mark Duchaineau						   =
//==============================================================
//= The header file for the ROAM implementation				   =
//==============================================================
//==============================================================
#ifndef __ROAM_H__
#define __ROAM_H__


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "../Base Code/camera.h"

#include "terrain.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CONSTANTS --------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#define IQMAX         4096		//number of buckets in priority queue
#define TRI_IMAX     65536		//number of triangle-chunk slots
#define DMCHECKLISTN 65536		//check list size

//frustum bitmasks
#define CULL_ALLIN 0x3f
#define CULL_OUT   0x40

//misc. diamond flags
#define ROAM_SPLIT    0x01
#define ROAM_TRI0     0x04
#define ROAM_TRI1     0x08
#define ROAM_CLIPPED  0x10
#define ROAM_SPLITQ   0x40
#define ROAM_MERGEQ   0x80
#define ROAM_ALLQ     0xc0
#define ROAM_UNQ      0x00


//parent/child split flags
#define SPLIT_K0      0x01
#define SPLIT_K       0x0f
#define SPLIT_P0      0x10
#define SPLIT_P       0x30


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DATA STRUCTURES --------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
struct SROAM_DIAMOND
{
	SROAM_DIAMOND* m_pParent[4];					//diamond's parents (two corners)
	SROAM_DIAMOND* m_pChild[4];					//diamond's kids
	SROAM_DIAMOND* m_pPrevDmnd, *m_pNextDmnd;		//prev and next links on queue or free list

	float m_fVert[3];							//vertex position
	float m_fBoundRad;							//radius of the diamond's bounding sphere
	float m_fErrorRad;							//radius of pointwise error squared

	short m_sQueueIndex;
	unsigned short m_usTriIndex[2];

	char m_cChildIndex[2];						//our kid index within each of our parents
	char m_cLevel;								//level of resolution
	unsigned char m_ucLockCount;				//number of references (if 0, the diamond is free for use)
	unsigned char m_ucFrameCount;
	unsigned char m_ucCull;
	unsigned char m_ucFlags;
	unsigned char m_ucSplitFlags;
	unsigned char m_ucPadding[3];
};


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CROAM : public CTERRAIN
{
	private:
		CCAMERA* m_pCamera;

		SROAM_DIAMOND* m_pDmndPool;						//diamond storage pool
		int m_iPoolSize;								//pool size (number of diamonds)

		SROAM_DIAMOND* m_pFreeDmnd[2];					//most recently unlocked diamonds
		SROAM_DIAMOND* m_pLevel0Dmnd[4][4];				//base diamonds level 0
		SROAM_DIAMOND* m_pLevel1Dmnd[4][4];				//base diamonds level -1,-2...

		SROAM_DIAMOND* m_pSplitQueue[IQMAX];				//split priority queue
		SROAM_DIAMOND* m_pMergeQueue[IQMAX];				//merge priority queue
		int m_iPQMin, m_iPQMax;							//min/max occupied bucket

		int* m_ipPDmndIS;								//packed diamond index and side

		int m_iFreeElements;							//number of elements on free list
		int m_iFrameCount;								//current frame count
		int m_iQueueCoarse;								//coarseness limit on priority index
		int m_iMaxTriChunks;							//max number of tri chunks allowed
		int m_iFreeTriCount;							//number of triangles on free list
		int m_iMaxTris;									//target triangle count
		int m_iFreeTri;									//first free tri chunk

		int m_iLog2Table[256];							//correction to float->int conversions
		
		float* m_fVertTexBuffer;						//data attachment specific to a library step

		float* m_fpLevelMDSize;							//max midpoint displacement per level
		int m_iMaxLevel;

	//--------------------------------------------------------------
	// Name:		 CROAM::ShiftCoords - private
	// Description:  Scale an (x, z) vertex
	// Arguments:	 -x, z: coordinates to shift
	// Return Value: None
	//--------------------------------------------------------------
	inline void ShiftCoords( float* x, float* z )
	{
		*x= ( *x+1.0f )/2.0f;	//shift X into [0, 2] then divide by two shift into [0...1]
		*z= ( *z+1.0f )/2.0f;	//shift Z into [0, 2] then divide by two shift into [0...1]

		*x*= m_iSize;			//translate into map-coords
		*z*= m_iSize;			//translate into map-coords
	}

	SROAM_DIAMOND* Create( void );
	SROAM_DIAMOND* GetChild( SROAM_DIAMOND* pDmnd, int iIndex );

	void Lock( SROAM_DIAMOND* pDmnd );
	void Unlock( SROAM_DIAMOND* pDmnd );

	void AllocateTri( SROAM_DIAMOND* dm, int j );
	void FreeTri( SROAM_DIAMOND* dm, int j );
	void AddTri( SROAM_DIAMOND* dm, int j );
	void RemoveTri( SROAM_DIAMOND* dm, int j );

	void UpdateDiamondTri( SROAM_DIAMOND* dm,int j );
	void UpdateChildCull( SROAM_DIAMOND* dm );
	void Split( SROAM_DIAMOND* dm );
	void Merge( SROAM_DIAMOND* dm );
	void UpdateCull( SROAM_DIAMOND* dm );
	void UpdatePriority( SROAM_DIAMOND* dm );
	void Enqueue( SROAM_DIAMOND* dm,int qflags,int iq_new );

	public:


	void Init( int maxLevel, int poolSize, CCAMERA* pCamera );
	void Shutdown( void );

	void Update( void );
	void Render( void );

	CROAM( void ) { }
	~CROAM( void ) { }
};


#endif	//__ROAM_H__