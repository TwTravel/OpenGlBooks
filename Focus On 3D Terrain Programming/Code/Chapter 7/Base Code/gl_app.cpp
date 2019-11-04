//==============================================================
//==============================================================
//= gl_app.cpp =================================================
//= Original coder: Trent Polack (trent@voxelsoft.com)			
//==============================================================
//= The application component from "the Engine Project."		
//==============================================================
//==============================================================


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include "gl_app.h"
#include "image.h"
#include "log.h"

#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )


//--------------------------------------------------------------
//--------------------------------------------------------------
//- VARIABLES --------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//the window's dimensions
int CGL_APP::m_iWidth;
int CGL_APP::m_iHeight;
int CGL_APP::m_iBPP;
WORD CGL_APP::m_menuCommand;

//input variables
int CGL_APP::m_iMouseX;
int CGL_APP::m_iMouseY;
int CGL_APP::m_iMouseButton;
bool CGL_APP::m_bKeys[256];

bool CGL_APP::m_bActive;
bool CGL_APP::m_bChangeSize;

PFNGLACTIVETEXTUREARBPROC	    glActiveTextureARB= NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB= NULL;
PFNGLMULTITEXCOORD2FARBPROC	    glMultiTexCoord2fARB;
PFNGLMULTITEXCOORD2DARBPROC     glMultiTexCoord2dARB;

PFNGLLOCKARRAYSEXTPROC   glLockArraysEXT;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;

PFNGLFOGCOORDFEXTPROC glFogCoordfEXT;

