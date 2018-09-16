// FScreen.c
// OpenGL SuperBible, Chapter 17
// Program by Richard S. Wright Jr.
// This program shows a how to create a full screen
// window and render into it with OpenGL.
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>


HPALETTE hPalette = NULL;

// Initial square position and size
GLfloat x1 = 100.0f;
GLfloat y1 = 150.0f;
GLsizei rsize = 50;

// Step size in x and y directions
// (number of pixels to move each time)
GLfloat xstep = 1.0f;
GLfloat ystep = 1.0f;

// Keep track of windows changing width and height
GLfloat windowWidth;
GLfloat windowHeight;


static LPCTSTR lpszAppName = "GLRect";

// Declaration for Window procedure
LRESULT CALLBACK WndProc(	HWND 	hWnd,
							UINT	message,
							WPARAM	wParam,
							LPARAM	lParam);

// Set Pixel Format function - forward declaration
void SetDCPixelFormat(HDC hDC);


void ChangeSize(GLsizei w, GLsizei h)
	{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Reset the coordinate system before modifying
    glLoadIdentity();


	// Keep the square square, this time, save calculated
	// width and height for later use
	if (w <= h) 
		{
		windowHeight = 250.0f*h/w;
		windowWidth = 250.0f;
		}
    else 
		{
		windowWidth = 250.0f*w/h;
		windowHeight = 250.0f;
		}

	// Set the clipping volume
	gluOrtho2D(0.0f, windowWidth, 0.0f, windowHeight);
	}


// Called by timer routine to effect movement of the rectangle.
void IdleFunction(void)
	{
	// Reverse direction when you reach left or right edge
	if(x1 > windowWidth-rsize || x1 < 0)
		xstep = -xstep;

	// Reverse direction when you reach top or bottom edge
	if(y1 > windowHeight-rsize || y1 < 0)
		ystep = -ystep;


	// Check bounds.  This is incase the window is made
	// smaller and the rectangle is outside the new
	// clipping volume
	if(x1 > windowWidth-rsize)
		x1 = windowWidth-rsize-1;

	if(y1 > windowHeight-rsize)
		y1 = windowHeight-rsize-1;

	// Actually move the square
	x1 += xstep;
	y1 += ystep;
	}



// Called by AUX library to draw scene
void RenderScene(void)
	{
	// Set background clearing color to blue
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT);

	// Set drawing color to Red, and draw rectangle at
	// current position.
	glColor3f(1.0f, 0.0f, 0.0f);
	glRectf(x1, y1, x1+rsize, y1+rsize);
	}



