// canvasFrame.cpp : implementation file
//
#define DIRECTINPUT_VERSION 0x0700
#include "stdafx.h"
#include "canvasr.h"
#include "canvasFrame.h"
#include "dinput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct bullet          
{
   int   x;           
   int   y;
   BOOL exist;
};

/////////////////////////////////////////////////////////////////////////////
// canvasFrame

IMPLEMENT_DYNCREATE(canvasFrame, CFrameWnd)

LPDIRECTINPUT7  pDI;        //宣告 DirectInput 物件指標
LPDIRECTINPUTDEVICE7  pDMO; //宣告輸入裝置物件指標
HRESULT result;             //宣告 HRESULT 型態變數
DIMOUSESTATE2 MState;       //滑鼠狀態結構  
int x,y,xlast,ylast;
bullet b[200];
int bcount=0;
int i;

canvasFrame::canvasFrame()
{
	int cx,cy;
	Create(NULL,"繪圖視窗");
	CClientDC dc(this);
	int width = dc.GetDeviceCaps(HORZRES);
	int height = dc.GetDeviceCaps(VERTRES);
	GetWindowRect( &rect );
	width = ( width - ( rect.right - rect.left ))/2 ;
	height = (height - (rect.bottom - rect.top ))/2 ;
	MoveWindow( width , height , (rect.right - rect.left ) , (rect.bottom - rect.top ) ,true);
	GetClientRect(&rect);
	ClientToScreen(&rect);    //轉換座標
	::ClipCursor(&rect);      //設定滑鼠移動範圍
	mdc = new CDC;
	mdc1 = new CDC;
	mdc->CreateCompatibleDC(&dc);
	mdc1->CreateCompatibleDC(&dc);
	bitmap = new CBitmap;
	bitmap->m_hObject = (HBITMAP)::LoadImage(NULL,"ship.bmp",IMAGE_BITMAP,100,74,LR_LOADFROMFILE); //載入圖檔
	mdc->SelectObject(bitmap);
	bitmap->m_hObject = (HBITMAP)::LoadImage(NULL,"bullet1.bmp",IMAGE_BITMAP,32,15,LR_LOADFROMFILE); //載入圖檔
	mdc1->SelectObject(bitmap);
	x = ((rect.right-rect.left) - 100)/2;    
	y = ((rect.bottom-rect.top) - 100)/2;   
	cx = x + 81/2;
	cy = y + 81/2;
	CPoint *p = new CPoint(cx,cy);
	ClientToScreen(p);               //轉換座標
	::ShowCursor(false);             //取消滑鼠游標
	::SetCursorPos(p->x,p->y);       //設定滑鼠位置
	ScreenToClient(&rect);           //轉換座標
	delete p;
}

canvasFrame::~canvasFrame()
{
	delete bitmap;
	delete mdc;
	delete mdc1;
	pDMO->Release();
	pDI->Release();
	::ClipCursor(NULL);
}

BEGIN_MESSAGE_MAP(canvasFrame, CFrameWnd)
	//{{AFX_MSG_MAP(canvasFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// canvasFrame message handlers

int canvasFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	HINSTANCE hinst = AfxGetInstanceHandle();  //取得應用程式的 handle
	result = DirectInputCreateEx(hinst, DIRECTINPUT_VERSION, 
        IID_IDirectInput7, (void**)&pDI, NULL); //建立 DirectInput 物件
	if(result != DI_OK)
		MessageBox("建立 DirectInput 物件失敗!");
	result = pDI->CreateDeviceEx(GUID_SysMouse, IID_IDirectInputDevice7,
        (void**)&pDMO, NULL); //建立輸入裝置物件
	if(result != DI_OK)
		MessageBox("建立滑鼠輸入裝置失敗!");
	result = pDMO->SetDataFormat(&c_dfDIMouse2);
	if(result != DI_OK)
		MessageBox("設定資料格式失敗!");
	result = pDMO->SetCooperativeLevel(m_hWnd, 
                   DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);  //設定協調層級
	if(result != DI_OK)
		MessageBox("設定程式協調層級失敗!");
	result = pDMO->Acquire();   //取用輸入裝置
	if(result != DI_OK)
		MessageBox("取用輸入裝置失敗!");
	SetTimer(500,1,NULL);
	// TODO: Add your specialized creation code here
	return 0;
}

void canvasFrame::OnTimer(UINT nIDEvent)
{
	CFrameWnd::OnTimer(nIDEvent);
	CClientDC dc(this);    
	result = pDMO->GetDeviceState(sizeof(MState),(LPVOID)&MState); //取得滑鼠狀態
	if(result != DI_OK )
		MessageBox("取得滑鼠狀態失敗!");
	x += MState.lX;      //設定飛機圖示的 x 座標
	y += MState.lY;      //設定飛機圖示的 y 座標
	if(x<rect.left)      //是否已至左邊界
		x = rect.left;
	if(x>rect.right-100) //是否已至右邊界
		x = rect.right-100;
	if(y<rect.top)       //是否已至上邊界
		y = rect.top;
	if(y>rect.bottom-74) //是否已至下邊界
		y = rect.bottom-74;
	dc.BitBlt(xlast,ylast,100,74,mdc,0,0,WHITENESS); //覆蓋上次的貼圖
	xlast = x;
	ylast = y;
	if(MState.rgbButtons[0] & 0x80)  //判斷是否按下滑鼠左鍵
	{
		for(i=0;i<200;i++)
		{
			if(b[i].exist == false)  //加入一顆新子彈
			{
				b[i].x = x;
				b[i].y = y+30;
				b[i].exist = true;
				bcount++;
				break;
			}
		}
	}
	if(bcount != 0)
		for(i=0;i<200;i++)          //貼上所有子彈
		{
			dc.BitBlt(b[i].x,b[i].y,32,15,mdc1,0,0,SRCCOPY);
			b[i].x -=17;
			if(b[i].x<-30)
			{
				b[i].exist = false;
				bcount--;
			}
		}
	dc.BitBlt(x,y,100,74,mdc,0,0,SRCCOPY); //貼上飛機
}

void canvasFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
	if( nChar== VK_ESCAPE )      //判斷是否按下 Esc 鍵
		PostMessage(WM_CLOSE );  //傳送WM_CLOSE訊息
}
