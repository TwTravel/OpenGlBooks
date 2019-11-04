
#include "stdafx.h"

//全域變數宣告
HINSTANCE hInst;
HBITMAP dra,bg;
HDC		hdc,mdc,bufdc;
HWND	hWnd;
DWORD	tPre,tNow;
int		num,x,y;

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
			if(tNow-tPre >= 100)
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
// 載入點陣圖並設定各物件初始值
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	char filename[20] = "";
	HBITMAP bmp;
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
	bufdc = CreateCompatibleDC(hdc);
	bmp = CreateCompatibleBitmap(hdc,640,480);

	SelectObject(mdc,bmp);

	dra = (HBITMAP)LoadImage(NULL,"dra.bmp",IMAGE_BITMAP,760,198,LR_LOADFROMFILE);
	bg = (HBITMAP)LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,640,480,LR_LOADFROMFILE);

	num = 0;    //顯示圖號
	x = 640;	//貼圖起始 X 座標
	y = 360;    //貼圖起始 Y 座標

	MyPaint(hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 1.恐龍跑動圖案透空
// 2.更新貼圖座標
void MyPaint(HDC hdc)
{
	if(num == 8)
		num = 0;

	//於mdc中貼上背景圖
	SelectObject(bufdc,bg);
	BitBlt(mdc,0,0,640,480,bufdc,0,0,SRCCOPY);

	//於mdc上進行透空處理
	SelectObject(bufdc,dra);
	BitBlt(mdc,x,y,95,99,bufdc,num*95,99,SRCAND);
	BitBlt(mdc,x,y,95,99,bufdc,num*95,0,SRCPAINT);

	//將最後畫面顯示於視窗中
	BitBlt(hdc,0,0,640,480,mdc,0,0,SRCCOPY);

	tPre = GetTickCount();     //記錄此次繪圖時間
	num++;

	x-=20;					   //計算下次貼圖座標
	if(x<=-95)
		x = 640;
}

//****訊息處理函式***********************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:					//視窗結束訊息
			DeleteDC(mdc);
			DeleteDC(bufdc);
			DeleteObject(dra);
			DeleteObject(bg);
			ReleaseDC(hWnd,hdc);
			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

