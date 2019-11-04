// canvasFrame.cpp : implementation file
//

#include "stdafx.h"
#include "canvasr.h"
#include "canvasFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// canvasFrame

IMPLEMENT_DYNCREATE(canvasFrame, CFrameWnd)

LPDIRECTSOUND pDS;          //宣告 DirectSound 物件指標
LPDIRECTSOUNDBUFFER pPBuf;  //宣告主緩衝區指標
LPDIRECTSOUNDBUFFER pSBuf;  //宣告次緩衝區
LPDIRECTSOUND3DBUFFER   Buffer3D;
LPDIRECTSOUND3DLISTENER Listener;
WAVEFORMATEX pwfmt;         //宣告聲音結構
WAVEFORMATEX swfmt;         //宣告聲音結構
DSBUFFERDESC dsdesc;          //宣告描述結構
MMCKINFO      ckRiff;       //RIFF 區塊的資訊  
MMCKINFO      ckInfo;       //子區塊的資訊
MMRESULT	  mmresult;     //傳回結果
HMMIO hmmio;                //開啟的多媒體檔案
DWORD size;                 //實際資料大小
HRESULT result;             //宣告HRESULT型態變數
LPVOID pAudio;
DWORD bytesAudio;

double xvector,yvector;
int xnow,ynow,xlast,ylast,i,c,xp;

canvasFrame::canvasFrame()
{
	Create(NULL,"繪圖視窗");
	CClientDC dc(this);
	int width = dc.GetDeviceCaps(HORZRES);
	int height = dc.GetDeviceCaps(VERTRES);
	GetWindowRect( &rect );
	width = ( width - ( rect.right - rect.left ))/2 ;
	height = (height - (rect.bottom - rect.top ))/2 ;
	MoveWindow( width , height , (rect.right - rect.left ) , (rect.bottom - rect.top ) ,true);
	mdc = new CDC;
	mdc->CreateCompatibleDC(&dc);
	char str[7];
	for(i=0;i<4;i++)              
	{
		sprintf(str,"m%d.bmp",i);
		m[i] = new CBitmap;
		m[i]->m_hObject = (HBITMAP)::LoadImage(NULL,str,IMAGE_BITMAP,440,74,LR_LOADFROMFILE);
		sprintf(str,"p%d.bmp",i);
		p[i] = new CBitmap;
		p[i]->m_hObject = (HBITMAP)::LoadImage(NULL,str,IMAGE_BITMAP,440,74,LR_LOADFROMFILE);
	}
	CPoint *p = new CPoint(250,150); //設定起始座標
	ClientToScreen(p);               //轉換座標
	::SetCursorPos(p->x,p->y);       //設定滑鼠所在位置
	ShowCursor(false);
	pSBuf->Play(0,0,1);
	delete p;
}

canvasFrame::~canvasFrame()
{
	if(mdc) delete mdc;                 //釋放資源
	for(i=0;i<=3;i++)
	{
		if(p[i]) delete p[i]; //釋放資源
		if(m[i]) delete m[i]; //釋放資源
	}
	Listener->Release();
	pSBuf->Release();
	pDS->Release();
}

BEGIN_MESSAGE_MAP(canvasFrame, CFrameWnd)
	//{{AFX_MSG_MAP(canvasFrame)
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// canvasFrame message handlers

void canvasFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	CClientDC dc(this);
	xlast = xnow;      //上一次的 X 座標
	ylast = ynow;      //上一次的 Y 座標
	xnow = point.x;    //此次的 X 座標
	ynow = point.y;    //此次的 Y 座標
	c++;
		if(c==8)
			c=0;
	xp=55*c;
	mdc->SelectObject(m[1]);
	dc.BitBlt(xlast,ylast,55,74,mdc,0,0,WHITENESS); //覆蓋上一次的貼圖
	if(fabs(xnow-xlast) >= fabs(ynow-ylast))
	{

		if(xnow-xlast>=0)              //往右
		{
  			mdc->SelectObject(m[1]);  //貼上人物
			dc.BitBlt(point.x,point.y,55,74,mdc,xp,0,SRCAND);
			mdc->SelectObject(p[1]);
			dc.BitBlt(point.x,point.y,55,74,mdc,xp,0,SRCPAINT);
		}
		else                           //往左
		{
			mdc->SelectObject(m[2]);
			dc.BitBlt(point.x,point.y,55,74,mdc,xp,0,SRCAND);
			mdc->SelectObject(p[2]);
			dc.BitBlt(point.x,point.y,55,74,mdc,xp,0,SRCPAINT);
		}
	}
	else
	{
		if(ynow-ylast >= 0)            //往下
		{
            mdc->SelectObject(m[3]);
			dc.BitBlt(point.x,point.y,55,74,mdc,xp,0,SRCAND);
			mdc->SelectObject(p[3]);
			dc.BitBlt(point.x,point.y,55,74,mdc,xp,0,SRCPAINT);
		}
		else                          //往上
		{
			mdc->SelectObject(m[0]);			
			dc.BitBlt(point.x,point.y,55,74,mdc,xp,0,SRCAND);
			mdc->SelectObject(p[0]);
			dc.BitBlt(point.x,point.y,55,74,mdc,xp,0,SRCPAINT);
		}
	}
	xvector = (xnow-250)/50*0.5;
	yvector = (ynow-150)/50*0.5;
	Listener->SetPosition(xvector,yvector,0,DS3D_IMMEDIATE);
	char str[40];
	sprintf(str,"目前傾聽者的位置：(%.1f,%.1f,0) ",xvector,yvector);
	dc.TextOut(0,0,str);
	CFrameWnd::OnMouseMove(nFlags, point);
}

void canvasFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if( nChar== VK_ESCAPE)     
		PostMessage(WM_CLOSE );  //傳送WM_CLOSE訊息
	CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
}

int canvasFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
    result = DirectSoundCreate( NULL, &pDS, NULL ); //建立 DirectSound 物件
	if(result != DS_OK)
		MessageBox("建立 DirectSound 物件失敗!");
    result = pDS->SetCooperativeLevel( m_hWnd, DSSCL_PRIORITY );
	if(result != DS_OK)
		MessageBox("設定協調層級失敗!");
    memset( &dsdesc,0, sizeof(dsdesc) );          //清空結構內容
    dsdesc.dwSize        = sizeof(dsdesc);        //配置描述結構大小
    dsdesc.dwFlags       = DSBCAPS_PRIMARYBUFFER|DSBCAPS_CTRL3D;
    dsdesc.dwBufferBytes = 0;
    dsdesc.lpwfxFormat   = NULL;
    result = pDS->CreateSoundBuffer( &dsdesc, &pPBuf, NULL );
	if(result != DS_OK)
		MessageBox("建立主緩衝區失敗!");
    memset( &pwfmt,0, sizeof(pwfmt) );
    pwfmt.wFormatTag      = WAVE_FORMAT_PCM;
    pwfmt.nChannels       = 2;               //播放聲道
    pwfmt.nSamplesPerSec  = 44100;           //播放頻率
    pwfmt.wBitsPerSample  = 16;              //位元
    pwfmt.nBlockAlign     = pwfmt.wBitsPerSample / 8 * pwfmt.nChannels;
    pwfmt.nAvgBytesPerSec = pwfmt.nSamplesPerSec * pwfmt.nBlockAlign;
    result = pPBuf->SetFormat(&pwfmt);       //設定播放格式
	if(result != DS_OK)
		MessageBox("設定播放格式失敗!");
    result = pPBuf->QueryInterface( IID_IDirectSound3DListener,                                                   (VOID**)&Listener );
	if(result != DS_OK)
		MessageBox("建立傾聽者失敗!");
	pPBuf->Release();
	hmmio = mmioOpen("s0.wav", NULL, MMIO_ALLOCBUF|MMIO_READ );
	//開啟檔案
	if(hmmio == NULL)                           //判斷是否為空
		MessageBox("檔案不存在!");
	ckRiff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	//設定檔案類型
	mmresult = mmioDescend(hmmio,&ckRiff,NULL,MMIO_FINDRIFF);
	//搜尋型態
	if(mmresult != MMSYSERR_NOERROR)
		MessageBox("檔案格式錯誤!");
	ckInfo.ckid = mmioFOURCC('f','m','t',' ');  //設定區塊類型
	mmresult = mmioDescend(hmmio,&ckInfo,&ckRiff,MMIO_FINDCHUNK);
	//搜尋區塊
	if(mmresult != MMSYSERR_NOERROR)
		MessageBox("檔案格式錯誤!");
	if(mmioRead(hmmio,(HPSTR)&swfmt,sizeof(swfmt)) == -1) //讀取檔案格式
		MessageBox("讀取格式失敗!");
	mmresult = mmioAscend(hmmio,&ckInfo,0);     //跳出子區塊
	ckInfo.ckid = mmioFOURCC('d','a','t','a');  //設定區塊類型
	mmresult = mmioDescend(hmmio,&ckInfo,&ckRiff,MMIO_FINDCHUNK);
	//搜尋區塊
	if(mmresult != MMSYSERR_NOERROR)
		MessageBox("檔案格式錯誤!");
	size = ckInfo.cksize;                       //取得實際資料大小
	memset( &dsdesc,0,sizeof(dsdesc));        //清空結構內容
    dsdesc.dwSize  = sizeof(dsdesc);          //配置結構大小
    dsdesc.dwFlags = 	 DSBCAPS_STATIC |DSBCAPS_CTRLPAN |DSBCAPS_CTRLVOLUME| DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRL3D;
    dsdesc.dwBufferBytes = size;            //設定緩衝區大小
    dsdesc.lpwfxFormat   = &swfmt;          //設定緩衝區格式
    result = pDS->CreateSoundBuffer( &dsdesc, &pSBuf, NULL );
	if(result != DS_OK)
		MessageBox("建立次緩衝區失敗!");
	result = pSBuf->Lock(0,size,&pAudio,&bytesAudio,NULL,NULL,NULL);
	//鎖定緩衝區
	if(result != DS_OK)
		MessageBox("鎖定緩衝區失敗!");
	mmresult = mmioRead(hmmio,(HPSTR)pAudio,bytesAudio);
	//讀取音檔資料
	if(mmresult == -1)
		MessageBox("讀取音檔資料失敗!");
	result = pSBuf->Unlock(pAudio,bytesAudio,NULL,NULL);
	//解除鎖定緩衝區
	if(result != DS_OK)
		MessageBox("解除鎖定緩衝區失敗!");
	mmioClose(hmmio,0);
    result = pSBuf->QueryInterface( IID_IDirectSound3DBuffer,                                                   (VOID**)&Buffer3D );
    if(result != DS_OK)
		MessageBox("建立 3D 緩衝區失敗!");	
	// TODO: Add your specialized creation code here
	return 0;
}
