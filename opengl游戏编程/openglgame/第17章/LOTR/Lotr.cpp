
#include <windows.h>													
#include <gl/gl.h>														
#include <gl/glu.h>

#include "GLFrame.h"														

#define WM_TOGGLEFULLSCREEN (WM_USER+1)								

static BOOL g_isProgramLooping;																													// Between Fullscreen / Windowed Mode
static BOOL g_createFullScreen;										
GL_Window			window;											   

//结束程序
void TerminateApplication (GL_Window* window)							
{
	PostMessage (window->hWnd, WM_QUIT, 0, 0);							
	g_isProgramLooping = FALSE;											
}

//切换模式
void ToggleFullscreen (GL_Window* window)								
{
	PostMessage (window->hWnd, WM_TOGGLEFULLSCREEN, 0, 0);				
}

//调整窗口 
void ReshapeGL (int width, int height)									
{
	glViewport (0, 0, (GLsizei)(width), (GLsizei)(height));				
	glMatrixMode (GL_PROJECTION);									
	glLoadIdentity ();													
	gluPerspective (45.0f, (GLfloat)(width)/(GLfloat)(height),		
					1.0f, 3000.0f);		
	glMatrixMode (GL_MODELVIEW);										
	glLoadIdentity ();													
}

///改变显示模式
BOOL ChangeScreenResolution (int width, int height, int bitsPerPixel)	
{
	DEVMODE dmScreenSettings;											
	ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));					
	dmScreenSettings.dmSize				= sizeof (DEVMODE);				
	dmScreenSettings.dmPelsWidth		= width;					
	dmScreenSettings.dmPelsHeight		= height;					
	dmScreenSettings.dmBitsPerPel		= bitsPerPixel;				
	dmScreenSettings.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	if (ChangeDisplaySettings (&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		return FALSE;													
	}
	return TRUE;													
}

