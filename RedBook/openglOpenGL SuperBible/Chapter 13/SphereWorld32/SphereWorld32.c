// SphereWorld32.c
// OpenGL SuperBible
// Program by Richard S. Wright Jr.
// This program demonstrates a full featured robust Win32
// OpenGL framework

///////////////////////////////////////////////////////////////////////////////
// Include Files
#include <windows.h>                // Win32 Framework (No MFC)
#include <gl\gl.h>                  // OpenGL
#include <gl\glu.h>                 // GLU Library
#include <stdio.h>                  // Standard IO (sprintf)
#include "..\..\common\wglext.h"    // WGL Extension Header
#include "..\..\common\glext.h"	    // OpenGL Extension Header
#include "..\..\common\gltools.h"   // GLTools library
#include "resource.h"               // Dialog resources


// Initial rendering options specified by the user.
struct STARTUPOPTIONS {
    DEVMODE	devMode;			// Display mode to use
    int	 nPixelFormat;			// Pixel format to use
    int	 nPixelFormatMS;		// Multisampled pixel format
    BOOL bFullScreen;			// Full screen?
    BOOL bFSAA;
    BOOL bVerticalSync;
    };


///////////////////////////////////////////////////////////////////////////////
// Module globals
static HPALETTE hPalette = NULL;                // Palette Handle
static HINSTANCE ghInstance = NULL;             // Module Instance Handle
static LPCTSTR lpszAppName = "SphereWorld32";   // Name of App
static GLint nFontList;                         // Base display list for font
static struct STARTUPOPTIONS startupOptions;    // Startup options info
static LARGE_INTEGER CounterFrequency;
static LARGE_INTEGER FPSCount;
static LARGE_INTEGER CameraTimer;

#define NUM_SPHERES      30         // Number of Spheres
GLTFrame    spheres[NUM_SPHERES];   // Location of spheres
GLTFrame    frameCamera;            // Location and orientation of camera

