// RThread.c
// OpenGL SuperBible
// Program by Richard S. Wright Jr.
// Demonstrates using a rendering thread and
// Using the WGL Extensions

#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "..\..\common\wglext.h"
#include "..\..\common\glext.h"
#include "..\..\common\gltools.h"
#include "resource.h"


//typedef int (APIENTRY * PFNWGLSWAPINTERVALEXTPROC) (int);
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

static HGLRC hRC = NULL;		// Permenant Rendering context
static HDC hDC = NULL;			// Private GDI Device context
static HWND hMainWindow = NULL;
static GLUquadricObj *pSphere = NULL;

// Shared rendering state information
struct RENDER_STATE
	{
	unsigned int uiFrames;	// Running Frame count
	BOOL bResize;			// Flag that window was resized	
	BOOL bTerminate;		// Flag to terminate rendering loop
	BOOL bFatalError;		// Fatal error has occured in render thread
	BOOL bModifyFlag;		// Flag set whenever something changes
	};

CRITICAL_SECTION	csThreadSafe;	// Critical section 
struct RENDER_STATE		rsRenderData;	// Communication between threads

// Rendering Thread Entry Point
void RenderingThreadEntryPoint(void *pVoid);

static LPCTSTR lpszAppName = "Rendering Thread";

// Declaration for Window procedure
LRESULT CALLBACK WndProc(	HWND 	hWnd,
							UINT	message,
							WPARAM	wParam,
							LPARAM	lParam);

// Set Pixel Format function - forward declaration
void SetDCPixelFormat(HDC hDC);

///////////////////////////////////////////////////////////////////////////////
// Load a Bitmaps bits from a resource file. Only works with 24-bit images
BYTE* gltResourceBMPBits(UINT nResource, int *nWidth, int *nHeight)
	{
	HINSTANCE hInstance;	// Instance Handle
	HANDLE hBitmap;			// Handle to bitmap resource
	BITMAPINFO bmInfo;
	BYTE *pData;

	// Find the bitmap resource
	hInstance = GetModuleHandle(NULL);
	hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(nResource));

	if(hBitmap == NULL)
		return NULL;

	GetObject(hBitmap,sizeof(BITMAPINFO),&bmInfo);
	DeleteObject(hBitmap);

	hBitmap = LoadResource(hInstance,
		 FindResource(hInstance,MAKEINTRESOURCE(nResource), RT_BITMAP));

	if(hBitmap == NULL)
		return NULL;

	pData = (BYTE *)LockResource(hBitmap);
	pData += sizeof(BITMAPINFO)-1;

	*nWidth = bmInfo.bmiHeader.biWidth; //bm.bmWidth;
	*nHeight = bmInfo.bmiHeader.biHeight;//bm.bmHeight;

	return pData;
	}


// Reset Viewport and viewing volume transormation
void ChangeSize(GLsizei w, GLsizei h)
	{
	GLfloat fAspect;

	// Prevent a divide by zero
	if(h == 0)
		h = 1;

	// Set Viewport to window dimensions
    glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    fAspect = (float)w/(float)h;
    gluPerspective(17.5, fAspect, 1.0, 200.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -200.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	}

///////////////////////////////////////////////////////////////////////////////
// Resets the viewport. This function is called when the window is resized
void ResetViewport()
	{
	// Get the Window dimensions
	RECT rect;
	GetClientRect(hMainWindow, &rect);
	ChangeSize(rect.right, rect.bottom);
	}


///////////////////////////////////////////////////////////////////////////////
// Check rendering states and make appropriate adjustements. Returns true on
// termination flag.
BOOL CheckStates()
	{
	BOOL bRet = FALSE;

	// Is it time to leave or anything else
	EnterCriticalSection(&csThreadSafe);

	if(	rsRenderData.bModifyFlag)
		{
		// Flag set to terminate
		if(rsRenderData.bTerminate)
			bRet = TRUE;

		// Window resized
		if(rsRenderData.bResize)
			ResetViewport();

		// Reset the modify flag and exit
		rsRenderData.bModifyFlag = FALSE;
		}

	// Increment frame count
	rsRenderData.uiFrames++;

	LeaveCriticalSection(&csThreadSafe);

	return bRet;
	}



void SetupRC(HDC hDC)
	{
	BYTE *pBytes;
	int nWidth, nHeight;

	SetDCPixelFormat(hDC);

	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);


	// Set background clearing color to blue
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	pSphere = gluNewQuadric();
	gluQuadricDrawStyle(pSphere, GLU_FILL);
	gluQuadricNormals(pSphere, GLU_NONE);
	gluQuadricTexture(pSphere, GLU_TRUE);

	pBytes = gltResourceBMPBits(IDB_BM_EARTH, &nWidth, &nHeight);

    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_DECAL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,nWidth, nHeight, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);


	// Tell SwapBuffers to swap only once per interval...
	if(gltIsExtSupported("WGL_EXT_swap_control"))
		{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if(wglSwapIntervalEXT != NULL)
			wglSwapIntervalEXT(1);
		}
	}

