
#include "stdafx.h"
#include <stdio.h>

//全域變數宣告
HINSTANCE hInst;
HBITMAP bg,ship,bird;
HDC		hdc,mdc,bufdc;
HWND	hWnd;
DWORD	tPre,tNow;
int		x,y,nowX,nowY;
int		w=0;
POINT	p[3];

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
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
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
// 1.設定飛機及小鳥的初始位置
// 2.設定滑鼠游標位置及隱藏
// 3.限制滑鼠游標移動區域
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HBITMAP bmp;
	POINT pt,lt,rb;
	RECT	rect;

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

	bg = (HBITMAP)LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,648,480,LR_LOADFROMFILE);
	ship = (HBITMAP)LoadImage(NULL,"ship.bmp",IMAGE_BITMAP,100,148,LR_LOADFROMFILE);
	bird = (HBITMAP)LoadImage(NULL,"bird.bmp",IMAGE_BITMAP,122,122,LR_LOADFROMFILE);

	x = 300;
	y = 300;
	nowX = 300;
	nowY = 300;

	//設定滑鼠游標位置
	pt.x = 300;
	pt.y = 300;
	ClientToScreen(hWnd,&pt);
	SetCursorPos(pt.x,pt.y);
	
	ShowCursor(false);		//隱藏滑鼠游標

	//限制滑鼠游標移動區域
	GetClientRect(hWnd,&rect);
	lt.x = rect.left;
	lt.y = rect.top;
	rb.x = rect.right;
	rb.y = rect.bottom;
	ClientToScreen(hWnd,&lt);
	ClientToScreen(hWnd,&rb);
	rect.left = lt.x;
	rect.top = lt.y;
	rect.right = rb.x;
	rect.bottom = rb.y;
	ClipCursor(&rect);

	p[0].x = 30;
	p[0].y = 100;

	p[1].x = 250;
	p[1].y = 250;

	p[2].x = 500;
	p[2].y = 400;

	MyPaint(hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 1.設定飛機座標並進行貼圖
// 2.設定小鳥座標並進行貼圖
void MyPaint(HDC hdc)
{
	int i;

	//貼上背景圖
	SelectObject(bufdc,bg);
	BitBlt(mdc,0,0,w,480,bufdc,640-w,0,SRCCOPY);
	BitBlt(mdc,w,0,640-w,480,bufdc,0,0,SRCCOPY);

	//貼上飛機圖
	if(nowX < x)
	{
		nowX += 10;
		if(nowX > x)
			nowX = x;
	}
	else
	{
		nowX -=10;
		if(nowX < x)
			nowX = x;
	}

	if(nowY < y)
	{
		nowY += 10;
		if(nowY > y)
			nowY = y;
	}
	else
	{
		nowY -= 10;
		if(nowY < y)
			nowY = y;
	}
	SelectObject(bufdc,ship);
	BitBlt(mdc,nowX,nowY,100,74,bufdc,0,74,SRCAND);
	BitBlt(mdc,nowX,nowY,100,74,bufdc,0,0,SRCPAINT);

	//貼上小鳥圖
	SelectObject(bufdc,bird);
	for(i=0;i<3;i++)
	{
		if(rand()%3 != 1)		//設定2/3機率進行追逐
		{
			if(p[i].y > nowY-16)
				p[i].y -= 5;
			else
				p[i].y += 5;
	
			if(p[i].x > nowX-25)
				p[i].x -= 5;
			else
				p[i].x += 5;
		}
		
		if(p[i].x > nowX-25)    //判斷小鳥移動方向
		{
			BitBlt(mdc,p[i].x,p[i].y,61,61,bufdc,61,61,SRCAND);
			BitBlt(mdc,p[i].x,p[i].y,61,61,bufdc,0,61,SRCPAINT);
		}
		else
		{
			BitBlt(mdc,p[i].x,p[i].y,61,61,bufdc,61,0,SRCAND);
			BitBlt(mdc,p[i].x,p[i].y,61,61,bufdc,0,0,SRCPAINT);
		}
	}

	BitBlt(hdc,0,0,640,480,mdc,0,0,SRCCOPY);

	tPre = GetTickCount();

	w += 10;
	if(w==640)
		w = 0;
}

//****訊息處理函式***********************************
// 1.處理WM_MOUSEMOVE訊息設定飛機貼圖座標
// 2.於視窗結束時恢復滑鼠移動區域
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:				//按鍵按下訊息
			if(wParam==VK_ESCAPE)		//按下 Esc 鍵
				PostQuitMessage(0);
			break;
		case WM_MOUSEMOVE:
			x = LOWORD(lParam);			//取得滑鼠 X 座標
			if(x > 530)
				x = 530;
			else if(x < 0)
				x = 0;

			y = HIWORD(lParam);			//取得滑鼠 Y 座標
			if(y > 380)
				y = 380;
			else if(y < 0)
				y = 0;
				
			break;
		case WM_DESTROY:				//視窗結束訊息
			ClipCursor(NULL);			//恢復滑鼠移動區域

			DeleteDC(mdc);
			DeleteDC(bufdc);
			DeleteObject(bg);
			DeleteObject(ship);
			DeleteObject(bird);
			ReleaseDC(hWnd,hdc);

			PostQuitMessage(0);
			break;
		default:						//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

