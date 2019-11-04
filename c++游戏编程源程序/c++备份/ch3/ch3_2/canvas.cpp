
#include "stdafx.h"
#include <stdio.h>

//全域變數宣告
HINSTANCE hInst;
HBITMAP man[7];
HDC		hdc,mdc;
HWND	hWnd;
DWORD	tPre,tNow,tCheck;
int		num,frame,fps;

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
// 從檔案載入點陣圖
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	char filename[20] = "";
	int i;

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

	//載入各人物點陣圖
	for(i=0;i<7;i++)
	{
		sprintf(filename,"man%d.bmp",i);
		man[i] = (HBITMAP)LoadImage(NULL,filename,IMAGE_BITMAP,640,480,LR_LOADFROMFILE);
	}
	
	num = 0;
	frame = 0;

	MyPaint(hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 1.計算與顯示每秒畫面更新次數
// 2.依圖號順序進行視窗貼圖
void MyPaint(HDC hdc)
{
	char str[40] = "";

	if(num == 7)
		num = 0;
	frame++;			       //畫面更新次數加1
	if(tNow - tCheck >= 1000)
	{
		fps = frame;
		frame = 0;
		tCheck = tNow;
	}

	SelectObject(mdc,man[num]);
	sprintf(str,"每秒鐘顯示 %d  個畫面  ",fps);
	TextOut(mdc,0,0,str,strlen(str));
	BitBlt(hdc,0,0,600,450,mdc,0,0,SRCCOPY);

	tPre = GetTickCount();     //記錄此次繪圖時間
	num++;
}

//****訊息處理函式***********************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i;

	switch (message)
	{
		case WM_DESTROY:					//視窗結束訊息
			DeleteDC(mdc);
			for(i=0;i<7;i++)
				DeleteObject(man[i]);
			ReleaseDC(hWnd,hdc);
			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

