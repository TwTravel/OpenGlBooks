
#include "stdafx.h"
#include <stdio.h>
#include <math.h>

//全域變數宣告
HINSTANCE hInst;
HBITMAP	chess[2];
HDC		hdc,mdc,bufdc;
HWND	hWnd;
DWORD	tPre,tNow;
int		board[10][10];
bool	ptab[10][10][192];
bool	ctab[10][10][192];
int		win[2][192];
int		num[2];
bool	turn,over;
int		winner;

//函式宣告
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				MyPaint(HDC hdc);
void				InitGame();
void				ComTurn();

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
// 棋盤拼接以及呼叫 InitGame() 函式開始棋局
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HBITMAP tile,bmp;
	int rowNum,colNum;
	int i,x,y;

	hInst = hInstance;
	hWnd = CreateWindow("canvas", "繪圖視窗" , WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	MoveWindow(hWnd,10,10,480,520,true);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	hdc = GetDC(hWnd);
	mdc = CreateCompatibleDC(hdc);
	bufdc = CreateCompatibleDC(hdc);

	bmp = CreateCompatibleBitmap(hdc,450,450);
	SelectObject(mdc,bmp);

	tile = (HBITMAP)LoadImage(NULL,"tile.bmp",IMAGE_BITMAP,45,45,LR_LOADFROMFILE);
	chess[0] = (HBITMAP)LoadImage(NULL,"chess0.bmp",IMAGE_BITMAP,38,38,LR_LOADFROMFILE);
	chess[1] = (HBITMAP)LoadImage(NULL,"chess1.bmp",IMAGE_BITMAP,38,38,LR_LOADFROMFILE);

	for (i=0;i<100;i++)
	{
		rowNum = i / 10;
		colNum = i % 10;
		x = colNum * 45;
		y = rowNum * 45;
		
		SelectObject(bufdc,tile);
		BitBlt(mdc,x,y,45,45,bufdc,0,0,SRCCOPY);
	}

	InitGame();
	MyPaint(hdc);

	return TRUE;
}

//****棋局初始函式***********************************
// 1.設定棋盤初始狀態及獲勝表內容
// 2.決定先下子的一方
void InitGame()
{
	int i,j,k;
	int count=0;

	over = false;
	num[0] = num[1] = 0;

	//設定玩家與電腦在各個獲勝組合中的棋子數
	for(i=0;i<192;i++)
	{
		win[0][i] = 0;
		win[1][i] = 0;
	}

	//初始化棋盤狀態
	for(i=0;i<10;i++)          
		for(j=0;j<10;j++)
			board[i][j] = 2;

	//設定水平方向的獲勝組合
	for(i=0;i<10;i++)          
		for(j=0;j<6;j++)
		{
			for(k=0;k<5;k++)
			{
				ptab[i][j+k][count] = true;
				ctab[i][j+k][count] = true;
			}
			count++;
		}

	//設定垂直方向的獲勝組合
	for(i=0;i<10;i++)          
		for(j=0;j<6;j++)
		{
			for(k=0;k<5;k++)
			{
				ptab[j+k][i][count] = true;
				ctab[j+k][i][count] = true;
			}
			count++;
		}

	//設定正對角線方向的獲勝組合
	for(i=0;i<6;i++)           
		for(j=0;j<6;j++)
		{
			for(k=0;k<5;k++)
			{
				ptab[j+k][i+k][count] = true;
				ctab[j+k][i+k][count] = true;
			}
			count++;
		}

	//設定反對角線方向的獲勝組合
	for(i=0;i<6;i++)           
		for(j=9;j>=4;j--)
		{
			for(k=0;k<5;k++)
			{
				ptab[j-k][i+k][count] = true;
				ctab[j-k][i+k][count] = true;
			}
			count++;
		}

	//亂數決定由那一方先下棋子
	srand(GetTickCount());
	if(rand()%2 == 0)
		turn = true;
	else
		turn = false;
}

