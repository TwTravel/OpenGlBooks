
#include "stdafx.h"

//全域變數宣告
HINSTANCE hInst;
HPEN hPen[7];
HBRUSH hBru[7];
int sPen[7] = {PS_SOLID,PS_DASH,PS_DOT,PS_DASHDOT,PS_DASHDOTDOT,PS_NULL,PS_INSIDEFRAME};
int sBru[6] = {HS_VERTICAL,HS_HORIZONTAL,HS_CROSS,HS_DIAGCROSS,HS_FDIAGONAL,HS_BDIAGONAL};

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
// 建立 7 種不同的畫筆及畫刷物件
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	HDC hdc;
	int i;

	hInst = hInstance;

	hWnd = CreateWindow("canvas", "繪圖視窗" , WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	MoveWindow(hWnd,10,10,650,350,true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	for(i=0;i<=6;i++)
	{
		hPen[i] = CreatePen(sPen[i],1,RGB(255,0,0));
		if(i==6)
			hBru[i] = CreateSolidBrush(RGB(0,255,0));
		else
			hBru[i] = CreateHatchBrush(sBru[i],RGB(0,255,0));
	}

	hdc = GetDC(hWnd);
	MyPaint(hdc);
	ReleaseDC(hWnd,hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 以各式畫筆及畫刷繪製線條與填充矩形
void MyPaint(HDC hdc)
{
	int i,x1,x2,y;
	
	//以 7 種不同畫筆繪製線條
	for(i=0;i<=6;i++)
	{
		y = (i+1) * 30;

		SelectObject(hdc,hPen[i]);
		MoveToEx(hdc,30,y,NULL);	 //移動線條起點
		LineTo(hdc,100,y);			 //畫線
	}
	
	x1 = 120;
	x2 = 180;

	//以 7 種不同畫刷填充矩形
	for(i=0;i<=6;i++)
	{
		SelectObject(hdc,hBru[i]);
		Rectangle(hdc,x1,30,x2,y);	 //畫封閉矩形
		x1 += 70;
		x2 += 70;
	}
}
//****訊息處理函式***********************************
// 1.視窗重繪訊息發生時呼叫MyPaint()
// 2.視窗結束訊息發生時刪除GDI物件
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	int i;

	switch (message) 
	{
		case WM_PAINT:						//視窗重繪訊息
			hdc = BeginPaint(hWnd, &ps);
			MyPaint(hdc);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:					//視窗結束訊息
			for(i=0;i<=6;i++)
			{
				DeleteObject(hPen[i]);		//刪除畫筆
				DeleteObject(hBru[i]);		//刪除畫刷
			}
			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