// Called by AUX library to draw scene
void RenderScene(void)
	{
	static float fRot = 0.0f;
	
	fRot += 1.0f / 60.0f * 90.0f;

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT);
	
	glPushMatrix();
	glRotatef(fRot, 0.0f, 0.0f, 1.0f);
	gluSphere(pSphere, 20.0f, 27, 13);
	glPopMatrix();
	}



// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC)
	{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,								// Version of this structure	
		PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |			// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,				// Double buffered mode
		PFD_TYPE_RGBA,					// RGBA Color mode
		32,								// Want 32 bit color 
		0,0,0,0,0,0,					// Not used to select mode
		0,0,							// Not used to select mode
		0,0,0,0,0,						// Not used to select mode
		16,								// Size of depth buffer
		0,								// Not used to select mode
		0,								// Not used to select mode
		0,	            				// Not used to select mode
		0,								// Not used to select mode
		0,0,0 };						// Not used to select mode

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);
	}



// Entry point of all Windows programs
int APIENTRY WinMain(	HINSTANCE 	hInstance,
						HINSTANCE 	hPrevInstance,
						LPSTR 		lpCmdLine,
						int			nCmdShow)
	{
	MSG			msg;		// Windows message structure
	WNDCLASS	wc;			// Windows class structure
	HWND		hWnd;		// Storeage for window handle


	// Register Window style
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance 		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	
	// No need for background brush for OpenGL window
	wc.hbrBackground	= NULL;		
	
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= lpszAppName;

	// Register the window class
	if(RegisterClass(&wc) == 0)
		return FALSE;

	// Create the main application window
	hWnd = CreateWindow(
				lpszAppName,
				lpszAppName,
				
				// OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS
				WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
	
				// Window position and size
				100, 100,
				400, 400,
				NULL,
				NULL,
				hInstance,
				NULL);

	// If window was not created, quit
	if(hWnd == NULL)
		return FALSE;

	// Display the window
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);
	
	hMainWindow = hWnd;


	// Process application messages until the application closes
	while(GetMessage(&msg, NULL, 0, 0))
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}

	return msg.wParam;
	}



// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(	HWND 	hWnd,
							UINT	message,
							WPARAM	wParam,
							LPARAM	lParam)
	{
	static HANDLE hThreadHandle = NULL;
	static int nLastFrames = 0;

	switch (message)
	   	{
		// Window creation, setup for OpenGL
		case WM_CREATE:
			// Store the device context
			hDC = GetDC(hWnd);		

			// Initialize the critical section
			InitializeCriticalSection(&csThreadSafe);

			// Initialize interthread communication
			rsRenderData.uiFrames = 0;
			rsRenderData.bResize = FALSE;
			rsRenderData.bTerminate = FALSE;
			rsRenderData.bFatalError = FALSE;
			rsRenderData.bModifyFlag = TRUE;


			// Create the rendering thread
			hThreadHandle = (HANDLE) _beginthread(RenderingThreadEntryPoint,0,hDC);
			break;


		// Tell the application to terminate after the window
		// is gone.
		case WM_DESTROY:
			{
			// Tell the rendering loop to terminate itself
			EnterCriticalSection(&csThreadSafe);
			rsRenderData.bTerminate = TRUE;
			rsRenderData.bModifyFlag = TRUE;
			LeaveCriticalSection(&csThreadSafe);

			// Need to wait for rendering thread to terminate so it can clean
			// up. Wait for the thread handle to become signaled. Give it five
			// seconds, then terminate anyway if it hasn't yet (it's probably
			// stuck if it hasn't terminated by now)
			WaitForSingleObject(hThreadHandle, 5000);

			DeleteCriticalSection(&csThreadSafe);

			ReleaseDC(hWnd, hDC);
			PostQuitMessage(0);
			}
			break;

		case WM_MOVE:
		case WM_SIZE:
			EnterCriticalSection(&csThreadSafe);
			rsRenderData.bResize = TRUE;
			rsRenderData.bModifyFlag = TRUE;
			LeaveCriticalSection(&csThreadSafe);
			break;

		case WM_ERASEBKGND :
			return 1L;			// Return handled to prevent erasing background
			break;

        default:   // Passes it on if unproccessed
            return (DefWindowProc(hWnd, message, wParam, lParam));

        }

    return (0L);
	}


///////////////////////////////////////////////////////////////////////////////
// Rendering Thread Entry Point
void RenderingThreadEntryPoint(void *pVoid)
	{
	// Setup the rendering context
	SetupRC((HDC)pVoid);

	while(1)
		{
		// Check rendering state info, terminate if flagged
		if(CheckStates())
			break;	

		RenderScene();

		// Swap buffers, but only once per video frame

		SwapBuffers(hDC);
		}

	// Do any necessary cleanup and terminate
	gluDeleteQuadric(pSphere);

	// Finally, shut down OpenGL Rendering context
	wglMakeCurrent(hDC, NULL);
	wglDeleteContext(hRC);

	_endthread();	// Terminate the thread nicely
	}
