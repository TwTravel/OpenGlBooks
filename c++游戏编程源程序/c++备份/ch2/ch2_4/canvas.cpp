#include "stdafx.h"

//全域變數宣告
HINSTANCE hInst;
HPEN hPen;
HBRUSH hBru[4];
int sBru[4] = {HS_VERTICAL,HS_HORIZONTAL,HS_CROSS,HS_DIAGCROSS};

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
// 建立畫筆與畫刷
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

	MoveWindow(hWnd,10,10,600,450,true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hPen = CreatePen(PS_SOLID,3,RGB(255,0,0));
	for(i=0;i<=3;i++)
	{
		hBru[i] = CreateHatchBrush(sBru[i],RGB(0,0,255));
	}

	hdc = GetDC(hWnd);
	MyPaint(hdc);
	ReleaseDC(hWnd,hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
void MyPaint(HDC hdc)
{
	SelectObject(hdc,hPen);

	SelectObject(hdc,hBru[0]);
	Ellipse(hdc,20,20,270,150);                 //畫橢圓形
	TextOut(hdc,120,160,"橢圓形",strlen("橢圓形"));

	SelectObject(hdc,hBru[1]);
	RoundRect(hdc,300,20,550,150,30,30);        //畫圓角矩形
	TextOut(hdc,400,160,"圓角矩形",strlen("圓角矩形"));

	SelectObject(hdc,hBru[2]);
	Pie(hdc,20,210,270,340,50,50,300,300);      //畫扇形
	TextOut(hdc,120,350,"扇形",strlen("扇形"));

	SelectObject(hdc,hBru[3]);
	Chord(hdc,300,210,550,340,50,50,600,300);   //畫弓形
	TextOut(hdc,400,350,"弓形",strlen("弓形"));
}

//****訊息處理函式***********************************
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
			DeleteObject(hPen);
			for(i=0;i<=3;i++)
				DeleteObject(hBru[i]);
			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}