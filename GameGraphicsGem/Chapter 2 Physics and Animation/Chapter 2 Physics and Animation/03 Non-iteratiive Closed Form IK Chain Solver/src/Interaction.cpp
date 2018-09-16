
#include <stdarg.h>
#include <stdio.h>

#include <GL/openglut.h>
#include <GLQuaternion.h>
#include <GLVector.h>

#include "IKChain.h"
#include "Display.h"
#include "Interaction.h"


//static void shapesPrintf (int row, int col, const char *fmt, ...);
static void key(unsigned char key, int x, int y);
static void onMouseButton(int button, int state, int x, int y);
static void onMouseMove(int x, int y);

static GLVector3f g_camPos(5, 3, 10);
static GLVector3f g_camTarget(0, 2, 0);

static unsigned long g_ulOptions = OPTION_DEBUG;

// Which solver is being used to calculate the IK chain pose
static unsigned long g_ulSolverIndex = 3;
static long g_lSelectedBone = -1;


enum MouseMode {
	kNothing,
	kPanning,
	kDollying,
	kManipulating
};

static MouseMode g_mouseMode = kNothing;


static GLint				g_mousedown_viewport[4];
static GLdouble				g_mousedown_modelview[16];
static GLdouble				g_mousedown_projection[16];

static GLVector3f			g_mouseDownPos;
static GLVector3f			g_mouseDownDir;
static float				g_mouseDownRayParam;
static float				g_mouseDownBoneFraction;

// During manipulation, we need to store the pre-manipulated state of the data
static GLVector3f			g_mouseDownCamPos;
static GLVector3f			g_mouseDownCamTarget;

static GLVector3f			g_mousedown_ikGoalPosition;
static GLQuaternionf*		g_mousedown_boneOrientations;
static GLVector3f*			g_mousedown_bonePositions;


//////////////////////////////////////////////////////////////////////////

void UpdateCameraTransform() 
{
	glLoadIdentity();
	gluLookAt( g_camPos[0], g_camPos[1], g_camPos[2], g_camTarget[0], g_camTarget[1], g_camTarget[2], 0, 1, 0);
}

void RegisterUICallbacks()
{
	glutKeyboardFunc(key);

	glutMouseFunc(onMouseButton);
	glutMotionFunc(onMouseMove);
}

unsigned long GetCurrentOptions() 
{
	return g_ulOptions;
}

unsigned long GetCurrentSolverIndex() 
{
	return g_ulSolverIndex;
}



GLfloat* GetCameraPos()
{
	return &g_camPos.x;
}

/*!
Does printf()-like work using freeglut/OpenGLUT
glutBitmapString().  Uses a fixed font.  Prints
at the indicated row/column position.

Limitation: Cannot address pixels.
Limitation: Renders in screen coords, not model coords.

\note Uses a fixed, 256-byte array for holding strings.
The best way around this would be to use vasprintf(),
but that is not available on WIN32, I believe.
Another alternative would be to write our own formatter
from scratch and emit the characters one at a time to
the GLUT bitmap single-character drawing routine.
We could also use vsnprintf(), but I'm not sure if
that is standard...
*/

void shapesPrintf (int row, int col, const char *fmt, ...)
{
	static char buf[256];
	int viewport[4];
	void *font = GLUT_BITMAP_9_BY_15;
	va_list args;

	va_start(args, fmt);
	(void) vsprintf_s (buf, fmt, args);
	va_end(args);

	glGetIntegerv(GL_VIEWPORT,viewport);

	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0,viewport[2],0,viewport[3],-1,1);

	glRasterPos2i(
		glutBitmapWidth(font, ' ') * col,
		- glutBitmapHeight(font) * (row+2) + viewport[3]
	);
	glutBitmapString (font, (const unsigned char*)buf);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


