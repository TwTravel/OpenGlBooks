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

LPDIRECTDRAW7              pDD7;  //宣告DirectDraw物件
LPDIRECTDRAWSURFACE7       pPSur; //宣告主繪圖頁
LPDIRECTDRAWSURFACE7       pBBuf; //宣告後緩衝區
LPDIRECTDRAWSURFACE7	   pOPla[8];
DDSCAPS2  caps;                   //宣告DDSCAPS結構
DDSURFACEDESC2             desc;  //宣告描述結構
HRESULT result;                   //宣告HRESULT型態變數
DDCOLORKEY key;					  //宣告顏色鍵
int i;
LPDIRECTSOUND pDS;          //宣告 DirectSound 物件指標
LPDIRECTSOUNDBUFFER pPBuf;  //宣告主緩衝區指標
LPDIRECTSOUNDBUFFER pSBuf[2];  //宣告次緩衝區
WAVEFORMATEX pwfmt;         //宣告聲音結構
WAVEFORMATEX swfmt;         //宣告聲音結構
DSBUFFERDESC dsdesc;          //宣告描述結構
MMCKINFO      ckRiff;       //RIFF 區塊的資訊  
MMCKINFO      ckInfo;       //子區塊的資訊
MMRESULT	  mmresult;     //傳回結果
HMMIO hmmio;                //開啟的多媒體檔案
DWORD size;                 //實際資料大小
LPVOID pAudio;
DWORD bytesAudio;
canvasFrame::canvasFrame()
{
	hdc = ::CreateCompatibleDC(NULL);
	Create(NULL,"繪圖視窗",WS_POPUP);
	pSBuf[0] = createbuffer("bground.wav"); //載入背景音樂
	pSBuf[1] = createbuffer("foot.wav"); //載入腳步聲
	pSBuf[0]->Play(0,0,1); //播放背景音樂
}

void canvasFrame::ColorKey(int i)
{
	key.dwColorSpaceHighValue = 0x001f;
	key.dwColorSpaceLowValue = 0x001f;
	pOPla[i]->SetColorKey(DDCKEY_SRCBLT,&key);
	return;
}
canvasFrame::~canvasFrame()
{
	delete hdc;
	delete hdc1;
	delete bitmap;
	pDD7->Release();
	pPSur->Release();
	for(i=0;i<8;i++)
		pOPla[i]->Release();
	pDS->Release();
	pPBuf->Release();
	for(i=0;i<1;i++)
		pSBuf[i]->Release();
}


BEGIN_MESSAGE_MAP(canvasFrame, CFrameWnd)
	//{{AFX_MSG_MAP(canvasFrame)
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// canvasFrame message handlers

int canvasFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
    result = DirectDrawCreateEx(NULL, (VOID**)&pDD7, IID_IDirectDraw7, NULL);
	//建立DirectDraw物件
    if (result != DD_OK)
		MessageBox("建立DirectDraw物件失敗!");
	result = pDD7->SetCooperativeLevel(m_hWnd,DDSCL_EXCLUSIVE |
			         DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT );
	//設定協調層級
	if(result !=DD_OK)
		MessageBox("設定程式協調層級失敗!");
	result = pDD7->SetDisplayMode(640,480,16,0,DDSDM_STANDARDVGAMODE);  //設定螢幕顯示模式
	//設定顯示模式
	if(result !=DD_OK)
		MessageBox("設定螢幕顯示模式失敗!");
	memset(&desc,0,sizeof(desc));       
	desc.dwSize = sizeof(desc);           
	desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	desc.dwBackBufferCount = 1;        
	desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
	result = pDD7->CreateSurface(&desc,&pPSur,NULL);
	//建立主繪圖頁
	if(result !=DD_OK)
	 	MessageBox("建立主繪圖頁失敗!");
	caps.dwCaps = DDSCAPS_BACKBUFFER;  
	result = pPSur->GetAttachedSurface(&caps,&pBBuf); 
	//連結後緩衝區
	if(result !=DD_OK)
		MessageBox("連接後緩衝區失敗!");
	memset(&desc,0,sizeof(desc));        
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH; 
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN ;
	desc.dwWidth = 640; 
	desc.dwHeight = 480; 
	char file[10];
	char num[5];
	for(i=0;i<=7;i++)
	{
		result = pDD7->CreateSurface(&desc, &pOPla[i], NULL); 
		//建立幕後暫存區
		if(result !=DD_OK)
  			MessageBox("建立幕後暫存區失敗!");
		ColorKey(i);  //設定顏色鍵
		sprintf( file , "b%d.bmp" , i ); //取得圖檔檔名字串
		bitmap = (HBITMAP)::LoadImage(NULL,file,IMAGE_BITMAP,640,480,LR_LOADFROMFILE); 
		if(bitmap==NULL)
			MessageBox("無法載入點陣圖!");
		::SelectObject(hdc,bitmap);       //設定hdc中的點陣圖為bitmap
		pOPla[i]->GetDC( &hdc1 );            //取得幕後暫存區的DC
		::BitBlt( hdc1 , 0 , 0 , 640 , 480 , hdc , 0 , 0 , SRCCOPY );
   		sprintf( num , "第 %d 張圖" , i );  //設定訊息字串
		::TextOut(hdc1, 0, 0, num, lstrlen(num));  //繪製訊息字串
		pOPla[i]->ReleaseDC( hdc1 );      //釋放DC
	}
	SetTimer(1,300,NULL);  //設定計時器
	i=0;                   //重設 i 值
    result = DirectSoundCreate( NULL, &pDS, NULL ); //建立 DirectSound 物件
	if(result != DS_OK)
		MessageBox("建立 DirectSound 物件失敗!");
    result = pDS->SetCooperativeLevel( m_hWnd, DSSCL_PRIORITY );
	if(result != DS_OK)
		MessageBox("設定協調層級失敗!");
    memset( &dsdesc,0, sizeof(dsdesc) );          //清空結構內容
    dsdesc.dwSize        = sizeof(dsdesc);        //配置描述結構大小
    dsdesc.dwFlags       = DSBCAPS_PRIMARYBUFFER;
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
	return 0;
}

void canvasFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
	if( nChar== VK_ESCAPE )      //判斷是否按下 Esc 鍵
		PostMessage(WM_CLOSE );  //傳送WM_CLOSE訊息
}

void canvasFrame::OnTimer(UINT nIDEvent) 
{
	CFrameWnd::OnTimer(nIDEvent);
	if(i==7)
		i=0;
	if(i==1 || i==4)  //判斷圖號
		pSBuf[1]->Play(0,0,0);  //播放腳步聲
	pBBuf->BltFast( 0 , 0 , pOPla[7], CRect(0,0,640,480) , DDBLTFAST_WAIT);  //貼上背景圖
	pBBuf->BltFast( 0 , 0 , pOPla[i], CRect(0,0,640,480) , DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY );
	//貼上人物圖
	pPSur->Flip( NULL , DDFLIP_WAIT ); //翻頁
	i++;
}

LPDIRECTSOUNDBUFFER canvasFrame::createbuffer(char* filename)
{	
	LPDIRECTSOUNDBUFFER buf;
	hmmio = mmioOpen(filename, NULL, MMIO_ALLOCBUF|MMIO_READ );
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
    dsdesc.dwFlags = 	 DSBCAPS_STATIC |DSBCAPS_CTRLPAN |DSBCAPS_CTRLVOLUME| DSBCAPS_GLOBALFOCUS;
    dsdesc.dwBufferBytes = size;            //設定緩衝區大小
    dsdesc.lpwfxFormat   = &swfmt;          //設定緩衝區格式
    result = pDS->CreateSoundBuffer( &dsdesc, &buf, NULL );
	if(result != DS_OK)
		MessageBox("建立次緩衝區失敗!");

	result = buf->Lock(0,size,&pAudio,&bytesAudio,NULL,NULL,NULL);
	//鎖定緩衝區
	if(result != DS_OK)
		MessageBox("鎖定緩衝區失敗!");
	mmresult = mmioRead(hmmio,(HPSTR)pAudio,bytesAudio);
	//讀取音檔資料
	if(mmresult == -1)
		MessageBox("讀取音檔資料失敗!");
	result = buf->Unlock(pAudio,bytesAudio,NULL,NULL);
	//解除鎖定緩衝區
	if(result != DS_OK)
		MessageBox("解除鎖定緩衝區失敗!");
	mmioClose(hmmio,0);
	return buf;
}

