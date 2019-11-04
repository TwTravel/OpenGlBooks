
#include "stdafx.h"
#include <stdio.h>

//全域變數宣告
HINSTANCE hInst;
HBITMAP bg,ball;
HDC		hdc,mdc,bufdc;
HWND	hWnd;
DWORD	tPre,tNow,tCheck;
RECT	rect;
int		x=0,y=100,vx=5,vy=0,gy=1;

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
			if(tNow-tPre >= 50)
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
// 載入點陣圖與取得內部視窗區域
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HBITMAP bmp;
	hInst = hInstance;

	hWnd = CreateWindow("canvas", "繪圖視窗" , WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	MoveWindow(hWnd,10,10,600,450,true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);
	bufdc = CreateCompatibleDC(hdc);
	bmp = CreateCompatibleBitmap(hdc,640,480);

	SelectObject(mdc,bmp);

	bg = (HBITMAP)LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,640,480,LR_LOADFROMFILE);
	ball = (HBITMAP)LoadImage(NULL,"ball.bmp",IMAGE_BITMAP,52,26,LR_LOADFROMFILE);
	
	GetClientRect(hWnd,&rect);
	SelectObject(bufdc,bg);
	BitBlt(mdc,0,0,640,480,bufdc,0,0,SRCCOPY);
	MyPaint(hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 1.視窗貼圖
// 2.計算小球速度、座標以及判斷是否碰到視窗下緣
void MyPaint(HDC hdc)
{
	SelectObject(bufdc,bg);
	BitBlt(mdc,0,0,640,480,bufdc,0,0,SRCCOPY);

	SelectObject(bufdc,ball);
	BitBlt(mdc,x,y,26,26,bufdc,26,0,SRCAND);
	BitBlt(mdc,x,y,26,26,bufdc,0,0,SRCPAINT);

	BitBlt(hdc,0,0,640,480,mdc,0,0,SRCCOPY);

	x += vx;			//計算 X 軸方向貼圖座標

	vy = vy + gy;		//計算 Y 軸方向速度分量
	y += vy;			//計算 Y 軸方向貼圖座標
	if(y >= rect.bottom-26)
	{
		y = rect.bottom - 26;
		vy = -vy;
	}

	tPre = GetTickCount();     //記錄此次繪圖時間
}

//****訊息處理函式***********************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:					//按鍵按下訊息
			if(wParam==VK_ESCAPE)			//按下 Esc 鍵
				PostQuitMessage(0);
			break;
		case WM_DESTROY:					//視窗結束訊息
			DeleteDC(mdc);
			DeleteDC(bufdc);
			DeleteObject(bg);
			DeleteObject(ball);
			ReleaseDC(hWnd,hdc);
			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