static void key( unsigned char key, int x, int y )
{
	switch (key)
	{
	case 'd': // enable/disable Debugging
		{
			if( g_ulOptions & OPTION_DEBUG ) 
			{
				g_ulOptions &= ~OPTION_DEBUG;
			}
			else
			{
				g_ulOptions |= OPTION_DEBUG;
			}
		}
		break;
	case 'h': // enable/disable Help
		{
			if( g_ulOptions & OPTION_DISPLAY_HELP ) 
			{
				g_ulOptions &= ~OPTION_DISPLAY_HELP;
			}
			else
			{
				g_ulOptions |= OPTION_DISPLAY_HELP;
			}
		}
		break;
	case '1':	g_ulSolverIndex = 1;	break;
	case '2':	g_ulSolverIndex = 2;	break;
	case '3':	g_ulSolverIndex = 3;	break;
	case '4':	g_ulSolverIndex = 4;	break;
	case '5':	g_ulSolverIndex = 5;	break;

	case 'a':
		{
			int numbones = g_ikChain->m_iNumBones + 1;  
			CleanUpChain( );	
			CreateIKChain( numbones );	
		}
		break;
	case 'z':
		{
			int numbones = g_ikChain->m_iNumBones - 1;  
			CleanUpChain( );	
			CreateIKChain( numbones );	
		}
		break;
	default:							break;
	}

	glutPostRedisplay();
}




//////////////////////////////////////////////////////////////////////////
// Picking stuff

int Selection( int mouse_x, int mouse_y )						// This Is Where Selection Is Done
{
	GLuint	buffer[512];										// Set Up A Selection Buffer
	GLint	hits;												// The Number Of Objects That We Selected

	// The Size Of The Viewport. [0] Is <x>, [1] Is <y>, [2] Is <length>, [3] Is <width>
	GLint	viewport[4];

	// This Sets The Array <viewport> To The Size And Location Of The Screen Relative To The Window
	glGetIntegerv( GL_VIEWPORT, viewport );
	glSelectBuffer( 512, buffer );								// Tell OpenGL To Use Our Array For Selection

	// Puts OpenGL In Selection Mode. Nothing Will Be Drawn.  Object ID's and Extents Are Stored In The Buffer.
	glRenderMode( GL_SELECT );
	glMatrixMode( GL_PROJECTION );								// Selects The Projection Matrix

	GLdouble proj[16];											// Get the current camera projection
	glGetDoublev( GL_PROJECTION_MATRIX, proj );

	glPushMatrix( );											// Push The Projection Matrix
	glLoadIdentity( );											// Resets The Matrix

	// This Creates A Matrix That Will Zoom Up To A Small Portion Of The Screen, Where The Mouse Is.
	gluPickMatrix( (GLdouble)mouse_x, (GLdouble)(viewport[3]-mouse_y), 8.0f, 8.0f, viewport );
	glMultMatrixd( proj );										// Apply the current camera projection

	// Apply The Perspective Matrix
	glMatrixMode( GL_MODELVIEW );								// Select The Modelview Matrix
	
	glInitNames();												// Initializes The Name Stack
	/////////////////////////////////////////////////////////////////////
	// Render The Chain bones to the selection Buffer
	UpdateChainFK( );
	for ( unsigned int i = 0; i < g_ikChain->m_iNumBones; i++ )
	{
		glPushName(i);
		DrawBone( i, true );
		glPopName();
	}

	glPushName( g_ikChain->m_iNumBones );
	DrawCross( g_ikGoalPosition, 0.3f ); 
	glPopName( );

	/////////////////////////////////////////////////////////////////////

	glMatrixMode(GL_PROJECTION);								// Select The Projection Matrix
	glPopMatrix();												// Pop The Projection Matrix
	glMatrixMode(GL_MODELVIEW);									// Select The Modelview Matrix
	hits=glRenderMode(GL_RENDER);								// Switch To Render Mode, Find Out How Many
																// Objects Were Drawn Where The Mouse Was
	if (hits > 0)												// If There Were More Than 0 Hits
	{
		int	choose = buffer[3];									// Make Our Selection The First Object
		int depth = buffer[1];									// Store How Far Away It Is 

		for (int loop = 1; loop < hits; loop++)					// Loop Through All The Detected Hits
		{
			// If This Object Is Closer To Us Than The One We Have Selected
			if (buffer[loop*4+1] < GLuint(depth))
			{
				choose = buffer[loop*4+3];						// Select The Closer Object
				depth = buffer[loop*4+1];						// Store How Far Away It Is
			}       
		}

		return choose;
    }
	return -1;
}