//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:			CGL_APP::Init - public
// Description:		Initiate an OpenGL application
// Arguments:		-iX, iY: the upperleft position of the window
//					-iWidth, iHeight: the dimensions of the window
//					-iBPP: the pixel depth of the window (usually 16 or 32)
//					-szTitle: the title to appear in the window's titlebar
//					-icon: a resource ID for a program application icon (defaults to NULL)
//					-menu: a resource ID for a menu (defaults to NULL)
// Return Value:	A boolean variable: -true: successful window initiation
//										-false: unsuccessful window initiation
//--------------------------------------------------------------
bool CGL_APP::Init( int iX, int iY, int iWidth, int iHeight, int iBPP, 
					const char* szTitle, WORD icon, WORD menu )
{
	WNDCLASS winclass;				//The windows class structure
	unsigned int uiPixelFormat;		//Hold the correct match for the pixel format

	//initiate the program log
	g_log.Init( "program log.html" );

	if( m_timer.Init( ) )
		g_log.Write( LOG_PLAINTEXT, "Performance timer has been initialized" );
	else
		g_log.Write( LOG_PLAINTEXT, "Multimedia timer has been initialized" );

	//record the dimensions in the class's members
	m_iWidth = iWidth;
	m_iHeight= iHeight;
	m_iBPP   = iBPP;

	//customize the window's parameters
	m_hInstance			  = GetModuleHandle( NULL );				//get the window's instance
	winclass.style		  = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		//redraw the window on resize
	winclass.lpfnWndProc  = WindowProc;								//handle the WINPROC messages
	winclass.cbClsExtra	  = 0;										//no extra win data
	winclass.cbWndExtra	  = 0;										//no extra win data
	winclass.hInstance	  = m_hInstance;							//get the window's instance
	winclass.hIcon		  = LoadIcon( NULL, MAKEINTRESOURCE( icon ) );//load an icon
	winclass.hCursor	  = LoadCursor( NULL, IDC_ARROW );			//load the mouse pointer
	winclass.lpszMenuName = MAKEINTRESOURCE( menu );				//load the menu
	winclass.hbrBackground= ( HBRUSH )GetStockObject( BLACK_BRUSH );//no background required for OpenGL stuff
	winclass.lpszClassName= APP_CLASSNAME;							//set the class name

	//register the window class
	if( !RegisterClass(&winclass ) )
	{
		g_log.Write( LOG_FAILURE, "WINDOW FAILURE: could not register class" );
		return false;		
	}
	//ruh-roh, something screwed up
	else
		g_log.Write( LOG_SUCCESS, "WINDOW SUCCESS: class registered" );

	//create the window
	if( !( m_hWnd=CreateWindowEx( WS_EX_CLIENTEDGE,			//the extended style for the window
							      APP_CLASSNAME,			//the class name
							      szTitle,					//the window's title
							      WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_VISIBLE,
							      iX, iY,					//the window's position
							      m_iWidth,					//calculate the window's width
							      m_iHeight,				//calculate the window's height
							      NULL,						//no parent window
							      NULL,						//no menu needed
							      m_hInstance,				//the window's instance
							      NULL ) ) )				//nothing to pass to WM_CREATE
	{
		//the window could not be created
		Shutdown( );				
		g_log.Write( LOG_FAILURE, "WINDOW ERROR: window could not be created" );
		return false;			
	}

	//get the pixel format's description
	static PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof( PIXELFORMATDESCRIPTOR ),	//Get the size of the structure
		1,									//Version number
		PFD_DRAW_TO_WINDOW |				//Format must support Windows
		PFD_SUPPORT_OPENGL |				//Format must support OpenGL
		PFD_DOUBLEBUFFER,					//Must support Double Buffering
		PFD_TYPE_RGBA,						//Request a RGBA (red,green,blue,alpha) format
		m_iBPP,								//Select the bits per pixel
		0, 0, 0, 0, 0, 0,					//Color bits ignored
		0,									//No alpha buffer
		0,									//shift bit ignored
		0,									//No accumulation buffer (advanced)
		0, 0, 0, 0,							//Accumulation bits ignored
		32,									//16 bit Z-Buffer (Depth Buffer)  
		0,									//No Stencil Buffer (advanced)
		0,									//No Auxiliary Buffer (advanced)
		PFD_MAIN_PLANE,						//The main drawing layer
		0,									//Reserved area
		0, 0, 0								//Layer masks ignored
	};
	
	//get a handle to the device context
	if( !( m_hDC= GetDC( m_hWnd ) ) )
	{
		//no device context for us
		Shutdown( );
		MessageBox( m_hWnd, "Could not create a OpenGL device context", "RENDERER ERROR", MB_OK | MB_ICONSTOP );
		return false;								
	}

	//find a compatible pixel format
	if(!(uiPixelFormat= ChoosePixelFormat(m_hDC, &pfd)))
	{
		//no suitable pixel format
		Shutdown( );
		MessageBox( m_hWnd, "Could not find a suitable pixel format", "RENDERER ERROR", MB_OK | MB_ICONSTOP );
		return false;
	}

	//set the previously found pixel format
	if( !SetPixelFormat( m_hDC, uiPixelFormat, &pfd ) )
	{
		//could not set the pixel format
		Shutdown( );
		MessageBox( m_hWnd, "Could not set pixel format", "RENDERER ERROR", MB_OK | MB_ICONSTOP );
		return false;
	}
	
	//get a handle to the application's rendering context
	if( !( m_hRC= wglCreateContext( m_hDC ) ) )
	{
		//could not get OpenGL support
		Shutdown( );
		MessageBox( m_hWnd, "Could not create the OpenGL rendering context", "RENDERER ERROR", MB_OK | MB_ICONSTOP );
		return false;		
	}

	//set the device and rendering context to our window (activating OpenGL)
	if( !wglMakeCurrent( m_hDC, m_hRC ) )
	{
		//could not activate OpenGL
		Shutdown( );
		MessageBox( m_hWnd, "Could not activate the OpenGL rendering context", "RENDERER ERROR", MB_OK | MB_ICONSTOP );
		return false;
	}

	glShadeModel( GL_SMOOTH );							//Enable smooth shading (so you can't see the individual polygons of a primitive, best shown when drawing a sphere)
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClearDepth( 1.0 );									//Depth buffer setup
	glDepthFunc( GL_LEQUAL );								//The type of depth testing to do (LEQUAL==less than or equal to)
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );	//The nicest perspective look

	ShowWindow( m_hWnd, SW_SHOW );						//Show the window
	SetForegroundWindow( m_hWnd );						//Give the window a high priority
	SetFocus( m_hWnd );									//Sets the keyboard's focus to the window

	//setup the program's perspective view
	ResizeScene( m_iWidth, m_iHeight );

	m_szSupportedGLExtensions= ( char* )glGetString( GL_EXTENSIONS );

	if( CheckExtension( "GL_ARB_multitexture" ) )
	{
		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &m_iNumTextureUnits);
		if(m_iNumTextureUnits > 1)
			m_bCanMultitexture= true;
		else
			m_bCanMultitexture= false;
	}
	else
		m_bCanMultitexture= false;

	glActiveTextureARB		= ( PFNGLACTIVETEXTUREARBPROC )	     wglGetProcAddress( "glActiveTextureARB" );
	glClientActiveTextureARB= ( PFNGLCLIENTACTIVETEXTUREARBPROC )wglGetProcAddress( "glClientActiveTextureARB" );
	glMultiTexCoord2fARB	= ( PFNGLMULTITEXCOORD2FARBPROC )	 wglGetProcAddress( "glMultiTexCoord2fARB" );
	glMultiTexCoord2dARB	= ( PFNGLMULTITEXCOORD2DARBPROC )	 wglGetProcAddress( "glMultiTexCoord2dARB" );

	g_log.Write( LOG_RENDERER, "Multitexturing has been initiated, and your video card supports %d texture units", m_iNumTextureUnits );

	if( CheckExtension( "GL_EXT_compiled_vertex_array" ) )
		m_bCanCVA= true;
	else
		m_bCanCVA= false;

	CheckExtension( "GL_CLAMP_TO_EDGE" );

	//configure the volumetric fog system
	if( CheckExtension( "GL_EXT_fog_coord" ) )
		glFogCoordfEXT= ( PFNGLFOGCOORDFEXTPROC )wglGetProcAddress( "glFogCoordfEXT" );

	glLockArraysEXT  = ( PFNGLLOCKARRAYSEXTPROC )  wglGetProcAddress( "glLockArraysEXT" );
	glUnlockArraysEXT= ( PFNGLUNLOCKARRAYSEXTPROC )wglGetProcAddress( "glUnlockArraysEXT" );

	m_bActive= APP_ACTIVE;
	g_log.Write( LOG_SUCCESS, "WINDOW SUCCESS: A %dx%dx%d window has been created", m_iWidth, m_iHeight, m_iBPP);
	return true;
}

