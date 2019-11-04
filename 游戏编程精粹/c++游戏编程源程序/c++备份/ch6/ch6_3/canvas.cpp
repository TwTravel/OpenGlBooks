
#include "stdafx.h"
#include <stdio.h>

//全域變數宣告
HINSTANCE hInst;
HBITMAP bg,ball;
HDC		hdc,mdc,bufdc;
HWND	hWnd;
DWORD	tPre,tNow,tCheck;
RECT	rect;
int		x=0,y=100,vx=8,vy=0;
int		gy=1,fx=-1,fy=-4;

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
// 從檔案載入點陣圖
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

	MoveWindow(hWnd,10,10,750,400,true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);
	bufdc = CreateCompatibleDC(hdc);
	bmp = CreateCompatibleBitmap(hdc,750,400);

	SelectObject(mdc,bmp);

	bg = (HBITMAP)LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,750,400,LR_LOADFROMFILE);
	ball = (HBITMAP)LoadImage(NULL,"ball.bmp",IMAGE_BITMAP,52,26,LR_LOADFROMFILE);
	
	GetClientRect(hWnd,&rect);

	MyPaint(hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 1.視窗貼圖
// 2.根據小球的運動狀態計算速度與貼圖座標
void MyPaint(HDC hdc)
{
	SelectObject(bufdc,bg);
	BitBlt(mdc,0,0,750,400,bufdc,0,0,SRCCOPY);

	SelectObject(bufdc,ball);
	BitBlt(mdc,x,y,26,26,bufdc,26,0,SRCAND);
	BitBlt(mdc,x,y,26,26,bufdc,0,0,SRCPAINT);

	BitBlt(hdc,0,0,750,400,mdc,0,0,SRCCOPY);

	x += vx;			//計算 X 軸方向貼圖座標

	vy = vy + gy;		//計算 Y 軸方向速度分量
	y += vy;			//計算 Y 軸方向貼圖座標

	if(y >= rect.bottom-26)			//小球落地
	{
		y = rect.bottom - 26;

		//改變 X 軸方向速度分量
		vx += fx;					
		if(vx < 0)
			vx = 0;

		//改變 Y 軸方向速度分量
		vy += fy;
		if(vy < 0)
			vy = 0;

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

