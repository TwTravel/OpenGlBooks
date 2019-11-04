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
	float m_fMass;
	
	CVECTOR m_vecColor;
	float m_fTranslucency;

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
		float m_fLife;

		CVECTOR m_vecPosition;

		CVECTOR m_vecSize;
		float m_fMass;

		CVECTOR m_vecColor;

		float m_fFriction;

		unsigned int m_uiTexID;

	void CreateParticle( float fVelX, float fVelY, float fVelZ );

	public:
		
	bool Init( int iNumParticles );
	void Shutdown( void );

	void Update( void );
	void Render( void );

	void Explode( float fMagnitude, int iNumParticles );

	//load a texture for the particle engine
	inline void LoadTexture( char* szFilename )
	{
		CIMAGE tex;

		tex.Load( szFilename, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, true );
		m_uiTexID= tex.GetID( );
	}

	//set the lifespan of a created particle
	void SetLife( float iLife )
	{	m_fLife= iLife;	}

	//set the particle emitter's position
	void SetEmissionPosition( float x, float y, float z )
	{	m_vecPosition.Set( x, y, z );	}

	//set the mass of a created particle
	void SetMass( float fMass )
	{	m_fMass= fMass;	}

	//set the size of a created particle
	void SetSize( float width, float height )
	{	m_vecSize.Set( width, 0.0f, height );	}

	//set the color of a created particle
	void SetColor( float fRed, float fGreen, float fBlue )
	{	m_vecColor.Set( fRed, fGreen, fBlue );	}

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