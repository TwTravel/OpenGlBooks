
#include "stdafx.h"
#include <stdio.h>

//定義結構
struct chr
{
	int		nHp;
	int		fHp;
	int		lv;
	int		w;
	int		kind;
};

//全域變數宣告
HINSTANCE hInst;
HBITMAP	bg,sheep,girl,skill,slash,magic,recover,game;
HDC		hdc,mdc,bufdc;
HWND	hWnd;
DWORD	tPre,tNow;
int		pNum,f,txtNum;
bool	attack,over;
chr		player,monster;
char	text[5][100];

//函式宣告
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void				MyPaint(HDC hdc);
void				MsgInsert(char*);
void				CheckDie(int hp,bool player);

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
// 初始玩家及怪物的各項狀態
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

	bg = (HBITMAP)LoadImage(NULL,"bg.bmp",IMAGE_BITMAP,640,480,LR_LOADFROMFILE);
	sheep = (HBITMAP)LoadImage(NULL,"sheep.bmp",IMAGE_BITMAP,133,220,LR_LOADFROMFILE);
	girl = (HBITMAP)LoadImage(NULL,"girl.bmp",IMAGE_BITMAP,480,148,LR_LOADFROMFILE);
	skill = (HBITMAP)LoadImage(NULL,"skill.bmp",IMAGE_BITMAP,74,60,LR_LOADFROMFILE);
	slash = (HBITMAP)LoadImage(NULL,"slash.bmp",IMAGE_BITMAP,196,162,LR_LOADFROMFILE);
	magic = (HBITMAP)LoadImage(NULL,"magic.bmp",IMAGE_BITMAP,200,100,LR_LOADFROMFILE);
	recover = (HBITMAP)LoadImage(NULL,"recover.bmp",IMAGE_BITMAP,300,150,LR_LOADFROMFILE);
	game = (HBITMAP)LoadImage(NULL,"over.bmp",IMAGE_BITMAP,289,74,LR_LOADFROMFILE);

	player.nHp = player.fHp = 50;	//設定玩家角色生命值及上限
	player.lv = 2;					//設定玩家角色等級
	player.w  = 4;					//設定攻擊傷害加權值

	monster.nHp = monster.fHp = 100;	//設定怪物角色生命值及上限
	monster.lv = 1;						//設定怪物角色等級
	monster.w = 1;						//設定攻擊傷害加權值

	txtNum = 0;		//顯示訊息數目

	MyPaint(hdc);

	return TRUE;
}

