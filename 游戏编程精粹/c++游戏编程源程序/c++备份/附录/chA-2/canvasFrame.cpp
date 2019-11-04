// canvasFrame.cpp : implementation file
//
#define DIRECTINPUT_VERSION 0x0700
#include "stdafx.h"
#include "canvasr.h"
#include "canvasFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
struct bullet       //飛機的子彈    
{
   int x;           //貼圖的 X 座標
   int y;           //貼圖的 Y 座標
   int hitx;        //碰撞點的 X 座標
   int hity;        //碰撞點的 Y 座標
   BOOL exist;      //子彈是否存在
};
struct cartridge    //怪物的子彈
{
	int x;
	int y;
	int hitx;
	int hity;
	BOOL exist;
};
struct ship         //飛機
{
	int x;          //貼圖的 X 座標
	int y;          //貼圖的 Y 座標
	RECT r[3];      //飛機的區塊
};
struct monster      //怪物
{
	int x;          //貼圖的 X 座標
	int y;          //貼圖的 Y 座標
	int type;       //怪物的類型
	int draw;       //控制貼圖速度的變數
	RECT r[3];      //怪物的區塊
	POINT p[4];     //怪物的碰撞點
	BOOL exist;     //怪物是否存在
};

/////////////////////////////////////////////////////////////////////////////
// canvasFrame

IMPLEMENT_DYNCREATE(canvasFrame, CFrameWnd)

//宣告 DirectDraw 變數
LPDIRECTDRAW7              DD;  
LPDIRECTDRAWSURFACE7       DDSur; 
LPDIRECTDRAWSURFACE7       DDBuf; 
LPDIRECTDRAWSURFACE7	   DDPla[10];
DDSCAPS2				   DDcaps;
DDSURFACEDESC2             DDde;
HRESULT					   result;
DDCOLORKEY                 key;

//宣告 DirectSound 變數
LPDIRECTSOUND			   DS;          
LPDIRECTSOUNDBUFFER        DSPri;  
LPDIRECTSOUNDBUFFER        DSBuf[6];  
WAVEFORMATEX			   DSfmt;         
WAVEFORMATEX			   wfmt;        
DSBUFFERDESC			   DSde;        
MMCKINFO				   ckRiff;       
MMCKINFO				   ckInfo;     
MMRESULT				   mmresult;    
HMMIO					   hmmio;              
DWORD					   wsize;                 
LPVOID					   pAudio;
DWORD					   bytesAudio;

//宣告 DirectInput 變數
LPDIRECTINPUT7			   DI;       
LPDIRECTINPUTDEVICE7	   DIms;
DIMOUSESTATE2			   DImstate;      


//宣告其它變數
HDC hdc,dhdc;
HBITMAP bitmap;
int i,j,m,n;
int bgx;
int bcount,mcount,ccount;
int t,f;              //控制螢幕貼圖
int score=0;          //分數
BOOL press,hit,over;
cartridge c[100];     //怪物子彈陣列
bullet b[100];        //飛機子彈陣列
monster mon[50];      //怪物陣列
ship ship;
char str[20];
canvasFrame::canvasFrame()
{
	Create(NULL,"繪圖視窗",WS_POPUP); 
	hdc = ::CreateCompatibleDC(NULL);
	::ShowCursor(false);
	CreateDDPla(640,480,"load.bmp",9);
	DDBuf->BltFast( 0 , 0 , DDPla[9], CRect(0,0,640,480) , DDBLTFAST_WAIT);
	DDSur->Flip( NULL , DDFLIP_WAIT );   //顯示載入中圖案
	CreateDDPla(640,480,"bgbmp.bmp",0);
	CreateDDPla(100,74,"ship.bmp",1);
	CreateDDPla(10,10,"bullet.bmp",2);
	CreateDDPla(80,59,"mon0.bmp",3);
	CreateDDPla(60,56,"mon1.bmp",4);
	CreateDDPla(79,51,"mon2.bmp",5);
	CreateDDPla(10,10,"cartridge.bmp",6);
	CreateDDPla(80,80,"hit.bmp",7);
	CreateDDPla(640,159,"over.bmp",8);
	CreateDSBuf("s0.wav",0);
	CreateDSBuf("s1.wav",1);
	CreateDSBuf("s2.wav",2);
	CreateDSBuf("s3.wav",3);
	ColorKey(1);
	ColorKey(2);
	ColorKey(3);
	ColorKey(4);
	ColorKey(5);
	ColorKey(6);
	ColorKey(7);
	ColorKey(8);
	ship.x = 540;     
	ship.y = 200;      
	DSBuf[0]->Play(0,0,1);   //播放背景音樂
}

