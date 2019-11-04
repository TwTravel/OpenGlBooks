
#include "stdafx.h"
#include <stdio.h>

//全域變數宣告
HINSTANCE hInst;
HBITMAP fullmap;
HDC		mdc;

//常數定義
const int rows = 10,cols = 10;

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
// 宣告地圖陣列，進行圖塊貼圖，完成地圖拼接
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	HDC hdc,bufdc;

	hInst = hInstance;

	hWnd = CreateWindow("canvas", "繪圖視窗" , WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	MoveWindow(hWnd,10,10,640,400,true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	int mapIndex[rows*cols] = { 2,2,2,2,2,0,1,0,1,0,  //第1列
								3,3,2,2,0,0,0,1,1,0,  //第2列
								3,0,0,0,0,0,0,0,1,2,  //第3列
								2,2,0,0,0,0,0,2,2,2,  //第4列
								2,2,0,0,0,0,2,2,2,2,  //第5列
								2,2,0,0,0,2,2,0,0,2,  //第6列
								2,0,0,2,2,2,0,0,1,0,  //第7列
								0,0,2,0,0,0,1,1,1,1,  //第8列
								0,2,0,3,3,3,3,3,3,1,  //第9列
								2,0,3,3,3,3,3,3,3,3 };//第10列

	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);
	bufdc = CreateCompatibleDC(hdc);

	HBITMAP map[4];
	char filename[20] = "";
	int rowNum,colNum;
	int i,x,y;
	int xstart,ystart;
	
	xstart = 32 * (rows-1);
	ystart = 0;

	fullmap = (HBITMAP)LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,640,480,LR_LOADFROMFILE); 
	SelectObject(mdc,fullmap);

	//載入各圖塊點陣圖
	for(i=0;i<4;i++)
	{
		sprintf(filename,"map%d.bmp",i);
		map[i] = (HBITMAP)LoadImage(NULL,filename,IMAGE_BITMAP,128,32,LR_LOADFROMFILE);
	}

	//依 mapIndex 陣列中的定義取出對應圖塊，進行地圖拼接
	for (i=0;i<rows*cols;i++)
	{
		SelectObject(bufdc,map[mapIndex[i]]);

		rowNum = i / cols;     //求列編號
		colNum = i % cols;     //求行編號
		x = xstart + colNum * 32 + rowNum * (-32);  //求貼圖 X 座標
		y = ystart + rowNum * 16 + colNum * 16;     //求貼圖 Y 座標
		
		BitBlt(mdc,x,y,64,32,bufdc,64,0,SRCAND);
		BitBlt(mdc,x,y,64,32,bufdc,0,0,SRCPAINT);
	}

	MyPaint(hdc);

	ReleaseDC(hWnd,hdc);
	DeleteDC(bufdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
void MyPaint(HDC hdc)
{
	//貼上拼接後的組合地圖
	SelectObject(mdc,fullmap);
	BitBlt(hdc,0,0,640,480,mdc,0,0,SRCCOPY);
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
			DeleteObject(fullmap);
			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

