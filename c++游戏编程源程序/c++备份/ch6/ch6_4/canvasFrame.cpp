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
int vx1=5,vx2=-5,x1=-70,x2=520,y1=165,y2=150;
canvasFrame::canvasFrame()
{
	Create(NULL,"繪圖視窗");
	CClientDC dc(this);
	int width = dc.GetDeviceCaps(HORZRES);
	int height = dc.GetDeviceCaps(VERTRES);
	GetWindowRect( &rect );
	width = ( width - ( rect.right - rect.left ))/2 ;
	height = (height - (rect.bottom - rect.top ))/2 ;
	MoveWindow( width , height , (rect.right - rect.left ) , (rect.bottom - rect.top ) ,true);
	GetClientRect(&rect);
	mdc = new CDC;
	mdc->CreateCompatibleDC(&dc);
	car1 = new CBitmap;
	car2 = new CBitmap;
	bomb = new CBitmap;
	car1->m_hObject = (HBITMAP)::LoadImage(NULL,"car1.bmp",IMAGE_BITMAP,196,66,LR_LOADFROMFILE);
	car2->m_hObject = (HBITMAP)::LoadImage(NULL,"car2.bmp",IMAGE_BITMAP,140,80,LR_LOADFROMFILE);
	bomb->m_hObject = (HBITMAP)::LoadImage(NULL,"bomb.bmp",IMAGE_BITMAP,187,200,LR_LOADFROMFILE);
}

canvasFrame::~canvasFrame()
{
	delete mdc;
	delete car1;
	delete car2;
	delete bomb;
}


BEGIN_MESSAGE_MAP(canvasFrame, CFrameWnd)
	//{{AFX_MSG_MAP(canvasFrame)
	ON_WM_TIMER()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// canvasFrame message handlers

void canvasFrame::OnTimer(UINT nIDEvent) 
{
	CFrameWnd::OnTimer(nIDEvent);
	CClientDC dc(this);
	mdc->SelectObject(car1);        //選擇第一部車
	dc.BitBlt(x1,y1,196,66,mdc,0,0,SRCCOPY);	//貼上第一部車
	mdc->SelectObject(car2);        //選擇第二部車
	dc.BitBlt(x2,y2,140,80,mdc,0,0,SRCCOPY);	//貼上第二部車
	if(x1+196>x2)     //判斷矩形是否交集
	{
		mdc->SelectObject(bomb);
		dc.BitBlt(x2-100,y2,187,100,mdc,0,100,SRCPAINT);
		dc.BitBlt(x2-100,y2,187,100,mdc,0,0,SRCAND);
		KillTimer(nIDEvent);
	}
	x1+=vx1;   //第一部車下次貼圖位置
	x2+=vx2;   //第二部車下次貼圖位置
}

int canvasFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetTimer(1,0,NULL);
	return 0;
}
