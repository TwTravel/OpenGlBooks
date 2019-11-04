#include "stdafx.h"

//全域變數宣告
HINSTANCE hInst;
HPEN hPen;
HBRUSH hBru;
POINT poly1[5],poly2[5],poly3[5];

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
// 1.各多邊形頂點陣列初始化
// 2.建立畫筆與畫刷
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	HDC hdc;
	int i;
	const double pi = 3.1415926535;

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

	for(i=0;i<=4;i++)
	{	
		poly1[i].x = 100 + 100 * sin(i*72*pi/180);
		poly1[i].y = 100 + 100 * cos(i*72*pi/180);

		poly2[i].x = poly1[i].x + 300;
		poly2[i].y = poly1[i].y;

		poly3[i].x = poly1[i].x + 180;
		poly3[i].y = poly1[i].y + 200;
	}

	hPen = CreatePen(PS_SOLID,5,RGB(255,0,0));
	hBru = CreateHatchBrush(HS_BDIAGONAL,RGB(0,255,0));

	hdc = GetDC(hWnd);
	MyPaint(hdc);
	ReleaseDC(hWnd,hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 繪製多邊形
void MyPaint(HDC hdc)
{
	SelectObject(hdc,hPen);
	SelectObject(hdc,hBru);
	PolylineTo(hdc,poly1,5);
	Polyline(hdc,poly2,5);
	Polygon(hdc,poly3,5);
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
			DeleteObject(hPen);
			DeleteObject(hBru);
			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}