// Light and material Data
GLfloat fLightPos[4]   = { -100.0f, 100.0f, 50.0f, 1.0f };  // Point source
GLfloat fNoLight[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat fLowLight[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat fBrightLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

// Shadow matrix
GLTMatrix mShadowMatrix;

// Textures identifiers
#define GROUND_TEXTURE  0
#define TORUS_TEXTURE   1
#define SPHERE_TEXTURE  2
#define NUM_TEXTURES    3
GLuint  textureObjects[NUM_TEXTURES];
const char *szTextureFiles[] = {"grass.tga", "wood.tga", "orb.tga"};

// Sphere and torus display lists
GLuint  lTorusList, lSphereList;

///////////////////////////////////////////////////////////////////////////////
// Forward Declarations

// Declaration for Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, 
                                        WPARAM wParam, LPARAM	lParam);

// Startup Dialog Procedure
BOOL APIENTRY StartupDlgProc (HWND hDlg, UINT message, 
                                        UINT wParam, LONG lParam);

// Find the best available pixelformat, including if Multisample is available
void FindBestPF(HDC hDC, int *nRegularFormat, int *nMSFormat);

BOOL ShowStartupOptions(void);          // Initial startup dialog
void ChangeSize(GLsizei w, GLsizei h);  // Change projection and viewport
void RenderScene(void);                 // Draw everything
void SetupRC(HDC hDC);                  // Set up the rendering context
void ShutdownRC(void);                  // Shutdown the rendering context
HPALETTE GetOpenGLPalette(HDC hDC);     // Create a 3-3-2 palette
void DrawInhabitants(GLint nShadow);    // Draw inhabitants of the world
void DrawGround(void);                  // Draw the ground

///////////////////////////////////////////////////////////////////////////////
// Extension function pointers
PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB = NULL;
PFNGLWINDOWPOS2IPROC glWindowPos2i = NULL;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;


//////////////////////////////////////////////////////////////
// Window has changed size. Reset to match window coordiantes
void ChangeSize(GLsizei w, GLsizei h)
	{
    GLfloat fAspect;

    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;

    glViewport(0, 0, w, h);
        
    fAspect = (GLfloat)w / (GLfloat)h;

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
    // Set the clipping volume
    gluPerspective(35.0f, fAspect, 1.0f, 50.0f);
        
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();    
	}


///////////////////////////////////////////////////////////
// Draw the ground as a series of triangle strips
void DrawGround(void)
    {
    GLfloat fExtent = 20.0f;
    GLfloat fStep = 1.0f;
    GLfloat y = -0.4f;
    GLfloat iStrip, iRun;
    GLfloat s = 0.0f;
    GLfloat t = 0.0f;
    GLfloat texStep = 1.0f / (fExtent * .075f);
    
    // Ground is a tiling texture
    glBindTexture(GL_TEXTURE_2D, textureObjects[GROUND_TEXTURE]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Lay out strips and repeat textures coordinates
    for(iStrip = -fExtent; iStrip <= fExtent; iStrip += fStep)
        {
        t = 0.0f;
        glBegin(GL_TRIANGLE_STRIP);

            for(iRun = fExtent; iRun >= -fExtent; iRun -= fStep)
                {
                glTexCoord2f(s, t);
                glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up
                glVertex3f(iStrip, y, iRun);
                
                glTexCoord2f(s + texStep, t);
                glNormal3f(0.0f, 1.0f, 0.0f);   // All Point up
                glVertex3f(iStrip + fStep, y, iRun);
                
                t += texStep;
                }
        glEnd();
        s += texStep;
        }
    }

///////////////////////////////////////////////////////////////////////
// Draw random inhabitants and the rotating torus/sphere duo
void DrawInhabitants(GLint nShadow)
    {
    static GLfloat yRot = 0.0f;         // Rotation angle for animation
    GLint i;

    if(nShadow == 0)
        {
        yRot += 0.5f;
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        }
    else
        glColor4f(0.0f, 0.0f, .0f, .75f);  // Shadow color
  
        
    // Draw the randomly located spheres
    glBindTexture(GL_TEXTURE_2D, textureObjects[SPHERE_TEXTURE]);
    for(i = 0; i < NUM_SPHERES; i++)
        {
        glPushMatrix();
        gltApplyActorTransform(&spheres[i]);
        glCallList(lSphereList);
        glPopMatrix();
        }

    glPushMatrix();
        glTranslatef(0.0f, 0.1f, -2.5f);
    
        glPushMatrix();
            glRotatef(-yRot * 2.0f, 0.0f, 1.0f, 0.0f);
            glTranslatef(1.0f, 0.0f, 0.0f);
            glCallList(lSphereList);
        glPopMatrix();
    
        if(nShadow == 0)
            {
            // Torus alone will be specular
            glMaterialfv(GL_FRONT, GL_SPECULAR, fBrightLight);
            }
        
        glRotatef(yRot, 0.0f, 1.0f, 0.0f);
        glBindTexture(GL_TEXTURE_2D, textureObjects[TORUS_TEXTURE]);
        glCallList(lTorusList);
        glMaterialfv(GL_FRONT, GL_SPECULAR, fNoLight);
    glPopMatrix();
    }


//////////////////////////////////////////////////
// Draw everything
void RenderScene(void)
	{
    static int iFrames = 0;   // Count frames to calculate fps ever 100 frames
    static float fps = 0.0f;  // Calculated fps

    // Clear the window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
    glPushMatrix();
        gltApplyCameraTransform(&frameCamera);  // Move camera/world
        
        // Position light before any other transformations
        glLightfv(GL_LIGHT0, GL_POSITION, fLightPos);
        
        // Draw the ground
        glColor3f(1.0f, 1.0f, 1.0f);
        DrawGround();
        
        // Draw shadows first
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_STENCIL_TEST);
        glPushMatrix();
            glMultMatrixf(mShadowMatrix);
            DrawInhabitants(1);
        glPopMatrix();
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        
        // Draw inhabitants normally
        DrawInhabitants(0);
    glPopMatrix();


    // Calculate Frame Rate, once every 100 frames
    iFrames++;
    if(iFrames == 100)
        {
        float fTime;

		// Get the current count
		LARGE_INTEGER lCurrent;
		QueryPerformanceCounter(&lCurrent);

		fTime = (float)(lCurrent.QuadPart - FPSCount.QuadPart) /
										(float)CounterFrequency.QuadPart;
        fps = (float)iFrames / fTime;
      

        // Reset frame count and timer
        iFrames = 0;
        QueryPerformanceCounter(&FPSCount);
        }

	// If we have the window position extension, display
    // the frame rate, and tell if multisampling was enabled
    // and if the VSync is turned on.
    if(glWindowPos2i != NULL)
        {
        int iRow = 10;
        char cBuffer[64];

        // Turn off depth test, lighting, and texture mapping
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
       	glColor3f(1.0f, 1.0f, 1.0f);


        // Set position and display message
        glWindowPos2i(0, iRow);
    	glListBase(nFontList);
	    glCallLists (13, GL_UNSIGNED_BYTE, "OpenGL Rocks!"); 	
        iRow+= 20;

        // Display the frame rate
        sprintf(cBuffer,"FPS: %.1f", fps);
        glWindowPos2i(0, iRow);
        glCallLists(strlen(cBuffer), GL_UNSIGNED_BYTE, cBuffer);
        iRow += 20;

        // MultiSampled?
        if(startupOptions.bFSAA == TRUE && startupOptions.nPixelFormatMS != 0)
            {
            glWindowPos2i(0, iRow);
            glCallLists(25 ,GL_UNSIGNED_BYTE,"Multisampled Frame Buffer");
            iRow += 20;
            }

        // VSync?
        if(wglSwapIntervalEXT != NULL && startupOptions.bVerticalSync == TRUE)
            {
            glWindowPos2i(0, iRow);
            glCallLists(9 ,GL_UNSIGNED_BYTE, "VSync On");
            iRow += 20;
            }


        // Put everything back
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        }
	}

///////////////////////////////////////////////////////////////////////////////
// Setup. Create font/bitmaps, load textures, create display lists
void SetupRC(HDC hDC)
	{
    GLTVector3 vPoints[3] = {{ 0.0f, -0.4f, 0.0f },
                             { 10.0f, -0.4f, 0.0f },
                             { 5.0f, -0.4f, -5.0f }};
    int iSphere;
    int i;

	// Setup the Font characteristics
	HFONT hFont;
	LOGFONT logfont;

	logfont.lfHeight = -20;
	logfont.lfWidth = 0;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfWeight = FW_BOLD;
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logfont.lfFaceName,"Arial");

	// Create the font and display list
	hFont = CreateFontIndirect(&logfont);
	SelectObject (hDC, hFont); 

	
	//Create display lists for glyphs 0 through 128
	nFontList = glGenLists(128);
	wglUseFontBitmaps(hDC, 0, 128, nFontList);

	DeleteObject(hFont);		// Don't need original font anymore
   
    // Grayish background
    glClearColor(fLowLight[0], fLowLight[1], fLowLight[2], fLowLight[3]);
   
    // Clear stencil buffer with zero, increment by one whenever anybody
    // draws into it. When stencil function is enabled, only write where
    // stencil value is zero. This prevents the transparent shadow from drawing
    // over itself
    glStencilOp(GL_INCR, GL_INCR, GL_INCR);
    glClearStencil(0);
    glStencilFunc(GL_EQUAL, 0x0, 0x01);
    
    // Cull backs of polygons
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    
    // Setup light parameters
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fNoLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, fLowLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, fBrightLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, fBrightLight);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Calculate shadow matrix
    gltMakeShadowMatrix(vPoints, fLightPos, mShadowMatrix);
    
    // Mostly use material tracking
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glMateriali(GL_FRONT, GL_SHININESS, 128);
    
    gltInitFrame(&frameCamera);  // Initialize the camera
    
    // Randomly place the sphere inhabitants
    for(iSphere = 0; iSphere < NUM_SPHERES; iSphere++)
        {
        gltInitFrame(&spheres[iSphere]);    // Initialize the frame
        
        // Pick a random location between -20 and 20 at .1 increments
        spheres[iSphere].vLocation[0] = (float)((rand() % 400) - 200) * 0.1f;
        spheres[iSphere].vLocation[1] = 0.0f; 
        spheres[iSphere].vLocation[2] = (float)((rand() % 400) - 200) * 0.1f;
        }
        
    // Set up texture maps
    glEnable(GL_TEXTURE_2D);
    glGenTextures(NUM_TEXTURES, textureObjects);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    // Load teach texture    
    for(i = 0; i < NUM_TEXTURES; i++)
        {
        GLubyte *pBytes;
        GLint iWidth, iHeight, iComponents;
        GLenum eFormat;
        
        glBindTexture(GL_TEXTURE_2D, textureObjects[i]);
        
        // Load this texture map
        pBytes = gltLoadTGA(szTextureFiles[i], &iWidth, &iHeight, &iComponents, &eFormat);
        gluBuild2DMipmaps(GL_TEXTURE_2D, iComponents, iWidth, iHeight, eFormat, GL_UNSIGNED_BYTE, pBytes);
        free(pBytes);
        
        // Trilinear mipmapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

    // Get window position function pointer if it exists
    glWindowPos2i = (PFNGLWINDOWPOS2IPROC)wglGetProcAddress("glWindowPos2i");

    // Get swap interval function pointer if it exists
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    if(wglSwapIntervalEXT != NULL && startupOptions.bVerticalSync == TRUE)
        wglSwapIntervalEXT(1);

    // If multisampling was available and was selected, enable
    if(startupOptions.bFSAA == TRUE && startupOptions.nPixelFormatMS != 0)
        glEnable(GL_MULTISAMPLE_ARB);

    // If sepearate specular color is available, make torus shiney
    if(gltIsExtSupported("GL_EXT_separate_specular_color"))
       glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);


    // Initialize the timers
	QueryPerformanceFrequency(&CounterFrequency);
	QueryPerformanceCounter(&FPSCount);
    CameraTimer = FPSCount;

    // Build display lists for the torus and spheres
    // (You could do one for the ground as well)
    lTorusList = glGenLists(2);
    lSphereList = lTorusList + 1;

    glNewList(lTorusList, GL_COMPILE);
        gltDrawTorus(0.35f, 0.15f, 61, 37);
    glEndList();

    glNewList(lSphereList, GL_COMPILE);
        gltDrawSphere(0.3f, 31, 16);
    glEndList();
	}


