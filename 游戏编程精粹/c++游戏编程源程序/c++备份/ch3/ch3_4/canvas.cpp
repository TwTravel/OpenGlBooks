
#include "stdafx.h"
#include <stdio.h>

//定義結構
struct dragon
{
	int x,y;
	int dir;
};

//常數定義與全域變數宣告
const int draNum = 10;

HINSTANCE hInst;
HBITMAP draPic[4],bg;
HDC		hdc,mdc,bufdc;
HWND	hWnd;
DWORD	tPre,tNow;
int		picNum;
dragon  dra[draNum];


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
// 載入點陣圖並設定各初始值
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HBITMAP bmp;
	hInst = hInstance;
	int i;

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

	draPic[0] = (HBITMAP)LoadImage(NULL,"dra0.bmp",IMAGE_BITMAP,528,188,LR_LOADFROMFILE);
	draPic[1] = (HBITMAP)LoadImage(NULL,"dra1.bmp",IMAGE_BITMAP,544,164,LR_LOADFROMFILE);
	draPic[2] = (HBITMAP)LoadImage(NULL,"dra2.bmp",IMAGE_BITMAP,760,198,LR_LOADFROMFILE);
	draPic[3] = (HBITMAP)LoadImage(NULL,"dra3.bmp",IMAGE_BITMAP,760,198,LR_LOADFROMFILE);
	bg = (HBITMAP)LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,640,480,LR_LOADFROMFILE);

	for(i=0;i<draNum;i++)
	{
		dra[i].dir = 3;    //起始方向
		dra[i].x = 200;	   //貼圖起始 X 座標
		dra[i].y = 200;    //貼圖起始 Y 座標
	}

	MyPaint(hdc);

	return TRUE;
}

//氣泡排序法
void BubSort(int n)
{
	int i,j;
	bool f;
	dragon tmp;

	for(i=0;i<n-1;i++)
	{
		f = false;
		for(j=0;j<n-i-1;j++)
		{
			if(dra[j+1].y < dra[j].y)
			{
				tmp = dra[j+1];
				dra[j+1] = dra[j];
				dra[j] = tmp;
				f = true;
			}
		}
		if(!f)
			break;
	}
}

//****自訂繪圖函式*********************************
// 1.對視窗中跑動的恐龍進行排序貼圖
// 2.恐龍貼圖座標修正
void MyPaint(HDC hdc)
{
	int w,h,i;

	if(picNum == 8)
		picNum = 0;

	//於mdc中先貼上背景圖
	SelectObject(bufdc,bg);
	BitBlt(mdc,0,0,640,480,bufdc,0,0,SRCCOPY);

	BubSort(draNum);

	for(i=0;i<draNum;i++)
	{
		SelectObject(bufdc,draPic[dra[i].dir]);
		switch(dra[i].dir)
		{
			case 0:
				w = 66;
				h = 94;
				break;
			case 1:
				w = 68;
				h = 82;
				break;
			case 2:
				w = 95;
				h = 99;
				break;
			case 3:
				w = 95;
				h = 99;
				break;
		}
		BitBlt(mdc,dra[i].x,dra[i].y,w,h,bufdc,picNum*w,h,SRCAND);
		BitBlt(mdc,dra[i].x,dra[i].y,w,h,bufdc,picNum*w,0,SRCPAINT);
	}

	//將最後畫面顯示於視窗中
	BitBlt(hdc,0,0,640,480,mdc,0,0,SRCCOPY);

	tPre = GetTickCount();         //記錄此次繪圖時間
	picNum++;

	for(i=0;i<draNum;i++)
	{
		switch(rand()%4)          //隨機決定下次移動方向
		{
			case 0:					     //上
				switch(dra[i].dir)
				{
					case 0:	
						dra[i].y -= 20;
						break;
					case 1:
						dra[i].x += 2;
						dra[i].y -= 31;
						break;
					case 2:	
						dra[i].x += 14;
						dra[i].y -= 20;
						break;
					case 3:
						dra[i].x += 14;
						dra[i].y -= 20;
						break;
				}
				if(dra[i].y < 0)
					dra[i].y = 0;
				dra[i].dir = 0;
				break;
			case 1:				     	//下
				switch(dra[i].dir)
				{
					case 0:
						dra[i].x -= 2;
						dra[i].y += 31;
						break;
					case 1:
						dra[i].y += 20;
						break;
					case 2:
						dra[i].x += 15;
						dra[i].y += 29;
						break;
					case 3:
						dra[i].x += 15;
						dra[i].y += 29;
						break;
				}

				if(dra[i].y > 370)
					dra[i].y = 370;
				dra[i].dir = 1;
				break;
			case 2:				    	//左
				switch(dra[i].dir)
				{
					case 0:
						dra[i].x -= 34;
						break;
					case 1:
						dra[i].x -= 34;
						dra[i].y -= 9;
						break;
					case 2:
						dra[i].x -= 20;
						break;
					case 3:
						dra[i].x -= 20;
						break;
				}
				if(dra[i].x < 0)
					dra[i].x = 0;
				dra[i].dir = 2;
				break;
			case 3:				    	//右
				switch(dra[i].dir)
				{
					case 0:
						dra[i].x += 6;
						break;
					case 1:
						dra[i].x += 6;
						dra[i].y -= 10;
						break;
					case 2:
						dra[i].x += 20;
						break;
					case 3:
						dra[i].x += 20;
						break;
				}
				if(dra[i].x > 535)
					dra[i].x = 535;
				dra[i].dir = 3;
				break;
		}
	}
}

//****訊息處理函式***********************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		int i;

		case WM_DESTROY:					//視窗結束訊息
			DeleteDC(mdc);
			DeleteDC(bufdc);
			for(i=0;i<4;i++)
				DeleteObject(draPic[i]);
			DeleteObject(bg);
			ReleaseDC(hWnd,hdc);
			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

