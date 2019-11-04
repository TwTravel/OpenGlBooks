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
int vx=-5,x=700,i,b,g,r;
canvasFrame::canvasFrame()
{
	Create(NULL,"繪圖視窗",WS_OVERLAPPEDWINDOW,CRect(0,0,800,300));
	CClientDC dc(this);
	int width = dc.GetDeviceCaps(HORZRES);
	int height = dc.GetDeviceCaps(VERTRES);
	GetWindowRect( &rect );
	width = ( width - ( rect.right - rect.left ))/2 ;
	height = (height - (rect.bottom - rect.top ))/2 ;
	MoveWindow( width , height , (rect.right - rect.left ) , (rect.bottom - rect.top ) ,true);
	GetClientRect(&rect);
	//以下程式碼建立各個所要用的 DC
	mdc = new CDC;
	mdc1 = new CDC;
	mdc2 = new CDC;
	mdc->CreateCompatibleDC(&dc);
	mdc1->CreateCompatibleDC(&dc);
	mdc2->CreateCompatibleDC(&dc);
	//以下程式碼建立與載入所要用的點陣圖物件
	car = new CBitmap;
	forest = new CBitmap;
	mask = new CBitmap;
	temp = new CBitmap;
	dark = new CBitmap;
	car->m_hObject = (HBITMAP)::LoadImage(NULL,"car.bmp",IMAGE_BITMAP,112,64,LR_LOADFROMFILE);
	forest->m_hObject = (HBITMAP)::LoadImage(NULL,"forest.bmp",IMAGE_BITMAP,800,300,LR_LOADFROMFILE);
	mask->m_hObject = (HBITMAP)::LoadImage(NULL,"mask.bmp",IMAGE_BITMAP,800,300,LR_LOADFROMFILE);
	temp->CreateCompatibleBitmap(&dc,rect.right,rect.bottom);
	dark->CreateCompatibleBitmap(&dc,112,64);
	mdc->SelectObject(temp);   //設定 mdc 中儲存的點陣圖
	car->GetObject(sizeof(BITMAP),&bm);
	px = new unsigned char[bm.bmHeight*bm.bmWidthBytes];
	car->GetBitmapBits(bm.bmHeight*bm.bmWidthBytes,px);
	mdc2->SelectObject(dark);  //設定 mdc2 中儲存的點陣圖
}

canvasFrame::~canvasFrame()
{
	delete mdc;
	delete mdc1;
	delete mdc2;
	delete car;
	delete forest;
	delete mask;
	delete dark;
	delete temp;
	delete px;
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
	mdc->BitBlt(0,0,800,300,mdc1,0,0,WHITENESS);
	mdc1->SelectObject(car);
	mdc->BitBlt(x,160,112,64,mdc1,0,0,SRCCOPY);
	mdc1->SelectObject(mask);
	mdc2->BitBlt(0,0,112,64,mdc1,x,160,SRCCOPY);
	dark->GetObject(sizeof(BITMAP),&bm);
	unsigned char *px1 = new unsigned char[bm.bmHeight*bm.bmWidthBytes];
	dark->GetBitmapBits(bm.bmHeight*bm.bmWidthBytes,px1);

	if (bm.bmBitsPixel != 32)
	{
		return;
	}

	int rgb_b,PixelBytes,tx,ty;

	PixelBytes = bm.bmBitsPixel / 8;

	for (ty=0;ty<bm.bmHeight;ty++)
	{
		for (tx=0;tx<bm.bmWidth;tx++)
		{
			rgb_b = ty*bm.bmWidthBytes+tx*PixelBytes;
			if(px[rgb_b] != 255 && px[rgb_b+1] != 255 && px[rgb_b+2] !=255)
			{
				b = px[rgb_b] & px1[rgb_b];
				g = px[rgb_b+1] & px1[rgb_b+1];
				r = px[rgb_b+2] & px1[rgb_b+2];
				if(b == 0 && g==0 && r==0)
				{
					mdc->TextOut(300,50,"車子在樹林中...");
					break;
				}
			}
		}
	}
	delete px1;
	mdc->BitBlt(0,0,800,300,mdc1,0,0,SRCAND);
	mdc1->SelectObject(forest);
	mdc->BitBlt(0,0,800,300,mdc1,0,0,SRCPAINT);
	dc.BitBlt(0,0,800,300,mdc,0,0,SRCCOPY);
	if(x<-112)           //判斷車子是否已完全沒入視窗中
		KillTimer(nIDEvent);   //停止計時器
	x+=vx;               //計算下一次的貼圖座標
}

int canvasFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetTimer(1,0,NULL);
	return 0;
}
