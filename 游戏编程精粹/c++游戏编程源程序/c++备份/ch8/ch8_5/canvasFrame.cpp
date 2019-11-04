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
	Create(NULL,"讀X檔案");
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
d3dXFileData a_Mesh ;

LRESULT canvasFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch( message )
	{
	case WM_CREATE :
		InitWindows();
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



BOOL canvasFrame::InitWindows()
{
	if( !d3dCreate( m_hWnd , 640 , 480 , true ))
		return PostMessage( WM_CLOSE );
	a_Bk.Create( "背景.tga" );
	a_Mesh.Create( "tiger.x" );
	//設相機位置
    D3DXVECTOR3 vFrom( 3, 3,-3 );
    D3DXVECTOR3 vAt( 0, 0, 0 );
    D3DXVECTOR3 vUp( 0, 1, 0 );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vFrom, &vAt, &vUp );
    d3d_Device->SetTransform( D3DTS_VIEW, &matView );
	//設投影矩陣
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4 , 1.0f , 1.0f , 100.0f );
    d3d_Device->SetTransform( D3DTS_PROJECTION, &matProj );

	//設光源
	SetLight();
	return true ;
}


void canvasFrame::SetLight()
{
	//環境光
	d3d_Device->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB( 64 , 64 , 64 ));
	//設點光源
	D3DLIGHT9 light ;
	memset( &light , 0 , sizeof( light ));
	light.Type = D3DLIGHT_POINT ;
	light.Diffuse.r = 1 ; 
	light.Diffuse.g = 1 ;
	light.Diffuse.b = 1 ; 
	light.Specular.r = 1 ;
	light.Specular.g = 1 ;
	light.Specular.b = 1 ;
	light.Position.x = 10 ; 
	light.Position.y = 10 ; 
	light.Position.z = 10 ; 
    D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, (D3DXVECTOR3*)&light.Position  );
	light.Range = 20 ;
	light.Attenuation1 = 0.01f ;
 	d3d_Device->SetLight( 0 , &light );
 	d3d_Device->SetRenderState( D3DRS_LIGHTING, TRUE );
	d3d_Device->LightEnable( 0 , true );

}


void canvasFrame::Render()
{
	//清空
	d3dClear();
	//開始繪製
	d3d_Device->BeginScene();
        d3d_Device->SetRenderState( D3DRS_SPECULARENABLE , false );
		d3d_Device->SetRenderState( D3DRS_SHADEMODE , D3DSHADE_GOURAUD );
		d3d_Device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		d3d_Device->SetTextureStageState( 0 , D3DTSS_COLORARG2 , D3DTA_DIFFUSE );
		d3d_Device->SetRenderState( D3DRS_ZENABLE , D3DZB_FALSE );
		a_Bk.BltFast( 0 , 0 );
		//螢幕上方面第一隻老虎
        d3d_Device->SetRenderState( D3DRS_SPECULARENABLE , true );
		d3d_Device->SetRenderState( D3DRS_ZENABLE , D3DZB_TRUE );
		a_Mesh.Draw();

	d3d_Device->EndScene();
	//成相
	d3d_Device->Present( NULL , NULL , NULL , NULL );

}
