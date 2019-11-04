
#include "stdafx.h"

//全域變數宣告
HINSTANCE hInst;
HBITMAP dra,bg[3];
HDC		hdc,mdc,bufdc;
HWND	hWnd;
DWORD	tPre,tNow;
int		x0=0,x1=0,x2=0,num=0;

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
// 載入點載圖並建立空點陣圖選用到mdc中
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

	bg[0] = (HBITMAP)LoadImage(NULL,"bg0.bmp",IMAGE_BITMAP,640,480,LR_LOADFROMFILE);
	bg[1] = (HBITMAP)LoadImage(NULL,"bg1.bmp",IMAGE_BITMAP,640,600,LR_LOADFROMFILE);
	bg[2] = (HBITMAP)LoadImage(NULL,"bg2.bmp",IMAGE_BITMAP,640,600,LR_LOADFROMFILE);
	dra = (HBITMAP)LoadImage(NULL,"dra.bmp",IMAGE_BITMAP,760,198,LR_LOADFROMFILE);

	MyPaint(hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 1.依各背景遠近順序進行循環背景貼圖
// 2.進行前景恐龍圖的透空貼圖
// 3.重設各背景圖切割寬度與跑動恐龍圖圖號
void MyPaint(HDC hdc)
{
	//貼上天空圖
	SelectObject(bufdc,bg[0]);
	BitBlt(mdc,0,0,x0,300,bufdc,640-x0,0,SRCCOPY);
	BitBlt(mdc,x0,0,640-x0,300,bufdc,0,0,SRCCOPY);

	//貼上草地圖
	BitBlt(mdc,0,300,x2,180,bufdc,640-x2,300,SRCCOPY);
	BitBlt(mdc,x2,300,640-x2,180,bufdc,0,300,SRCCOPY);

	//貼上山巒圖並透空
	SelectObject(bufdc,bg[1]);
	BitBlt(mdc,0,0,x1,300,bufdc,640-x1,300,SRCAND);
	BitBlt(mdc,x1,0,640-x1,300,bufdc,0,300,SRCAND);
	BitBlt(mdc,0,0,x1,300,bufdc,640-x1,0,SRCPAINT);
	BitBlt(mdc,x1,0,640-x1,300,bufdc,0,0,SRCPAINT);

	//貼上房屋圖並透空
	SelectObject(bufdc,bg[2]);
	BitBlt(mdc,0,250,x2,300,bufdc,640-x2,300,SRCAND);
	BitBlt(mdc,x2,250,640-x2,300,bufdc,0,300,SRCAND);
	BitBlt(mdc,0,250,x2,300,bufdc,640-x2,0,SRCPAINT);
	BitBlt(mdc,x2,250,640-x2,300,bufdc,0,0,SRCPAINT);

	//貼上恐龍圖並透空
	SelectObject(bufdc,dra);
	BitBlt(mdc,250,350,95,99,bufdc,num*95,99,SRCAND);
	BitBlt(mdc,250,350,95,99,bufdc,num*95,0,SRCPAINT);

	BitBlt(hdc,0,0,640,480,mdc,0,0,SRCCOPY);

	tPre = GetTickCount();

	x0 += 5;		//重設天空背景切割寬度
	if(x0==640)
		x0 = 0;

	x1 += 8;		//重設山蠻背景切割寬度
	if(x1==640)
		x1 = 0;

	x2 += 16;		//重設草地及房屋背景切割寬度
	if(x2==640)
		x2 = 0;

	num++;			//重設跑動圖號
	if(num == 8)
		num = 0;
}

//****訊息處理函式***********************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:					//視窗結束訊息
			DeleteDC(mdc);
			DeleteDC(bufdc);
			DeleteObject(bg[0]);
			DeleteObject(bg[1]);
			DeleteObject(bg[2]);
			DeleteObject(dra);
			ReleaseDC(hWnd,hdc);
			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