///////////////////////////////////////////////////////////////////////////////
// Shutdown the rendering context
void ShutdownRC(void)
    {
    glDeleteLists(nFontList, 128);  // Delete font display list
    glDeleteLists(lTorusList, 2);   // Delete object display lists
    glDeleteTextures(NUM_TEXTURES, textureObjects); // Release textures
    }

///////////////////////////////////////////////////////////////////////
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




///////////////////////////////////////////////////////////////////
// Entry point of all Windows programs
int APIENTRY WinMain(	HINSTANCE 	hInstance,
						HINSTANCE 	hPrevInstance,
						LPSTR 		lpCmdLine,
						int			nCmdShow)
	{
	MSG			msg;		// Windows message structure
	WNDCLASS	wc;			// Windows class structure
	HWND		hWnd;		// Storeage for window handle
	UINT uiStyle,uiStyleX;

    ghInstance = hInstance; // Save instance handle

    // Get startup options, or shutdown
    if(ShowStartupOptions() == FALSE)
        return 0;


	if(startupOptions.bFullScreen == TRUE)
		if(ChangeDisplaySettings(&startupOptions.devMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
		// Replace with string resource, and actual width and height
		MessageBox(NULL, TEXT("Cannot change to selected desktop resolution."),
							  NULL, MB_OK | MB_ICONSTOP);
		return -1;
		}

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


	// Select window styles
	if(startupOptions.bFullScreen == TRUE)
		{
		uiStyle = WS_POPUP;
		uiStyleX = WS_EX_TOPMOST;
		}
	else
		{
		uiStyle = WS_OVERLAPPEDWINDOW;
		uiStyleX = 0;
		}

	// Create the main 3D window
	hWnd = CreateWindowEx(uiStyleX, wc.lpszClassName, lpszAppName, uiStyle,
      0, 0, startupOptions.devMode.dmPelsWidth, startupOptions.devMode.dmPelsHeight, NULL, NULL, hInstance, NULL);


	// If window was not created, quit
	if(hWnd == NULL)
		return FALSE;

	// Make sure window manager stays hidden
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);

	// Process application messages until the application closes
	while( GetMessage(&msg, NULL, 0, 0))
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}


	// Restore Display Settings
	if(startupOptions.bFullScreen == TRUE)
		ChangeDisplaySettings(NULL, 0);

	return msg.wParam;
	}