//--------------------------------------------------------------
// Name:			CGL_APP::Shutdown - public
// Description:		Shutdown an OpenGL application
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CGL_APP::Shutdown( void )
{
	ShowCursor( TRUE );

	//Release the main window handle
	if( m_hWnd && !DestroyWindow( m_hWnd ) )
	{
		g_log.Write( LOG_FAILURE, "WINDOW ERROR: could not release main handle" );
		m_hWnd= NULL;					
	}
	else
		g_log.Write( LOG_SUCCESS, "WINDOW SUCCESS: released Main handle" );

	//Unregister the window's class
	if( !UnregisterClass( APP_CLASSNAME, m_hInstance ) )
	{
		g_log.Write( LOG_FAILURE, "WINDOW ERROR: could not unregister class" );	
		m_hInstance= NULL;			
	}
	else
		g_log.Write( LOG_SUCCESS, "WINDOW SUCCESS: unregistered class" );
}

//--------------------------------------------------------------
// Name:			CGL_APP::Shutdown - public
// Description:		Check to see if the current graphics card supports
//					the given OpenGL extension
// Arguments:		-szExtensionName: the extension to check
// Return Value:	A boolean variable: -true: the extension is supported
//										-false: the extenson is not supported
//--------------------------------------------------------------
bool CGL_APP::CheckExtension( char* szExtensionName )
{
	unsigned int uiNextExtension;
	char*		 szSupExt= m_szSupportedGLExtensions;
	char*		 cEndExtensions;

	//find the end of the extension list
	cEndExtensions= szSupExt+strlen( m_szSupportedGLExtensions );

	//search through the entire list
	while( szSupExt<cEndExtensions )
	{
		//find the next extension in the list
		uiNextExtension= strcspn( szSupExt, " " );

		//check the extension to the one given in the argument list
		if( ( strlen( szExtensionName )==uiNextExtension ) && 
			( strncmp( szExtensionName, szSupExt, uiNextExtension )==0 ) )
		{
			//the extension is supported
			g_log.Write( LOG_RENDERER, "Your video card supports extension: %s", szExtensionName );
			return true;
		}

		//move to the nexte extension in the list
		szSupExt+= ( uiNextExtension+1 );
	}

	//the extension is not supported
	g_log.Write( LOG_RENDERER, "Your video card does not support extension: %s", szExtensionName );
	return false;
}

