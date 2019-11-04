
#include "stdafx.h"
#include <stdio.h>

//全域變數宣告
HINSTANCE hInst;
HBITMAP girl[7];
HDC		mdc,hdc;
int		num;

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

	//訊息迴圈
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
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
// 1.從檔案載入點陣圖
// 2.建立計時器
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
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
		sprintf(filename,"girl%d.bmp",i);
		girl[i] = (HBITMAP)LoadImage(NULL,filename,IMAGE_BITMAP,640,480,LR_LOADFROMFILE);
	}
	
	num = 0;
	SetTimer(hWnd,1,500,NULL);

	MyPaint(hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 依圖號順序進行視窗貼圖
void MyPaint(HDC hdc)
{
	if(num == 7)
		num = 0;

	SelectObject(mdc,girl[num]);
	BitBlt(hdc,0,0,600,450,mdc,0,0,SRCCOPY);

	num++;
}

//****訊息處理函式***********************************
// 1.加入處理WM_TIMER訊息
// 2.於視窗結束時刪除計時器
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i;

	switch (message)
	{
		case WM_TIMER:						//時間訊息
			MyPaint(hdc);
			break;
		case WM_DESTROY:					//視窗結束訊息
			DeleteDC(mdc);
			ReleaseDC(hWnd,hdc);
			for(i=0;i<7;i++)
				DeleteObject(girl[i]);

			KillTimer(hWnd,1);

			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}