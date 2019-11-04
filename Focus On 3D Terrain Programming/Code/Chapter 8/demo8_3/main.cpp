//==============================================================
//==============================================================
//= main.cpp ===================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= demo8_3: main program source.							   =
//==============================================================
//==============================================================


//--------------------------------------------------------------
//--------------------------------------------------------------
//- INCLUDES ---------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "../Base Code/gl_app.h"
#include "../Base Code/math_ops.h"
#include "../Base Code/camera.h"

#include "ROAM.h"
#include "water.h"
#include "skybox.h"
#include "resource.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- GLOBALS ----------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
const int g_iScreenWidth = 640;
const int g_iScreenHeight= 480;

int g_iMouseX, g_iMouseY;
float g_fMovementSpeed= 4.0f;
float g_fMouseSensitivity= 1.0f;

CGL_APP g_glApp;

CCAMERA g_camera;
CROAM g_ROAM;
CWATER g_water;
CSKYBOX g_skybox;

int g_iLevel= 15;


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:			DemoInit - global
// Description:		Initiate the things needed for the demo
// Arguments:		None
// Return Value:	A boolean variable: -true: successful initiation
//										-false: unsuccessful initiation
//--------------------------------------------------------------
bool DemoInit( void )
{
	g_glApp.Init( 10, 10, g_iScreenWidth, g_iScreenHeight, 16, "Demo 8_3: Skybox Fun!", IDI_ICON1, IDR_MENU1 );
	g_glApp.CreateTTFont( "Lucida Console", 16 );

	//set the clear color (background) to black
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glDisable( GL_TEXTURE_2D );								//disable two dimensional texture mapping
	glDisable( GL_LIGHTING );								//disable lighting
	glDisable( GL_BLEND );									//disable blending
	glEnable( GL_DEPTH_TEST );								//enable depth testing

	glShadeModel( GL_SMOOTH );								//enable smooth shading
	glClearDepth( 1.0 );									//depth buffer setup
	glDepthFunc( GL_LEQUAL );								//set the type of depth test
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );	//the nicest perspective look
	
	//initialize the ROAM system
	g_ROAM.MakeTerrainPlasma( 1024, 1.0f );

	//load the various terrain tiles
	g_ROAM.LoadTile( LOWEST_TILE,  "../Data/lowestTile.tga" );
	g_ROAM.LoadTile( LOW_TILE,     "../Data/lowTile.tga" );
	g_ROAM.LoadTile( HIGH_TILE,    "../Data/highTile.tga" );
	g_ROAM.LoadTile( HIGHEST_TILE, "../Data/highestTile.tga" );

	//make the texture map, and then save it
	g_ROAM.GenerateTextureMap( 512 );
	g_ROAM.DoTextureMapping( true );
	g_ROAM.DoMultitexturing( true );

	//initialize the ROAM system
	g_ROAM.Init( g_iLevel, 65536, &g_camera );
	g_ROAM.SetMaxTrisPerFrame( 5000 );

	//initialize the water system
	g_water.Init( 1024.0f );
	g_water.LoadReflectionMap( "../Data/reflection_map.tga" );
	g_water.SetColor( 1.0f, 1.0f, 1.0f, 0.9f );

	//load the skybox textures
	g_skybox.LoadTexture( SBX_FRONT,  "../Data/skybox_front.tga" );
	g_skybox.LoadTexture( SBX_BACK,   "../Data/skybox_back.tga" );
	g_skybox.LoadTexture( SBX_RIGHT,  "../Data/skybox_right.tga" );
	g_skybox.LoadTexture( SBX_LEFT,   "../Data/skybox_left.tga" );
	g_skybox.LoadTexture( SBX_TOP,    "../Data/skybox_top.tga" );
	g_skybox.LoadTexture( SBX_BOTTOM, "../Data/skybox_bottom.tga" );

	//set the camera's position
	g_camera.SetPosition( 128.0f, 512.0f, 256.0f );

	g_camera.m_fYaw  += 150;
	g_camera.m_fPitch-= 40;

	//the initiation was successful
	g_log.Write( LOG_SUCCESS, "INITIATION SUCCESSFUL" );
	return true;
}

//--------------------------------------------------------------
// Name:			DemoRender - global
// Description:		The rendering procedure (that is passed to GLUT)
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void DemoRender( void )
{
	//clear the color and depth buffers
	g_glApp.BeginRendering( );
	
	//reset the modelview matrix
	glLoadIdentity( );

	//setup the viewing matrix
	g_camera.ComputeViewMatrix( );
	g_camera.SetViewMatrix( );

	//calculate the viewing frustum
	g_camera.CalculateViewFrustum( );

	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );
	glDepthMask( GL_FALSE );
		g_skybox.Set( g_camera.m_vecEyePos[0], g_camera.m_vecEyePos[1], g_camera.m_vecEyePos[2], 1024.0f );
		g_skybox.Render( );
	glDepthMask( GL_TRUE );
	glEnable( GL_DEPTH_TEST );

	//render the simple terrain!
	g_ROAM.DoTextureMapping( true );

	glFrontFace( GL_CW );
	glEnable( GL_CULL_FACE );

	//update the ROAM mesh
	g_ROAM.Update( );

	//update the water's vertices and re-calculate polygon normals
	g_water.Update( 0.001f );
	g_water.CalcNormals( );

	//render the terrain mesh
	glPushMatrix( );
		g_ROAM.Render( );
	glPopMatrix( );

	//render the water mesh
	glPushMatrix( );
		glTranslatef( 0.0f, 75.0f, 0.0f );

		glDepthMask( GL_FALSE );
		g_water.Render( true );
		glDepthMask( GL_TRUE );
	glPopMatrix( );
	
	//render some text to the screen
	glDisable( GL_TEXTURE_2D );
	g_glApp.BeginTextMode( );
		//render the number of frames per second
		g_glApp.Print( g_iScreenWidth-175, g_iScreenHeight-70, CVECTOR( 0.0f, 1.0f, 0.0f), 
					   "FPS:  %7.0f", g_glApp.GetFPS( ) );

		//render the number of vertices per frame
		g_glApp.Print( g_iScreenWidth-175, g_iScreenHeight-85, CVECTOR( 0.0f, 1.0f, 0.0f),
					   "Vertices: %d", g_ROAM.GetNumVertsPerFrame( )+g_water.GetNumVertices( ) );

		//render how many triangles are getting rendered per second
		g_glApp.Print( g_iScreenWidth-175, g_iScreenHeight-100, CVECTOR( 0.0f, 1.0f, 0.0f),
					   "Tris/S:   %d", g_ROAM.GetNumTrisPerFrame( )+g_water.GetNumTriangles( ) );

		//render how many million triangles are rendered per second
		g_glApp.Print( g_iScreenWidth-175, g_iScreenHeight-115, CVECTOR( 0.0f, 1.0f, 0.0f),
					   "MTris/S:  %.3f", ( ( g_ROAM.GetNumTrisPerFrame( )+g_water.GetNumTriangles( ) )*g_glApp.GetFPS( ) )/1000000.0f );
	g_glApp.EndTextMode( );

	//force a render finish, and then swap buffers
	g_glApp.EndRendering( );
}