void GetOGLPos(int x, int y, GLVector3f& start, GLVector3f& dir)
{
	GLfloat winX, winY;
	GLdouble posX, posY, posZ;

	winX = (float)x;
	winY = (float)g_mousedown_viewport[3] - (float)y;

	gluUnProject(winX, winY, 0, g_mousedown_modelview, g_mousedown_projection, g_mousedown_viewport, &posX, &posY, &posZ);
	start.set( (float)posX, (float)posY, (float)posZ);
	gluUnProject(winX, winY, 1.0, g_mousedown_modelview, g_mousedown_projection, g_mousedown_viewport, &posX, &posY, &posZ);

	dir = GLVector3f( (float)posX, (float)posY, (float)posZ) - start;
	dir.normalize();
}



void StoreMouseDownCameraTransforms( int x, int y )
{
	glGetDoublev( GL_MODELVIEW_MATRIX, g_mousedown_modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, g_mousedown_projection );
	glGetIntegerv( GL_VIEWPORT, g_mousedown_viewport );

	GetOGLPos( x, y, g_mouseDownPos, g_mouseDownDir );
	g_mouseDownCamPos = g_camPos;
	g_mouseDownCamTarget = g_camTarget;
}


//===================================================================
// Borrowed from Soft Surfer.com
// http://softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm

#define SMALL_NUM  0.00000001 // anything that avoids division overflow

//===================================================================

// dist_Point_to_Line(): get the distance of a point to a line.
//    Input:  a Point P and a Line L (in any dimension)
//    Return: the shortest distance from P to L
float dist_Point_to_Line( GLVector3f P, GLVector3f LP0, GLVector3f LP1, float &outLparam )
{
	GLVector3f Pb;
    GLVector3f v = LP1 - LP0;
    GLVector3f w = P - LP0;

    float c1 = w.dot(v);
    float c2 = v.dot(v);
    outLparam = c1 / c2;

	Pb.setMultiplyAdd( v, outLparam, LP0 );
	Pb.setSub( P, Pb );
    return Pb.length();
}

// dist3D_Line_to_Line():
//    Input:  two 3D lines L1 and L2
//    Return: the shortest distance between L1 and L2
float dist3D_Line_to_Line( GLVector3f L1P0, GLVector3f L1P1, GLVector3f L2P0, GLVector3f L2P1, float &outL1param, float &outL2param )
{
    GLVector3f   u = L1P1 - L1P0;
    GLVector3f   v = L2P1 - L2P0;
    GLVector3f   w = L1P0 - L2P0;
    float    a = u.dot(u);        // always >= 0
    float    b = u.dot(v);
    float    c = v.dot(v);        // always >= 0
    float    d = u.dot(w);
    float    e = v.dot(w);
    float    D = a*c - b*b;       // always >= 0
    float    sc, tc;

    // compute the line parameters of the two closest points
    if (D < SMALL_NUM) {         // the lines are almost parallel
        sc = 0.0;
        tc = (b>c ? d/b : e/c);   // use the largest denominator
    }
    else {
        sc = (b*e - c*d) / D;
        tc = (a*e - b*d) / D;
    }

	outL1param = sc;
	outL2param = tc;

    // get the difference of the two closest points
    GLVector3f   dP = w + (u * sc) - (v * tc);

	return dP.length();   // return the closest distance
}

