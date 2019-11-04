// canvasFrame.cpp : implementation file
//
#define DIRECTINPUT_VERSION 0x0700
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

LPDIRECTINPUT7  pDI;        //宣告 DirectInput 物件指標
LPDIRECTINPUTDEVICE7  pDKB; //宣告輸入裝置物件指標
HRESULT         result;     //宣告 HRESULT 型態變數
char buffer[256];           //設定緩衝區
int x,y;                    //宣告貼圖座標
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
	ball = new CBitmap;
	ball->m_hObject = (HBITMAP)::LoadImage(NULL,"ball.bmp",IMAGE_BITMAP,81,81,LR_LOADFROMFILE); //載入圖檔
	mdc->SelectObject(ball);
	x = ((rect.right-rect.left) - 81)/2;    //設定小球起點 X 座標
	y = ((rect.bottom-rect.top) - 81)/2;    //設定小球起點 Y 座標
}

canvasFrame::~canvasFrame()
{
	delete ball;
	delete mdc;
	pDKB->Release();
	pDI->Release();
}


BEGIN_MESSAGE_MAP(canvasFrame, CFrameWnd)
	//{{AFX_MSG_MAP(canvasFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
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
	result = pDI->CreateDeviceEx(GUID_SysKeyboard, IID_IDirectInputDevice7,
        (void**)&pDKB, NULL); //建立輸入裝置物件
	if(result != DI_OK)
		MessageBox("建立鍵盤輸入裝置失敗!");
	result = pDKB->SetDataFormat(&c_dfDIKeyboard);
	if(result != DI_OK)
		MessageBox("設定資料格式失敗!");
	result = pDKB->SetCooperativeLevel(m_hWnd, 
                   DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);  //設定協調層級
	if(result != DI_OK)
		MessageBox("設定程式協調層級失敗!");
	result = pDKB->Acquire();   //取用輸入裝置
	if(result != DI_OK)
		MessageBox("取用輸入裝置失敗!");
	SetTimer(1,0,NULL);
	// TODO: Add your specialized creation code here
	return 0;
}

void canvasFrame::OnTimer(UINT nIDEvent) 
{
	CFrameWnd::OnTimer(nIDEvent);
	CClientDC dc(this);                   
	dc.BitBlt(x,y,81,81,mdc,0,0,SRCCOPY); 
    result = pDKB->GetDeviceState(sizeof(buffer),(LPVOID)&buffer);  //取得鍵盤狀態
    if(result != DI_OK)
		MessageBox("取得鍵盤狀態失敗!");
	if(buffer[DIK_RIGHT] & 0x80)    //判斷右鍵是否被按下
		if(x+80 > rect.right)       //判斷是否碰到右邊緣
			x = rect.right - 60;
		else
			x+=20;
	if(buffer[DIK_LEFT] & 0x80)     //判斷左鍵是否被按下
		if(x-20 < -21)              //判斷是否碰到左邊緣
			x = -21;
		else
			x-=20;
	if(buffer[DIK_UP] & 0x80)       //判斷上鍵是否被按下
		if(y-20 < -21)              //判斷是否碰到上邊緣
			y = -21;
		else
			y-=20;
	if(buffer[DIK_DOWN] & 0x80)     //判斷下鍵是否被按下
		if(y+80 > rect.bottom)      //判斷是否碰到下邊緣
			y = rect.bottom-60;
		else
			y+=20;
	if(buffer[DIK_ESCAPE] & 0x80)   //判斷Esc鍵是否被按下
	{
		KillTimer(1);               //刪除計時器
		PostMessage(WM_CLOSE);      //結束程式
	}
}
