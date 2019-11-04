//==============================================================
//==============================================================
//= particle.h =================================================
//= Original coder: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= A *VERY* simple Particle Engine							   =
//==============================================================
//==============================================================

//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "../Base Code/gl_app.h"

#include "particle.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:		 CPARTICLE_ENGINE::Init - public
// Description:	 Initialize the particle engine
// Arguments:	 -iNumParticles: number of particles in the system
// Return Value: A boolean variable: -true: successful initiation
//									 -false: unsuccessful initiation
//--------------------------------------------------------------
bool CPARTICLE_ENGINE::Init( int iNumParticles )
{
	m_iNumParticles= iNumParticles;
	m_pParticles= new SPARTICLE [m_iNumParticles];
	if( m_pParticles==NULL )
	{
		g_log.Write( LOG_FAILURE, "Could not allocate memory for the particle buffer" );
		return false;
	}

	return true;
}

//--------------------------------------------------------------
// Name:		 CPARTICLE_ENGINE::Shutdown - public
// Description:	 Shutdown the system
// Arguments:	 None
// Return Value: None
//--------------------------------------------------------------
void CPARTICLE_ENGINE::Shutdown( void )
{
	delete[] m_pParticles;
}

//--------------------------------------------------------------
// Name:		 CPARTICLE_ENGINE::CreateParticle - private
// Description:	 Create a new particle
// Arguments:	 -fVelX, fVelY, fVelZ: the new particle's velocity
// Return Value: None
//--------------------------------------------------------------
void CPARTICLE_ENGINE::CreateParticle( float fVelX, float fVelY, float fVelZ )
{
	int iChoice;
	int i;

	iChoice= -1;

	//find an open particle
	for( i=0; i<m_iNumParticles; i++ )
	{
		if( m_pParticles[i].m_fLife<=0.0f )
		{
			iChoice= i;
			break;
		}
	}

	if(iChoice==-1)
		return;

	//set the particle's lifespan
	m_pParticles[iChoice].m_fLife= RANDOM_FLOAT*m_fMaxLife;

	//set the particle's position
	m_pParticles[iChoice].m_vecPosition= m_vecPosition;

	//set the particle's velocity
	m_pParticles[iChoice].m_vecVelocity.Set( fVelX, fVelY, fVelZ );
	
	//set the particle's color and transparency
	m_pParticles[iChoice].m_vecColor= m_vecStartColor;
	m_pParticles[iChoice].m_vecColorCounter= ( m_vecEndColor-m_pParticles[iChoice].m_vecColor )/m_pParticles[iChoice].m_fLife;

	//set the particle's transparency
	m_pParticles[iChoice].m_fTranslucency= m_fStartTranslucency;
	m_pParticles[iChoice].m_fTranslucencyCounter= ( m_fEndTranslucency-m_pParticles[iChoice].m_fTranslucency )/m_pParticles[iChoice].m_fLife;
	
	//set the particle's size
	m_pParticles[iChoice].m_vecSize= m_vecStartSize;
	m_pParticles[iChoice].m_vecSizeCounter= ( m_vecEndSize-m_pParticles[iChoice].m_vecSize )/m_pParticles[iChoice].m_fLife;

	//set the particles mass
	m_pParticles[iChoice].m_fMass= m_fMass;

	//set the particle's air-resistance
	m_pParticles[iChoice].m_fFriction= m_fFriction;
}

//--------------------------------------------------------------
// Name:		 CPARTICLE_ENGINE::Update - public
// Description:	 Update the particle engine
// Arguments:	 None
// Return Value: None
//--------------------------------------------------------------
void CPARTICLE_ENGINE::Update( void )
{
	CVECTOR	vecMomentum;
	int		i;

	//loop through the particles
	for( i=0; i<m_iNumParticles; i++ )
	{
		//age the particle
		m_pParticles[i].m_fLife-= 1;

		//only update the particle if it's alive (no WAY!)
		if( m_pParticles[i].m_fLife>0.0f )
		{
			vecMomentum= m_pParticles[i].m_vecVelocity*m_pParticles[i].m_fMass;

			//update the particle's position
			m_pParticles[i].m_vecPosition+= vecMomentum;

			//update the particle's members
			m_pParticles[i].m_vecColor+= m_pParticles[i].m_vecColorCounter;
			m_pParticles[i].m_fTranslucency+= m_pParticles[i].m_fTranslucencyCounter;
			m_pParticles[i].m_vecSize+= m_pParticles[i].m_vecSizeCounter;

			//now its time for the external forces to take their toll
			m_pParticles[i].m_vecVelocity*= 1-m_pParticles[i].m_fFriction;
			m_pParticles[i].m_vecVelocity+= m_vecForces;
		}
	}
}

