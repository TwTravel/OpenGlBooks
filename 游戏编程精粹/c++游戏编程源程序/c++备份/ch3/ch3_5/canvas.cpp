
#include "stdafx.h"

//全域變數宣告
HINSTANCE hInst;
HBITMAP map;
HDC		hdc,mdc;
HWND	hWnd;
DWORD	tPre,tNow;
int		x=0,y=0;

//函式宣告
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				MyPaint(HDC hdc);

//****主程式**************************************
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);

	//執行初始化函式
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	//遊戲迴圈
    while( msg.message!=WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0,0 ,PM_REMOVE) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
		else
		{
			tNow = GetTickCount();
			if(tNow-tPre >= 40)
				MyPaint(hdc);
		}
    }

	return msg.wParam;
}

//****定義及註冊視窗類別函式*************************
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "canvas";
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

//****初始函式*************************************
// 建立視窗及載入背景圖
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	hWnd = CreateWindow("canvas", "繪圖視窗" , WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	MoveWindow(hWnd,10,10,640,480,true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);

	map = (HBITMAP)LoadImage(NULL,"map.bmp",IMAGE_BITMAP,1548,1129,LR_LOADFROMFILE);

	SelectObject(mdc,map);

	MyPaint(hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 依背景圖剪裁的左上角點座標進行貼圖
void MyPaint(HDC hdc)
{
	BitBlt(hdc,0,0,640,480,mdc,x,y,SRCCOPY);
	tPre = GetTickCount();
}

//****訊息處理函式***********************************
// 依據按下的方向鍵來改變背景圖剪裁的左上角點座標
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:			     //按下鍵盤訊息
			switch (wParam) 
			{
				case VK_UP:				 //按下上鍵
					y -= 20;
					if(y < 0)
						y = 0;
					break;
				case VK_DOWN:			 //按下下鍵
					y += 20;
					if(y > 660)
						y = 660;
					break;
				case VK_LEFT:			 //按下左鍵
					x -= 20;
					if(x < 0)
						x = 0;
					break;
				case VK_RIGHT:			 //按下右鍵
					x += 20;
					if(x > 910)
						x = 910;
					break;
			}
			break;
		case WM_DESTROY:					//視窗結束訊息
			DeleteDC(mdc);
			DeleteObject(map);
			ReleaseDC(hWnd,hdc);
			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

