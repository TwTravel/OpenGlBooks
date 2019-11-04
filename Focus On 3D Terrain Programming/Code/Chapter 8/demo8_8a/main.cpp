//==============================================================
//==============================================================
//= main.cpp ===================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= demo8_8a: main program source.							   =
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

#include "geomipmapping.h"
#include "skydome.h"
#include "water.h"
#include "resource.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- GLOBALS ----------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
const int g_iScreenWidth = 640;
const int g_iScreenHeight= 480;

int g_iMouseX, g_iMouseY;
float g_fMovementSpeed= 2.0f;
float g_fMouseSensitivity= 1.0f;

CGL_APP g_glApp;

CCAMERA g_camera;
CGEOMIPMAPPING g_geomipmapping;
CWATER g_water;
CSKYDOME g_skydome;

float g_fFogDepth= 150.0f;

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
	static float fFogColor[4]= {	0.9f, 0.9f, 0.9f, 1.0f	};

	srand( GetCurrentTime( ) );

	g_glApp.Init( 10, 10, g_iScreenWidth, g_iScreenHeight, 16, "Demo 8_8a: Vertex-Based Fog Made Easy", IDI_ICON1, IDR_MENU1 );
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

	//load the height map in
	g_geomipmapping.MakeTerrainFault( 513, 64, 0, 255, 0.15f );

	//set the terrain's lighting system up
	g_geomipmapping.SetLightingType( SLOPE_LIGHT );
	g_geomipmapping.SetLightColor( CVECTOR( 1.0f, 1.0f, 1.0f ) );
	g_geomipmapping.CustomizeSlopeLighting( 1, 1, 0.2f, 0.9f, 15 );
	g_geomipmapping.CalculateLighting( );
	
	//load the various terrain tiles
	g_geomipmapping.LoadTile( LOWEST_TILE,  "../Data/lowestTile.tga" );
	g_geomipmapping.LoadTile( LOW_TILE,     "../Data/lowTile.tga" );
	g_geomipmapping.LoadTile( HIGH_TILE,    "../Data/highTile.tga" );
	g_geomipmapping.LoadTile( HIGHEST_TILE, "../Data/highestTile.tga" );

	//load the terrain's detail map
	g_geomipmapping.LoadDetailMap( "../Data/detailMap.tga" );
	g_geomipmapping.DoDetailMapping( true, 16 );

	//make the texture map, and then save it
	g_geomipmapping.GenerateTextureMap( 256 );
	g_geomipmapping.DoTextureMapping( true );
	g_geomipmapping.DoMultitexturing( g_glApp.CanMultitexture( ) );

	//initiate the geomipmapping system
	g_geomipmapping.Init( 17 );

	glFogi( GL_FOG_MODE, GL_LINEAR );		//set a linear fog mode
	glFogfv( GL_FOG_COLOR, fFogColor );		//set the color of the fog
	glFogf( GL_FOG_START, 0.0f );			//set the starting depth to 0
	glFogf( GL_FOG_END, 150.0f );			//set the fog's depth to 150 world units

	g_geomipmapping.SetFogDepth( g_fFogDepth );

	//turn the volumetric fog extension on
	glFogi( GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT );

	//initialize the water system
	g_water.Init( 1024.0f );
	g_water.LoadReflectionMap( "../Data/reflection_map.tga" );
	g_water.SetColor( 1.0f, 1.0f, 1.0f, 0.9f );

	//initialize the skydome
	g_skydome.Init( 5.0f, 5.0f, 256.0f );
	g_skydome.GenCloudTexture( 256, 0.5f, 25, 1.0f, 0.25f, 0.25f, 0.0f );

	//set the camera's position
	g_camera.SetPosition( 128.0f, g_geomipmapping.GetScaledHeightAtPoint( 128, 256 )+50.0f , 256.0f );

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
	float fScale;
	float ucHeight;

	//clear the color and depth buffers
	g_glApp.BeginRendering( );
	
	//reset the modelview matrix
	glLoadIdentity( );

	fScale= g_geomipmapping.m_vecScale[0];

	//perform collision detection and response against the terrain mesh
	CLAMP( g_camera.m_vecEyePos[0], 100, ( g_geomipmapping.m_iSize*fScale )-100 );
	CLAMP( g_camera.m_vecEyePos[2], 100, ( g_geomipmapping.m_iSize*fScale )-100 );

	ucHeight= ( float )g_geomipmapping.GetScaledHeightAtPoint( ( int )( g_camera.m_vecEyePos[0]/fScale ),
															   ( int )( g_camera.m_vecEyePos[2]/fScale ) );

	if( g_camera.m_vecEyePos[1]<( ucHeight+8 ) )
		g_camera.m_vecEyePos[1]= ucHeight+8;

	//setup the viewing matrix
	g_camera.ComputeViewMatrix( );
	g_camera.SetViewMatrix( );

	//calculate the viewing frustum
	g_camera.CalculateViewFrustum( );

	//render the skydome
	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );
	glDepthMask( GL_FALSE );
		g_skydome.Set( g_camera.m_vecEyePos[0], g_camera.m_vecEyePos[1]-200.0f, g_camera.m_vecEyePos[2] );
		g_skydome.Render( 0.009f, true );
	glDepthMask( GL_TRUE );
	glEnable( GL_DEPTH_TEST );

	glCullFace( GL_CCW );
	glEnable( GL_CULL_FACE );

	//update the water's vertices and re-calculate polygon normals
	g_water.Update( 0.001f );
	g_water.CalcNormals( );

	//setup the terrain
	g_geomipmapping.Update( g_camera );

	g_geomipmapping.SetFogDepth( g_fFogDepth );
	glEnable( GL_FOG );

	//render the simple terrain!
	glPushMatrix( );
		g_geomipmapping.Scale( 2.0f, 1.0f, 2.0f );
		g_geomipmapping.Render( );
	glPopMatrix( );

	glDisable( GL_FOG );

	glDisable( GL_CULL_FACE );

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
					   "Vertices: %d", g_geomipmapping.GetNumVertsPerFrame( )+g_water.GetNumVertices( )+g_skydome.GetNumVertices( ) );

		//render how many triangles are getting rendered per second
		g_glApp.Print( g_iScreenWidth-175, g_iScreenHeight-100, CVECTOR( 0.0f, 1.0f, 0.0f),
					   "Tris:   %d", g_geomipmapping.GetNumTrisPerFrame( )+g_water.GetNumTriangles( )+g_skydome.GetNumTriangles( ) );

		//render how many million triangles are rendered per second
		g_glApp.Print( g_iScreenWidth-175, g_iScreenHeight-115, CVECTOR( 0.0f, 1.0f, 0.0f),
					   "MTris/S:  %.3f", ( ( g_geomipmapping.GetNumTrisPerFrame( )+g_water.GetNumTriangles( )+g_skydome.GetNumTriangles( ) )*g_glApp.GetFPS( ) )/1000000.0f );

		//render volumetric fog control text
		g_glApp.Print( 30, g_iScreenHeight-70, CVECTOR( 1.0f, 0.0f, 0.0f ), "+    Increase Fog Depth" );
		g_glApp.Print( 30, g_iScreenHeight-86, CVECTOR( 1.0f, 0.0f, 0.0f ), "-    Decrease Fog Depth" );
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
	g_skydome.Shutdown( );

	g_geomipmapping.Shutdown( );
	g_geomipmapping.UnloadAllTiles( );
	g_geomipmapping.UnloadTexture( );
	g_geomipmapping.UnloadHeightMap( );

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

	//increase the fog depth
	if( g_glApp.KeyDown( VK_ADD ) )
		g_fFogDepth+= 1.0f;

	//lower the fog depth
	else if( g_glApp.KeyDown( VK_SUBTRACT ) )
		g_fFogDepth-= 1.0f;

	CLAMP( g_fFogDepth, 0.0f, 250.0f );

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