//--------------------------------------------------------------
// Name:			CGL_APP::CreateTTFont - public
// Description:		Create a truetype font (for font rendering)
// Arguments:		-szFontName: the truetype font to be created
//					-iSize: the size of the font
// Return Value:	None
//--------------------------------------------------------------
void CGL_APP::CreateTTFont( char* szFontName, int iSize )
{
	HFONT font;

	m_uiFont= glGenLists( 256 );

	font= CreateFont( -iSize,
					  0,
					  0,
			 		  0,
					  FW_NORMAL,
					  FALSE,
					  FALSE,
					  FALSE,
					  ANSI_CHARSET,
					  OUT_TT_PRECIS,
					  CLIP_DEFAULT_PRECIS,
					  ANTIALIASED_QUALITY,
					  FF_DONTCARE | DEFAULT_PITCH,
					  szFontName);

	SelectObject( m_hDC, font );
	wglUseFontBitmaps( m_hDC, 0, 256, m_uiFont );
}

//--------------------------------------------------------------
// Name:			CGL_APP::DestroyFont - public
// Description:		Destroy the font system created earlier
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CGL_APP::DestroyFont( void )
{	glDeleteLists( m_uiFont, 256 );	}

//--------------------------------------------------------------
// Name:			CGL_APP::Print - public
// Description:		Print a string onto the screen
// Arguments:		-iX, iY: Position on the screen to start printing
//					-vecColor: The color of the printed text
//					-szString: the string to be printed
// Return Value:	None
//--------------------------------------------------------------
void CGL_APP::Print( int iX, int iY, CVECTOR vecColor, char* szString, ... )
{
	va_list va;
	char	szParsedString[256];

	//make sure that the string actually exists
	if( szString==NULL )
		return;

	//parse the string
	va_start( va, szString );
	    vsprintf( szParsedString, szString, va );
	va_end( va );

	//set the color for the text
	glColor3f( vecColor[0], vecColor[1], vecColor[2] );

	glPushMatrix( );
		glLoadIdentity( );

		//position and render the text
		glRasterPos2d( iX, iY );
		glListBase( m_uiFont );
		glCallLists( strlen( szParsedString ), GL_UNSIGNED_BYTE, szParsedString );
	glPopMatrix( );
}

//--------------------------------------------------------------
// Name:			CGL_APP::BeginTextMode - public
// Description:		Begin text mode (2D mode)
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CGL_APP::BeginTextMode( void )
{
	glMatrixMode( GL_PROJECTION );					//select the projection matrix
	glPushMatrix( );								//store the projection matrix
		glLoadIdentity( );							//reset the projection matrix
		glOrtho( 0, m_iWidth, 0, m_iHeight, -1, 1 );//set up an ortho screen
		glMatrixMode( GL_MODELVIEW );				//select the modelview matrix
}

//--------------------------------------------------------------
// Name:			CGL_APP::EndTextMode - public
// Description:		End text mode (2D mode)
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CGL_APP::EndTextMode( void )
{
		glMatrixMode(GL_PROJECTION);	//select the projection matrix
	glPopMatrix();						//restore the old projection matrix
	glMatrixMode(GL_MODELVIEW);			//select the modelview matrix
}