// If necessary, creates a 3-3-2 palette for the device context listed.
HPALETTE GetOpenGLPalette(HDC hDC)
	{
	HPALETTE hRetPal = NULL;	// Handle to palette to be created
	PIXELFORMATDESCRIPTOR pfd;	// Pixel Format Descriptor
	LOGPALETTE *pPal;			// Pointer to memory for logical palette
	int nPixelFormat;			// Pixel format index
	int nColors;				// Number of entries in palette
	int i;						// Counting variable
	BYTE RedRange,GreenRange,BlueRange;
								// Range for each color entry (7,7,and 3)


	// Get the pixel format index and retrieve the pixel format description
	nPixelFormat = GetPixelFormat(hDC);
	DescribePixelFormat(hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	// Does this pixel format require a palette?  If not, do not create a
	// palette and just return NULL
	if(!(pfd.dwFlags & PFD_NEED_PALETTE))
		return NULL;

	// Number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;	

	// Allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) +nColors*sizeof(PALETTEENTRY));

	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size

	// Build mask of all 1's.  This creates a number represented by having
	// the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
	// pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) -1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) -1;

	// Loop through all the palette entries
	for(i = 0; i < nColors; i++)
		{
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double) pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char) NULL;
		}
		

	// Create the palette
	hRetPal = CreatePalette(pPal);

	// Go ahead and select and realize the palette for this device context
	SelectPalette(hDC,hRetPal,FALSE);
	RealizePalette(hDC);

	// Free the memory used for the logical palette structure
	free(pPal);

	// Return the handle to the new palette
	return hRetPal;
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
	HWND		hDesktopWnd;// Storeage for desktop window handle
	HDC			hDesktopDC; // Storeage for desktop window device context
	int			nScreenX, nScreenY; // Screen Dimensions

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

    // Get he Window handle and Device context to the desktop
	hDesktopWnd = GetDesktopWindow();
	hDesktopDC = GetDC(hDesktopWnd);

    // Get the screen size
	nScreenX = GetDeviceCaps(hDesktopDC, HORZRES);
	nScreenY = GetDeviceCaps(hDesktopDC, VERTRES);

    // Release the desktop device context
    ReleaseDC(hDesktopWnd, hDesktopDC);

	// Create the main application window
	hWnd = CreateWindow(
				lpszAppName,
				lpszAppName,
				
				// OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS
				WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
	
				// Window position and size
				0, 0,
				nScreenX, nScreenY,
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

	// Process application messages until the application closes
	while( GetMessage(&msg, NULL, 0, 0))
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
	static HGLRC hRC;		// Permenant Rendering context
	static HDC hDC;			// Private GDI Device context

	switch (message)
	   	{
		// Window creation, setup for OpenGL
		case WM_CREATE:
			// Store the device context
			hDC = GetDC(hWnd);		

			// Select the pixel format
			SetDCPixelFormat(hDC);		

			// Create the rendering context and make it current
			hRC = wglCreateContext(hDC);
			wglMakeCurrent(hDC, hRC);

			// Create the palette
			hPalette = GetOpenGLPalette(hDC);

			// Create a timer that fires 30 times a second
			SetTimer(hWnd,33,1,NULL);
			break;

		// Window is being destroyed, cleanup
		case WM_DESTROY:
			// Kill the timer that we created
			KillTimer(hWnd,101);

			// Deselect the current rendering context and delete it
			wglMakeCurrent(hDC,NULL);
			wglDeleteContext(hRC);

			// Delete the palette
			if(hPalette != NULL)
				DeleteObject(hPalette);

			// Tell the application to terminate after the window
			// is gone.
			PostQuitMessage(0);
			break;
        
		// Window is resized.
		case WM_SIZE:
			// Call our function which modifies the clipping
			// volume and viewport
			ChangeSize(LOWORD(lParam), HIWORD(lParam));
			break;

		// Timer, moves and bounces the rectangle, simply calls
		// our previous OnIdle function, then invalidates the 
		// window so it will be redrawn.
		case WM_TIMER:
			{
			IdleFunction();
		
			InvalidateRect(hWnd,NULL,FALSE);
			}
			break;

		// The painting function.  This message sent by Windows 
		// whenever the screen needs updating.
		case WM_PAINT:
			{
			// Call OpenGL drawing code
			RenderScene();

			// Call function to swap the buffers
			SwapBuffers(hDC);

			// Validate the newly painted client area
			ValidateRect(hWnd,NULL);
			}
			break;


		// Windows is telling the application that it may modify
		// the system palette.  This message in essance asks the 
		// application for a new palette.
		case WM_QUERYNEWPALETTE:
			// If the palette was created.
			if(hPalette)
				{
				int nRet;

				// Selects the palette into the current device context
				SelectPalette(hDC, hPalette, FALSE);

				// Map entries from the currently selected palette to
				// the system palette.  The return value is the number 
				// of palette entries modified.
				nRet = RealizePalette(hDC);

				// Repaint, forces remap of palette in current window
				InvalidateRect(hWnd,NULL,FALSE);

				return nRet;
				}
			break;

	
		// This window may set the palette, even though it is not the 
		// currently active window.
		case WM_PALETTECHANGED:
			// Don't do anything if the palette does not exist, or if
			// this is the window that changed the palette.
			if((hPalette != NULL) && ((HWND)wParam != hWnd))
				{
				// Select the palette into the device context
				SelectPalette(hDC,hPalette,FALSE);

				// Map entries to system palette
				RealizePalette(hDC);
				
				// Remap the current colors to the newly realized palette
				UpdateColors(hDC);
				return 0;
				}
			break;


        default:   // Passes it on if unproccessed
            return (DefWindowProc(hWnd, message, wParam, lParam));

        }

    return (0L);
	}