//****自訂繪圖函式*********************************
// 1.畫面貼圖與對戰訊息顯示
// 2.怪物行為判斷及各項數值處理與計算
void MyPaint(HDC hdc)
{
	char str[100];
	int i,damage;

	//貼上背景圖
	SelectObject(bufdc,bg);
	BitBlt(mdc,0,0,640,480,bufdc,0,0,SRCCOPY);

	//顯示對戰訊息
	for(i=0;i<txtNum;i++)
		TextOut(mdc,0,360+i*18,text[i],strlen(text[i]));

	//貼上怪物圖
	if(monster.nHp>0)
	{
		SelectObject(bufdc,sheep);
		BitBlt(mdc,70,180,133,110,bufdc,0,110,SRCAND);
		BitBlt(mdc,70,180,133,110,bufdc,0,0,SRCPAINT);
		sprintf(str,"%d / %d",monster.nHp,monster.fHp);
		TextOut(mdc,100,320,str,strlen(str));
	}

	//貼上玩家圖
	if(player.nHp>0)
	{
		SelectObject(bufdc,girl);
		BitBlt(mdc,500,200,60,74,bufdc,pNum*60,74,SRCAND);
		BitBlt(mdc,500,200,60,74,bufdc,pNum*60,0,SRCPAINT);
		sprintf(str,"%d / %d",player.nHp,player.fHp);
		TextOut(mdc,510,320,str,strlen(str));
	}

	if(over)				//貼上遊戲結束圖示
	{
		SelectObject(bufdc,game);
		BitBlt(mdc,200,200,289,37,bufdc,0,37,SRCAND);
		BitBlt(mdc,200,200,289,37,bufdc,0,0,SRCPAINT);
	}
	else if(!attack)		//貼上攻擊指令圖示
	{
		SelectObject(bufdc,skill);
		BitBlt(mdc,500,350,74,30,bufdc,0,30,SRCAND);
		BitBlt(mdc,500,350,74,30,bufdc,0,0,SRCPAINT);
	}	
	else
	{
		f++;

		//第5~10個畫面時顯示玩家攻擊圖示
		if(f>=5 && f<=10)
		{
			SelectObject(bufdc,slash);
			BitBlt(mdc,100,160,98,162,bufdc,98,0,SRCAND);
			BitBlt(mdc,100,160,98,162,bufdc,0,0,SRCPAINT);

			//第10個畫面時計算怪物受傷害程度以及加入顯示訊息
			if(f == 10)
			{
				damage = rand()%10 + player.lv*player.w;
				monster.nHp -= (int)damage;
					
				sprintf(str,"玩家攻擊...玩家對怪物造成了 %d 的傷害。",damage);
				MsgInsert(str);

				CheckDie(monster.nHp,false);
			}
		}

		srand(tPre);

		//第15個畫面時判斷怪物進行何項動作
		if(f == 15)
		{
			if(monster.nHp > 20)				//生命值大於20
			{
				if(rand()%10 != 1)
					monster.kind = 0;
				else
					monster.kind = 1;
			}
			else								//生命值小於20
			{
				switch(rand()%5)
				{
					case 0:						//普通攻擊
						monster.kind = 0;
						break;
					case 1:						//施放攻擊魔法
						monster.kind = 1;
						break;
					case 2:						//使盡全力攻擊
						monster.kind = 2;
						break;
					case 3:						//補血
						monster.kind = 3;
						break;
					case 4:						//逃跑
						monster.kind = 4;
						break;
				}
			}
		}

		//第26~30個畫面時顯示怪物行為圖示
		if(f>=26  && f<=30)
		{
			switch(monster.kind)
			{
				case 0:							//普通攻擊
					SelectObject(bufdc,slash);
					BitBlt(mdc,480,150,98,162,bufdc,98,0,SRCAND);
					BitBlt(mdc,480,150,98,162,bufdc,0,0,SRCPAINT);

					//第30個畫面時計算玩家受傷害程度以及加入顯示訊息
					if(f == 30)
					{
						damage = rand()%10 + monster.lv*monster.w;
						player.nHp -= (int)damage;
						
						sprintf(str,"怪物攻擊...怪物對玩家造成了 %d 的傷害。",damage);
						MsgInsert(str);

						CheckDie(player.nHp,true);
					}
					break;
				case 1:							//施放攻擊魔法
					SelectObject(bufdc,magic);
					BitBlt(mdc,480,190,100,100,bufdc,100,0,SRCAND);
					BitBlt(mdc,480,190,100,100,bufdc,0,0,SRCPAINT);

					//第30個畫面時計算玩家受傷害程度以及加入顯示訊息
					if(f == 30)
					{
						damage = rand()%10 + 3*monster.w;
						player.nHp -= (int)damage;	
						
						sprintf(str,"怪物魔法攻擊...怪物對玩家造成了 %d 的傷害。",damage);
						MsgInsert(str);

						CheckDie(player.nHp,true);
					}
					break;
				case 2:							//使盡全力攻擊
					SelectObject(bufdc,slash);
					BitBlt(mdc,480,150,98,162,bufdc,98,0,SRCAND);
					BitBlt(mdc,480,150,98,162,bufdc,0,0,SRCPAINT);

					//第30個畫面時計算玩家受傷害程度以及加入顯示訊息
					if(f == 30)
					{
						damage = rand()%10 + monster.lv*monster.w*5;
						player.nHp -= (int)damage;
						
						sprintf(str,"怪物全力攻擊...怪物對玩家造成了 %d 的傷害。",damage);
						MsgInsert(str);

						CheckDie(player.nHp,true);
					}
					break;
				case 3:							//補血
					SelectObject(bufdc,recover);
					BitBlt(mdc,60,160,150,150,bufdc,150,0,SRCAND);
					BitBlt(mdc,60,160,150,150,bufdc,0,0,SRCPAINT);

					//第30個畫面時怪物回復生命值以及加入顯示訊息
					if(f == 30)
					{
						monster.nHp += 30;
						
						sprintf(str,"怪物魔法補血...回復了 30 生命值。",damage);
						MsgInsert(str);
					}
					break;
				case 4:
					//第30個畫面時判斷怪物是否逃跑成功
					if(f == 30)
					{
						if(rand()%3 == 1)	//逃跑成功機率為1/3
						{
							over = true;
							monster.nHp = 0;

							sprintf(str,"怪物逃跑...逃跑成功。");
							MsgInsert(str);
						}
						else
						{
							sprintf(str,"怪物逃跑...逃跑失敗。");
							MsgInsert(str);
						}
					}
					break;
			}
		}

		if(f == 30)			//回合結束
		{
			attack = false;
			f = 0;
		}
	}

	BitBlt(hdc,0,0,640,480,mdc,0,0,SRCCOPY);

	tPre = GetTickCount();

	pNum++;
	if(pNum == 8)
		pNum = 0;
}

//****對戰訊息新增函式*********************************
void MsgInsert(char* str)
{
	if(txtNum < 5)
	{
		sprintf(text[txtNum],str);
		txtNum++;
	}
	else
	{
		for(int i=0;i<txtNum;i++)
			sprintf(text[i],text[i+1]);

		sprintf(text[4],str);
	}
}

//****玩家怪物生命值判斷函式*************************
void CheckDie(int hp,bool player)
{
	char str[100];

	if(hp <= 0)
	{
		over = true;
		if(player)
		{
			sprintf(str,"你被怪物打敗了...");
			MsgInsert(str);
		}
		else
		{
			sprintf(str,"怪物殲滅...");
			MsgInsert(str);
		}
	}
}

//****訊息處理函式***********************************
// 判斷玩家是否下達攻擊指令
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int x,y;

	switch (message)
	{
		case WM_KEYDOWN:				//按鍵按下訊息
			if(wParam==VK_ESCAPE)		//按下 Esc 鍵
				PostQuitMessage(0);
			break;
		case WM_LBUTTONDOWN:			//按下滑鼠左鍵訊息
			if(!attack)
			{
				x = LOWORD(lParam);		//取得滑鼠 X 座標
				y = HIWORD(lParam);		//取得滑鼠 Y 座標
			
				if(x >= 500 && x <= 574 && y >= 350 && y <= 380)
					attack = true;
			}
			break;
		case WM_DESTROY:				//視窗結束訊息
			DeleteDC(mdc);
			DeleteDC(bufdc);
			DeleteObject(bg);
			DeleteObject(sheep);
			DeleteObject(girl);
			DeleteObject(skill);
			DeleteObject(slash);
			DeleteObject(magic);
			DeleteObject(recover);
			DeleteObject(game);

			ReleaseDC(hWnd,hdc);

			PostQuitMessage(0);
			break;
		default:						//其它訊息
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

