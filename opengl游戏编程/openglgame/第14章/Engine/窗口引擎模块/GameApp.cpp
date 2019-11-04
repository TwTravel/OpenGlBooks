//========================================================
/**
*  @file      GameApp.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  应用程序类,完成程序的初始化、
*             窗口的创建，这是最顶层的类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================


#include "stdafx.h"
#include "GameApp.h"
#include "resource.h"
#include "main.h"


//! 输入法
CIme	m_cIme;

GameApp::GameApp(void){}

GameApp::~GameApp(void)
{
	m_cEngine.Close();
	KillGLWindow();
}



/*! 程序的主循环 */
void GameApp::Run(void)
{
	while(TRUE)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	
		{
			if(msg.message == WM_QUIT)break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_cEngine.Prepare();
			m_cEngine.Render();
			m_cEngine.SwapBuffer();
		}
	}
}


/**\brief
 * 创建窗口
 *
 *\param	_sWinInfo	窗口信息
 *\return	BOOL		创建是否成功
 */
BOOL GameApp::CreateGLWindow(SWindowInfo _sWinInfo)
{
	m_sWinInfo=_sWinInfo;
	m_sWinInfo.m_hInstance=GetModuleHandle(NULL);

	WNDCLASS	wc;
	DWORD		dwExStyle;
	DWORD		dwStyle;
	RECT		WindowRect;
	WindowRect.left=(long)0;
	WindowRect.right=(long)m_sWinInfo.m_iWidth;
	WindowRect.top=(long)0;
	WindowRect.bottom=(long)m_sWinInfo.m_iHeight;


	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= m_sWinInfo.m_hInstance;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor			= NULL;//LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= "OpenGL";

	if (!RegisterClass(&wc))return FALSE;
	
	if (m_sWinInfo.m_bFullScreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= m_sWinInfo.m_iWidth;
		dmScreenSettings.dmPelsHeight	= m_sWinInfo.m_iHeight;
		dmScreenSettings.dmBitsPerPel	= m_sWinInfo.m_iBits;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				m_sWinInfo.m_bFullScreen=FALSE;
			}
			else
			{
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	if (m_sWinInfo.m_bFullScreen)
	{
		dwExStyle=WS_EX_APPWINDOW;
		dwStyle=WS_POPUP;
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle=WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	if (!(m_sWinInfo.m_hWnd=CreateWindowEx(dwExStyle,
								"OpenGL",
								m_sWinInfo.m_strWinName,
								dwStyle|
								WS_CLIPSIBLINGS|
								WS_CLIPCHILDREN,
								0, 0,
								WindowRect.right-WindowRect.left,
								WindowRect.bottom-WindowRect.top,
								NULL,NULL,
								m_sWinInfo.m_hInstance,
								NULL)))
	{
		KillGLWindow();
		return FALSE;
	}
	ShowWindow(m_sWinInfo.m_hWnd,SW_SHOW);
	SetForegroundWindow(m_sWinInfo.m_hWnd);
	SetFocus(m_sWinInfo.m_hWnd);
	
	return TRUE;
}


/*! 窗口消息处理函数 */
LRESULT WINAPI GameApp::MessageHandler(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message)
	{
		case WM_ACTIVATE:  
		{
			if (!HIWORD(wParam))
				m_bActive = TRUE;
			else
				m_bActive=FALSE;
		
			return 0;
	    }

		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage(0);return 0;

		case WM_CREATE:return 0;

		case WM_SIZE:
		{
			m_cEngine.SetProjection(45,1);
			return 0;
		}
		///////////////////////////////////
		//-----下面的用来处理输入法--------
		case WM_KEYDOWN:
			switch( wParam )
			{
				case VK_LEFT:	m_cIme.LeftKey();break;
				case VK_RIGHT:	m_cIme.RightKey();break;
				case VK_DELETE:	m_cIme.DeleteKey();break;
				case VK_RETURN:	m_cIme.ReturnKey();break;
				default:break;
			}
			break;

		case WM_CHAR:		//!IME消息
			m_cIme.ReceiveChar(wParam);
			break;

		//case WM_IME_SETCONTEXT:
		//	return m_cIme.OnWM_IME_SETCONTEXT()? 0 : DefWindowProc(hWnd, message, wParam, lParam);

		case WM_INPUTLANGCHANGEREQUEST:
			return m_cIme.OnWM_INPUTLANGCHANGEREQUEST()? 0 : DefWindowProc(hWnd, message, wParam, lParam);

		case WM_IME_STARTCOMPOSITION:
			return m_cIme.OnWM_IME_STARTCOMPOSITION()? 0 : DefWindowProc(hWnd, message, wParam, lParam);

		case WM_IME_ENDCOMPOSITION:
			return m_cIme.OnWM_IME_ENDCOMPOSITION()? 0 : DefWindowProc(hWnd, message, wParam, lParam);

		//case WM_IME_NOTIFY:
		//	return m_cIme.OnWM_IME_NOTIFY( m_sWinInfo.m_hWnd, wParam )? 0 : DefWindowProc(hWnd, message, wParam, lParam);
	
		case WM_IME_COMPOSITION:
			return m_cIme.OnWM_IME_COMPOSITION( m_sWinInfo.m_hWnd, lParam )? 0 : DefWindowProc(hWnd, message, wParam, lParam);

		case WM_INPUTLANGCHANGE:
			return m_cIme.OnWM_INPUTLANGCHANGE( m_sWinInfo.m_hWnd )? 0 : DefWindowProc(hWnd, message, wParam, lParam);

		case WM_SYSCOMMAND:
			return ( wParam == SC_KEYMENU )? 0 : DefWindowProc(hWnd, message, wParam, lParam);
	}

	return  DefWindowProc(hWnd, message, wParam, lParam);
}



/*! 初始化程序，窗口、输入等 */
BOOL GameApp::Init(SWindowInfo _sWinInfo)
{
	if(!CreateGLWindow(_sWinInfo))
	{
		_ERROR("create window fail!");
		return FALSE;
	}

	if(!m_cEngine.Init())
	{
		_ERROR("failed to init opengl!");
		return FALSE;
	}

	return TRUE;
}



/*! 销毁窗口 */
void GameApp::KillGLWindow(void)
{
	if (m_sWinInfo.m_bFullScreen)ChangeDisplaySettings(NULL,0);

	if (m_sWinInfo.m_hWnd && !DestroyWindow(m_sWinInfo.m_hWnd))m_sWinInfo.m_hWnd=NULL;

	if (!UnregisterClass("OpenGL",m_sWinInfo.m_hInstance))m_sWinInfo.m_hInstance=NULL;
}

