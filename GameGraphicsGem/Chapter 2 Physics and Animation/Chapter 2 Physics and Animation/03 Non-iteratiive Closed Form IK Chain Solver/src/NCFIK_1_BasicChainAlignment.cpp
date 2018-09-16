	
#include <stdlib.h>
#include "Display.h"
#include "Interaction.h"
#include "IKChain.h"


///////////////////////////////////////////////
// Note 1: 
// 3 modifications have been made to the GLQuaternion class.

    //!Create a rotation quaternion from 2 normalized vectors. 
	//	The resulting rotation transforms vec1 onto vec2.
//	GLQuaternion( const GLVector3<T>& vec1, const GLVector3<T>& vec2 );

	// Build a rotation that would transform vec1 onto vec2 using the shortest arc
//	void setFrom2Vectors( const GLVector3<T>& vec1, const GLVector3<T>& vec2 );

	// Calculate the vector that is aligned with the quaternions X axis frame
//	GLVector3<T> GetXaxis( )

///////////////////////////////////////////////
// Note 2: 
// 1 modification has been made to the GLVector3f class.

    //!Normalize this vector in place, makes this a unit vector, US spelling
	// Returns the previous length of the vector.
//    float normalizeInPlace()


///////////////////////////////////////////////
// NCFIK_1_BasicChainAlignment
// This function simply peforms the alignment step.
// The entire chain is aligned with the IK goal, and
// each bone after the first bone is also corrected based 
// on any modification to the bones position cause by the 
// parents re-orientation

void NCFIK_1_BasicChainAlignment( IKChain *pChain, GLVector3f goalPosition, unsigned int options )
{
	// Calculate the position at the tip of the last bone. 
	GLVector3f	fkChainTip	= pChain->m_bonePositions[ pChain->m_iNumBones - 1 ] + ( pChain->m_boneOrientations[ pChain->m_iNumBones-1 ].GetXaxis() * pChain->m_fBoneLengths[ pChain->m_iNumBones - 1 ] );

	GLVector3f	vecToIkGoal,	vecToFkChainTip;
	float		distToIkGoal,	distToFkChainTip;
	GLQuaternionf				chainOffsetRotation;
	
	for( unsigned int i = 0; i < pChain->m_iNumBones; i++ )
	{
		if( i > 0 )
		{	// Transform the bone position by the overall chain offset.
			pChain->m_bonePositions[ i ] = pChain->m_bonePositions[ 0 ] + chainOffsetRotation.rotateVector( pChain->m_bonePositions[ i ] - pChain->m_bonePositions[ 0 ] );
		}
		vecToFkChainTip		= fkChainTip - pChain->m_bonePositions[ i ];
		distToFkChainTip	= vecToFkChainTip.normalizeRetLength();

		if( i > 0 )
		{	// Calculate a new global position based on the parent bones new orientation
			pChain->m_bonePositions[ i ] = pChain->m_bonePositions[ i-1 ] + ( pChain->m_boneOrientations[ i-1 ].GetXaxis() * pChain->m_fBoneLengths[ i-1 ] );
		}
		vecToIkGoal			= goalPosition - pChain->m_bonePositions[ i ];
		distToIkGoal		= vecToIkGoal.normalizeRetLength();
		
		if( i==0 )
		{	// For the first bone calculate and store the overall chain offset towards the ik target
			chainOffsetRotation.setFrom2Vectors( vecToFkChainTip, vecToIkGoal );
			fkChainTip = pChain->m_bonePositions[ i ] + ( vecToIkGoal * distToFkChainTip );
			pChain->m_boneOrientations[ i ] = chainOffsetRotation * pChain->m_boneOrientations[ i ];
		}
		else
		{	// Apply the chain offset, and apply any incremental correction.
			// Thie additional correction is required due to a new bone position based on the new parent orientation
			pChain->m_boneOrientations[ i ] = GLQuaternionf( vecToFkChainTip, vecToIkGoal ) * chainOffsetRotation * pChain->m_boneOrientations[ i ];
		}

		// Based on the bone index, select an appropriate method to solve
		if(i <= ( pChain->m_iNumBones - 2 ) )
		{
			GLVector3f boneLengthVector		= pChain->m_boneOrientations[i].GetXaxis();
			GLVector3f bendAxis				= vecToIkGoal.getCross( boneLengthVector );
			bendAxis.normalize();

			// this is the current angle of the bone.
			float fkBoneAngle				= acosf( boneLengthVector.dot( vecToIkGoal ) );

			if( options&OPTION_DEBUG )
			{
				GLVector3f debugFkChainTip		= pChain->m_bonePositions[ i ] + ( vecToIkGoal * distToFkChainTip );

				glColor3f(0.1f, 0.9f, 0.1f);
				DrawDashedLine( pChain->m_bonePositions[ i ], debugFkChainTip );
				DrawDashedArc( pChain->m_bonePositions[ i ], bendAxis, vecToIkGoal * ( pChain->m_fBoneLengths[ i ] * 0.7f ), 0.0f, fkBoneAngle );
			}
		}
	}
}
