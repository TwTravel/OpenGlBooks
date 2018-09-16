
#include <stdlib.h>
#include "Display.h"
#include "Interaction.h"
#include "IKChain.h"

///////////////////////////////////////////////
// Note: 
// 3 modifications have been made to the GLQuaternion class.

    //!Create a rotation quaternion from 2 normalized vectors. 
	//	The resulting rotation transforms vec1 onto vec2.
//	GLQuaternion( const GLVector3<T>& vec1, const GLVector3<T>& vec2 );

	// Build a rotation that would transform vec1 onto vec2 using the shortest arc
//	void setFrom2Vectors( const GLVector3<T>& vec1, const GLVector3<T>& vec2 );

	// Calculate the vector that is aligned with the quaternions X axis frame
//	GLVector3<T> GetXaxis( )


///////////////////////////////////////////////
// NCFIK_3_SolveLongerChains
// This implimentation features 2 methods to calculate the maximum bone angles
// When the chain is getting close to its extension limit ( distToFkChainTip > remainingChainLength )
// Then trigonometry is used to calculate the maximum bone angle. This ensures that
// the chain will stretch out straigh and reach the limit of its extension correctly.
// When the goal is closer the to root of the chain then the maximum bone angle is
// calculated directly from the remaining chain length

void NCFIK_3_SolveLongerChains( IKChain *pChain, GLVector3f goalPosition, unsigned int options )
{
	// Calculate the position at the tip of the last bone. 
	GLVector3f	fkChainTip	= pChain->m_bonePositions[ pChain->m_iNumBones - 1 ] + ( pChain->m_boneOrientations[ pChain->m_iNumBones-1 ].GetXaxis() * pChain->m_fBoneLengths[ pChain->m_iNumBones - 1 ] );

	GLVector3f	vecToIkGoal,	vecToFkChainTip;
	float		distToIkGoal,	distToFkChainTip;
	GLQuaternionf				chainOffsetRotation;
	float		maxFkBoneAngle, maxIkBoneAngle;
	float		fkBoneAngle,	ikBoneAngle, deltaBoneAngle;
	float		remainingChainLength = pChain->m_fChainLength;

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
			// Remove the current bones length from the chain. 
			remainingChainLength		-= pChain->m_fBoneLengths[ i ];
			GLVector3f boneLengthVector = pChain->m_boneOrientations[i].GetXaxis();
			GLVector3f bendAxis			= vecToIkGoal.getCross( boneLengthVector );
			bendAxis.normalizeRetLength();

			// this is the current angle of the bone.
			fkBoneAngle					= acosf( boneLengthVector.dot( vecToIkGoal ) );
			if(i == ( pChain->m_iNumBones - 2 ) )
			{	// Use trigonometry to determine the ikBoneAngle
				// Law of cosines. a = BoneLength; b = Child BoneLength; c = Distance to the Ik Goal;
				ikBoneAngle = acosf( Clamp( (Sq(pChain->m_fBoneLengths[ i ]) + Sq( distToIkGoal ) - Sq( remainingChainLength ))/( 2.0f * pChain->m_fBoneLengths[ i ] * distToIkGoal ), -1.0f, 1.0f ));
			}	
			else
			{
				// distance from the bone tip to the ik gloal
				if( distToFkChainTip > remainingChainLength )
				{	// Using the law of cosines, calculate the maximum angle of this bone using the fk Chain tip 
					maxFkBoneAngle = acosf( Clamp( ( Sq(pChain->m_fBoneLengths[ i ]) + Sq( distToFkChainTip ) - Sq( remainingChainLength ))/(2 * pChain->m_fBoneLengths[i] * distToFkChainTip), -1.0f, 1.0f ));
				}
				else
				{	// Add on the remaining chain length as radians.
					maxFkBoneAngle = acosf( Clamp( ( pChain->m_fBoneLengths[ i ] * 0.5f ) / remainingChainLength, 0.0f, 1.0f ) );
					maxFkBoneAngle += (( remainingChainLength - distToFkChainTip ) / pChain->m_fBoneLengths[ i ]);
				}
				
				// distance from the bone tip to the ik gloal
				if( distToIkGoal > remainingChainLength )
				{	// Using the law of cosines, calculate the maximum angle of this bone using the fk Chain tip 
					maxIkBoneAngle = acosf( Clamp( ( Sq( pChain->m_fBoneLengths[ i ] ) + Sq( distToIkGoal ) - Sq( remainingChainLength ))/(2 * pChain->m_fBoneLengths[i] * distToIkGoal), -1.0f, 1.0f ));
				}
				else
				{	// Add on the remaining chain length as radians.
					maxIkBoneAngle = acosf( Clamp( ( pChain->m_fBoneLengths[ i ] * 0.5f ) / remainingChainLength, 0.0f, 1.0f ) );
					maxIkBoneAngle += (( remainingChainLength - distToIkGoal ) / pChain->m_fBoneLengths[ i ]);
				}

				ikBoneAngle = maxIkBoneAngle * ( fkBoneAngle / maxFkBoneAngle );

				if( options&OPTION_DEBUG )
				{
					GLVector3f debugFkChainTip = pChain->m_bonePositions[ i ] + ( vecToIkGoal * distToFkChainTip );

					// Draw the max fk bone angle lines
					glColor3f(0.1f, 0.9f, 0.1f);

					DrawDashedLine( pChain->m_bonePositions[ i ], debugFkChainTip );
					DrawDashedArc( pChain->m_bonePositions[ i ], bendAxis, vecToIkGoal * ( pChain->m_fBoneLengths[ i ] * 0.7f ), 0.0f, fkBoneAngle );
					DrawDashedLine( pChain->m_bonePositions[ i ], pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( fkBoneAngle ) ).rotateVector( vecToIkGoal ) * pChain->m_fBoneLengths[ i ] * 0.7f  ) );

					// Draw the Max angle line
					DrawDashedLine( pChain->m_bonePositions[ i ], pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( maxFkBoneAngle ) ).rotateVector( vecToIkGoal ) * pChain->m_fBoneLengths[ i ] ) );
					
					if( distToFkChainTip > remainingChainLength )
					{	// Draw the line to the fk chain tip
						DrawDashedLine( pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( maxFkBoneAngle ) ).rotateVector( vecToIkGoal ) * pChain->m_fBoneLengths[ i ] ), debugFkChainTip );
					}
					else
					{
						float arcStart = acosf( Clamp( ( pChain->m_fBoneLengths[ i ] * 0.5f ) / remainingChainLength, 0.0f, 1.0f ) );
						float arcEnd = maxFkBoneAngle;
						DrawDashedArc( pChain->m_bonePositions[ i ], bendAxis, vecToIkGoal * pChain->m_fBoneLengths[ i ], arcStart, arcEnd );
						DrawDashedLine( pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( arcStart ) ).rotateVector( vecToIkGoal ) * pChain->m_fBoneLengths[ i ] ), debugFkChainTip );
					}
					
					glColor3f(0.1f, 0.1f, 0.9f);
					// Draw the Max angle line
					DrawDashedLine( pChain->m_bonePositions[ i ], pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( maxIkBoneAngle ) ).rotateVector( vecToIkGoal ) * pChain->m_fBoneLengths[ i ] ) );

					if( distToIkGoal > remainingChainLength )
					{	// Draw the line to the ik goal
						DrawDashedLine( pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( maxIkBoneAngle ) ).rotateVector( vecToIkGoal ) * pChain->m_fBoneLengths[ i ] ), goalPosition );
					}
					else
					{
						float arcStart = acosf( Clamp( ( pChain->m_fBoneLengths[ i ] * 0.5f ) / remainingChainLength, 0.0f, 1.0f ) );
						float arcEnd = maxIkBoneAngle;
						DrawDashedArc( pChain->m_bonePositions[ i ], bendAxis, vecToIkGoal * pChain->m_fBoneLengths[ i ], arcStart, arcEnd );
						DrawDashedLine( pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( arcStart ) ).rotateVector( vecToIkGoal ) * pChain->m_fBoneLengths[ i ] ), goalPosition );
					}
				}
			}
			// Subtract off the current angle the bone has with the vecToIkGoal to keep the delta
			deltaBoneAngle = ikBoneAngle - fkBoneAngle;

			// Apply the rotation to the current bones 
			pChain->m_boneOrientations[ i ] = GLQuaternionf( bendAxis, RadToDeg( deltaBoneAngle ) ) * pChain->m_boneOrientations[ i ];
		}
	}
}