canvasFrame::~canvasFrame()
{
	delete bitmap;
	delete hdc;
	delete dhdc;
	DD->Release();
	DDSur->Release();
	for(i=0;i<=9;i++)
		DDPla[i]->Release();
	DI->Release();
	DIms->Release();
	DS->Release();
	DSPri->Release();
	for(i=0;i<=2;i++)
		DSBuf[i]->Release();

}


BEGIN_MESSAGE_MAP(canvasFrame, CFrameWnd)
	//{{AFX_MSG_MAP(canvasFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// canvasFrame message handlers

int canvasFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	InitDD();
	InitDS();
	InitDI();
	SetTimer(1,0,NULL);
	return 0;
}


//建立幕後暫存區並載入圖檔的自訂函式
void canvasFrame::CreateDDPla(int width,int height,char* filename,int num)
{
	DDde.dwWidth = width; 
	DDde.dwHeight = height; 
	result = DD->CreateSurface(&DDde, &DDPla[num], NULL); 
	if(result !=DD_OK)
	{
  		MessageBox("建立幕後暫存區失敗!");
		return;
	}
	bitmap = (HBITMAP)::LoadImage(NULL,filename,IMAGE_BITMAP,width,height,LR_LOADFROMFILE); 
	if(bitmap==NULL)
	{
		MessageBox("無法載入點陣圖!");
		return;
	}
	::SelectObject(hdc,bitmap);
	DDPla[num]->GetDC( &dhdc );
	::BitBlt( dhdc , 0 , 0 ,width,height, hdc , 0 , 0 , SRCCOPY );
	DDPla[num]->ReleaseDC(dhdc);
}
void canvasFrame::ColorKey(int num)
{
	key.dwColorSpaceHighValue = 0;
	key.dwColorSpaceLowValue = 0;
	DDPla[num]->SetColorKey(DDCKEY_SRCBLT,&key);
}
void canvasFrame::Generate()
{
	switch(rand()%3)
	{
	case 0:
		for(i=0;i<50;i++)
		{
			if(!mon[i].exist)
			{
				mon[i].type = 0;
				mon[i].x = 0;
				mon[i].y = rand()%420;
				mon[i].exist = true;
				mon[i].r[0].left = mon[i].x + 28;
				mon[i].r[0].top = mon[i].y + 7;
				mon[i].r[0].right = mon[i].x + 72;
				mon[i].r[0].bottom = mon[i].y + 34;
				mon[i].r[1].left = mon[i].x;
				mon[i].r[1].top = mon[i].y + 23;
				mon[i].r[1].right = mon[i].x + 49;
				mon[i].r[1].bottom = mon[i].y + 59;
				mon[i].r[2].left = mon[i].x + 24;
				mon[i].r[2].top = mon[i].y + 50;
				mon[i].r[2].right = mon[i].x + 48;
				mon[i].r[2].bottom = mon[i].y + 59;
				mon[i].p[0].x = mon[i].x +46;
				mon[i].p[0].y = mon[i].y + 8;
				mon[i].p[1].x = mon[i].x + 71;
				mon[i].p[1].y = mon[i].y + 22;
				mon[i].p[2].x = mon[i].x + 36;
				mon[i].p[2].y = mon[i].y + 57;
				mon[i].p[3].x = mon[i].x + 1;
				mon[i].p[3].y = mon[i].y + 31;
				mcount++;
				break;
			}
		}
		break;
	case 1:
		for(i=0;i<50;i++)
		{
			if(!mon[i].exist)
			{
				mon[i].type = 1;
				mon[i].x = 0;
				mon[i].y = rand()%420;
				mon[i].exist = true;
				mon[i].r[0].left = mon[i].x + 14;
				mon[i].r[0].top = mon[i].y + 4;
				mon[i].r[0].right = mon[i].x + 57;
				mon[i].r[0].bottom = mon[i].y + 26;
				mon[i].r[1].left = mon[i].x+1;
				mon[i].r[1].top = mon[i].y + 14;
				mon[i].r[1].right = mon[i].x + 43;
				mon[i].r[1].bottom = mon[i].y + 41;
				mon[i].r[2].left = mon[i].x + 5;
				mon[i].r[2].top = mon[i].y + 34;
				mon[i].r[2].right = mon[i].x + 22;
				mon[i].r[2].bottom = mon[i].y + 55;
				mon[i].p[0].x = mon[i].x +52;
				mon[i].p[0].y = mon[i].y + 18;
				mon[i].p[1].x = mon[i].x + 20;
				mon[i].p[1].y = mon[i].y + 4;
				mon[i].p[2].x = mon[i].x + 3;
				mon[i].p[2].y = mon[i].y + 22;
				mon[i].p[3].x = mon[i].x + 14;
				mon[i].p[3].y = mon[i].y + 50;
				mcount++;
				break;
			}
		}
		break;
	case 2:
		for(i=0;i<50;i++)
		{
			if(!mon[i].exist)
			{
				mon[i].type = 2;
				mon[i].x = 580;
				mon[i].y = rand()%420;
				mon[i].exist = true;
				mon[i].r[0].left = mon[i].x + 1;
				mon[i].r[0].top = mon[i].y + 8;
				mon[i].r[0].right = mon[i].x + 78;
				mon[i].r[0].bottom = mon[i].y + 37;
				mon[i].r[1].left = mon[i].x+8;
				mon[i].r[1].top = mon[i].y + 36;
				mon[i].r[1].right = mon[i].x + 52;
				mon[i].r[1].bottom = mon[i].y + 50;
				mon[i].r[2].left = mon[i].x + 20;
				mon[i].r[2].top = mon[i].y;
				mon[i].r[2].right = mon[i].x + 37;
				mon[i].r[2].bottom = mon[i].y + 8;
				mon[i].p[0].x = mon[i].x +4;
				mon[i].p[0].y = mon[i].y + 21;
				mon[i].p[1].x = mon[i].x + 31;
				mon[i].p[1].y = mon[i].y + 3;
				mon[i].p[2].x = mon[i].x + 72;
				mon[i].p[2].y = mon[i].y + 24;
				mon[i].p[3].x = mon[i].x + 36;
				mon[i].p[3].y = mon[i].y + 45;
				mcount++;
				break;
			}
		}
		break;
	}
}