///创建窗口 
BOOL CreateWindowGL (GL_Window* window)									
{
	DWORD windowStyle = WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX;;							
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;						

	PIXELFORMATDESCRIPTOR pfd =											
	{
		sizeof (PIXELFORMATDESCRIPTOR),									
		1,																
		PFD_DRAW_TO_WINDOW |											
		PFD_SUPPORT_OPENGL |											
		PFD_DOUBLEBUFFER,												
		PFD_TYPE_RGBA,													
		window->init.bitsPerPixel,										
		0, 0, 0, 0, 0, 0,												
		0,																
		0,																
		0,																
		0, 0, 0, 0,														
		16,																
		0,																
		0,																
		PFD_MAIN_PLANE,													
		0,																
		0, 0, 0															
	};

	// 询问是否在全屏状态下运行?
	if (MessageBox(HWND_DESKTOP, "你想在全屏状态下运行么 ?", "是否全屏运行?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		window->init.isFullScreen = false;										
	else
		window->init.isFullScreen = true;	


	ScreenW=window->init.width;
	ScreenH=window->init.height;

	RECT windowRect = {0, 0, window->init.width, window->init.height};	

	GLuint PixelFormat;													

	if (window->init.isFullScreen == TRUE)								
	{
		if (ChangeScreenResolution (window->init.width, window->init.height, window->init.bitsPerPixel) == FALSE)
		{
			//转换失败
			MessageBox (HWND_DESKTOP, "Mode Switch Failed.\nRunning In Windowed Mode.", "Error", MB_OK | MB_ICONEXCLAMATION);
			window->init.isFullScreen = FALSE;							
		}
		else															
		{
			ShowCursor (FALSE);	    ///隐藏鼠标										
			windowStyle = WS_POPUP;										
			windowExtendedStyle |= WS_EX_TOPMOST;						
		}																
	}
	else																
	{
		AdjustWindowRectEx (&windowRect, windowStyle, 0, windowExtendedStyle);
	}

	// 创建窗口
	window->hWnd = CreateWindowEx (windowExtendedStyle,					
								   window->init.application->className,	
								   window->init.title,					
								   windowStyle,							
								   0, 0,								
								   windowRect.right - windowRect.left,	
								   windowRect.bottom - windowRect.top,	
								   HWND_DESKTOP,						
								   0,									
								   window->init.application->hInstance, 
								   window);

	if (window->hWnd == 0)												
	{
		return FALSE;													
	}

	window->hDC = GetDC (window->hWnd);									
	if (window->hDC == 0)											
	{
		// 失败
		DestroyWindow (window->hWnd);								
		window->hWnd = 0;											
		return FALSE;												
	}

	// 选择像素格式
	PixelFormat = ChoosePixelFormat (window->hDC, &pfd);				
	if (PixelFormat == 0)											
	{
		// 失败
		ReleaseDC (window->hWnd, window->hDC);							
		window->hDC = 0;												
		DestroyWindow (window->hWnd);								
		window->hWnd = 0;												
		return FALSE;												
	}

	if (SetPixelFormat (window->hDC, PixelFormat, &pfd) == FALSE)		
	{
		// 失败
		ReleaseDC (window->hWnd, window->hDC);							
		window->hDC = 0;												
		DestroyWindow (window->hWnd);								
		window->hWnd = 0;												
		return FALSE;												
	}

	window->hRC = wglCreateContext (window->hDC);						
	if (window->hRC == 0)											
	{
		// 失败
		ReleaseDC (window->hWnd, window->hDC);							
		window->hDC = 0;											
		DestroyWindow (window->hWnd);								
		window->hWnd = 0;											
		return FALSE;													
	}

	if (wglMakeCurrent (window->hDC, window->hRC) == FALSE)
	{
		// 失败
		wglDeleteContext (window->hRC);									
		window->hRC = 0;											
		ReleaseDC (window->hWnd, window->hDC);						
		window->hDC = 0;											
		DestroyWindow (window->hWnd);								
		window->hWnd = 0;											
		return FALSE;												
	}

	//显示窗口
	ShowWindow (window->hWnd, SW_NORMAL);								
	window->isVisible = TRUE;											

	//调整窗口
	ReshapeGL (window->init.width, window->init.height);				

	ZeroMemory (window->keys, sizeof (Keys));							

	window->lastTickCount = GetTickCount ();						

	return TRUE;														
																		
}

BOOL DestroyWindowGL (GL_Window* window)								
{
	if (window->hWnd != 0)												
	{	
		if (window->hDC != 0)											
		{
			wglMakeCurrent (window->hDC, 0);							
			if (window->hRC != 0)										
			{
				wglDeleteContext (window->hRC);							
				window->hRC = 0;										

			}
			ReleaseDC (window->hWnd, window->hDC);						
			window->hDC = 0;											
		}
		DestroyWindow (window->hWnd);									
		window->hWnd = 0;												
	}

	if (window->init.isFullScreen)										
	{
		ChangeDisplaySettings (NULL,0);									
		ShowCursor (TRUE);												
	}	
	return TRUE;														
}

//消息处理回调函数
LRESULT CALLBACK WindowProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	
	GL_Window* window = (GL_Window*)(GetWindowLong (hWnd, GWL_USERDATA));

	switch (uMsg)														
	{
		case WM_SYSCOMMAND:												
		{
			switch (wParam)												
			{
				case SC_SCREENSAVE:										
				case SC_MONITORPOWER:									
				return 0;												
			}
			break;														
		}
		return 0;														

		case WM_CREATE:													
		{
			CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);			
			window = (GL_Window*)(creation->lpCreateParams);
			SetWindowLong (hWnd, GWL_USERDATA, (LONG)(window));
		}
		return 0;														

		case WM_CLOSE:													
			TerminateApplication(window);								
		return 0;														

		case WM_SIZE:													
			switch (wParam)												
			{
				case SIZE_MINIMIZED:									
					window->isVisible = FALSE;							
				return 0;												

				case SIZE_MAXIMIZED:									
					window->isVisible = TRUE;							
					ReshapeGL (LOWORD (lParam), HIWORD (lParam));		
				return 0;												

				case SIZE_RESTORED:										
					window->isVisible = TRUE;							
					ReshapeGL (LOWORD (lParam), HIWORD (lParam));		
				return 0;												
			}
		break;														

		case WM_KEYDOWN:											
			if ((wParam >= 0) && (wParam <= 255))					
			{
				window->keys->keyDown [wParam] = TRUE;				
				return 0;												
			}
		break;															

		case WM_KEYUP:													
			if ((wParam >= 0) && (wParam <= 255))					
			{
				window->keys->keyDown [wParam] = FALSE;					
				return 0;											
			}
		break;														

		case WM_TOGGLEFULLSCREEN:										
			g_createFullScreen = (g_createFullScreen == TRUE) ? FALSE : TRUE;
			PostMessage (hWnd, WM_QUIT, 0, 0);
		break;														
	}

	return DefWindowProc (hWnd, uMsg, wParam, lParam);					
}