/////////////////////////////////////////////////////////////////
// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND hWnd,	UINT message, WPARAM wParam, LPARAM lParam)
    {
    static HGLRC hRC;		// Permenant Rendering context
    static HDC hDC;			// Private GDI Device context

    switch (message)
        {
		// Window creation, setup for OpenGL
		case WM_CREATE:
			// Store the device context
			hDC = GetDC(hWnd);		

            // The screen and desktop may have changed, so do this again
			FindBestPF(hDC, &startupOptions.nPixelFormat, &startupOptions.nPixelFormatMS);

			// Set pixelformat
			if(startupOptions.bFSAA == TRUE && (startupOptions.nPixelFormatMS != 0))
				SetPixelFormat(hDC, startupOptions.nPixelFormatMS, NULL);
			else
				SetPixelFormat(hDC, startupOptions.nPixelFormat, NULL);

			// Create the rendering context and make it current
			hRC = wglCreateContext(hDC);
			wglMakeCurrent(hDC, hRC);

			// Create the palette
			hPalette = GetOpenGLPalette(hDC);

			SetupRC(hDC);

			break;

        // Check for ESC key
        case WM_CHAR:
            if(wParam == 27)
                DestroyWindow(hWnd);
            break;

		// Window is either full screen, or not visible
		case WM_ACTIVATE:
			{
            // Ignore this altogether unless we are in full screen mode
            if(startupOptions.bFullScreen == TRUE)
                {
                // Construct windowplacement structure
			    WINDOWPLACEMENT wndPlacement;
			    wndPlacement.length = sizeof(WINDOWPLACEMENT);
			    wndPlacement.flags = WPF_RESTORETOMAXIMIZED;
			    wndPlacement.ptMaxPosition.x = 0;
			    wndPlacement.ptMaxPosition.y = 0;
			    wndPlacement.ptMinPosition.x = 0;
			    wndPlacement.ptMinPosition.y = 0;
			    wndPlacement.rcNormalPosition.bottom = startupOptions.devMode.dmPelsHeight;
			    wndPlacement.rcNormalPosition.left = 0;
			    wndPlacement.rcNormalPosition.top = 0;
			    wndPlacement.rcNormalPosition.right = startupOptions.devMode.dmPelsWidth;

                // Switching away from window
			    if(LOWORD(wParam) == WA_INACTIVE)
				    {
				    wndPlacement.showCmd = SW_SHOWMINNOACTIVE;
				    SetWindowPlacement(hWnd, &wndPlacement);
				    ShowCursor(TRUE);
				    }
			    else    // Switching back to window
				    {
				    wndPlacement.showCmd = SW_RESTORE;
				    SetWindowPlacement(hWnd, &wndPlacement);
				    ShowCursor(FALSE);
				    }
			    }
            }
			break;


		// Window is being destroyed, cleanup
		case WM_DESTROY:
			// Kill the timer that we created
			KillTimer(hWnd,101);
    
            ShutdownRC();

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

		// The painting function.  This message sent by Windows 
		// whenever the screen needs updating.
		case WM_PAINT:
			{
            // Only poll keyboard when this window has focus
            if(GetFocus() == hWnd)
                {
                float fTime;
                float fLinear, fAngular;

        		// Get the time since the last time we rendered a frame
		        LARGE_INTEGER lCurrent;
		        QueryPerformanceCounter(&lCurrent);

		        fTime = (float)(lCurrent.QuadPart - CameraTimer.QuadPart) /
										(float)CounterFrequency.QuadPart;

                CameraTimer = lCurrent;

                // Camera motion will be time based. This keeps the motion constant
                // regardless of frame rate. Higher frame rates produce smoother
                // animation and motion, they should not produce "faster" motion.
                fLinear = fTime * 1.0f;
                fAngular = (float)gltDegToRad(60.0f * fTime);

                // Move the camera around, poll the keyboard
                if(GetAsyncKeyState(VK_UP))
                    gltMoveFrameForward(&frameCamera, fLinear);

                if(GetAsyncKeyState(VK_DOWN))
                    gltMoveFrameForward(&frameCamera, -fLinear);

                if(GetAsyncKeyState(VK_LEFT))
                    gltRotateFrameLocalY(&frameCamera, fAngular);
      
                if(GetAsyncKeyState(VK_RIGHT))
                    gltRotateFrameLocalY(&frameCamera, -fAngular);
                }

			// Call OpenGL drawing code
			RenderScene();

			// Call function to swap the buffers
			SwapBuffers(hDC);

            // Not validated on purpose, gives and endless series
            // of paint messages... this is akin to having
            // a rendering loop
            //ValidateRect(hWnd,NULL);
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



/////////////////////////////////////////////////////////////////////////////
// Dialog procedure for the startup dialog
BOOL APIENTRY StartupDlgProc (HWND hDlg, UINT message, UINT wParam, LONG lParam)
	{
    switch (message)
    	{
		// Initialize the dialog box
	    case WM_INITDIALOG:
			{
            int nPF;
            HDC hDC;                        // Dialogs device context
            HGLRC hRC;
            DEVMODE devMode;
			unsigned int iMode;
			unsigned int nWidth;	// Current settings
			unsigned int nHeight;
			char cBuffer[64];
			HWND hListBox;
			
			PIXELFORMATDESCRIPTOR pfd = {   // Not going to be too picky
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,		// Full color
			32,					// Color depth
			0,0,0,0,0,0,0,		// Ignored
			0,0,0,0,		    // Accumulation buffer
			16,					// Depth bits
			8,					// Stencil bits
			0,0,0,0,0,0 };		// Some used, some not


            // Initialize render options
			startupOptions.bFSAA = FALSE;
			startupOptions.bFullScreen = FALSE;
			startupOptions.bVerticalSync = FALSE;

			// Create a "temporary" OpenGL rendering context
			hDC = GetDC(hDlg);
		
			// Set pixel format one time....
			nPF = ChoosePixelFormat(hDC, &pfd);
			SetPixelFormat(hDC, nPF, &pfd);
			DescribePixelFormat(hDC, nPF, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

			// Create the GL context
			hRC = wglCreateContext(hDC);
			wglMakeCurrent(hDC, hRC);

			// Set text in dialog
			SetDlgItemText(hDlg, IDC_VENDOR, (const char *)glGetString(GL_VENDOR));
			SetDlgItemText(hDlg, IDC_RENDERER, (const char *)glGetString(GL_RENDERER));
			SetDlgItemText(hDlg, IDC_VERSION, (const char *)glGetString(GL_VERSION));
			
			// Vertical Sync off by default
			if(gltIsExtSupported("WGL_EXT_swap_control"))
				EnableWindow(GetDlgItem(hDlg, IDC_VSYNC_CHECK), TRUE);

			// Find a multisampled and non-multisampled pixel format
			FindBestPF(hDC, &startupOptions.nPixelFormat, &startupOptions.nPixelFormatMS);

			// Done with GL context
			wglMakeCurrent(hDC, NULL);
			wglDeleteContext(hRC);

			// Enumerate display modes
			iMode = 0;
			nWidth = GetSystemMetrics(SM_CXSCREEN);	// Current settings
			nHeight = GetSystemMetrics(SM_CYSCREEN);
			hListBox = GetDlgItem(hDlg, IDC_DISPLAY_COMBO);
			while(EnumDisplaySettings(NULL, iMode, &devMode))
                {
				//if(devMode.dmBitsPerPel == pfd.cColorBits)
				    {
                    int iItem;
					sprintf(cBuffer,"%d x %d x %dbpp @%dhz", devMode.dmPelsWidth,
						devMode.dmPelsHeight, devMode.dmBitsPerPel, devMode.dmDisplayFrequency);

					iItem = SendMessage(hListBox, CB_ADDSTRING, 0, (LPARAM)cBuffer);
					SendMessage(hListBox, CB_SETITEMDATA, iItem, iMode);

					if(devMode.dmPelsHeight == nHeight &&
						devMode.dmPelsWidth == nWidth)
						SendMessage(hListBox, CB_SETCURSEL, iItem, 0);
				    }
				iMode++;
                }
			

			// Set other defaults /////////////
			// Windowed or full screen
			CheckDlgButton(hDlg, IDC_FS_CHECK, BST_CHECKED);	

			// FSAA, but only if support detected
			if(startupOptions.nPixelFormatMS != 0)
				EnableWindow(GetDlgItem(hDlg, IDC_MULTISAMPLED_CHECK), TRUE);
				
			return (TRUE);
			}
			break;

		// Process command messages
	    case WM_COMMAND:      
			{
			// Validate and Make the changes
			if(LOWORD(wParam) == IDOK)
			    {
				// Read options ////////////////////////////////////////
				// Display mode
				HWND hListBox = GetDlgItem(hDlg, IDC_DISPLAY_COMBO);
				int iMode = SendMessage(hListBox, CB_GETCURSEL, 0, 0);
				iMode = SendMessage(hListBox, CB_GETITEMDATA, iMode, 0);
				EnumDisplaySettings(NULL, iMode, &startupOptions.devMode);
			
				// Full screen or windowed?
				if(IsDlgButtonChecked(hDlg, IDC_FS_CHECK))
					startupOptions.bFullScreen = TRUE;
				else
					startupOptions.bFullScreen = FALSE;
			
				
				// FSAA
				if(IsDlgButtonChecked(hDlg, IDC_MULTISAMPLED_CHECK))
					startupOptions.bFSAA = TRUE;
				else
					startupOptions.bFSAA = FALSE;

				// Vertical Sync.
				if(IsDlgButtonChecked(hDlg, IDC_VSYNC_CHECK))
					startupOptions.bVerticalSync = TRUE;
				else
					startupOptions.bVerticalSync = FALSE;


				EndDialog(hDlg,TRUE);
                }

			if(LOWORD(wParam) == IDCANCEL)
				EndDialog(hDlg, FALSE);
			}
			break;

		// Closed from sysbox
		case WM_CLOSE:
			EndDialog(hDlg,FALSE); // Same as cancel
			break;
		}

	return FALSE;
	}


///////////////////////////////////////////////////////////////////////////////
// Display the startup screen (just a modal dialog box)
BOOL ShowStartupOptions(void)
    {
	return DialogBox (ghInstance,
		MAKEINTRESOURCE(IDD_DLG_INTRO),
		NULL,
		StartupDlgProc);
    }


///////////////////////////////////////////////////////////////////////////////
// Select pixelformat with desired attributes
// Returns the best available "regular" pixel format, and the best available
// Multisampled pixelformat (0 if not available)
void FindBestPF(HDC hDC, int *nRegularFormat, int *nMSFormat)
    {
    *nRegularFormat = 0;
    *nMSFormat = 0;

    // easy check, just look for the entrypoint
    if(gltIsWGLExtSupported(hDC, "WGL_ARB_pixel_format"))
        if(wglGetPixelFormatAttribivARB == NULL)
            wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)
                                 wglGetProcAddress("wglGetPixelFormatAttribivARB");

    // First try to use new extended wgl way
    if(wglGetPixelFormatAttribivARB != NULL)
        {
        // Only care about these attributes
        int nBestMS = 0;
        int i;
        int iResults[9];
        int iAttributes [9] = {    WGL_SUPPORT_OPENGL_ARB, // 0
                                   WGL_ACCELERATION_ARB,   // 1
                                   WGL_DRAW_TO_WINDOW_ARB, // 2
                                   WGL_DOUBLE_BUFFER_ARB,  // 3
                                   WGL_PIXEL_TYPE_ARB,     // 4
                                   WGL_DEPTH_BITS_ARB,     // 5
                                   WGL_STENCIL_BITS_ARB,   // 6
                                   WGL_SAMPLE_BUFFERS_ARB, // 7
                                   WGL_SAMPLES_ARB };      // 8

        // How many pixelformats are there?
        int nFormatCount[] = { 0 };
        int attrib[] = { WGL_NUMBER_PIXEL_FORMATS_ARB };
        wglGetPixelFormatAttribivARB(hDC, 1, 0, 1, attrib, nFormatCount);

        // Loop through all the formats and look at each one
        for(i = 0; i < nFormatCount[0]; i++)
            {
            // Query pixel format
            wglGetPixelFormatAttribivARB(hDC, i+1, 0, 9, iAttributes, iResults);

            // Match? Must support OpenGL AND be Accelerated AND draw to Window 
            if(iResults[0] == 1 && iResults[1] == WGL_FULL_ACCELERATION_ARB && iResults[2] == 1)
            if(iResults[3] == 1)                    // Double buffered
            if(iResults[4] == WGL_TYPE_RGBA_ARB)    // Full Color
            if(iResults[5] >= 16)                   // Any Depth greater than 16
            if(iResults[6] > 0)                     // Any Stencil depth (not zero)
                {
                // We have a candidate, look for most samples if multisampled
                if(iResults[7] == 1)	            // Multisampled
                    {
                    if(iResults[8] > nBestMS)       // Look for most samples
                        {
                        *nMSFormat = i;			// Multisamples	
                        nBestMS = iResults[8];	// Looking for the best
                        }
                    }
                else // Not multisampled
                    {
                    // Good enough for "regular". This will fall through
                    *nRegularFormat = i;
                    }
                }
            }
        }
    else
        {
        // Old fashioned way... 
        // or multisample
        PIXELFORMATDESCRIPTOR pfd = { 
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,      // Full color
        32,                 // Color depth
        0,0,0,0,0,0,0,      // Ignored
        0,0,0,0,            // Accumulation buffer
        24,                 // Depth bits
        8,                  // Stencil bits
        0,0,0,0,0,0 };      // Some used, some not

        *nRegularFormat = ChoosePixelFormat(hDC, &pfd);
        }
    }