void canvasFrame::OnTimer(UINT nIDEvent)
{
	if(!over)
	{
		t++;
		if(score>10000)
			f = 20;
		else if(score<=10000 && score>5500)
			f = 30;
		else if(score<=5500 && score >2000)
			f = 40;
		else 
			f = 50;
		if(t%f==0)
		{
			Generate();            //產生怪物
			t=0;
		}
		result = DIms->GetDeviceState(sizeof(DImstate),(LPVOID)&DImstate); 
		if(result != DI_OK )
		{
			MessageBox("取得滑鼠狀態失敗!");
			return;
		}
		DDBuf->BltFast( bgx , 0 , DDPla[0], CRect(0,0,640-bgx,480) , DDBLTFAST_WAIT);
		DDBuf->BltFast( 0 , 0 , DDPla[0], CRect(640-bgx,0,640,480) , DDBLTFAST_WAIT);
		ShipMove();       
		PasteMon();      
		Shoot();            
		if(ccount !=0)            //貼上怪物子彈
			for(i=0;i<100;i++)
				if(c[i].exist)
					DDBuf->BltFast(c[i].x,c[i].y,DDPla[6],CRect(0,0,10,10),DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);
		if(bcount != 0)           //貼上飛機子彈
			for(i=0;i<100;i++)
				if(b[i].exist)
					DDBuf->BltFast(b[i].x,b[i].y,DDPla[2],CRect(0,0,10,10),DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);
		MonBombShip();
		HitShip();
		HitMon();
		bgx+=10;
		if(bgx==640)
			bgx=0;
		DDBuf->GetDC( &hdc );           
		sprintf(str,"分數：%d",score);
		::TextOut(hdc, 0, 0,str, lstrlen(str));  //顯示分數
		DDBuf->ReleaseDC( hdc);      
		DDSur->Flip( NULL , DDFLIP_WAIT );
	}
	CFrameWnd::OnTimer(nIDEvent);
}