//****電腦下子函式***********************************
// 1.計算獲勝分數
// 2.選擇最佳位置進行下子動作
void ComTurn()
{
	int	grades[2][10][10];
	int m,n,i,max=0;
	int u,v;

	for(m=0;m<10;m++) 
		for(n=0;n<10;n++)
		{
			grades[0][m][n] = 0;
			grades[1][m][n] = 0;

			if(board[m][n] == 2)
			{
				for(i=0;i<192;i++)
				{
					//計算玩家在空棋格上的獲勝分數
					if(ptab[m][n][i] && win[0][i] != 7)
					{
						switch(win[0][i])
						{
							case 0:
								grades[0][m][n]+=1;
								break;
							case 1:
								grades[0][m][n]+=200;
								break;
							case 2:
								grades[0][m][n]+=400;
								break;
							case 3:
								grades[0][m][n]+=2000;
								break;
							case 4:
								grades[0][m][n]+=10000;
								break;
						}
					}

					//計算電腦在空棋格上的獲勝分數
					if(ctab[m][n][i] && win[1][i] != 7)
					{
						switch(win[1][i])
						{
							case 0:
								grades[1][m][n]+=1;
								break;
							case 1:
								grades[1][m][n]+=220;
								break;
							case 2:
								grades[1][m][n]+=420;
								break;
							case 3:
								grades[1][m][n]+=2100;
								break;
							case 4:
								grades[1][m][n]+=20000;
								break;
						}
					}
				}

				if(max == 0)
				{
					u = m;
					v = n;
				}

				if(grades[0][m][n] > max)
				{
					max = grades[0][m][n];
					u = m;
					v = n;
				}
				else if(grades[0][m][n] == max)
				{
					if(grades[1][m][n] > grades[1][u][v])
					{
						u = m;
						v = n;
					}
				}

				if(grades[1][m][n] > max)
				{
					max = grades[1][m][n];
					u = m;
					v = n;
				}
				else if(grades[1][m][n] == max)
				{
					if(grades[0][m][n] > grades[0][u][v])
					{
						u = m;
						v = n;
					}
				}
			}
		}

	board[u][v] = 1;      //設定為電腦的棋子
	num[1]++;

	if(num[0] == 50 && num[1] == 50)
	{
		winner = 2;			//平手
		over = true;
	}
	else
		for(i=0;i<192;i++)
		{
			if(ctab[u][v][i])
			{
				win[1][i]++;
				ptab[u][v][i] = false;
				win[0][i] = 7;

				if(win[1][i] == 5)
				{
					winner = 1;
					over = true;
				}
			}
		}
	turn = true;			//換玩家下
}

//****自訂繪圖函式*********************************
// 視窗貼圖及顯示訊息
void MyPaint(HDC hdc)
{
	int m,n;
	char *str;

	if(over)
	{
		switch(winner)
		{
			case 0:
				str = "您贏了 ! 按下 F1 可重新進行遊戲..";
				break;
			case 1:
				str = "電腦贏了 ! 按下 F1 可重新進行遊戲..";
				break;
			case 2:
				str = "不分勝負 ! 按下 F1 可重新進行遊戲..";
				break;
		}
		TextOut(hdc,10,470,str,strlen(str));
	}
	else if(!turn)						//電腦下子
	{
		str = "電腦思考中...                                      ";
		TextOut(hdc,10,470,str,strlen(str));
		ComTurn();                 
	}
	else
	{
		str = "該您下了...                                        ";
		TextOut(hdc,10,470,str,strlen(str));
	}

	for(m=0;m<10;m++)
		for(n=0;n<10;n++)
		{
			if(board[m][n] == 0)			//貼上玩家棋子
			{
				SelectObject(bufdc,chess[0]);
				BitBlt(mdc,m*45+3,n*45+3,38,38,bufdc,0,0,SRCCOPY);
			}
			else if(board[m][n] == 1)      //貼上電腦棋子
			{
				SelectObject(bufdc,chess[1]);
				BitBlt(mdc,m*45+3,n*45+3,38,38,bufdc,0,0,SRCCOPY);
			}
			else							//貼上空格
			{
				SelectObject(bufdc,chess[1]);
				BitBlt(mdc,m*45+3,n*45+3,38,38,bufdc,0,0,WHITENESS);
			}
		}

	BitBlt(hdc,10,10,450,450,mdc,0,0,SRCCOPY);

	tPre = GetTickCount();
}

//****訊息處理函式***********************************
// 1.設定按下 F1 鍵重新開始遊戲
// 2.處理玩家按下滑鼠左鍵下棋子的動作
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int x,y,m,n,i;

	switch (message)
	{
		case WM_KEYDOWN:				  //按下按鍵訊息
			switch (wParam) 
			{
				case VK_ESCAPE:           //按下 Esc 鍵
					PostQuitMessage( 0 );
					break;
				case VK_F1:				  //按下 F1 鍵
					InitGame();
					break;
			}
		case WM_LBUTTONDOWN:			  //按下滑鼠左鍵訊息
			if(!over)
				if(turn)
				{
					x = LOWORD(lParam);		//取得滑鼠 X 座標
					y = HIWORD(lParam);		//取得滑鼠 Y 座標
					
					if(x > 10 && x < 460 && y> 10 && y < 460)
					{
						m = (int)floor((x-10)/45);
						n = (int)floor((y-10)/45);

						if(board[m][n] == 2)
						{
							board[m][n] = 0;   //設定為玩家的棋子
							num[0]++;
							
							if(num[0] == 50 && num[1] == 50)
							{
								winner = 2;	   //平手
								over = true;
							}
							else
								for(i=0;i<192;i++)
								{
									if(ptab[m][n][i])
									{
										win[0][i]++;
										ctab[m][n][i] = false;
										win[1][i] = 7;

										if(win[0][i] == 5)
										{
											winner = 0;
											over = true;
										}
									}
								}
							turn = false;       //換電腦下
						}
					}
				}
				break;
		case WM_DESTROY:					//視窗結束訊息
			DeleteDC(mdc);
			DeleteDC(bufdc);
			DeleteObject(chess[0]);
			DeleteObject(chess[1]);

			ReleaseDC(hWnd,hdc);

			PostQuitMessage(0);
			break;
		default:							//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}