// Based on the mouse values and the selected bone, find the closest intersection 
// between the Bone and the mouse ray. 
void GetSelectedBoneRayParams( GLVector3f& mouseClickPos, GLVector3f& mouseClickDir, float &boneParam, float &mouseRayParam )
{
	GLVector3f boneBasePos = g_mousedown_bonePositions[ g_lSelectedBone ];
	GLVector3f boneTipPos = boneBasePos + ( g_mousedown_boneOrientations[ g_lSelectedBone ].GetXaxis() * g_ikChain->m_fBoneLengths[ g_lSelectedBone ] );

	dist3D_Line_to_Line( boneBasePos, boneTipPos, mouseClickPos, mouseClickPos + mouseClickDir, boneParam, mouseRayParam );
};

// Based on the mouse values, find the closest point on the mouse ray from the ik goal
void GetIKGoalRayParams( GLVector3f& mouseClickPos, GLVector3f& mouseClickDir, float &mouseRayParam )
{
	dist_Point_to_Line( g_mousedown_ikGoalPosition, mouseClickPos, mouseClickPos + mouseClickDir, mouseRayParam );
};



static void onMouseButton(int button, int state, int x, int y)
{
	switch (button )
	{
	case GLUT_MIDDLE_BUTTON:
		{
			switch (state )
			{
			case GLUT_DOWN:
				StoreMouseDownCameraTransforms( x, y );
				g_mouseMode = kPanning;
				break;
			case GLUT_UP:
				g_mouseMode = kNothing;
				break;
			}
		}
		break;
	case GLUT_LEFT_BUTTON:
		{
			switch (state )
			{
			case GLUT_DOWN:
				{
					StoreMouseDownCameraTransforms( x, y );
					UpdateChainFK( );

					int selection = Selection( x, y );
					if( selection >= 0 )
					{
						printf ("Picked Bone : %d \n", selection );
						g_mouseMode = kManipulating;
						g_lSelectedBone = selection;
						if( g_lSelectedBone < (long)g_ikChain->m_iNumBones )
						{
							g_mousedown_boneOrientations	= (GLQuaternionf*)malloc( sizeof( GLQuaternionf ) *  g_ikChain->m_iNumBones );
							g_mousedown_bonePositions		= (GLVector3f*)malloc( sizeof( GLVector3f ) *  g_ikChain->m_iNumBones );

							// Copy our current pose values into our mousedown pose values.
							memcpy( g_mousedown_boneOrientations,	g_ikChain->m_boneOrientations,	sizeof( GLQuaternionf ) * g_ikChain->m_iNumBones );
							memcpy( g_mousedown_bonePositions,		g_ikChain->m_bonePositions,		sizeof( GLVector3f )	* g_ikChain->m_iNumBones );

							GetSelectedBoneRayParams( g_mouseDownPos, g_mouseDownDir, g_mouseDownBoneFraction, g_mouseDownRayParam );
						}
						else
						{
							g_mousedown_ikGoalPosition = g_ikGoalPosition;
							GetIKGoalRayParams( g_mouseDownPos, g_mouseDownDir, g_mouseDownRayParam );
						}
					}
					else
					{
						g_mouseMode = kDollying;
					}
				}
				break;
			case GLUT_UP:
				g_mouseMode = kNothing;
				// Free the mouse down pose, as we don't need it anymore. 
				if( g_mousedown_boneOrientations ){ free( g_mousedown_boneOrientations );	g_mousedown_boneOrientations = NULL;	}
				if( g_mousedown_bonePositions ){	free( g_mousedown_bonePositions );		g_mousedown_bonePositions = NULL;		}
				break;
			}
		}
		break;
	case 3:
		g_camPos += (g_camPos - g_camTarget).unit() * 0.5;
		break;
	case 4:
		g_camPos -= (g_camPos - g_camTarget).unit() * 0.5;
		break;
	}
}