void canvasFrame::ShipMove()
{
	ship.x += DImstate.lX;     
	ship.y += DImstate.lY;      
	if(ship.x<0)				 //是否已至左邊界
		ship.x = 0;
	if(ship.x>540)				 //是否已至右邊界
		ship.x = 540;
	if(ship.y<0)			     //是否已至上邊界
		ship.y = 0;
	if(ship.y>406)				 //是否已至下邊界
		ship.y = 406;
	ship.r[0].left = ship.x + 16;
	ship.r[0].top = ship.y + 3;
	ship.r[0].right = ship.x + 50;
	ship.r[0].bottom = ship.y + 53;
	ship.r[1].left = ship.x + 8;
	ship.r[1].top = ship.y + 16;
	ship.r[1].right = ship.x + 78;
	ship.r[1].bottom = ship.y + 50;
	ship.r[2].left = ship.x + 35;
	ship.r[2].top = ship.y + 35;
	ship.r[2].right = ship.x + 81;
	ship.r[2].bottom = ship.y + 71;
	DDBuf->BltFast(ship.x,ship.y,DDPla[1],CRect(0,0,100,74),DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);
}
void canvasFrame::Shoot()
{
	if(DImstate.rgbButtons[0] & 0x80)  //判斷是否按下滑鼠左鍵
	{
		if(!press)
		{
			for(i=0;i<100;i++)
			{
				if(!b[i].exist)
				{
					DSBuf[1]->Play(0,0,0);
					b[i].x = ship.x;
					b[i].y = ship.y+30;
					b[i].exist = true;
					bcount++;
					break;
				}
			}
			press = true;
		}
	}
	else
		press = false;
}

void canvasFrame::CreateDSBuf(char* filename,int num)
{	
	hmmio = mmioOpen(filename, NULL, MMIO_ALLOCBUF|MMIO_READ );
	if(hmmio == NULL)      
	{
		MessageBox("檔案不存在!");
		return;
	}
	ckRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	mmresult = mmioDescend(hmmio,&ckRiff,NULL,MMIO_FINDRIFF);
	if(mmresult != MMSYSERR_NOERROR)
	{
		MessageBox("檔案格式錯誤!");
		return;
	}
	ckInfo.ckid = mmioFOURCC('f','m','t',' '); 
	mmresult = mmioDescend(hmmio,&ckInfo,&ckRiff,MMIO_FINDCHUNK);
	if(mmresult != MMSYSERR_NOERROR)
	{
		MessageBox("檔案格式錯誤!");
		return;
	}
	mmresult = mmioRead(hmmio,(HPSTR)&wfmt,sizeof(wfmt));
	if(mmresult == -1)
	{
		MessageBox("讀取格式失敗!");
		return;
	}
	mmresult = mmioAscend(hmmio,&ckInfo,0);     
	ckInfo.ckid = mmioFOURCC('d','a','t','a'); 
	mmresult = mmioDescend(hmmio,&ckInfo,&ckRiff,MMIO_FINDCHUNK);
	if(mmresult != MMSYSERR_NOERROR)
	{
		MessageBox("檔案格式錯誤!");
		return;
	}
	wsize = ckInfo.cksize;                   
	memset( &DSde,0,sizeof(DSde));     
    DSde.dwSize  = sizeof(DSde);      
    DSde.dwFlags = 	 DSBCAPS_STATIC |DSBCAPS_CTRLPAN |DSBCAPS_CTRLVOLUME| DSBCAPS_GLOBALFOCUS;
    DSde.dwBufferBytes = wsize;         
    DSde.lpwfxFormat   = &wfmt;         
    result = DS->CreateSoundBuffer( &DSde, &DSBuf[num], NULL );
	if(result != DS_OK)
	{
		MessageBox("建立次緩衝區失敗!");
		return;
	}
	result = DSBuf[num]->Lock(0,wsize,&pAudio,&bytesAudio,NULL,NULL,NULL);
	if(result != DS_OK)
	{
		MessageBox("鎖定緩衝區失敗!");
		return;
	}
	mmresult = mmioRead(hmmio,(HPSTR)pAudio,bytesAudio);
	if(mmresult == -1)
	{
		MessageBox("讀取音檔資料失敗!");
		return;
	}
	result = DSBuf[num]->Unlock(pAudio,bytesAudio,NULL,NULL);
	if(result != DS_OK)
	{
		MessageBox("解除鎖定緩衝區失敗!");
		return;
	}
	mmioClose(hmmio,0);
}

