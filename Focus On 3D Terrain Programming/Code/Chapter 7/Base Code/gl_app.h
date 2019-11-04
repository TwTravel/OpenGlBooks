//==============================================================
//==============================================================
//= gl_app.cpp =================================================
//= Original coder: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= Simple framework for an OpenGL application (Windows only.) =
//==============================================================
//==============================================================
#ifndef __GL_APP_H__
#define __GL_APP_H__

//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <math.h>

#include <gl/gl.h>
#include <gl/glu.h>
#include "glext.h"

#include "log.h"
#include "timer.h"
#include "math_ops.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CONSTANTS --------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#define APP_CLASSNAME "window"

#define APP_IDLE   false
#define APP_ACTIVE true


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CGL_APP
{
	private:
		//the window's variables
		HINSTANCE m_hInstance;
		HWND	  m_hWnd;
		HDC		  m_hDC;
		HGLRC	  m_hRC;

		//extensions supported by OpenGL
		char* m_szSupportedGLExtensions;

		//font variables
		unsigned int m_uiFont;

		//timing variables
		CTIMER m_timer;

		//multitexturing variables
		int	 m_iNumTextureUnits;
		bool m_bCanMultitexture;

		//compiled vertex array (CVA) flag
		bool m_bCanCVA;

	static LRESULT CALLBACK WindowProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam );

	public:
		//the window's variables
		static int m_iWidth;
		static int m_iHeight;
		static int m_iBPP;
		static WORD m_menuCommand;

		//input-related functions
		static int m_iMouseX, m_iMouseY;
		static int m_iMouseButton;
		static bool m_bKeys[256];

		static bool m_bChangeSize;
		static bool m_bActive;

	bool Init( int iX, int iY, int iWidth, int iHeight, int iBPP,  const char* szTitle, WORD icon= NULL, WORD menu= NULL );
	void Shutdown( void );

	bool CheckExtension( char* szExtensionName );

	//font routines
	void CreateTTFont( char* szFontName, int iSize );
	void DestroyFont( void );
	void Print( int iX, int iY, CVECTOR vecColor, char* szString, ... );
	void BeginTextMode( void );
	void EndTextMode( void );

	void BeginRendering( void );
	void EndRendering( void );

	void TakeScreenshot( char* szFilename );

	bool HandleMessages( void );

	//----------------------------------------------------------
	// Name:			CGL_APP::CreateDialogBox - public
	// Description:		Create (bring up) a dialog box
	// Arguments:		-resourceID: a handle to the resource ID of the dialog box
	//					-dlgProc: the dialog box's message processing function
	// Return Value:	None
	//----------------------------------------------------------
	inline void CreateDialogBox( WORD resourceID, DLGPROC dlgProc )
	{	DialogBox( m_hInstance, MAKEINTRESOURCE( resourceID ), m_hWnd, dlgProc );	}

	//----------------------------------------------------------
	// Name:			CGL_APP::GetHInstance - public
	// Description:		Get a "copy" of the class's hInstance member
	// Arguments:		None
	// Return Value:	The class's HINSTANCE variable
	//----------------------------------------------------------
	inline HINSTANCE GetHInstance( void )
	{	return m_hInstance;	}

	//----------------------------------------------------------
	// Name:			CGL_APP::GetHWnd - public
	// Description:		Get a "copy" of the class's hWnd member
	// Arguments:		None
	// Return Value:	The class's HWND variable
	//----------------------------------------------------------
	inline HWND GetHWnd( void )
	{	return m_hWnd;	}

	//----------------------------------------------------------
	// Name:			CGL_APP::MenuDown - public
	// Description:		Check to see if a menu item has been selected
	// Arguments:		-ID: The menu item to check for
	// Return Value:	A boolean variable: -true: the menu item has been selected
	//										-false: the menu item has not been selected
	//----------------------------------------------------------
	inline bool MenuDown( WORD ID )
	{	return ( m_menuCommand==ID );	}

	//----------------------------------------------------------
	// Name:			CGL_APP::KeyDown - public
	// Description:		Check to see if a key on the keyboard is down
	// Arguments:		-ucKey: The key to check for
	// Return Value:	A boolean variable: -true: the key is down
	//										-false: the key is up
	//----------------------------------------------------------
	inline bool KeyDown( unsigned char ucKey )
	{	return m_bKeys[ucKey];	}

	//----------------------------------------------------------
	// Name:			CGL_APP::KeyUp - public
	// Description:		Check to see if a key on the keyboard is up
	// Arguments:		-ucKey: The key to check for
	// Return Value:	A boolean variable: -true: the key is up
	//										-false: the key is down
	//----------------------------------------------------------
	inline bool KeyUp( unsigned char ucKey )
	{	return !m_bKeys[ucKey];	}

	//----------------------------------------------------------
	// Name:			CGL_APP::GetMouseX - public
	// Description:		Get the X coordinate of the mouse's position
	// Arguments:		None
	// Return Value:	An int value: the mouse's X coordinate
	//----------------------------------------------------------
	inline int GetMouseX( void )
	{	return m_iMouseX;	}

	//----------------------------------------------------------
	// Name:			CGL_APP::GetMouseY - public
	// Description:		Get the Y coordinate of the mouse's position
	// Arguments:		None
	// Return Value:	An int value: the mouse's Y coordinate
	//----------------------------------------------------------
	inline int GetMouseY( void )
	{	return m_iMouseY;	}

	//----------------------------------------------------------
	// Name:			CGL_APP::MouseDown - public
	// Description:		Check to see if a button on the mouse is down
	// Arguments:		-button: The button to check for
	// Return Value:	A boolean variable: -true: the button is down
	//										-false: the button is up
	//----------------------------------------------------------
	inline bool MouseDown( WORD button )
	{
		if( m_iMouseButton & button )
			return true;
		else
			return false;
	}

	//----------------------------------------------------------
	// Name:			CGL_APP::MouseUp - public
	// Description:		Check to see if a button on the mouse is up
	// Arguments:		-button: The button to check for
	// Return Value:	A boolean variable: -true: the button is up
	//										-false: the button is down
	//----------------------------------------------------------
	inline bool MouseUp( WORD button )
	{
		if( m_iMouseButton & button )
			return false;
		else
			return true;
	}

	//----------------------------------------------------------
	// Name:			CGL_APP::GetFPS - public
	// Description:		Get the number of frames per second
	// Arguments:		None
	// Return Value:	A floating-point value: the number of frames
	//											per second.
	//----------------------------------------------------------
	inline float GetFPS( void )
	{	return m_timer.GetFPS( );	}

	//----------------------------------------------------------
	// Name:			CGL_APP::CGL_APP - public
	// Description:		Check to see if the application is active or not
	// Arguments:		None
	// Return Value:	A boolean variable: -true: the application is active
	//										-false: the application is inactive
	//----------------------------------------------------------
	inline bool IsActive( void )
	{	return m_bActive;	}

	//----------------------------------------------------------
	// Name:			CGL_APP::CanMultitexture - public
	// Description:		Check to see if we can or cannot use hardware multitexturing
	// Arguments:		None
	// Return Value:	A boolean variable: -true: can use multitexturing
	//										-false: cannot use multitexturing
	//----------------------------------------------------------
	inline bool CanMultitexture( void )
	{	return m_bCanMultitexture;	}

	//----------------------------------------------------------
	// Name:			CGL_APP::CGL_APP - public
	// Description:		Default constructor
	// Arguments:		None
	// Return Value:	None
	//----------------------------------------------------------
	CGL_APP(void)
	{	}

	//----------------------------------------------------------
	// Name:			CGL_APP::~CGL_APP - public
	// Description:		Default destructor
	// Arguments:		None
	// Return Value:	None
	//----------------------------------------------------------
	~CGL_APP(void)
	{	}
};


//--------------------------------------------------------------
//--------------------------------------------------------------
//- MACROS -----------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#define MIN( a, b )  ( ( ( a )<( b ) )?( a ):( b ) )
#define MAX( a, b )  ( ( ( a )<( b ) )?( b ):( a ) )

#define RANDOM_FLOAT		( ( ( float )rand( ) )/RAND_MAX )
#define CLAMP(x, min, max)	( ( x= ( x<min  ? min : x<max ? x : max ) ) )
#define SWAP_ROWS(a, b)		{	float *tmp= a;	a= b;	b= tmp;	}


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DECLARATIONS -----------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ResizeScene( int iWidth, int iHeight );

extern PFNGLACTIVETEXTUREARBPROC	   glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
extern PFNGLMULTITEXCOORD2FARBPROC	   glMultiTexCoord2fARB;
extern PFNGLMULTITEXCOORD2DARBPROC     glMultiTexCoord2dARB;

extern PFNGLLOCKARRAYSEXTPROC   glLockArraysEXT;
extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;

extern PFNGLFOGCOORDFEXTPROC glFogCoordfEXT;

#endif	//__GL_APP_H__