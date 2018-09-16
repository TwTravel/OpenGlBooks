
#include <stdlib.h>
#include "Display.h"
#include "Interaction.h"
#include "IKChain.h"


///////////////////////////////////////////////
// NCFIK_5_SolveWithAnimatedJointOffsets
//
// this version of the NCF IK solver supports an fk pose where the distance between bones may be changin from frame to frame.
// Also, the actual local offset vector of the bone may be changing, meaning that a child bone may not be offset from
// its parent along the parent bones x axis. 
//
// This impimentation was mentioned only briefly in the paper.
// The benefit of this implimentation, is that it doesn't rely on a set of precomputed bone lengths, and is also coordinate system independant. 
// The previouse implimentations assumed that a bone is aligned on the X axis, and that the bone length is stored as a scalar value. 
// Aside from the 2 exceptions noted below, this implimentation computes all the values needed directly from the fk pose. 

void NCFIK_5_SolveWithAnimatedJointOffsets( IKChain *pChain, GLVector3f goalPosition, unsigned int options )
{
	// Calculate the position at the tip of the last bone. 
	// Note: If we had a chain that included more bones, then the length of the last bone
	// could be derived from the fk chain also. In many cases, this could be an ankle/toe/hoof bone
	// or, in 3dsmax terms, a 'nub' bone. 
	GLVector3f	fkChainTip	= pChain->m_bonePositions[ pChain->m_iNumBones - 1 ] + ( pChain->m_boneOrientations[ pChain->m_iNumBones-1 ].GetXaxis() * pChain->m_fBoneLengths[ pChain->m_iNumBones - 1 ] );

	GLVector3f	vecToIkGoal,	vecToFkChainTip;
	float		distToIkGoal,	distToFkChainTip;
	GLQuaternionf				boneOffsetRotation, chainOffsetRotation;
	float		maxFkBoneAngle, maxIkBoneAngle;
	float		fkBoneAngle,	ikBoneAngle, deltaBoneAngle;
	unsigned int i;

	// Calculate the total chain length
	GLVector3f	boneLengthVector;
	float		boneLength,		remainingChainLength = 0.0f;
	for( i = 1; i < pChain->m_iNumBones; i++ )
	{
		remainingChainLength += ( pChain->m_bonePositions[ i ] - pChain->m_bonePositions[ i-1 ] ).length();
	}
	// Note: Once again, we need the length of the last bone to be able to
	// calculate the total length of the chain. If the bones length is not
	// stored, then we would need a method to calculate it. In many cases, there
	// will be a child bone that could be used to calculate the length of this bone. 
	remainingChainLength += pChain->m_fBoneLengths[ pChain->m_iNumBones - 1 ];

	for( i = 0; i < pChain->m_iNumBones; i++ )
	{
		boneLengthVector		= ( pChain->m_bonePositions[ i+1 ] - pChain->m_bonePositions[ i ] );
		boneLength				= boneLengthVector.normalizeRetLength();

		if( i > 0 )
		{	// Transform the bone position by the overall chain offset.
			pChain->m_bonePositions[ i ] = pChain->m_bonePositions[ 0 ] + chainOffsetRotation.rotateVector( pChain->m_bonePositions[ i ] - pChain->m_bonePositions[ 0 ] );
		}
		vecToFkChainTip		= fkChainTip - pChain->m_bonePositions[ i ];
		distToFkChainTip	= vecToFkChainTip.normalizeRetLength();

		if( i > 0 )
		{	// Calculate a new global position based on the parent bones new orientation
			pChain->m_bonePositions[ i ] = pChain->m_bonePositions[ i-1 ] + ( pChain->m_boneOrientations[ i-1 ].rotateVector( pChain->m_boneLocalTranslations[ i ] ) );
		}
		vecToIkGoal			= goalPosition - pChain->m_bonePositions[ i ];
		distToIkGoal		= vecToIkGoal.normalizeRetLength();
		
		if( i==0 )
		{	// For the first bone calculate and store the overall chain offset towards the ik target
			chainOffsetRotation.setFrom2Vectors( vecToFkChainTip, vecToIkGoal );
			// project the fk chain tip onto the vecToIkGoal. All subsequent math will be based on this new fk chain tip value
			fkChainTip = pChain->m_bonePositions[ i ] + ( vecToIkGoal * distToFkChainTip );
			boneOffsetRotation = chainOffsetRotation;
		}
		else
		{	// Apply the chain offset, and apply any incremental correction.
			// Thie additional correction is required due to a new bone position based on the new parent orientation
			boneOffsetRotation = GLQuaternionf( vecToFkChainTip, vecToIkGoal ) * chainOffsetRotation;
		}

		// Apply the offsetRotation to align the bone with the new ik goal position.
		pChain->m_boneOrientations[ i ] = boneOffsetRotation * pChain->m_boneOrientations[ i ];
		// increment the world space vector by the same offset rotation used to
		// modify the bone's orientation. 
		boneLengthVector = boneOffsetRotation.rotateVector( boneLengthVector );

		// Based on the bone index, select an appropriate method to solve
		if(i <= ( pChain->m_iNumBones - 2 ) )
		{
			remainingChainLength	-= boneLength;
			GLVector3f bendAxis		= vecToIkGoal.getCross( boneLengthVector );
			bendAxis.normalize();

			// this is the current angle of the bone.
			fkBoneAngle				= acosf( boneLengthVector.dot( vecToIkGoal ) );

			if(i == ( pChain->m_iNumBones - 2 ) )
			{	// Use trigonometry to determine the ikBoneAngle
				// Law of cosines. a = BoneLength; b = Child BoneLength; c = Distance to the Ik Goal;
				ikBoneAngle = acosf( Clamp( (Sq( boneLength ) + Sq( distToIkGoal ) - Sq( remainingChainLength ))/( 2.0f * boneLength * distToIkGoal ), -1.0f, 1.0f ));
			}	
			else
			{
				// distance from the bone tip to the ik gloal
				if( distToFkChainTip > remainingChainLength )
				{	// Using the law of cosines, calculate the maximum angle of this bone using the fk Chain tip 
					maxFkBoneAngle = acosf( Clamp( ( Sq( boneLength ) + Sq( distToFkChainTip ) - Sq( remainingChainLength ))/(2 * boneLength * distToFkChainTip), -1.0f, 1.0f ));
				}
				else
				{	// Add on the remaining chain length as radians.
					maxFkBoneAngle = acosf( Clamp( ( boneLength * 0.5f ) / remainingChainLength, 0.0f, 1.0f ) );
					maxFkBoneAngle += (( remainingChainLength - distToFkChainTip ) / boneLength);
				}
				
				// distance from the bone tip to the ik gloal
				if( distToIkGoal > remainingChainLength )
				{	// Using the law of cosines, calculate the maximum angle of this bone using the fk Chain tip 
					maxIkBoneAngle = acosf( Clamp( ( Sq( boneLength ) + Sq( distToIkGoal ) - Sq( remainingChainLength ))/(2 * boneLength * distToIkGoal), -1.0f, 1.0f ));
				}
				else
				{	// Add on the remaining chain length as radians.
					maxIkBoneAngle = acosf( Clamp( ( boneLength * 0.5f ) / remainingChainLength, 0.0f, 1.0f ) );
					maxIkBoneAngle += (( remainingChainLength - distToIkGoal ) / boneLength);
				}

				ikBoneAngle = maxIkBoneAngle * ( fkBoneAngle / maxFkBoneAngle );

				if( options&OPTION_DEBUG )
				{
					GLVector3f debugFkChainTip = pChain->m_bonePositions[ i ] + ( vecToIkGoal * distToFkChainTip );

					// Draw the max fk bone angle lines
					glColor3f(0.1f, 0.9f, 0.1f);

					DrawDashedLine( pChain->m_bonePositions[ i ], debugFkChainTip );
					DrawDashedArc( pChain->m_bonePositions[ i ], bendAxis, vecToIkGoal * ( boneLength * 0.7f ), 0.0f, fkBoneAngle );
					DrawDashedLine( pChain->m_bonePositions[ i ], pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( fkBoneAngle ) ).rotateVector( vecToIkGoal ) * boneLength * 0.7f  ) );

					// Draw the Max angle line
					DrawDashedLine( pChain->m_bonePositions[ i ], pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( maxFkBoneAngle ) ).rotateVector( vecToIkGoal ) * boneLength ) );
					
					if( distToFkChainTip > remainingChainLength )
					{	// Draw the line to the fk chain tip
						DrawDashedLine( pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( maxFkBoneAngle ) ).rotateVector( vecToIkGoal ) * boneLength ), debugFkChainTip );
					}
					else
					{
						float arcStart = acosf( Clamp( ( boneLength * 0.5f ) / remainingChainLength, 0.0f, 1.0f ) );
						float arcEnd = maxFkBoneAngle;
						DrawDashedArc( pChain->m_bonePositions[ i ], bendAxis, vecToIkGoal * boneLength, arcStart, arcEnd );
						DrawDashedLine( pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( arcStart ) ).rotateVector( vecToIkGoal ) * boneLength ), debugFkChainTip );
					}
					
					glColor3f(0.1f, 0.1f, 0.9f);
					// Draw the Max angle line
					DrawDashedLine( pChain->m_bonePositions[ i ], pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( maxIkBoneAngle ) ).rotateVector( vecToIkGoal ) * boneLength ) );

					if( distToIkGoal > remainingChainLength )
					{	// Draw the line to the ik goal
						DrawDashedLine( pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( maxIkBoneAngle ) ).rotateVector( vecToIkGoal ) * boneLength ), goalPosition );
					}
					else
					{
						float arcStart = acosf( Clamp( ( boneLength * 0.5f ) / remainingChainLength, 0.0f, 1.0f ) );
						float arcEnd = maxIkBoneAngle;
						DrawDashedArc( pChain->m_bonePositions[ i ], bendAxis, vecToIkGoal * boneLength, arcStart, arcEnd );
						DrawDashedLine( pChain->m_bonePositions[ i ] + ( GLQuaternionf( bendAxis, RadToDeg( arcStart ) ).rotateVector( vecToIkGoal ) * boneLength ), goalPosition );
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
