//==============================================================
//==============================================================
//= particle.h =================================================
//= Original coder: Trent Polack (trent@voxelsoft.com)	       =
//==============================================================
//= A *VERY* simple Particle Engine							   =
//==============================================================
//==============================================================
#ifndef __PARTICLE_H__
#define __PARTICLE_H__

//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "../Base Code/math_ops.h"
#include "../Base Code/image.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- STRUCTURES -------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//The particle structure
typedef struct SPARTICLE_TYP
{
	float m_fLife;

	CVECTOR m_vecPosition;
	CVECTOR m_vecVelocity;

	CVECTOR m_vecSize;
	CVECTOR m_vecSizeCounter;
	float m_fMass;
	
	CVECTOR m_vecColor;
	CVECTOR m_vecColorCounter;
	float m_fTranslucency;
	float m_fTranslucencyCounter;

	float m_fFriction;
} SPARTICLE, *SPARTICLE_PTR;


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CPARTICLE_ENGINE
{
	private:
		SPARTICLE* m_pParticles;
		int m_iNumParticles;

		int m_iNumParticlesOnScreen;

		//gravity
		CVECTOR m_vecForces;

		//base particle attributes
		float m_fMaxLife;

		CVECTOR m_vecPosition;

		CVECTOR m_vecStartSize;
		CVECTOR m_vecEndSize;
		float m_fMass;

		CVECTOR m_vecStartColor;
		CVECTOR m_vecEndColor;

		float m_fStartTranslucency;
		float m_fEndTranslucency;

		float m_fFriction;

		unsigned int m_uiTexID;

	void CreateParticle( float fVelX, float fVelY, float fVelZ );

	//--------------------------------------------------------------
	// Name:			CPARTICLE_ENGINE::RangedRandom - private
	// Description:		Get a random value between the two arguments
	// Arguments:		-f1, f2: Random boundaries
	// Return Value:	A floating point value: the random number
	//--------------------------------------------------------------
	inline float RangedRandom( float f1, float f2 )
	{	return ( f1+( f2-f1 )*( ( float )rand( ) )/( ( float )RAND_MAX ) );	}

	public:
		
	bool Init( int iNumParticles );
	void Shutdown( void );

	void Update( float fTimeStep= 1.0f );
	void Render( void );

	void Explode( float fMagnitude, int iNumParticles );
	void CreateRaindrops( float fMinX, float fMinY, float fMinZ,
						  float fMaxX, float fMaxY, float fMaxZ, int iDropSpeed, int iNumDrops );

	//load a texture for the particle engine
	inline void LoadTexture( char* szFilename )
	{
		CIMAGE tex;

		tex.Load( szFilename, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true );
		m_uiTexID= tex.GetID( );
	}

	//set the lifespan of a created particle
	void SetMaxLife( float iLife )
	{	m_fMaxLife= iLife;	}

	//set the particle emitter's position
	void SetEmissionPosition( float x, float y, float z )
	{	m_vecPosition.Set( x, y, z );	}

	//set the mass of a created particle
	void SetMass( float fMass )
	{	m_fMass= fMass;	}

	//set the size range of a created particle
	void SetSize( float fStartWidth, float fStartHeight,
				  float fEndWidth, float fEndHeight )
	{
		m_vecStartSize.Set( fStartWidth, 0.0f, fStartHeight );
		m_vecEndSize.Set( fEndWidth, 0.0f, fEndHeight );
	}

	//set the color range of a created particle
	void SetColor( float fStartRed, float fStartGreen, float fStartBlue,
				   float fEndRed, float fEndGreen, float fEndBlue )
	{
		m_vecStartColor.Set( fStartRed, fStartGreen, fStartBlue );
		m_vecEndColor.Set( fEndRed, fEndGreen, fEndBlue );
	}

	//set the translucency range of a created particle
	void SetTranslucency( float fStartTranslucency, float fEndTranslucency )
	{
		m_fStartTranslucency= fStartTranslucency;
		m_fEndTranslucency= fEndTranslucency;
	}

	//set the friction (air resistance) of a created particle
	void SetFriction( float fFriction )
	{	m_fFriction= fFriction;	}

	//set the external forces acting against a particle (gravity, air, etc.)
	void SetExternalForces( float x, float y, float z )
	{	m_vecForces.Set( x, y, z );	}

	//get the number of particles on screen
	int GetNumParticlesOnScreen( void )
	{	return m_iNumParticlesOnScreen;	}

	CPARTICLE_ENGINE( void )
	{	}
	~CPARTICLE_ENGINE( void )
	{	}
};


#endif	//__PARTICLE_H__