// GLWindow.cpp : implementation file
//

#include "stdafx.h"
#include "GLView.h"
#include "GLWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



BYTE* gltResourceBMPBits(UINT nResource, int *nWidth, int *nHeight)
	{
	HINSTANCE hInstance;	// Instance Handle
	HANDLE hBitmap;			// Handle to bitmap resource
	BITMAPINFO bmInfo;


	// Find the bitmap resource
	hInstance = GetModuleHandle(NULL);
	hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(nResource));

	if(hBitmap == NULL)
		return NULL;

	GetObject(hBitmap,sizeof(BITMAPINFO),&bmInfo);
	DeleteObject(hBitmap);

	hBitmap = ::LoadResource(hInstance,
		 ::FindResource(hInstance,MAKEINTRESOURCE(nResource), RT_BITMAP));

	if(hBitmap == NULL)
		return NULL;

	BYTE *pData = (BYTE *)LockResource(hBitmap);
	pData += sizeof(BITMAPINFO)-1;

	*nWidth = bmInfo.bmiHeader.biWidth; //bm.bmWidth;
	*nHeight = bmInfo.bmiHeader.biHeight;//bm.bmHeight;

	return pData;
	}

/////////////////////////////////////////////////////////////////////////////
// CGLWindow

CGLWindow::CGLWindow()
	{
	m_hRC = NULL;
	m_hDC = NULL;
	m_nPixelFormat;
	}

CGLWindow::~CGLWindow()
	{
	}


BEGIN_MESSAGE_MAP(CGLWindow, CWnd)
	//{{AFX_MSG_MAP(CGLWindow)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGLWindow message handlers

void CGLWindow::OnPaint() 
	{
	static float fX = 0.0f;
	static float fY = 0.0f;

	if(m_hRC != NULL)
		{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//fX += 0.015f;
		//fY += 0.01f;
		fX += 1.0f;
		fY += 1.0f;

		if(fX > 360.0f)
			fX = 0.015f;

		if(fY > 360.0f)
			fY = 0.01f;

		glPushMatrix();
		glRotatef(fX, 1.0f, 0.0f, 0.0f);
		glRotatef(fY, 0.0f, 1.0f, 0.0f);

		float fSize = 20.0f;

		// Front face of Cube
		glBindTexture(GL_TEXTURE_2D, m_Sides[0]);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-fSize, fSize, fSize);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-fSize, -fSize, fSize);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(fSize,-fSize, fSize);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(fSize,fSize, fSize);
		glEnd();

		// Back face of Cube
		glBindTexture(GL_TEXTURE_2D,  m_Sides[1]);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(fSize,fSize, -fSize);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(fSize,-fSize, -fSize);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-fSize, -fSize, -fSize);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-fSize, fSize, -fSize);
		glEnd();

		// Top Face of Cube
		glBindTexture(GL_TEXTURE_2D,  m_Sides[2]);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-fSize, fSize, fSize);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(fSize, fSize, fSize);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(fSize, fSize, -fSize);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-fSize, fSize, -fSize);
		glEnd();


		// Bottom Face of Cube
		glBindTexture(GL_TEXTURE_2D,  m_Sides[3]);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-fSize, -fSize, -fSize);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(fSize, -fSize, -fSize);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(fSize, -fSize, fSize);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-fSize, -fSize, fSize);
		glEnd();


		// Left hand side of cube
		glBindTexture(GL_TEXTURE_2D,  m_Sides[4]);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-fSize, fSize, -fSize);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-fSize, -fSize, -fSize);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-fSize, -fSize, fSize);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-fSize, fSize, fSize);
		glEnd();


		// Right hand side of cube
		glBindTexture(GL_TEXTURE_2D,  m_Sides[5]);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(fSize, fSize, fSize);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(fSize, -fSize, fSize);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(fSize, -fSize, -fSize);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(fSize, fSize, -fSize);
		glEnd();

		glPopMatrix();

		GLint eBuffer;
		glGetIntegerv(GL_DRAW_BUFFER, &eBuffer);

		if(eBuffer == GL_FRONT)
			glFinish();
		else
			SwapBuffers(m_hDC);
		}
	else
		{
		CPaintDC dc(this); // device context for painting

		RECT rect;
		GetClientRect(&rect);
		HDC m_hMemDC;
		HBITMAP m_hBitmap, m_hOldBitmap;

		m_hMemDC = CreateCompatibleDC(m_hDC);
	
		// Load the bitmap
		m_hBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BM_NOGL));
		m_hOldBitmap = (HBITMAP) SelectObject(m_hMemDC, m_hBitmap);

		// Do the blit for the background
		StretchBlt(m_hDC, 0,0, rect.right, rect.bottom, m_hMemDC, 0, 0, 200, 200,SRCCOPY);

		SelectObject(m_hMemDC, m_hOldBitmap);
		DeleteObject(m_hBitmap);
		::DeleteDC(m_hMemDC);
		}

	ValidateRect(NULL);
	}

int CGLWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Get the device context
	m_hDC = ::GetDC(this->m_hWnd);
	
	// Set the Pixel Format
	SetPixelFormat(m_hDC, m_nPixelFormat, NULL);

	// Create the rendering context and make it current
	m_hRC = wglCreateContext(m_hDC);

	if(m_hRC != NULL)
		wglMakeCurrent(m_hDC, m_hRC);

	// Setup the context
	if(m_hRC != NULL)
		{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glGenTextures(6, m_Sides);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);

	    BYTE *pBytes;
	    int nWidth, nHeight;
	
	    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);

		// Load the texture objects
		pBytes = gltResourceBMPBits(IDB_BITMAP1, &nWidth, &nHeight);
		glBindTexture(GL_TEXTURE_2D, m_Sides[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,nWidth, nHeight, 0,
			GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

		pBytes = gltResourceBMPBits(IDB_BITMAP2, &nWidth, &nHeight);
		glBindTexture(GL_TEXTURE_2D, m_Sides[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,nWidth, nHeight, 0,
			GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

		pBytes = gltResourceBMPBits(IDB_BITMAP3, &nWidth, &nHeight);
		glBindTexture(GL_TEXTURE_2D, m_Sides[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,nWidth, nHeight, 0,
			GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

		pBytes = gltResourceBMPBits(IDB_BITMAP4, &nWidth, &nHeight);
		glBindTexture(GL_TEXTURE_2D, m_Sides[3]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,nWidth, nHeight, 0,
			GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

		pBytes = gltResourceBMPBits(IDB_BITMAP5, &nWidth, &nHeight);
		glBindTexture(GL_TEXTURE_2D, m_Sides[4]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,nWidth, nHeight, 0,
			GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

		pBytes = gltResourceBMPBits(IDB_BITMAP6, &nWidth, &nHeight);
		glBindTexture(GL_TEXTURE_2D, m_Sides[5]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,nWidth, nHeight, 0,
			GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

		}
		
	SetTimer(101, 30, NULL);

	return 0;
	}

void CGLWindow::OnDestroy() 
	{
	KillTimer(101);

	CWnd::OnDestroy();

	if(m_hRC)
		{	
		glDeleteTextures(6, m_Sides);


		wglMakeCurrent(m_hDC, NULL);
		wglDeleteContext(m_hRC);
		m_hRC = NULL;
		}

	::ReleaseDC(this->m_hWnd, m_hDC);
	m_hDC = NULL;
	}

void CGLWindow::OnSize(UINT nType, int w, int h) 
	{
	CWnd::OnSize(nType, w, h);
	
	GLfloat nRange = 100.0f;
	GLfloat fAspect;

	// Prevent a divide by zero
	if(h == 0)
		h = 1;

	fAspect = (GLfloat)w/(GLfloat)h;

	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);

	// Reset coordinate system
	glLoadIdentity();

	// Setup perspective for viewing
	gluPerspective(17.5f,fAspect,60,300);

	// Viewing transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -250.0f);
	}

void CGLWindow::OnTimer(UINT nIDEvent) 
{
	InvalidateRect(NULL);	
	CWnd::OnTimer(nIDEvent);
}
