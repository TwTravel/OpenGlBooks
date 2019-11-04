
#include "stdafx.h"
#include <stdio.h>

//全域變數宣告
HINSTANCE hInst;
HBITMAP bg,girl;
HDC		mdc;

//常數定義
const int xstart = 50;
const int ystart = 20;

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
// 半透明處理，處理後將半透明圖存回 girl
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	HDC hdc;

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

	BITMAP bm1,bm2;
	unsigned char *px1,*px2;

	//處理背景圖
	bg = (HBITMAP)LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,600,450,LR_LOADFROMFILE); 
	GetObject(bg,sizeof(BITMAP),&bm1);

	if(bm1.bmBitsPixel != 32 && bm1.bmBitsPixel != 24) 
	{
		MessageBox(NULL,"此程式只能在 32 bit 或 24 bit 顯示模式中執行","警告",0);
		return FALSE;
	}

	px1 = new unsigned char [bm1.bmHeight * bm1.bmWidthBytes];
	GetBitmapBits(bg,bm1.bmHeight * bm1.bmWidthBytes,px1);

	//處理前景圖
	girl = (HBITMAP)LoadImage(NULL,"girl.bmp",IMAGE_BITMAP,298,329,LR_LOADFROMFILE); 
	GetObject(girl,sizeof(BITMAP),&bm2);
	px2 = new unsigned char [bm2.bmHeight * bm2.bmWidthBytes];
	GetBitmapBits(girl,bm2.bmHeight * bm2.bmWidthBytes,px2);

	int xend,yend;
	int x,y,i;				//迴圈用變數
	int rgb_b;
	int PxBytes = bm1.bmBitsPixel / 8 ;

	xend = xstart + 298;
	yend = ystart + 329;

	//處理背景圖像素顏色
	for(y=ystart;y<yend;y++) 	
	{
		for(x=xstart;x<xend;x++) 
		{
			rgb_b = y * bm1.bmWidthBytes + x * PxBytes ;

			px1[rgb_b] = px1[rgb_b] * 0.7;		
			px1[rgb_b+1] = px1[rgb_b+1] * 0.7;	
			px1[rgb_b+2] = px1[rgb_b+2] * 0.7;	
		}
	}

	//處理前景圖像素顏色
	for(y=0;y<(bm2.bmHeight); y++) 	
	{
		for(x=0;x<bm2.bmWidth; x++) 
		{
			rgb_b = y * bm2.bmWidthBytes + x * PxBytes ;
			i = (ystart+y) * bm1.bmWidthBytes + (xstart+x) * PxBytes;

			px2[rgb_b]   = px2[rgb_b]  *0.3 + px1[i];	
			px2[rgb_b+1] = px2[rgb_b+1] *0.3 + px1[i+1];	
			px2[rgb_b+2] = px2[rgb_b+2] *0.3 + px1[i+2];	
		}
	}

	SetBitmapBits(girl,bm2.bmHeight*bm2.bmWidthBytes,px2);

	MyPaint(hdc);

	ReleaseDC(hWnd,hdc);
	delete [] px1;
	delete [] px2;

	return TRUE;
}

//****自訂繪圖函式*********************************
void MyPaint(HDC hdc)
{
	//貼上背景圖
	SelectObject(mdc,bg);
	BitBlt(hdc,0,0,600,450,mdc,0,0,SRCCOPY);

	//貼上處理後的半透明圖
	SelectObject(mdc,girl);
	BitBlt(hdc,xstart,ystart,298,329,mdc,0,0,SRCCOPY);
}

//****訊息處理函式***********************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
		case WM_PAINT:						//視窗重繪訊息
			hdc = BeginPaint(hWnd, &ps);
			MyPaint(hdc);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:					//視窗結束訊息
			DeleteDC(mdc);
			DeleteObject(bg);
			DeleteObject(girl);
			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

