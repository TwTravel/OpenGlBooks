/*! \file    shapes.c
    \ingroup demos

	TODO:
    This program is an example of how the Closed Form IK solver works
*/


#include <GL/openglut.h>

#include <vector>
#include "IKChain.h"
#include "Interaction.h"
#include "Display.h"


static void ResizeWindow(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	if( ar > .5 )
        glFrustum( -ar, ar, -1.0, 1.0, 2.0, 100.0 );
    else
        glFrustum( -1.0, 1.0, -1/ar, 1/ar, 2.0, 100.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}


static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_LINES);

	float coodSystemSize = 5.0f;
	// Draw the Red Green and Blue Coordinate System;
	glColor3f( 1.0f, 0.1f, 0.1f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( coodSystemSize, 0.0f, 0.0f ); // origin of the line

	glColor3f( 0.1f, 1.0f, 0.1f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, coodSystemSize, 0.0f );
	
	glColor3f( 0.1f, 0.1f, 1.0f );
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, coodSystemSize );

	glEnd();

	// At the start of each frame the FK system generates a new base pose for the chain
	UpdateChainFK( );

	// Draw the fk chain in blue
	glColor3d( 0.5, 0.5, 0.5 );

	// Draw the chain in its initial fk pose
	DrawChain( );

	switch( GetCurrentSolverIndex() )
	{
	case 1:	NCFIK_1_BasicChainAlignment( g_ikChain, g_ikGoalPosition, GetCurrentOptions() );			break;
	case 2:	NCFIK_2_BasicSolve( g_ikChain, g_ikGoalPosition, GetCurrentOptions() );						break;
	case 3:	NCFIK_3_SolveLongerChains( g_ikChain, g_ikGoalPosition, GetCurrentOptions() );				break;
	case 4:	NCFIK_4_SolveWithExtensionDampening( g_ikChain, g_ikGoalPosition, GetCurrentOptions() );	break;
	case 5:	NCFIK_5_SolveWithAnimatedJointOffsets( g_ikChain, g_ikGoalPosition, GetCurrentOptions() );	break;
	}

	// Draw the solved ik chain in green
	glColor3d( 0.1, 0.1, 1.0 );

	DrawChain( );

	glColor3d( 1.0, 0.1, 0.1 );

	DrawCross( g_ikGoalPosition, 0.3f ); 

	UpdateCameraTransform();

	DisplayHelp();

    glutSwapBuffers();
}


static void idle(void)
{
    glutPostRedisplay();
}

/* Program entry point */

int main(int argc, char *argv[])
{
    glutInitWindowSize(800,600);
    glutInitWindowPosition(40,40);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	// Create a 3 bone chain. 
	CreateIKChain( 3 );

    glutCreateWindow("Game Programming Gems - Closed form IK Solver");

	// Set the glut callback functions. 
    glutReshapeFunc(ResizeWindow);
    glutDisplayFunc(display);
    glutIdleFunc(idle);

	RegisterUICallbacks();

    glClearColor(1,1,1,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    glutMainLoop();

	// Free any allocated memory
	CleanUpChain( );

    return EXIT_SUCCESS;
}
