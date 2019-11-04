// canvasFrame.cpp : implementation file
//

#include "stdafx.h"
#include "canvasr.h"
#include "canvasFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// canvasFrame

IMPLEMENT_DYNCREATE(canvasFrame, CFrameWnd)

canvasFrame::canvasFrame()
{
	RECT rect;
	Create(NULL,"ch07-1:如何建立DirectGraphics");
	CClientDC dc(this);
	int width = dc.GetDeviceCaps(HORZRES);
	int height = dc.GetDeviceCaps(VERTRES);
	GetWindowRect( &rect );
	width = ( width - ( rect.right - rect.left ))/2 ;
	height = (height - (rect.bottom - rect.top ))/2 ;
	MoveWindow( width , height , (rect.right - rect.left ) , (rect.bottom - rect.top ) ,true);

}

canvasFrame::~canvasFrame()
{
}


BEGIN_MESSAGE_MAP(canvasFrame, CFrameWnd)
	//{{AFX_MSG_MAP(canvasFrame)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// canvasFrame message handlers


LRESULT canvasFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch( message )
	{
	case WM_CREATE :
		if( !d3dCreate( m_hWnd , 640 , 480 , true ))
			PostMessage( WM_CLOSE );
		return 0 ;
	case WM_DESTROY :
		d3dRelease();
		return 0 ;
	}
	return CFrameWnd::WindowProc(message, wParam, lParam);
}

void canvasFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	Render();
	// Do not call CFrameWnd::OnPaint() for painting messages
}

void canvasFrame::Render()
{
	//清空繪圖頁
	d3dClear(0);
	//將繪圖頁畫到螢幕上
	d3d_Device->Present( NULL , NULL , NULL , NULL );
}
