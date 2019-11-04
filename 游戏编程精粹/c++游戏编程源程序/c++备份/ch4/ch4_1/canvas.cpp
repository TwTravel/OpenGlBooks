
#include "stdafx.h"
#include <stdio.h>

//全域變數宣告
HINSTANCE hInst;
HBITMAP girl[4],bg;
HDC		hdc,mdc,bufdc;
HWND	hWnd;
DWORD	tPre,tNow;
int		num,dir,x,y;

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
// 載入點陣圖並設定各初始值
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

	MoveWindow(hWnd,10,10,640,480,true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);
	bufdc = CreateCompatibleDC(hdc);

	bmp = CreateCompatibleBitmap(hdc,640,480);
	SelectObject(mdc,bmp);

	x = 300;
	y = 250;
	dir = 0;
	num = 0;

	girl[0] = (HBITMAP)LoadImage(NULL,"girl0.bmp",IMAGE_BITMAP,440,148,LR_LOADFROMFILE);
	girl[1] = (HBITMAP)LoadImage(NULL,"girl1.bmp",IMAGE_BITMAP,424,154,LR_LOADFROMFILE);
	girl[2] = (HBITMAP)LoadImage(NULL,"girl2.bmp",IMAGE_BITMAP,480,148,LR_LOADFROMFILE);
	girl[3] = (HBITMAP)LoadImage(NULL,"girl3.bmp",IMAGE_BITMAP,480,148,LR_LOADFROMFILE);
	bg = (HBITMAP)LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,640,480,LR_LOADFROMFILE);

	MyPaint(hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 人物貼圖座標修正及視窗貼圖
void MyPaint(HDC hdc)
{
	int w,h;

	SelectObject(bufdc,bg);
	BitBlt(mdc,0,0,640,480,bufdc,0,0,SRCCOPY);

	SelectObject(bufdc,girl[dir]);
	switch(dir)
	{
		case 0:
			w = 55;
			h = 74;
			break;
		case 1:
			w = 53;
			h = 77;
			break;
		case 2:
			w = 60;
			h = 74;
			break;
		case 3:
			w = 60;
			h = 74;
			break;
	}
	BitBlt(mdc,x,y,w,h,bufdc,num*w,h,SRCAND);
	BitBlt(mdc,x,y,w,h,bufdc,num*w,0,SRCPAINT);
	
	BitBlt(hdc,0,0,640,480,mdc,0,0,SRCCOPY);

	tPre = GetTickCount();         //記錄此次繪圖時間

	num++;
	if(num == 8)
		num = 0;

}

//****訊息處理函式***********************************
// 1.按下Esc鍵結束程式
// 2.按下方向鍵重設貼圖座標
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:			      //按鍵按下訊息
			switch (wParam) 
			{
				case VK_ESCAPE:           //按下 Esc 鍵
					PostQuitMessage( 0 );
					break;
				case VK_UP:				  //按下上鍵
					switch(dir)
					{
						case 0:	
							y -= 10;
							break;
						case 1:
							x -= 1;
							y -= 8;
							break;
						case 2:	
							x += 2;
							y -= 10;
							break;
						case 3:
							x += 2;
							y -= 10;
							break;
					}
					if(y < 0)
						y = 0;
					dir = 0;
					break;
				case VK_DOWN:			  //按下下鍵
					switch(dir)
					{
						case 0:
							x += 1;
							y += 8;
							break;
						case 1:
							y += 10;
							break;
						case 2:
							x += 3;
							y += 6;
							break;
						case 3:
							x += 3;
							y += 6;
							break;
					}

					if(y > 375)
						y = 375;
					dir = 1;
					break;
				case VK_LEFT:			  //按下左鍵
					switch(dir)
					{
						case 0:
							x -= 12;
							break;
						case 1:
							x -= 13;
							y += 4;
							break;
						case 2:
							x -= 10;
							break;
						case 3:
							x -= 10;
							break;
					}
					if(x < 0)
						x = 0;
					dir = 2;
					break;
				case VK_RIGHT:			   //按下右鍵
					switch(dir)
					{
						case 0:
							x += 8;
							break;
						case 1:
							x += 7;
							y += 4;
							break;
						case 2:
							x += 10;
							break;
						case 3:
							x += 10;
							break;
					}
					if(x > 575)
						x = 575;
					dir = 3;
					break;
			}
			break;
		case WM_DESTROY:			    	//視窗結束訊息
			int i;

			DeleteDC(mdc);
			DeleteDC(bufdc);
			for(i=0;i<4;i++)
				DeleteObject(girl[i]);
			DeleteObject(bg);
			ReleaseDC(hWnd,hdc);

			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

