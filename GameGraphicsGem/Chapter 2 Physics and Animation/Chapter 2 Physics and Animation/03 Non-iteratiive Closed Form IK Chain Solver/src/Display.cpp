
#include "Display.h"
#include "Interaction.h"
#include "IKChain.h"
#include <GL/openglut.h>




void DisplayHelp()
{
	glColor3f( 0.0f, 0.0f, 0.0f );

	switch( GetCurrentSolverIndex() )
	{
	case 1:	shapesPrintf (1, 2, "Basic Alignment of the Chain with the IK Goal");					break;
	case 2:	shapesPrintf (1, 2, "Basic N Bone IK Solver");											break;
	case 3:	shapesPrintf (1, 2, "N Bone IK Solver handling longer chains and greater compression");	break;
	case 4:	shapesPrintf (1, 2, "N Bone IK Solver with dampening");									break;
	case 5:	shapesPrintf (1, 2, "N Bone IK Solver supporting animated joint offsets");
			shapesPrintf (2, 2, "Click and drag on the base of a bone to dislocate it from its parent");	break;
	}

	

	if( GetCurrentOptions() & OPTION_DISPLAY_HELP ) 
	{
		glColor3d( 0.5, 0.5, 0.5 );
		shapesPrintf (3, 2, "The Grey Chain represents the original FK pose");

		glColor3f( 0.0f, 0.0f, 1.0f );
		shapesPrintf (4, 2, "The Blue Chain represents the solved IK pose");

		glColor3f( 1.0f, 0.0f, 0.0f );
		shapesPrintf (5, 2, "The Red Cross represents the IK Goal");

		glColor3f( 0.0f, 0.7f, 0.0f );
		shapesPrintf (6, 2, "Press 'd' to display debuging information");
		shapesPrintf (7, 2, "Press 1, 2, 3, 4, 5 to cycle through various levels of the algorithm");
		shapesPrintf (8, 2, "Press 'a' to increase the number of bones, press 'z' to decrease the number of bones");
		shapesPrintf (9, 2, "Left Click and drag on the viewport to dolly the camera.");
		shapesPrintf (10, 2, "Middle Click and drag on the viewport to pan the camera.");
		shapesPrintf (11, 2, "Manipulate the mouse wheel to zoom.");

		glColor3f( 0.7f, 0.0f, 0.0f );
		shapesPrintf (12, 2, "Click and drag on the FK Chain to manipulate it.");
		shapesPrintf (13, 2, "Click and drag on the IK Goal at the end of the chain to manipulate it.");
	}
	else
	{
		shapesPrintf (3, 2, "Press 'h' to display Help");
	}
}


void DrawChain( ) 
{
	// Render all the bones
	for ( unsigned int i = 0; i < g_ikChain->m_iNumBones; i++ )
	{
		DrawBone( i );
	}
}


void DrawBone( int index, bool picking )
{	
	// This is the fraction of the bones length where the 
	// Circumference will be drawn
	static float midFraction	= 0.1f;	
	static float size			= 0.1f;
	float ratio					= 3.0f;
	float length				= g_ikChain->m_fBoneLengths[ index ];
	GLVector3f position;
	GLQuaternionf orientation;

	position		= g_ikChain->m_bonePositions[ index ]; 
	orientation		= g_ikChain->m_boneOrientations[ index ];

	glPushMatrix();

	glTranslatef( position.x, position.y, position.z );
	orientation.matrix().glMultMatrix();	

	if( picking )
	{	// Draw shaded during picking to make selection easier
		glBegin(GL_TRIANGLE_STRIP);
	}
	else
	{
		glBegin(GL_LINE_STRIP);
	}
	
	glVertex3f( 0, 0, 0 );
	glVertex3f( midFraction * length,  size * ratio, size );
	glVertex3f( length, 0, 0 );
	glVertex3f( midFraction * length, -size * ratio, size );
	glVertex3f( 0, 0, 0);
	glVertex3f( midFraction * length, -size * ratio, -size );
	glVertex3f( length, 0, 0 );
	glVertex3f( midFraction * length,  size * ratio, -size );
	glVertex3f( 0, 0, 0 );
	glVertex3f( midFraction * length,  size * ratio, size );
	glVertex3f( length, 0, 0);

	glEnd();

	if( picking )
	{	// Draw shaded during picking to make selection easier
		glBegin(GL_TRIANGLE_STRIP);
	}
	else
	{
		glBegin(GL_LINE_STRIP);
	}

	glVertex3f( midFraction * length,  size * ratio, size );
	glVertex3f( midFraction * length, -size * ratio, size );
	glVertex3f( midFraction * length, -size * ratio, -size );
	glVertex3f( midFraction * length,  size * ratio, -size );
	glVertex3f( midFraction * length,  size * ratio, size );

	glEnd();

	glPopMatrix();
}

void DrawCross( GLVector3f position, float size ) 
{
	glPushMatrix();

	glTranslatef( position.x, position.y, position.z );

	glBegin(GL_LINES);

	glVertex3f(-0.5f * size, 0.0f, 0.0f); // origin of the line
	glVertex3f(0.5f * size, 0.0f, 0.0f); // ending point of the line

	glVertex3f(0.0f, -0.5f * size, 0.0f); // origin of the line
	glVertex3f(0.0f, 0.5f * size, 0.0f); // ending point of the line

	glVertex3f(0.0f, 0.0f, -0.5f * size); // origin of the line
	glVertex3f(0.0f, 0.0f, 0.5f * size); // ending point of the line

	glEnd();
	
	glPopMatrix();
}



///////////////////////////////////////////////////////////////////////
// Debugging Display helper functions. 

static const float g_dashLength = 0.3f;

void DrawLine( GLVector3f startPosition,  GLVector3f endPosition )
{	
	glBegin(GL_LINES);

	glVertex3f( startPosition.x, startPosition.y, startPosition.z);
	glVertex3f( endPosition.x, endPosition.y, endPosition.z);

	glEnd(); 
}

void DrawDashedLine( GLVector3f startPosition,  GLVector3f endPosition )
{	
	float numDashes = ( endPosition - startPosition ).length() / (g_dashLength * 0.5f);

	glBegin(GL_LINES);

	GLVector3f dashPosition;
	for( int i=0; i < numDashes; i++ )
	{
		dashPosition = startPosition + (( endPosition - startPosition ) * ( (float)i / (float)numDashes ));
		glVertex3f( dashPosition.x, dashPosition.y, dashPosition.z);
	}

	glEnd(); 
}

void DrawDashedArc( GLVector3f position, GLVector3f axis, GLVector3f baseVector, float startAngle , float endAngle )
{
	float arcLength = (endAngle - startAngle) * baseVector.length();
	float numDashes = fabs( arcLength / (g_dashLength * 0.5f));

	glBegin(GL_LINES);

	GLVector3f dashPosition;
	for( int i=0; i < numDashes; i++ )
	{
		float angle = RadToDeg( startAngle + ( ( endAngle - startAngle ) * ( (float)i / (float)numDashes ) ) );
		dashPosition = position + GLQuaternionf( axis, angle ).rotateVector( baseVector );
		glVertex3f( dashPosition.x, dashPosition.y, dashPosition.z );
	}

	glEnd();
}