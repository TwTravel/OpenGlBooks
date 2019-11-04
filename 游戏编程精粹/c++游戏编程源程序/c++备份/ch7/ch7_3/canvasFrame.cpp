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
	Create(NULL,"D3D初始設定");
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
d3dTexture a_Bk ;
d3dTexture a_Role ;



LRESULT canvasFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch( message )
	{
	case WM_CREATE :
		if( !d3dCreate( m_hWnd , 640 , 480 , true ))
			return PostMessage( WM_CLOSE );
		a_Bk.Create( "背景.tga" );
		a_Role.Create( "娃娃.tga" );
		return 0 ;
	case WM_DESTROY :
		d3dRelease();
		return 0 ;
	}
	return CFrameWnd::WindowProc(message, wParam, lParam);
}

void canvasFrame::OnPaint() 
{
	// TODO: Add your message handler code here
	Render();
	// Do not call CFrameWnd::OnPaint() for painting messages
	CPaintDC dc(this); // device context for painting
}

void canvasFrame::Render()
{
//	d3dTexture bk ;
	//清空
	d3dClear();
	//開始繪製
	d3d_Device->BeginScene();
		d3d_Device->SetRenderState( D3DRS_CULLMODE , D3DCULL_NONE );
		d3d_Device->SetRenderState( D3DRS_ZENABLE , D3DZB_FALSE );
		a_Bk.BltFast( 0 , 0 );
		a_Role.BltFast( 0 , 0 );
	d3d_Device->EndScene();
	//成像
	d3d_Device->Present( NULL , NULL , NULL , NULL );

}