// 注册窗口
BOOL RegisterWindowClass (Application* application)						
{																		
	
	WNDCLASSEX windowClass;											
	ZeroMemory (&windowClass, sizeof (WNDCLASSEX));						
	windowClass.cbSize			= sizeof (WNDCLASSEX);					
	windowClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	
	windowClass.lpfnWndProc		= (WNDPROC)(WindowProc);				
	windowClass.hInstance		= application->hInstance;			
	windowClass.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE);		
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);			
	windowClass.lpszClassName	= application->className;				
	if (RegisterClassEx (&windowClass) == 0)						
	{
		MessageBox (HWND_DESKTOP, "RegisterClassEx Failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;													
	}
	return TRUE;														
}

// 程序入口WinMain函数
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application			application;									
	Keys				keys;											
	BOOL				isMessagePumpActive;							
	MSG					msg;											
	DWORD				tickCount;										

	application.className = "OpenGL";									
	application.hInstance = hInstance;									

	// 填充窗口结构
	ZeroMemory (&window, sizeof (GL_Window));							
	window.keys					= &keys;								
	window.init.application		= &application;							
	window.init.title			= "指环王动画实例";			
	window.init.width			= 800;									
	window.init.height			= 600;									
	window.init.bitsPerPixel	= 16;									
	window.init.isFullScreen	= FALSE;									

	ZeroMemory (&keys, sizeof (Keys));									

	if (RegisterWindowClass (&application) == FALSE)					
	{
		MessageBox (HWND_DESKTOP, "Error Registering Window Class!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;														
	}

	g_isProgramLooping = TRUE;											
	g_createFullScreen = window.init.isFullScreen;						
	while (g_isProgramLooping)											
	{
		window.init.isFullScreen = g_createFullScreen;					
		if (CreateWindowGL (&window) == TRUE)							
		{
			
			if (Initialize (&window, &keys) == FALSE)					
			{
				TerminateApplication (&window);							
			}
			else														
			{	
				isMessagePumpActive = TRUE;								
				while (isMessagePumpActive == TRUE)						
				{
					if (PeekMessage (&msg, window.hWnd, 0, 0, PM_REMOVE) != 0)
					{
						if (msg.message != WM_QUIT)						
						{
							DispatchMessage (&msg);						
						}
						else											
						{
							isMessagePumpActive = FALSE;				
						}
					}
					else												
					{
						if (window.isVisible == FALSE)					
						{
							WaitMessage ();								
						}
						else									
						{
							tickCount = GetTickCount ();				
							Update (tickCount - window.lastTickCount);	
							window.lastTickCount = tickCount;			
							Draw ();									// 渲染
							SwapBuffers (window.hDC);					// 交换缓存
						}
					}
				}													
			}														

			Deinitialize ();											

			DestroyWindowGL (&window);								
		}
		else														
		{
			MessageBox (HWND_DESKTOP, "Error Creating OpenGL Window", "Error", MB_OK | MB_ICONEXCLAMATION);
			g_isProgramLooping = FALSE;									
		}
	}																

	UnregisterClass (application.className, application.hInstance);		
	return 0;
}																	