//--------------------------------------------------------------
// Name:		 CPARTICLE_ENGINE::Render - public
// Description:	 Render the particle engine
// Arguments:	 None
// Return Value: None
//--------------------------------------------------------------
void CPARTICLE_ENGINE::Render( void )
{
	CVECTOR vecMtrxRight, vecMtrxUp;
	CVECTOR vecPosition, vecSize, vecTemp;
	float fMatrix[16];
	int i;

	//enable blending and texturing
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );

	//turn off depth testing
	glDisable( GL_DEPTH_TEST );

	//bind the particle image
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, m_uiTexID );

	//extract the up and right vectors (for billboarding) from the view matrix
	glGetFloatv( GL_MODELVIEW_MATRIX, fMatrix );
	vecMtrxRight.Set( fMatrix[0], fMatrix[4], fMatrix[8] );
	vecMtrxUp.Set( fMatrix[1], fMatrix[5], fMatrix[9] );

	m_iNumParticlesOnScreen= 0;

	for( i=0; i<m_iNumParticles; i++ )
	{
		if( m_pParticles[i].m_fLife>0.0f )
		{
			//store our particle's values in temp variables (to save on space in the lines to follow)
			vecPosition[0]= m_pParticles[i].m_vecPosition[0];
			vecPosition[1]= m_pParticles[i].m_vecPosition[1];
			vecPosition[2]= m_pParticles[i].m_vecPosition[2];
			vecSize[0]= m_pParticles[i].m_vecSize[0];
			vecSize[1]= m_pParticles[i].m_vecSize[2];
			vecSize[2]= ( vecSize[0]+vecSize[2] )/2;

			//render our particle (as a triangle strip)
			glBegin(GL_TRIANGLE_STRIP);
				glColor4f( m_pParticles[i].m_vecColor[0],
						   m_pParticles[i].m_vecColor[1],
						   m_pParticles[i].m_vecColor[2],
						   m_pParticles[i].m_fTranslucency );

				//top right
				vecTemp= ( ( vecMtrxRight+vecMtrxUp )*vecSize )+vecPosition;
				glTexCoord2f( 1, 1 );	glVertex3f( vecTemp[0], vecTemp[1], vecTemp[2] );

				//top left
				vecTemp= ( ( vecMtrxUp-vecMtrxRight )*vecSize )+vecPosition;
				glTexCoord2f( 0, 1 );	glVertex3f( vecTemp[0], vecTemp[1], vecTemp[2] );

				//bottom right
				vecTemp= ( ( vecMtrxRight-vecMtrxUp )*vecSize )+vecPosition;
				glTexCoord2f( 1, 0 );	glVertex3f( vecTemp[0], vecTemp[1], vecTemp[2] );

				//bottom left
				vecTemp= ( ( vecMtrxRight+vecMtrxUp )*-vecSize )+vecPosition;
				glTexCoord2f( 0, 0 );	glVertex3f( vecTemp[0], vecTemp[1], vecTemp[2] );
			glEnd( );

			m_iNumParticlesOnScreen++;
		}
	}

	glEnable( GL_DEPTH_TEST );
}

//--------------------------------------------------------------
// Name:		 CPARTICLE_ENGINE::Explode - public
// Description:	 Make an explosion of particles
// Arguments:	 -fMagnitude: power of the explosion
//				 -iNumParticles: number of particles involved
// Return Value: None
//--------------------------------------------------------------
void CPARTICLE_ENGINE::Explode( float fMagnitude, int iNumParticles )
{
	float fYaw;
	float fPitch;

	//create our particles
	while( --iNumParticles>0 )
	{
		//set the particle's angle
		fYaw  = RANDOM_FLOAT*PI*2.0f;
		fPitch= DEG_TO_RAD( RANDOM_FLOAT*( rand( )%360 ) );

		//create the particle
		CreateParticle( ( cosf( fPitch ) )*( fMagnitude*RANDOM_FLOAT ),
						( sinf( fPitch )*cosf( fYaw ) )*( fMagnitude*RANDOM_FLOAT ),
						( sinf( fPitch )*sinf( fYaw ) )*( fMagnitude*RANDOM_FLOAT ) );
	}
}
