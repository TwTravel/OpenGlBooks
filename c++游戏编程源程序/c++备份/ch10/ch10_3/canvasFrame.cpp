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
struct bullet       //宣告子彈結構
{
   int  x;          //X座標
   int  y;          //Y座標
   BOOL exist;      //是否存在
};           
/////////////////////////////////////////////////////////////////////////////
// canvasFrame

IMPLEMENT_DYNCREATE(canvasFrame, CFrameWnd)
LPDIRECTINPUT7       pDI;  //宣告 DirectInput 物件指標
LPDIRECTINPUTDEVICE7 pDJS; //宣告輸入裝置物件指標
HRESULT result;            //宣告 HRESULT 型態變數
DIDEVCAPS JSCap;           //宣告搖桿特性結構
DIJOYSTATE  JState;        //宣告搖桿狀態結構 
int x,y,xlast,ylast;
bullet b[200];             //宣告子彈陣列
int bcount=0;              //子彈數
int i;
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
	ClientToScreen(&rect);    //轉換座標
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
	::ShowCursor(false);        //取消滑鼠游標
	ScreenToClient(&rect);		//轉換座標
}

canvasFrame::~canvasFrame()
{
	delete bitmap;
	delete mdc;
	delete mdc1;
	pDJS->Release();
	pDI->Release();
}


BEGIN_MESSAGE_MAP(canvasFrame, CFrameWnd)
	//{{AFX_MSG_MAP(canvasFrame)
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// canvasFrame message handlers
BOOL CALLBACK Joysticks(LPCDIDEVICEINSTANCE lpddi,LPVOID pvRef)
{
    result = pDI->CreateDeviceEx( lpddi->guidInstance,IID_IDirectInputDevice7,
		                        (VOID**)&pDJS, NULL );
	if(result != DI_OK)
		return DIENUM_CONTINUE;
	else
		return DIENUM_STOP;
}

BOOL CALLBACK Objects(LPCDIDEVICEOBJECTINSTANCE lpddoi,LPVOID pvRef)
{
    DIPROPRANGE dr; 
    dr.diph.dwSize       = sizeof(DIPROPRANGE);
    dr.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dr.diph.dwHow        = DIPH_BYOFFSET;
    dr.diph.dwObj        = lpddoi->dwOfs; //取得軸的類型
    switch( lpddoi->dwOfs )
    {
        case DIJOFS_X:
                dr.lMin              = -100; 
				dr.lMax              = 100; 
				result = pDJS->SetProperty(DIPROP_RANGE,&dr.diph);
			    if(result != DI_OK)
       				MessageBox(NULL,"設定軸範圍失敗!","錯誤訊息",MB_OK);
	            break;
        case DIJOFS_Y:
                dr.lMin              = -50; 
				dr.lMax              = 50; 
				result = pDJS->SetProperty(DIPROP_RANGE,&dr.diph);
			    if(result != DI_OK)
       				MessageBox(NULL,"設定軸範圍失敗!","錯誤訊息",MB_OK);
	            break;
        case DIJOFS_Z:
	            //設定 z 方向軸範圍的程式碼(略)
	            break;
        case DIJOFS_RX:
     			//設定 x 旋轉軸範圍的程式碼(略)
                break;
        case DIJOFS_RY:
     			//設定 y 旋轉軸範圍的程式碼(略)
                break;
        case DIJOFS_RZ:
     			//設定 z 旋轉軸範圍的程式碼(略)
                break;
        case DIJOFS_SLIDER(0):
                //設定滑軸 1 範圍的程式碼(略)
			    break;
        case DIJOFS_SLIDER(1):
                //設定滑軸 2 範圍的程式碼(略)
				break;
    }
    return DIENUM_CONTINUE;
}
int canvasFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	HINSTANCE hinst = AfxGetInstanceHandle();
    result = DirectInputCreateEx(hinst, DIRECTINPUT_VERSION,IID_IDirectInput7, (LPVOID*)&pDI, NULL );
	if(result != DI_OK)
		MessageBox("建立 DirectInput 物件失敗!");
	result = pDI->EnumDevices(DIDEVTYPE_JOYSTICK, Joysticks,
                             NULL, DIEDFL_ATTACHEDONLY ); //列舉搖桿裝置
	if(result != DI_OK)
		MessageBox("列舉搖桿失敗!");
	if(pDJS == NULL) //判斷是否找到搖桿
		MessageBox("無可使用的搖桿裝置!");
    result = pDJS->SetDataFormat(&c_dfDIJoystick);
	if(result != DI_OK)
		MessageBox("設定資料格式失敗!");
    result = pDJS->SetCooperativeLevel(m_hWnd, DISCL_EXCLUSIVE|DISCL_BACKGROUND );
	if(result != DI_OK)
		MessageBox("設定協調層級失敗!");
	JSCap.dwSize = sizeof(JSCap);
    result = pDJS->GetCapabilities(&JSCap);
	if(result != DI_OK)
		MessageBox("取得搖桿特性失敗!");
    result = pDJS->EnumObjects(Objects, (VOID*)m_hWnd,DIDFT_AXIS);
	if(result != DI_OK)
		MessageBox("列舉搖桿元件失敗!");
	result = pDJS->Acquire();
	if(result != DI_OK)
		MessageBox("取得搖桿裝置失敗!");
	SetTimer(1,1,NULL);
	return 0;
}

void canvasFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
	if( nChar== VK_ESCAPE )      //判斷是否按下 Esc 鍵
		PostMessage(WM_CLOSE );  //傳送WM_CLOSE訊息
	CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
}

void canvasFrame::OnTimer(UINT nIDEvent) 
{
	CFrameWnd::OnTimer(nIDEvent);
	CClientDC dc(this);
	result = pDJS->Poll();   //偵測搖桿
	if(result != DI_OK && result != DI_NOEFFECT)
		MessageBox("偵測搖桿失敗!");
    result = pDJS->GetDeviceState( sizeof(JState), &JState );
    if (result != DI_OK) 
		MessageBox("取得搖桿狀態失敗!");   
	x += JState.lX;      //設定飛機圖示的 X 座標
	y += JState.lY;      //設定飛機圖示的 Y 座標
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
	if(JState.rgbButtons[0] & 0x80)  //判斷是否按下發射子彈鈕
	{
		for(i=0;i<200;i++)
		{
			if(b[i].exist == false)
			{
				b[i].x = x;
				b[i].y = y+30;
				b[i].exist = true;
				bcount++;
				break;
			}
		}
	}
	if(JState.rgbButtons[1] & 0x80) //是否按下結束鈕
		PostMessage(WM_CLOSE );  //傳送WM_CLOSE訊息
	if(bcount != 0)
		for(i=0;i<200;i++)
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

	