void canvasFrame::InitDD()
{
//建立 DirectDraw 物件
	result = DirectDrawCreateEx(NULL, (VOID**)&DD, IID_IDirectDraw7, NULL);
    if (result != DD_OK)
	{
		MessageBox("建立DirectDraw物件失敗!");
		return;
	}
//設定協調層級
	result = DD->SetCooperativeLevel(m_hWnd,DDSCL_EXCLUSIVE |
			         DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT );
	if(result !=DD_OK)
	{
		MessageBox("設定程式協調層級失敗!");
		return;
	}
//設定顯示模式
	result = DD->SetDisplayMode(640,480,16,0,DDSDM_STANDARDVGAMODE); 
	if(result !=DD_OK)
	{
		MessageBox("設定螢幕顯示模式失敗!");
		return;
	}
//建立主繪圖頁
	memset(&DDde,0,sizeof(DDde));       
	DDde.dwSize = sizeof(DDde);           
	DDde.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	DDde.dwBackBufferCount = 1;        
	DDde.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
	result = DD->CreateSurface(&DDde,&DDSur,NULL);
	if(result !=DD_OK)
	{
	 	MessageBox("建立主繪圖頁失敗!");
		return;
	}
//連結後緩衝區
	DDcaps.dwCaps = DDSCAPS_BACKBUFFER;  
	result = DDSur->GetAttachedSurface(&DDcaps,&DDBuf); 
	if(result !=DD_OK)
	{
		MessageBox("連接後緩衝區失敗!");
		return;
	}
//宣告幕後幕存區的共同特性
	memset(&DDde,0,sizeof(DDde));        
	DDde.dwSize = sizeof(DDde);
	DDde.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH; 
	DDde.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN ;
}

void canvasFrame::InitDS()
{
//建立 DirectSound 物件
    result = DirectSoundCreate( NULL, &DS, NULL ); 
	if(result != DS_OK)
	{
		MessageBox("建立 DirectSound 物件失敗!");
		return;
	}
//設定協調層級
    result = DS->SetCooperativeLevel( m_hWnd, DSSCL_PRIORITY );
	if(result != DS_OK)
	{
		MessageBox("設定協調層級失敗!");
		return;
	}
//建立主緩衝區
    memset( &DSde,0, sizeof(DSde) );          
    DSde.dwSize        = sizeof(DSde);       
    DSde.dwFlags       = DSBCAPS_PRIMARYBUFFER;
    DSde.dwBufferBytes = 0;
    DSde.lpwfxFormat   = NULL;
    result = DS->CreateSoundBuffer( &DSde, &DSPri, NULL );
	if(result != DS_OK)
	{
		MessageBox("建立主緩衝區失敗!");
		return;
	}
//設定聲音播放格式
    memset( &DSfmt,0, sizeof(DSfmt));
    DSfmt.wFormatTag      = WAVE_FORMAT_PCM;
    DSfmt.nChannels       = 2;               
    DSfmt.nSamplesPerSec  = 44100;           
    DSfmt.wBitsPerSample  = 16;          
    DSfmt.nBlockAlign     = DSfmt.wBitsPerSample / 8 * DSfmt.nChannels;
    DSfmt.nAvgBytesPerSec = DSfmt.nSamplesPerSec * DSfmt.nBlockAlign;
    result = DSPri->SetFormat(&DSfmt);     
	if(result != DS_OK)
	{
		MessageBox("設定播放格式失敗!");
		return;
	}
}

