//==============================================================
//==============================================================
//= main.cpp ===================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= demo4_1: main program source.							   =
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

#include "brute_force.h"
#include "resource.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- STRUCTURES -------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
enum FRACTAL_ALGORITHM
{
	FAULT_FORMATION= 0,
	MIDPOINT_DISPLACEMENT
};


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
CBRUTE_FORCE g_bruteForce;

FRACTAL_ALGORITHM g_iFractalAlgo;

int g_iCurrentHeightmap= 0;

bool g_bTexture= true;
bool g_bDetail = true;

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
	srand( GetCurrentTime( ) );

	g_glApp.Init( 10, 10, g_iScreenWidth, g_iScreenHeight, 16, "Demo 4_1: Height-Based Lighting", IDI_ICON1, IDR_MENU1 );
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
	g_bruteForce.MakeTerrainPlasma( 128, 1.0f );
	g_bruteForce.SetHeightScale( 0.25f );

	//set the terrain's lighting system up
	g_bruteForce.SetLightColor( CVECTOR( 1.0f, 1.0f, 1.0f ) );
	
	//load the various terrain tiles
	g_bruteForce.LoadTile( LOWEST_TILE,  "../Data/lowestTile.tga" );
	g_bruteForce.LoadTile( LOW_TILE,     "../Data/lowTile.tga" );
	g_bruteForce.LoadTile( HIGH_TILE,    "../Data/highTile.tga" );
	g_bruteForce.LoadTile( HIGHEST_TILE, "../Data/highestTile.tga" );

	//load the terrain's detail map
	g_bruteForce.LoadDetailMap( "../Data/detailMap.tga" );
	g_bruteForce.DoDetailMapping( g_bDetail, 8 );

	//make the texture map, and then save it
	g_bruteForce.GenerateTextureMap( 256 );
	g_bruteForce.DoTextureMapping( g_bTexture );
	g_bruteForce.DoMultitexturing( g_glApp.CanMultitexture( ) );

	//set the camera's position
	g_camera.SetPosition( 64.0f, 128.0f, 256.0f );
	g_camera.m_fPitch-= 25;

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

	//setup the terrain
	g_bruteForce.DoTextureMapping( g_bTexture );
	g_bruteForce.DoDetailMapping( g_bDetail, 8 );

	//render the simple terrain!
	glPushMatrix( );
		glScalef( 2.0f, 2.0f, 2.0f );
		g_bruteForce.Render( );
	glPopMatrix( );

	//render some text to the screen
	glDisable( GL_TEXTURE_2D );
	g_glApp.BeginTextMode( );
		//render the number of frames per second
		g_glApp.Print( g_iScreenWidth-175, g_iScreenHeight-70, CVECTOR( 0.0f, 1.0f, 0.0f), 
					   "FPS:  %7.0f", g_glApp.GetFPS( ) );

		//render the number of vertices per frame
		g_glApp.Print( g_iScreenWidth-175, g_iScreenHeight-85, CVECTOR( 0.0f, 1.0f, 0.0f),
					   "Vertices: %d", g_bruteForce.GetNumVertsPerFrame( ) );

		//render how many million triangles are rendered per second
		g_glApp.Print( g_iScreenWidth-175, g_iScreenHeight-100, CVECTOR( 0.0f, 1.0f, 0.0f),
					   "MTris/S:  %.3f", ( g_bruteForce.GetNumTrisPerFrame( )*g_glApp.GetFPS( ) )/1000000.0f );

		if( g_bTexture )
			g_glApp.Print( 0, g_iScreenHeight-70, CVECTOR( 0.0f, 1.0f, 0.0f), "Texturing: Enabled", g_glApp.GetFPS( ) );
		else
			g_glApp.Print( 0, g_iScreenHeight-70, CVECTOR( 0.0f, 1.0f, 0.0f), "Texturing: Disabled", g_glApp.GetFPS( ) );

		if( g_bDetail )
			g_glApp.Print( 0, g_iScreenHeight-90, CVECTOR( 0.0f, 1.0f, 0.0f), "Detail Mapping: Enabled", g_glApp.GetFPS( ) );
		else
			g_glApp.Print( 0, g_iScreenHeight-90, CVECTOR( 0.0f, 1.0f, 0.0f), "Detail Mapping: Disabled", g_glApp.GetFPS( ) );
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
	g_bruteForce.UnloadAllTiles( );
	g_bruteForce.UnloadTexture( );
	g_bruteForce.UnloadHeightMap( );

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
	else if( g_glApp.KeyDown( 'Q' ) )
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
	if( g_glApp.KeyDown( 'S' ) )
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	//toggle detail mapping
	if( g_glApp.KeyDown( 'D' ) )
	{
		//wait a few seconds after the last press
		if( iToggleWait<10 )
			return true;

		if( g_bDetail )
			g_bDetail= false;

		else
			g_bDetail= true;

		iToggleWait= 0;
	}

	//toggle texture mapping
	if( g_glApp.KeyDown( 'T' ) )
	{
		//wait a few seconds after the last press
		if( iToggleWait<10 )
			return true;

		if( g_bTexture )
			g_bTexture= false;

		else
			g_bTexture= true;

		iToggleWait= 0;
	}

	//increase mouse sensitivity
	if( g_glApp.KeyDown( VK_ADD ) )
	{
		g_fMouseSensitivity+= 0.25f;

		if( g_fMouseSensitivity>10.0f )
			g_fMouseSensitivity= 10.0f;
	}

	//decrease mouse sensitivity
	else if( g_glApp.KeyDown( VK_SUBTRACT ) )
	{
		g_fMouseSensitivity-= 0.25f;

		if( g_fMouseSensitivity<0.01f )
			g_fMouseSensitivity= 0.01f;
	}

	//increase movement sensitivity
	if( g_glApp.KeyDown( ']' ) )
	{
		g_fMovementSpeed+= 0.25f;

		if( g_fMovementSpeed>20.0f )
			g_fMovementSpeed= 20.0f;
	}

	//decrease movement sensitivity
	else if( g_glApp.KeyDown( '[' ) )
	{
		g_fMovementSpeed-= 0.25f;

		if( g_fMovementSpeed<0.01f )
			g_fMovementSpeed= 0.01f;
	}

	return true;
}

