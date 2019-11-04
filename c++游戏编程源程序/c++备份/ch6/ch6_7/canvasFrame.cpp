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

struct snow
{
	int x;
	int y;
	BOOL exist;
};
/////////////////////////////////////////////////////////////////////////////
// canvasFrame

IMPLEMENT_DYNCREATE(canvasFrame, CFrameWnd)

int i,count;
snow drop[50];
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
	mdc1 = new CDC;
	mdc->CreateCompatibleDC(&dc); //將mdc轉換成與dc相容的DC
	mdc1->CreateCompatibleDC(&dc);
	bgbmp = new CBitmap;
	snow = new CBitmap;
	mask = new CBitmap;
	temp = new CBitmap;
	bgbmp->m_hObject = (HBITMAP)::LoadImage(NULL,"bground.bmp",IMAGE_BITMAP,rect.right,rect.bottom,LR_LOADFROMFILE); //載入圖檔
	snow->m_hObject = (HBITMAP)::LoadImage(NULL,"snow.bmp",IMAGE_BITMAP,20,20,LR_LOADFROMFILE); //載入圖檔
	mask->m_hObject = (HBITMAP)::LoadImage(NULL,"mask.bmp",IMAGE_BITMAP,20,20,LR_LOADFROMFILE); //載入圖檔
	temp->CreateCompatibleBitmap(&dc,rect.right,rect.bottom);
	mdc->SelectObject(temp);

}

canvasFrame::~canvasFrame()
{
	delete bgbmp;
	delete snow;
	delete mask;
	delete temp;
	delete mdc;
	delete mdc1;
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
	if(count<50)
	{
		drop[count].x = rand()%rect.right;
		drop[count].y = 0;
		drop[count].exist = true;
		count++;
	}
	CClientDC dc(this);
	mdc1->SelectObject(bgbmp);
	mdc->BitBlt(0,0,rect.right,rect.bottom,mdc1,0,0,SRCCOPY);
	for(i=0;i<50;i++)
	{
		if(drop[i].exist)
		{
			mdc1->SelectObject(mask);
			mdc->BitBlt(drop[i].x,drop[i].y,20,20,mdc1,0,0,SRCAND);
			mdc1->SelectObject(snow);
			mdc->BitBlt(drop[i].x,drop[i].y,20,20,mdc1,0,0,SRCPAINT);
			if(rand()%2==0)
				drop[i].x+=3;
			else 
				drop[i].x-=3;
			drop[i].y+=10;
			if(drop[i].y > rect.bottom)
			{
				drop[i].x = rand()%rect.right;
				drop[i].y = 0;
			}
		}
	}

	dc.BitBlt(0,0,rect.right,rect.bottom,mdc,0,0,SRCCOPY);

	
	CFrameWnd::OnTimer(nIDEvent);
}

int canvasFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetTimer(1,0,NULL);
	return 0;
}