void canvasFrame::InitDI()
{
//建立 DirectInput 物件
	HINSTANCE hinst = AfxGetInstanceHandle(); 
	result = DirectInputCreateEx(hinst, DIRECTINPUT_VERSION,IID_IDirectInput7, (void**)&DI, NULL); 
	if(result != DI_OK)
	{
		MessageBox("建立 DirectInput 物件失敗!");
		return;
	}
//建立輸入裝置物件
	result = DI->CreateDeviceEx(GUID_SysMouse, IID_IDirectInputDevice7,(void**)&DIms, NULL); 
	if(result != DI_OK)
	{
		MessageBox("建立滑鼠輸入裝置失敗!");
		return;
	}
//設定資料格式
	result = DIms->SetDataFormat(&c_dfDIMouse2);
	if(result != DI_OK)
	{
		MessageBox("設定資料格式失敗!");
		return;
	}
//設定協調層級
	result = DIms->SetCooperativeLevel(m_hWnd,DISCL_BACKGROUND | DISCL_NONEXCLUSIVE); 
	if(result != DI_OK)
	{
		MessageBox("設定程式協調層級失敗!");
		return;
	}
//取用輸入裝置
	result = DIms->Acquire();
	if(result != DI_OK)
	{
		MessageBox("取用輸入裝置失敗!");
		return;
	}
}

void canvasFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( nChar== VK_ESCAPE )      //按下 Esc 鍵
		PostMessage(WM_CLOSE );  //結束程式
	if( nChar == VK_F1)          //按下 F1 鍵
	{
		over = false;
		score = 0;
		ship.x = 540;
		ship.y = 200;
		ship.r[0].left = ship.x + 16;
		ship.r[0].top = ship.y + 3;
		ship.r[0].right = ship.x + 50;
		ship.r[0].bottom = ship.y + 53;
		ship.r[1].left = ship.x + 8;
		ship.r[1].top = ship.y + 16;
		ship.r[1].right = ship.x + 78;
		ship.r[1].bottom = ship.y + 50;
		ship.r[2].left = ship.x + 35;
		ship.r[2].top = ship.y + 35;
		ship.r[2].right = ship.x + 81;
		ship.r[2].bottom = ship.y + 71;
		DSBuf[0]->Play(0,0,1);        //重播背景音樂
	}
	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