//--------------------------------------------------------------
// Name:			IDD_ROUGHNESSProc - global
// Description:		Process the messages for the roughness dialog box
// Arguments:		None as far as you're concerned :)
// Return Value:	A BOOL variable
//--------------------------------------------------------------
BOOL CALLBACK IDD_ROUGHNESSProc( HWND hWnd,
								 unsigned int uiMsg,
								 WPARAM wParam,
							     LPARAM lParam )
{
	float fDetail;

	switch( uiMsg )
	{
		//initiate the dialog box
		case WM_INITDIALOG:
			//set the dialog box's position
			SetWindowPos( hWnd, 
						  NULL, 
			 			  100, 100, 
						  0, 0, 
						  SWP_NOSIZE );


			return TRUE;
			break;

		case WM_COMMAND:
			switch( LOWORD( wParam ) )
				{
					case ID_ROUGHNESSDD_GENERATE:
						//get the value that was entered into the "detail" field
						fDetail= GetDlgItemInt( hWnd, IDC_ROUGHNESS, NULL, true )/100.0f;

						//check to see which algorithm we are doing
						if( g_iFractalAlgo==FAULT_FORMATION )
							g_bruteForce.MakeTerrainFault( 128, 64, 0, 255, fDetail );

						else if( g_iFractalAlgo==MIDPOINT_DISPLACEMENT )
							g_bruteForce.MakeTerrainPlasma( 128, fDetail );

						//close the dialog box now that generation is done
						EndDialog( hWnd, 0 );
						return TRUE;
						break;
						
					//cancel was pressed, back out
					case ID_ROUGHNESSDD_CANCEL:
						EndDialog( hWnd, 0 );
						return TRUE;
						break;
				}
			break;
	}

	//message not handled
	return FALSE;
}

//--------------------------------------------------------------
// Name:			IDD_TEXTUREProc - global
// Description:		Process the messages for the texture map dialog box
// Arguments:		None as far as you're concerned :)
// Return Value:	A BOOL variable
//--------------------------------------------------------------
BOOL CALLBACK IDD_TEXTUREProc( HWND hWnd,
						 	   unsigned int uiMsg,
							   WPARAM wParam,
							   LPARAM lParam )
{
	unsigned int uiSize;

	switch( uiMsg )
	{
		//initiate the dialog box
		case WM_INITDIALOG:
			//set the dialog box's position
			SetWindowPos( hWnd, 
						  NULL, 
			 			  100, 100, 
						  0, 0, 
						  SWP_NOSIZE );


			return TRUE;
			break;

		case WM_COMMAND:
			switch( LOWORD( wParam ) )
				{
					case ID_TEXTUREDD_GENERATE:
						//get the value that was entered into the "detail" field
						uiSize= ( unsigned int )GetDlgItemInt( hWnd, IDC_ROUGHNESS, NULL, true );

						//create the texture map
						g_bruteForce.GenerateTextureMap( uiSize );

						//close the dialog box now that generation is done
						EndDialog( hWnd, 0 );
						return TRUE;
						break;
						
					//cancel was pressed, back out
					case ID_TEXTUREDD_CANCEL:
						EndDialog( hWnd, 0 );
						return TRUE;
						break;
				}
			break;
	}

	//message not handled
	return FALSE;
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
	char szBuffer[256];

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

		//save the current height map
		else if( g_glApp.MenuDown( ID_DEMO_SAVEMAP ) )
		{
			sprintf( szBuffer, "height%d_%d.RAW", g_bruteForce.m_iSize, g_iCurrentHeightmap );

			g_bruteForce.SaveHeightMap( szBuffer );

			g_iCurrentHeightmap++;
		}

		//save the current texture map
		else if( g_glApp.MenuDown( ID_DEMO_SAVE_TEXTUREMAP ) )
			g_bruteForce.SaveTextureMap( "texturemap.bmp" );

		//generate a texture map (with the size provided)
		else if( g_glApp.MenuDown( ID_DEMO_GENERATE_TEXTURE_MAP ) )
			g_glApp.CreateDialogBox( IDD_TEXTURESIZE, IDD_TEXTUREProc );

		//generate a new height map using fault formation
		else if( g_glApp.MenuDown( ID_DEMO_FF ) )
		{
			g_iFractalAlgo= FAULT_FORMATION;
			g_glApp.CreateDialogBox( IDD_ROUGHNESS, IDD_ROUGHNESSProc );
		}

		//generate a new height map using midpoint displacement
		else if( g_glApp.MenuDown( ID_DEMO_MD ) )
		{
			g_iFractalAlgo= MIDPOINT_DISPLACEMENT;
			g_glApp.CreateDialogBox( IDD_ROUGHNESS, IDD_ROUGHNESSProc );
		}

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