//--------------------------------------------------------------
// Name:			DemoShutdown - global
// Description:		Runs all of the shutdown procedures that are needed
// Arguments:		None
// Return Value:	A boolean variable: -true: successful shutdown
//										-false: unsuccessful shutdown
//--------------------------------------------------------------
bool DemoShutdown( void )
{
	g_ROAM.Shutdown( );
	g_ROAM.UnloadAllTiles( );
	g_ROAM.UnloadTexture( );
	g_ROAM.UnloadHeightMap( );

	//exit the program
	g_glApp.DestroyFont( );
	g_glApp.Shutdown( );

	g_log.Write( LOG_SUCCESS, "SHUTDOWN SUCCESSFUL" );
	return true;
}

//--------------------------------------------------------------
// Name:			KeyboardInput - global
// Description:		Gathers keyboard input
// Arguments:		-ucKey: the button being pressed
//					-iX, iY: indicates the m_vecEyePos of the mouse
//							 at the time of the event
// Return Value:	None
//--------------------------------------------------------------
bool DemoInput( void )
{
	static int iToggleWait;

	//only move when a button is down (makes life so much easier)
	if( g_glApp.MouseDown( MK_LBUTTON ) || g_glApp.MouseDown( MK_RBUTTON ) )
	{
		g_camera.m_fYaw  += ( ( float )g_glApp.GetMouseX( )-g_iMouseX )*g_fMouseSensitivity;
		g_camera.m_fPitch-= ( ( float )g_glApp.GetMouseY( )-g_iMouseY )*g_fMouseSensitivity;
	}

	//always update the mouse coordinates, so that when the user wants to move
	//the view (hold down a button), we don't get a very wierd view "jump"
	g_iMouseX= g_glApp.GetMouseX( );
	g_iMouseY= g_glApp.GetMouseY( );

	iToggleWait++;

	//exit the program
	if( g_glApp.KeyDown( VK_ESCAPE ) )
	{
		PostQuitMessage( 0 );
		return false;
	}

	//exit the program
	if( g_glApp.KeyDown( 'Q' ) )
	{
		PostQuitMessage( 0 );
		return false;
	}

	//move forward
	if( g_glApp.KeyDown( VK_UP ) )
		g_camera.m_vecEyePos+= g_camera.m_vecForward*g_fMovementSpeed;

	//move backward
	else if( g_glApp.KeyDown( VK_DOWN ) )
		g_camera.m_vecEyePos-= g_camera.m_vecForward*g_fMovementSpeed;

	//strafe right
	if( g_glApp.KeyDown( VK_RIGHT ) )
		g_camera.m_vecEyePos+= g_camera.m_vecSide*g_fMovementSpeed;

	//strafe left
	else if( g_glApp.KeyDown( VK_LEFT ) )
		g_camera.m_vecEyePos-= g_camera.m_vecSide*g_fMovementSpeed;

	//wireframe rendering
	if( g_glApp.KeyDown( 'W' ) )
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	//normal rendering
	else if( g_glApp.KeyDown( 'S' ) )
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	return true;
}

//--------------------------------------------------------------
// Name:			WinMain - global
// Description:		The equivalant to Main( ) in console apps
// Arguments:		Just some default arguments that windows requires
// Return Value:	An integer variable
//--------------------------------------------------------------
int WINAPI WinMain( HINSTANCE hInstance, 
				    HINSTANCE hPrevInstance, 
				    LPSTR	 lpCmdLine, 
				    int		 nCmdShow ) 
{
	//Do all of the Initiation stuff
	if( !DemoInit( ) )
		return false;

	while( true )
	{
		if( !g_glApp.HandleMessages( ) )
			break;

		//exit the demo
		if( g_glApp.MenuDown( ID_DEMO_EXIT ) )
			PostQuitMessage( 0 );

		//take a screenshot
		else if( g_glApp.MenuDown( ID_DEMO_SCREENSHOT ) )
			g_glApp.TakeScreenshot( "screenshot.bmp" );

		if( g_glApp.IsActive( )==APP_ACTIVE )
		{
			//check input
			if( !DemoInput( ) )
				break;

			//rendering loop
			DemoRender( );
		}
	} 

	//Shut the game down
	DemoShutdown( );
	return false;
}