static void onMouseMove(int x, int y)
{
	GLVector3f mouseMovePos;
	GLVector3f mouseMoveDir;
	GetOGLPos( x, y, mouseMovePos, mouseMoveDir );

	switch( g_mouseMode )
	{
	case kPanning:
		{
			GLVector3f movement = ( g_mouseDownPos - mouseMovePos ) * 2.0f;
			
			g_camTarget = g_mouseDownCamTarget + movement;
			g_camPos	= g_mouseDownCamPos + movement;
		}
		break;
	case kDollying:
		{
			// Move the click points towards thee camera target.
			// this increaces the angle between vec1 and vec2.
			float click_point_track = (g_mouseDownCamPos - g_camTarget).length() * 0.5f;

			GLVector3f vec1 = g_mouseDownPos - g_camTarget;
			GLVector3f vec2 = mouseMovePos - g_camTarget;

			vec1 += ( g_mouseDownDir * click_point_track );
			vec1.normalize();
			
			vec2 += ( mouseMoveDir * click_point_track );
			vec2.normalize();
			
			GLQuaternionf	cameraOffsetRotation( vec2, vec1 );
			g_camPos = g_camTarget + ( cameraOffsetRotation.matrix() * (g_mouseDownCamPos - g_camTarget));
		}
		break;
	case kManipulating:
		{
			// the IK goal has been assigned the index of iNumBones. 
			if( g_lSelectedBone == (long)g_ikChain->m_iNumBones )
			{
				float mouseRayParam = 0.0f;
				GetIKGoalRayParams( mouseMovePos, mouseMoveDir, mouseRayParam );
				GLVector3f ikGoalTranslation = ( mouseMovePos + ( mouseMoveDir * mouseRayParam ) ) - ( g_mouseDownPos + ( g_mouseDownDir * g_mouseDownRayParam ) );
				g_ikGoalPosition = g_mousedown_ikGoalPosition + ikGoalTranslation;
			}
			else if( g_lSelectedBone >= 0 )
			{
				UpdateChainFK( );

				float boneParam = 0.0f;
				float mouseMoveRayParam = 0.0f;
				GetSelectedBoneRayParams( mouseMovePos, mouseMoveDir, boneParam, mouseMoveRayParam );

				GLVector3f vec1 = g_mousedown_boneOrientations[ g_lSelectedBone ].GetXaxis();
				GLVector3f vec2 = ( mouseMovePos + ( mouseMoveDir * mouseMoveRayParam ) ) - g_mousedown_bonePositions[ g_lSelectedBone ];
				vec2.normalize();

				GLQuaternionf	worldSpaceRotation = GLQuaternionf( vec1, vec2 );
		
				if( GetCurrentSolverIndex() == 5 )
				{
					// We only alow manipulation off the bones local fk positions when
					// solving using NCFIK_5_SolveWithAnimatedJointOffsets
					GLVector3f boneTranslation = ( mouseMovePos + ( mouseMoveDir * mouseMoveRayParam ) ) - ( g_mouseDownPos + ( g_mouseDownDir * g_mouseDownRayParam ) );

					// Blend off the rotatio based on the position of the click on the bone. 
					// If the bone was clicked on close to the base, tbe the g_mouseDownBoneFraction value
					// is close to 0.0f, if it is clicked close to the tip, then the value is close to 1.0f
					worldSpaceRotation.setSlerp( GLQuaternionf(), worldSpaceRotation, g_mouseDownBoneFraction );
					boneTranslation *= 1.0f - g_mouseDownBoneFraction;

					GLVector3f newBonePosition = g_mousedown_bonePositions[ g_lSelectedBone ] + boneTranslation;
					SetBoneFKPosition( g_lSelectedBone, newBonePosition );
				}


				// Modify the bone's orentation based on the mouse down pose fo the chain
				GLQuaternionf	newBoneOrientation = worldSpaceRotation * g_mousedown_boneOrientations[ g_lSelectedBone ];
			
				SetBoneFKOrientation( g_lSelectedBone, newBoneOrientation );
				if( g_lSelectedBone < (long)(g_ikChain->m_iNumBones-1) )
				{	
					// Counter rotate the child bone to keep the remaining chain the same shape
					SetBoneFKOrientation( g_lSelectedBone+1,  g_mousedown_boneOrientations[ g_lSelectedBone+1 ] );
				}

			}
		}
		break;
	}
}
