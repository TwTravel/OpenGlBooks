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

struct fireball
{
	int x;       //火球所在的 X 座標
	int y;       //火球所在的 Y 座標
	int vx;      //火球 X 方向的速度
	int vy;      //火球 Y 方向的速度
	int lasted;  //火球的存在時間
	BOOL exist;  //是否存在
};
/////////////////////////////////////////////////////////////////////////////
// canvasFrame

IMPLEMENT_DYNCREATE(canvasFrame, CFrameWnd)

int i,count;
int x,y;
fireball fireball[50];

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
	fire = new CBitmap;
	mask = new CBitmap;
	temp = new CBitmap;
	bgbmp->m_hObject = (HBITMAP)::LoadImage(NULL,"bground.bmp",IMAGE_BITMAP,rect.right,rect.bottom,LR_LOADFROMFILE); //載入圖檔
	fire->m_hObject = (HBITMAP)::LoadImage(NULL,"snow.bmp",IMAGE_BITMAP,10,10,LR_LOADFROMFILE); //載入圖檔
	mask->m_hObject = (HBITMAP)::LoadImage(NULL,"mask.bmp",IMAGE_BITMAP,10,10,LR_LOADFROMFILE); //載入圖檔
	temp->CreateCompatibleBitmap(&dc,rect.right,rect.bottom);
	mdc->SelectObject(temp);
}

canvasFrame::~canvasFrame()
{
	delete bgbmp;
	delete fire;
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
	if(count == 0)              //新增爆炸點
	{
		x=rand()%rect.right;
		y=rand()%rect.bottom;
		for(i=0;i<50;i++)       //產生火球粒子
		{
			fireball[i].x = x;
			fireball[i].y = y;
			fireball[i].lasted = 0;
			if(i%2==0)
			{
				fireball[i].vx =  -rand()%30;
				fireball[i].vy =  -rand()%30;
			}
			if(i%2==1)
			{
				fireball[i].vx = rand()%30;
				fireball[i].vy = rand()%30;
			}
			if(i%4==2)
			{
				fireball[i].vx = -rand()%30;
				fireball[i].vy = rand()%30;
			}
			if(i%4==3)
			{
				fireball[i].vx = rand()%30;
				fireball[i].vy = -rand()%30;
			}
			fireball[i].exist = true;
		}
		count = 50;
	}
	CClientDC dc(this);
	mdc1->SelectObject(bgbmp);
	mdc->BitBlt(0,0,rect.right,rect.bottom,mdc1,0,0,SRCCOPY);
	for(i=0;i<50;i++)
	{
		if(fireball[i].exist)
		{
			mdc1->SelectObject(mask);
			mdc->BitBlt(fireball[i].x,fireball[i].y,10,10,mdc1,0,0,SRCAND);
			mdc1->SelectObject(fire);
			mdc->BitBlt(fireball[i].x,fireball[i].y,10,10,mdc1,0,0,SRCPAINT);
			fireball[i].x+=fireball[i].vx;
			fireball[i].y+=fireball[i].vy;
			fireball[i].lasted++;
			if(fireball[i].x<=-10 || fireball[i].x>rect.right || fireball[i].y<=-10 || fireball[i].y>rect.bottom || fireball[i].lasted>50)
			{
				fireball[i].exist = false;   //刪除火球粒子
				count--;                     //遞減火球總數
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