void canvasFrame::MonBombShip()
{
	if(mcount != 0)
	for(i=0;i<50;i++)          //取出所有怪物
		if(mon[i].exist)
			for(j=0;j<=3;j++)  //取出所有怪物的碰撞點
				for(m=0;m<3;m++)   //取出飛機各個區塊
					if(mon[i].p[j].x >= ship.r[m].left && mon[i].p[j].x <= ship.r[m].right && mon[i].p[j].y >= ship.r[m].top && mon[i].p[j].y <= ship.r[m].bottom)
					{
						DDBuf->BltFast(mon[i].p[j].x-40,mon[i].p[j].y-40,DDPla[7],CRect(0,0,80,80),DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);
						DDBuf->BltFast(0,150,DDPla[8],CRect(0,0,640,159),DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);
						DSBuf[0]->Stop();          //停止背景音樂
						DSBuf[3]->Play(0,0,0);     //播放碰撞聲音
						for(i=0;i<50;i++)          //清空怪物陣列
							if(mon[i].exist)
								mon[i].exist = false;
						for(i=0;i<100;i++)         //清空怪物子彈陣列
							if(c[i].exist)
								c[i].exist = false;
						for(i=0;i<100;i++)         //清空飛機子彈陣列
							if(b[i].exist)
								b[i].exist = false;
						bcount = 0;
						ccount = 0;
						mcount = 0;
						over = true;               //遊戲結束
						break;
					}
}
void canvasFrame::HitShip()
{
	if(ccount !=0)            
		for(i=0;i<100;i++)   //取出所有怪物子彈
			if(c[i].exist)
			{
				c[i].hitx = c[i].x + 10;
				c[i].hity = c[i].y + 5;
				hit = false;
				for(j=0;j<=2;j++) //取出飛機各個區塊
				{
					if(c[i].hitx >= ship.r[j].left && c[i].hitx <= ship.r[j].right && c[i].hity >= ship.r[j].top && c[i].hity <= ship.r[j].bottom)
					{
						hit = true;
						DDBuf->BltFast(ship.x,ship.y,DDPla[7],CRect(0,0,80,80),DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);
						break;
					}
				}
				if(hit)
				{
					DDBuf->BltFast(0,150,DDPla[8],CRect(0,0,640,159),DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);
					DSBuf[0]->Stop();        //停止背景音樂
					DSBuf[3]->Play(0,0,0);   //播放碰撞聲音
					for(i=0;i<50;i++)        //清空怪物陣列
						if(mon[i].exist)
							mon[i].exist = false;
					for(i=0;i<100;i++)       //清空怪物子彈陣列
						if(c[i].exist)
							c[i].exist = false;
					for(i=0;i<100;i++)       //清空飛機子彈陣列
						if(b[i].exist)
							b[i].exist = false;
					over = true;             //遊戲結束
					bcount = 0;
					ccount = 0;
					mcount = 0;
					break;
				}
				else
				{
					c[i].x +=5;       //計算下次 X 座標
					if(c[i].x>=640)   //子彈移動超出螢幕
					{
						c[i].exist = false;
						ccount--;
					}
				}
			}
}
void canvasFrame::HitMon()
{
	if(bcount != 0)   
		for(i=0;i<100;i++)     //取出所有飛機子彈
		{
			if(b[i].exist)
			{
				b[i].hitx = b[i].x + 3;
				b[i].hity = b[i].y + 5;
				hit = false;
				for(m=0;m<50;m++)            //取出所有怪物
				{
					if(mon[m].exist)
					{
						for(j=0;j<=2;j++)    //取出怪物各個區塊
						{
							if(b[i].hitx >= mon[m].r[j].left && b[i].hitx <= mon[m].r[j].right && b[i].hity >= mon[m].r[j].top && b[i].hity <= mon[m].r[j].bottom)
							{
								hit = true;
								DDBuf->BltFast(mon[m].x,mon[m].y,DDPla[7],CRect(0,0,80,80),DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);
								switch(mon[m].type)
								{
								case 0:            //打中蝸牛
									score+=100;
									break;
								case 1:            //打中小鳥
									score+=50;
									break;
								case 2:            //打中魚
									score+=150;
								}
								break;
							}
						}
						if(hit)
						{
							mon[m].exist = false;
							b[i].exist = false;
							DSBuf[2]->Play(0,0,0);
							mcount--;
							bcount--;
							break;
						}
					}
				}
				if(hit)
					continue;
				b[i].x -=20;	//計算下次 X 座標
				if(b[i].x<-10)	//子彈移動超出螢幕
				{
					b[i].exist = false;
					bcount--;
				}
			}
		}
}