//--------------------------------------------------------------
// Name:			CGL_APP::BeginRendering - public
// Description:		Begin rendering, and update the timer
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CGL_APP::BeginRendering( void )
{
    m_timer.Update( );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

//--------------------------------------------------------------
// Name:			CGL_APP::EndRendering - public
// Description:		Finish all rendering and swap buffers
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CGL_APP::EndRendering( void )
{
	glFlush( );
	SwapBuffers( m_hDC );
}

//--------------------------------------------------------------
// Name:			CGL_APP::EndRendering - public
// Description:		Finish all rendering and swap buffers
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void CGL_APP::TakeScreenshot( char* szFilename )
{
	CIMAGE screenshot;

	//allocate the memory
	screenshot.Create( m_iWidth+10, m_iHeight, 24 );

	//read the screen data
	glReadPixels( 0, 0, m_iWidth+10, m_iHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshot.GetData( ) );

	//write the image data
	screenshot.Save( szFilename );
}

//--------------------------------------------------------------
// Name:			CGL_APP::HandleMessages - public
// Description:		Handle the messages that the window is receiving
// Arguments:		None
// Return Value:	A boolean variable: -true: continue running the program
//										-false: end the program
//--------------------------------------------------------------
bool CGL_APP::HandleMessages(void)
{
	MSG msg;

	m_menuCommand= 0;

	//These are functions that are needed to process the messages
	//that the window is recieving.
	while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
	{ 
	    TranslateMessage( &msg );
	    DispatchMessage( &msg ); 

		if( msg.message==WM_QUIT )
			return false;
	}
	return true;
}

//--------------------------------------------------------------
// Name:			CGL_APP::WindowProc - public
// Description:		Handle all window events
// Arguments:		None (that you need to worry about :))
// Return Value:	A LRESULT variable
//--------------------------------------------------------------
LRESULT CALLBACK CGL_APP::WindowProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uiMsg )
	{
		case WM_ACTIVATE:
			if( !HIWORD( wParam ) )
				m_bActive= APP_ACTIVE;

			else
				m_bActive= APP_IDLE;
			return 0;
			break;

		case WM_SYSCOMMAND:
			switch( wParam )
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;

		case WM_COMMAND:
			m_menuCommand= LOWORD( wParam );
			break;

		case WM_KEYDOWN:
			m_bKeys[wParam]= true;
			return 0;
			break;

		case WM_KEYUP:
			m_bKeys[wParam]= false;
			return 0;
			break;

		case WM_MOUSEMOVE:
			//get the current mouse position
			m_iMouseX= ( int )LOWORD( lParam );
			m_iMouseY= ( int )HIWORD( lParam );

			//get the current mouse button being pressed
			m_iMouseButton= ( int )wParam;

			break;

		case WM_CLOSE:
			PostQuitMessage( 0 );
			return 0;
			break;

		case WM_SIZE:
			m_iWidth = LOWORD( lParam );
			m_iHeight= HIWORD( lParam );

			m_bChangeSize= true;
			return 0;
			break;
	}

	return DefWindowProc( hWnd, uiMsg, wParam, lParam );
}

//--------------------------------------------------------------
// Name:			ResizeScene - global
// Description:		Resize the perspective view according to the
//					width and height of the window
// Arguments:		None
// Return Value:	None
//--------------------------------------------------------------
void ResizeScene( int iWidth, int iHeight )
{
	if( iHeight==0 )											//Prevent a divide by zero (bad)
		iHeight= 1;											//Making height equal one

	glViewport( 0, 0, iWidth, iHeight );						//Reset the current viewport

	glMatrixMode( GL_PROJECTION );								//Select the projection matrix
	glLoadIdentity( );											//Reset the projection matrix

	//Calculate the aspect ratio of the window
	gluPerspective( 45, iWidth/iHeight, 4.0f, 4000.0f );

	glMatrixMode( GL_MODELVIEW );									//Select the modelview matrix
	glLoadIdentity( );											//Reset The modelview matrix
}