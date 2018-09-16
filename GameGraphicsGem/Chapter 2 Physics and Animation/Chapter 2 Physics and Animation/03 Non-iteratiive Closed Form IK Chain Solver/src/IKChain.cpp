
#include "IKChain.h"
#include <GL/openglut.h>

IKChain*		g_ikChain = NULL;
GLVector3f		g_ikGoalPosition;

//////////////////////////////////////////////////////////////////

IKChain* CreateIKChain( int numBones )
{
	CleanUpChain( );

	g_ikChain = (IKChain*)malloc( sizeof( IKChain ) );
	
	// Allocate the space of our bones
	g_ikChain->m_boneLocalRotations = (GLQuaternionf*)malloc( sizeof( GLQuaternionf ) * numBones );
	g_ikChain->m_boneLocalTranslations	= (GLVector3f*)malloc( sizeof( GLVector3f ) * numBones );
	g_ikChain->m_boneOrientations	= (GLQuaternionf*)malloc( sizeof( GLQuaternionf ) * numBones );
	g_ikChain->m_bonePositions		= (GLVector3f*)malloc( sizeof( GLVector3f ) * numBones );
	g_ikChain->m_fBoneLengths		= (float*)malloc( sizeof( float ) * numBones );
	g_ikChain->m_iNumBones			= numBones;

	for( int i = 0; i < numBones; i++ )
	{
		// Generate a random fk pose for the chain
		GLVector3f	axis(0.0f, 0.0f, 1.0f);
		float randomAngle = RangeRand(-160.0f, -20.0f);

		g_ikChain->m_boneLocalRotations[i].set( axis, randomAngle );

		if(i==0)
		{
			// Generate a position for the root bone
			g_ikChain->m_boneLocalTranslations[i].set( 0.0f, (float)numBones, 0.0f );

			g_ikChain->m_bonePositions[ i ]		= g_ikChain->m_boneLocalTranslations[ i ];
			g_ikChain->m_boneOrientations[ i ]	= g_ikChain->m_boneLocalRotations[ i ];
		}
		else
		{
			g_ikChain->m_boneLocalTranslations[i].set( RangeRand( 1.0f, 2.0f ), 0.0f, 0.0f );
			// Calculate a new global position based on the parent bones new orientation
			g_ikChain->m_bonePositions[ i ]		= g_ikChain->m_bonePositions[ i - 1 ] + g_ikChain->m_boneOrientations[ i - 1 ].rotateVector( g_ikChain->m_boneLocalTranslations[ i ] );

			g_ikChain->m_boneOrientations[ i ]	= g_ikChain->m_boneLocalRotations[ i ];
			g_ikChain->m_boneLocalRotations[i] = g_ikChain->m_boneOrientations[ i - 1 ].inverse() * g_ikChain->m_boneLocalRotations[ i ];

		}
		
		if( i > 0 )
		{
			// Each bones length is defined b its childs offset. 
			g_ikChain->m_fBoneLengths[ i - 1 ] = g_ikChain->m_boneLocalTranslations[ i ].length();
			
			if( i == (numBones - 1) )
			{	// Except for the last bone where we have no child.
				g_ikChain->m_fBoneLengths[ i ] = RangeRand( 1.0f, 2.0f );
			}
		}
	}

	// Put the goal at the tip of the chain. 
	g_ikGoalPosition = g_ikChain->m_bonePositions[numBones-1] + ( g_ikChain->m_boneOrientations[numBones-1].GetXaxis() * g_ikChain->m_fBoneLengths[numBones-1] );

	// Sum the bone lengths to get the total chain length. 
	// Skip the first bone so that the length is remaining chain length
	// Note: We assume that the bone lengths don't change from frame to frame.
	g_ikChain->m_fChainLength = 0.0f;
	for( unsigned int i=0; i < g_ikChain->m_iNumBones; i++ )
	{
		g_ikChain->m_fChainLength += g_ikChain->m_fBoneLengths[ i ];
	}

	return g_ikChain;
}


void	 CleanUpChain( )
{
	if( !g_ikChain ) return;
	free( g_ikChain->m_boneLocalRotations );
	free( g_ikChain->m_boneLocalTranslations );
	free( g_ikChain->m_boneOrientations );
	free( g_ikChain->m_fBoneLengths );
	free( g_ikChain );
	g_ikChain = NULL;
}

//////////////////////////////////////////////////////////////////
// This funtction ,mimics what would normally be your
// animation system evaluating and gerating a new global 
// pose.
void UpdateChainFK( )
{
	for( unsigned int i = 0; i < g_ikChain->m_iNumBones; i++ )
	{		
		if(i==0)
		{
			g_ikChain->m_bonePositions[ i ]		= g_ikChain->m_boneLocalTranslations[ i ];
			g_ikChain->m_boneOrientations[ i ]	= g_ikChain->m_boneLocalRotations[ i ];
		}
		else
		{
			// Calculate a new global position based on the parent bones new orientation
			g_ikChain->m_bonePositions[ i ]		= g_ikChain->m_bonePositions[ i - 1 ] + g_ikChain->m_boneOrientations[ i - 1 ].rotateVector( g_ikChain->m_boneLocalTranslations[ i ] );
			g_ikChain->m_boneOrientations[ i ]	= g_ikChain->m_boneOrientations[ i - 1 ] * g_ikChain->m_boneLocalRotations[ i ];
		}
	}

}



void SetBoneFKOrientation( int boneid, GLQuaternionf	globalOrientation )
{
	if( boneid == 0 )
	{
		g_ikChain->m_boneLocalRotations[ boneid ] = globalOrientation;
	}
	else
	{
		// Calculate a local offset based  on the parents global orientation
		GLQuaternionf parentGlobal = g_ikChain->m_boneOrientations[ boneid - 1 ];
		g_ikChain->m_boneLocalRotations[ boneid ] = parentGlobal.inverse() * globalOrientation;
	}
}

void SetBoneFKPosition( int boneid, GLVector3f bonePosition )
{
	if( boneid == 0 )
	{
		g_ikChain->m_boneLocalTranslations[ boneid ] = bonePosition;
	}
	else
	{
		// Calculate a local offset based on the parents global orientation
		GLQuaternionf parentGlobal = g_ikChain->m_boneOrientations[ boneid - 1 ];
		g_ikChain->m_boneLocalTranslations[ boneid ] = parentGlobal.inverse().rotateVector( bonePosition - g_ikChain->m_bonePositions[ boneid - 1 ] );
	}
}