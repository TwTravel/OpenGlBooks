//引用標頭檔
#include "stdafx.h"
#include <stack>
using namespace std;

//常數定義
const int rows = 8,cols = 8;

//全域變數宣告
HINSTANCE hInst;
HBITMAP ball;
HDC		hdc,mdc,bufdc;
HWND	hWnd;
DWORD	tPre,tNow;
char	*str;
int		nowPos,prePos;
bool	find;
stack<int> path;

int mapIndex[rows*cols] = { 0,2,0,0,0,0,0,0,   //第1列
							0,1,0,1,1,1,1,0,   //第2列
							0,1,0,1,0,1,1,0,   //第3列
							0,1,0,0,0,1,1,0,   //第4列
							0,1,1,1,1,1,1,0,   //第5列
							0,1,0,0,0,0,1,0,   //第6列
							0,0,1,1,1,1,1,0,   //第7列
							0,0,0,0,0,0,3,0 }; //第8列
int record[rows*cols];

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
// 1.迷宮拼接貼圖
// 2.設定小球於迷宮入口處
// 3.依 mapIndex 設定 record 陣列的初始內容
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

	MoveWindow(hWnd,10,10,430,450,true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);
	bufdc = CreateCompatibleDC(hdc);

	bmp = CreateCompatibleBitmap(hdc,cols*50,rows*50);
	SelectObject(mdc,bmp);

	HBITMAP tile;
	int rowNum,colNum;
	int i,x,y;

	tile = (HBITMAP)LoadImage(NULL,"tile.bmp",IMAGE_BITMAP,50,50,LR_LOADFROMFILE);
	ball = (HBITMAP)LoadImage(NULL,"ball.bmp",IMAGE_BITMAP,50,50,LR_LOADFROMFILE);

	//依 mapIndex 陣列中的定義進行迷宮拼接
	for (i=0;i<rows*cols;i++)
	{
		record[i] = mapIndex[i];

		rowNum = i / cols;			//求列編號
		colNum = i % cols;			//求行編號
		x = colNum * 50;			//求貼圖 X 座標
		y = rowNum * 50;			//求貼圖 Y 座標
		
		SelectObject(bufdc,tile);

		if(!mapIndex[i])
			BitBlt(mdc,x,y,50,50,bufdc,0,0,SRCCOPY);
		else
		{
			if(mapIndex[i] == 2)	//找到迷宮出口
			{
				nowPos = i;
				path.push(i);
				record[i] = 0;
			}
			BitBlt(mdc,x,y,50,50,bufdc,0,0,WHITENESS);
		}
	}
	prePos = cols * rows + 1;

	MyPaint(hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 搜尋可行路徑及小球移動貼圖
void MyPaint(HDC hdc)
{
	int rowNum,colNum;
	int x,y;
	int up,down,left,right;

	//清除上次貼圖
	rowNum = prePos / cols;
	colNum = prePos % cols;
	x = colNum * 50;
	y = rowNum * 50;

	SelectObject(bufdc,ball);
	BitBlt(mdc,x,y,50,50,bufdc,0,0,	WHITENESS);

	//小球貼圖
	rowNum = nowPos / cols;
	colNum = nowPos % cols;
	x = colNum * 50;
	y = rowNum * 50;

	SelectObject(bufdc,ball);
	BitBlt(mdc,x,y,50,50,bufdc,0,0,	SRCCOPY);

	if(!find)
	{
		str = "找尋出口中...";

		up    = nowPos - cols;
		down  = nowPos + cols;
		left  = nowPos - 1;
		right = nowPos + 1;

		if(up>=0 && record[up])						//往上走
		{
			path.push(up);
			record[up] = 0;
			prePos = nowPos;
			nowPos = up;

			if(mapIndex[nowPos] == 3)				//找到出口
				find = true;
		}
		else if(down<=cols*rows-1 && record[down])  //往下走
		{
			path.push(down);
			record[down] = 0;
			prePos = nowPos;
			nowPos = down;

			if(mapIndex[nowPos] == 3)
				find = true;
		}
		else if(left>=rowNum*cols && record[left])  //往左走
		{
			path.push(left);
			record[left] = 0;
			prePos = nowPos;
			nowPos = left;

			if(mapIndex[nowPos] == 3)
				find = true;
		}
		else if(right<=(rowNum+1)*cols-1 && record[right])  //往右走
		{
			path.push(right);
			record[right] = 0;
			prePos = nowPos;
			nowPos = right;

			if(mapIndex[nowPos] == 3)
				find = true;
		}
		else						//無可移動方格
		{
			if(path.size() <= 1)	//回到入口
				str = "迷宮無出口...";
			else
			{
				path.pop();
				prePos = nowPos;
				nowPos = path.top();
			}
		}
	}
	else
	{
		str = "找到出口了!";
	}

	TextOut(mdc,0,0,str,strlen(str));
	BitBlt(hdc,10,10,cols*50,rows*50,mdc,0,0,SRCCOPY);

	tPre = GetTickCount();
}

//****訊息處理函式***********************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_KEYDOWN:				//按鍵按下訊息
			if(wParam==VK_ESCAPE)		//按下 Esc 鍵
				PostQuitMessage(0);
			break;
		case WM_DESTROY:					//視窗結束訊息
			DeleteDC(mdc);
			DeleteDC(bufdc);
			DeleteObject(ball);

			ReleaseDC(hWnd,hdc);

			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