void canvasFrame::PasteMon()
{
	if(mcount != 0)
	for(i=0;i<50;i++)      //怪物座標與貼圖
		if(mon[i].exist)
		{
			switch(mon[i].type)
			{
			case 0:
				if(mon[i].draw%10 == 0)
				{
					mon[i].draw = 0;
					mon[i].x += 20;
					if(mon[i].x >= 640)
					{
						mon[i].exist = false;
						break;
					}
					if(rand()%2 == 0)    //以亂數決定往上或往下移動
					{
						mon[i].y -= 20;
						if(mon[i].y<0)   //移動到最上方
							mon[i].y =0;
					}
					else
					{
						mon[i].y +=20;
						if(mon[i].y>420) //移動到最下方
							mon[i].y = 420;
					}
					if(mon[i].x <100)    //判斷是否發射子彈
						for(j=0;j<100;j++)
						{
							if(!c[j].exist)
							{
								c[j].x = mon[i].x + 61;
								c[j].y = mon[i].y+30;
								c[j].exist = true;
								ccount++;
								break;
							}
						}
				}
				mon[i].r[0].left = mon[i].x + 28;
				mon[i].r[0].top = mon[i].y + 7;
				mon[i].r[0].right = mon[i].x + 72;
				mon[i].r[0].bottom = mon[i].y + 34;
				mon[i].r[1].left = mon[i].x;
				mon[i].r[1].top = mon[i].y + 23;
				mon[i].r[1].right = mon[i].x + 49;
				mon[i].r[1].bottom = mon[i].y + 59;
				mon[i].r[2].left = mon[i].x + 24;
				mon[i].r[2].top = mon[i].y + 50;
				mon[i].r[2].right = mon[i].x + 48;
				mon[i].r[2].bottom = mon[i].y + 59;
				mon[i].p[0].x = mon[i].x +46;
				mon[i].p[0].y = mon[i].y + 8;
				mon[i].p[1].x = mon[i].x + 71;
				mon[i].p[1].y = mon[i].y + 22;
				mon[i].p[2].x = mon[i].x + 36;
				mon[i].p[2].y = mon[i].y + 57;
				mon[i].p[3].x = mon[i].x + 1;
				mon[i].p[3].y = mon[i].y + 31;
				mon[i].draw++;
				DDBuf->BltFast(mon[i].x,mon[i].y,DDPla[3],CRect(0,0,80,59),DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);
				break;
			case 1:
				if(mon[i].draw%5 == 0)
				{
					mon[i].draw = 0;
					if(mon[i].x > ship.x)
						mon[i].x-=30;
					else
						mon[i].x+=30;
					if(mon[i].y > ship.y)
						mon[i].y-=30;
					else 
						mon[i].y+=30;

				}
				mon[i].r[0].left = mon[i].x + 14;
				mon[i].r[0].top = mon[i].y + 4;
				mon[i].r[0].right = mon[i].x + 57;
				mon[i].r[0].bottom = mon[i].y + 26;
				mon[i].r[1].left = mon[i].x+1;
				mon[i].r[1].top = mon[i].y + 14;
				mon[i].r[1].right = mon[i].x + 43;
				mon[i].r[1].bottom = mon[i].y + 41;
				mon[i].r[2].left = mon[i].x + 5;
				mon[i].r[2].top = mon[i].y + 34;
				mon[i].r[2].right = mon[i].x + 22;
				mon[i].r[2].bottom = mon[i].y + 55;
				mon[i].p[0].x = mon[i].x +52;
				mon[i].p[0].y = mon[i].y + 18;
				mon[i].p[1].x = mon[i].x + 20;
				mon[i].p[1].y = mon[i].y + 4;
				mon[i].p[2].x = mon[i].x + 3;
				mon[i].p[2].y = mon[i].y + 22;
				mon[i].p[3].x = mon[i].x + 14;
				mon[i].p[3].y = mon[i].y + 50;
				mon[i].draw++;
				DDBuf->BltFast(mon[i].x,mon[i].y,DDPla[4],CRect(0,0,60,56),DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);
				break;
			case 2:
				if(mon[i].draw%10 == 0)
				{
					mon[i].draw = 0;
					mon[i].x -= 20;
					if(mon[i].x < 0)
					{
						mon[i].exist = false;
						break;
					}
				}
				mon[i].r[0].left = mon[i].x + 1;
				mon[i].r[0].top = mon[i].y + 8;
				mon[i].r[0].right = mon[i].x + 78;
				mon[i].r[0].bottom = mon[i].y + 37;
				mon[i].r[1].left = mon[i].x+8;
				mon[i].r[1].top = mon[i].y + 36;
				mon[i].r[1].right = mon[i].x + 52;
				mon[i].r[1].bottom = mon[i].y + 50;
				mon[i].r[2].left = mon[i].x + 20;
				mon[i].r[2].top = mon[i].y + 0;
				mon[i].r[2].right = mon[i].x + 37;
				mon[i].r[2].bottom = mon[i].y + 8;
				mon[i].p[0].x = mon[i].x +4;
				mon[i].p[0].y = mon[i].y + 21;
				mon[i].p[1].x = mon[i].x + 31;
				mon[i].p[1].y = mon[i].y + 3;
				mon[i].p[2].x = mon[i].x + 72;
				mon[i].p[2].y = mon[i].y + 24;
				mon[i].p[3].x = mon[i].x + 36;
				mon[i].p[3].y = mon[i].y + 45;
				mon[i].draw++;
				DDBuf->BltFast(mon[i].x,mon[i].y,DDPla[5],CRect(0,0,79,51),DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY);
				break;
			}
		}
}
