#include <stdio.h>
#include <time.h>
#include <windows.h>

#include <gl\gl.h>
#include <gl\glu.h>

#include "render.h"
#include "mesh.h"



LRESULT CALLBACK WndProc(HWND, UINT ,WPARAM ,LPARAM);


float X=0.f,Y=0.f,Z=0.f; //  camera coordinates

int iMx,iMy;  // mouse position - camera rotation angles

int iWwidth,iWheight; // window width and height

float fDt=0.000001f; // [s] single simulation step ( on begining)

GLvoid BuildFont(HDC hDC)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping

	base = glGenLists(96);								// Storage For 96 Characters

	font = CreateFont(	-14,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_NORMAL,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Courier New");					// Font Name

	oldfont = (HFONT)SelectObject(hDC, font);           // Selects The Font We Want
	wglUseFontBitmaps(hDC, 32, 96, base);				// Builds 96 Characters Starting At Character 32
	SelectObject(hDC, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font
}

GLvoid KillFont(GLvoid)									// Delete The Font List
{
	glDeleteLists(base, 96);							// Delete All 96 Characters
}

void SetDCPixelFormat(HDC hDC)
    {
    int nPixelFormat;

    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,                              // structure version
        PFD_DRAW_TO_WINDOW |            // Draw to window ( not to bitmap)
        PFD_SUPPORT_OPENGL |            // ogl support in this window
        PFD_DOUBLEBUFFER,               // double buffering mode
        PFD_TYPE_RGBA,                  // RGBA color mode
        8,                              // 8-bit per color
        0,0,0,0,0,0,                    // not used
        0,0,                            // not used
        0,0,0,0,0,                      // not used
        16,                             // 16-bit per depth buffer
        0,                              // not used
        0,                              // not used
        PFD_MAIN_PLANE,                 // drawing on main plane
        0,                              // not used
        0,0,0 };                        // not used

    // choose the best ( the most similar) pixel format
    nPixelFormat = ChoosePixelFormat(hDC, &pfd);

    // set pixel format for device
    SetPixelFormat(hDC, nPixelFormat, &pfd);
    }


int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance ,
				   PSTR szCmdLine,int iCmdShow)
{

	static char szAppName[]="Physic Application";
	HWND	hwnd;
	MSG	msg;
	WNDCLASSEX wndclass;


    if (!hPrevInstance)
    {
		wndclass.cbSize		=sizeof(wndclass);
		wndclass.style		=CS_HREDRAW|CS_VREDRAW;
		wndclass.lpfnWndProc=WndProc;
		wndclass.cbClsExtra	=0;
		wndclass.cbWndExtra	=0;
		wndclass.hInstance	=hInstance;
		wndclass.hIcon		=LoadIcon(NULL,szAppName);
		wndclass.hCursor		=LoadCursor(NULL,IDC_ARROW);
		wndclass.hbrBackground = NULL;
		wndclass.lpszMenuName =0;
		wndclass.lpszClassName=szAppName;
		wndclass.hIconSm		  =LoadIcon(NULL,IDI_APPLICATION);

		RegisterClassEx(&wndclass);
    }


        hwnd	= CreateWindow(szAppName,
		"OpenGL",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		NULL,
		NULL,
		hInstance,
		NULL);

    ShowWindow(hwnd,iCmdShow);
    UpdateWindow(hwnd);

	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}



LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	static HDC	hDC; // device context handler
	static HGLRC hRC; // render context handler

	static long time_actual=0;
    static long time_old=0;
	static long counter=0;  // frame counter per one time check

	switch(iMsg)
	{
	case WM_CREATE:
		hDC = GetDC(hwnd); // get device context
		SetDCPixelFormat(hDC);  //set pixel format for this device context

		hRC = wglCreateContext(hDC); // create render context for this device
		wglMakeCurrent(hDC, hRC);

		initGL();  // init render state
    	BuildFont(hDC);
		return 0;

	case WM_SIZE:
		resizeGLScene(LOWORD(lParam), HIWORD(lParam));
                iWwidth=LOWORD(lParam);
                iWheight=HIWORD(lParam);
		return 0;

	case WM_PAINT:
   {    // draw screen

	if (counter++>100)
	{
	    char text[200];
	    float fFps; // frame per secound
	    time_old=time_actual;
	    time_actual=clock();
	    fFps=(counter*CLOCKS_PER_SEC)/(float)(time_actual-time_old);
	    if (fFps!=0) fDt=1.0f/fFps; else fDt=1;

        if (fDt>0.03f) fDt=0.03f; // if simulation step is too long -  do not crash simulation protector

	    sprintf(text," %0.0f fps  %0.2fms  Example of Quasi Fluid Dynamic - Solid Object ",fFps,1000.0/fFps); // prepare info with fps and draw+sim time
	    counter=0;
		SetWindowText(hwnd,text); // put info to window title bar
	}

		drawGLScene();  // drawing OGL sceen
		SwapBuffers(hDC);



        float fdt=fDt/5.0f; // calc 5 simulations per one frame rendreing

        for (int i=0;i<5;i++)
        {
            Duck->CalcPressure();
            Duck->Sim(fdt);
        }

		return 0;
    }



	case WM_MOUSEMOVE:
		{
			iMx=LOWORD(lParam);
			iMy=HIWORD(lParam);
			return 0;
		}

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;


    // CAMERA move
        case VK_UP:
	  Y+=0.5;
			return 0;
        case VK_DOWN:
	  Y-=0.5;
			return 0;

        case VK_LEFT:
	  X-=0.5;
			return 0;
        case VK_RIGHT:
	  X+=0.5;
			return 0;

        case VK_PRIOR:
	  Z-=0.5;
			return 0;
        case VK_NEXT:
	  Z+=0.5;
 			return 0;




        // set FORCE control parameter
        case 'Q':
            fControlParameter[0]+=0.05f;
            if (fControlParameter[0]>1.0) fControlParameter[0]=1.0;
        return 0;

        case 'A':
            fControlParameter[0]-=0.05f;
            if (fControlParameter[0]<0.0) fControlParameter[0]=0.0;
        return 0;

        // set AILERONS control parameter
        case 'W':
            fControlParameter[1]+=0.05f;
            if (fControlParameter[1]>1.0) fControlParameter[1]=1.0;
        return 0;

        case 'S':
            fControlParameter[1]-=0.05f;
            if (fControlParameter[1]<-1.0) fControlParameter[1]=-1.0;
        return 0;

        // set ELEVATOR control parameter
        case 'E':
            fControlParameter[2]+=0.05f;
            if (fControlParameter[2]>1.0) fControlParameter[2]=1.0;
        return 0;

        case 'D':
            fControlParameter[2]-=0.05f;
            if (fControlParameter[2]<-1.0) fControlParameter[2]=-1.0;
        return 0;

    	default:
			return 0;

		}


	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_QUIT:
		wglMakeCurrent(hDC,NULL);  //
		wglDeleteContext(hRC);  //
		return 0;

	} // end switch
    return DefWindowProc(hwnd ,iMsg ,wParam ,lParam);

}
