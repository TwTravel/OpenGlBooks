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

//frustum bitmasks
#define CULL_ALLIN 0x3f
#define CULL_OUT   0x40


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DATA STRUCTURES --------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
struct SROAM_DIAMOND
{
    SROAM_DIAMOND* m_pParent[4];				//diamond's parents (two corners)
    SROAM_DIAMOND* m_pChild[4];					//diamond's kids
    SROAM_DIAMOND* m_pPrevDmnd, *m_pNextDmnd;	//prev and next links on queue or free list

    float m_fVert[3];							//vertex position
    float m_fBoundRad;							//radius of the diamond's bounding sphere
    float m_fErrorRad;							//radius of pointwise error squared
	short m_sPQIndex;
    char m_cChildIndex[2];						//our kid index within each of our parents
    char m_cLevel;								//level of resolution

	unsigned char m_ucCull;						//visible/not-visible cull flag
	unsigned char m_ucFlags;					//misc. flags
    unsigned char m_ucLockCount;				//number of references (if 0, the diamond is free for use)

	char m_cPadding[4];							//keep the structure properly aligned
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
		SROAM_DIAMOND* m_pLevel0Dmnd[3][3];				//base diamonds level 0
		SROAM_DIAMOND* m_pLevel1Dmnd[4][4];				//base diamonds level -1,-2...

		float* m_fpLevelMDSize;				//max midpoint displacement per level
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

	void RenderChild( SROAM_DIAMOND* pDmnd, int iIndex, int iCull );

	SROAM_DIAMOND* Create( void );
	SROAM_DIAMOND* GetChild( SROAM_DIAMOND* pDmnd, int iIndex );

	void Lock( SROAM_DIAMOND* pDmnd );
	void Unlock( SROAM_DIAMOND* pDmnd );

	public:


	void Init( int maxLevel, int poolSize, CCAMERA* pCamera );
	void Shutdown( void );
	void Render( void );

	CROAM( void ) { }
	~CROAM( void ) { }
};


#endif	//__ROAM_H__