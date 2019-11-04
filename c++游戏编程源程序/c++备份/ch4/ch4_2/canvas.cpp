
#include "stdafx.h"
#include <stdio.h>

//定義結構
struct BULLET
{
	int x,y;
	bool exist;
};

//全域變數宣告
HINSTANCE hInst;
HBITMAP bg,ship,bullet;
HDC		hdc,mdc,bufdc;
HWND	hWnd;
DWORD	tPre,tNow;
int		x,y,nowX,nowY;
int		w=0,bcount;
BULLET  b[30];

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
// 1.設定飛機初始位置
// 2.設定滑鼠游標位置及隱藏
// 3.限制滑鼠游標移動區域
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HBITMAP bmp;
	POINT pt,lt,rb;
	RECT rect;

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
	bullet = (HBITMAP)LoadImage(NULL,"bullet.bmp",IMAGE_BITMAP,10,20,LR_LOADFROMFILE);
	
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

	MyPaint(hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 1.設定飛機座標並進行貼圖
// 2.設定所有子彈座標並進行貼圖
// 3.顯示真正的滑鼠所在座標
void MyPaint(HDC hdc)
{
	char str[20] = "";
	int i;

	//貼上背景圖
	SelectObject(bufdc,bg);
	BitBlt(mdc,0,0,w,480,bufdc,640-w,0,SRCCOPY);
	BitBlt(mdc,w,0,640-w,480,bufdc,0,0,SRCCOPY);
	
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

	//貼上飛機圖
	SelectObject(bufdc,ship);
	BitBlt(mdc,nowX,nowY,100,74,bufdc,0,74,SRCAND);
	BitBlt(mdc,nowX,nowY,100,74,bufdc,0,0,SRCPAINT);

	SelectObject(bufdc,bullet);
	if(bcount!=0)
		for(i=0;i<30;i++)
			if(b[i].exist)
			{
				//貼上子彈圖
				BitBlt(mdc,b[i].x,b[i].y,10,10,bufdc,0,10,SRCAND);
				BitBlt(mdc,b[i].x,b[i].y,10,10,bufdc,0,0,SRCPAINT);

				b[i].x -= 10;
				if(b[i].x < 0)
				{
					bcount--;
					b[i].exist = false;
				}
			}

	//顯示滑鼠座標
	sprintf(str,"X座標：%d    ",x);
	TextOut(mdc,0,0,str,strlen(str));
	sprintf(str,"Y座標：%d    ",y);
	TextOut(mdc,0,20,str,strlen(str));

	BitBlt(hdc,0,0,640,480,mdc,0,0,SRCCOPY);

	tPre = GetTickCount();

	w += 10;
	if(w==640)
		w = 0;
}

//****訊息處理函式***********************************
// 1.處理WM_LBUTTONDOWN訊息發射子彈
// 2.處理WM_MOUSEMOVE訊息設定飛機貼圖座標
// 3.於視窗結束時恢復滑鼠移動區域
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i;

	switch (message)
	{
		case WM_KEYDOWN:				//按鍵按下訊息
			if(wParam==VK_ESCAPE)		//按下 Esc 鍵
				PostQuitMessage(0);
			break;
		case WM_LBUTTONDOWN:			//滑鼠左鍵按下訊息
			for(i=0;i<30;i++)
			{
				if(!b[i].exist)
				{
					b[i].x = nowX;		//子彈X座標
					b[i].y = nowY + 30; //子彈Y座標
					b[i].exist = true;
					bcount++;			//累加子彈數目
					break;
				}
			}
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
			DeleteObject(bullet);
			DeleteObject(ship);
			ReleaseDC(hWnd,hdc);

			PostQuitMessage(0);
			break;
		default:						//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

