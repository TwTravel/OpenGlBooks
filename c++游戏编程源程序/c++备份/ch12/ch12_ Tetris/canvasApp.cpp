// canvasApp.cpp: implementation of the canvasApp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "canvasr.h"
#include "canvasApp.h"
#include "canvasFrame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
canvasApp canvas;
canvasApp::canvasApp()
{

}

canvasApp::~canvasApp()
{

}

canvasApp::InitInstance()
{
	m_pMainWnd = new canvasFrame;
	m_pMainWnd->ShowWindow( m_nCmdShow );
	m_pMainWnd->UpdateWindow();
	return true;
}

/*/////////////////////////////////////////////////

  MFC訊息迴圈

/*/////////////////////////////////////////////////
int canvasApp::Run()
{
	MSG msg ;

	while(true)
	{
		if( PeekMessage( &msg , 0 , 0 , 0 , PM_REMOVE ))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			if( msg.message == WM_QUIT )
				return msg.wParam ;
		}else 
		{
			//操作更新
			diRun();
			//回呼執行
			if( app_Proc )
				app_Proc->Run();
		}
	}

	return 0 ;
}
