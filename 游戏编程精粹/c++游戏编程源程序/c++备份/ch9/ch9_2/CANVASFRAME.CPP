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
LPDIRECTSOUNDBUFFER pSBuf[6];  //宣告次緩衝區
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
int now;
int volume=-1000;
int pan=0;
canvasFrame::canvasFrame()
{
	RECT rect;
	Create(NULL,"繪圖視窗",WS_OVERLAPPEDWINDOW,CRect(0,0,400,380));
	CClientDC dc(this);
	int width = dc.GetDeviceCaps(HORZRES);
	int height = dc.GetDeviceCaps(VERTRES);
	GetWindowRect( &rect );
	width = ( width - ( rect.right - rect.left ))/2 ;
	height = (height - (rect.bottom - rect.top ))/2 ;
	MoveWindow( width , height , (rect.right - rect.left ) , (rect.bottom - rect.top ) ,true);
}

canvasFrame::~canvasFrame()
{
	delete play;
	delete stop;
	delete loop;
	delete listbox;
	delete volumn;
	delete channel;
	for(int i=0;i<7;i++)
		delete title[i];
	pPBuf->Release();
	for(i=0;i<6;i++)
		pSBuf[i]->Release();
	pDS->Release();
}


BEGIN_MESSAGE_MAP(canvasFrame, CFrameWnd)
	//{{AFX_MSG_MAP(canvasFrame)
	ON_WM_CREATE()
	ON_BN_CLICKED(3,OnPlayDown)
	ON_BN_CLICKED(4,OnLoopDown)
	ON_BN_CLICKED(2,OnStopDown)
	ON_WM_VSCROLL()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// canvasFrame message handlers
void canvasFrame::OnStopDown()
{
	pSBuf[now]->Stop();
}
void canvasFrame::OnPlayDown()
{
	pSBuf[now]->Stop();
	DWORD num ;
	int index;
	index = listbox->GetCurSel();  	   //選得選項索引值
	num = listbox->GetItemData(index); //取得選項資料值
	pSBuf[num]->SetCurrentPosition(0); //設定播放起點
	pSBuf[num]->SetVolume(volume);     //設定音量
	pSBuf[num]->SetPan(pan);           //設定聲道
	pSBuf[num]->Play(0,0,0);           //播放
	now = num;                         //重設播放曲號
}
void canvasFrame::OnLoopDown()
{
	pSBuf[now]->Stop();
	DWORD num ;
	int index;
	index = listbox->GetCurSel();	   //選得選項索引值
	num = listbox->GetItemData(index); //取得選項資料值
	pSBuf[num]->SetCurrentPosition(0); //設定播放起點
	pSBuf[num]->SetVolume(volume);     //設定音量
	pSBuf[num]->SetPan(pan);           //設定聲道
	pSBuf[num]->Play(0,0,1);           //循環播放
	now = num;                         //重設播放曲號
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
	title[0] = new CStatic;
	title[0]->Create("請選擇要播放的音樂",WS_VISIBLE,CRect(10,10,240,40),this,100);
	title[1] = new CStatic;
	title[1]->Create("音量",WS_VISIBLE,CRect(250,10,290,40),this,101);
	title[2] = new CStatic;
	title[2]->Create("聲道",WS_VISIBLE,CRect(290,10,330,40),this,102);
	title[3] = new CStatic;
	title[3]->Create("小",WS_VISIBLE|SS_CENTER,CRect(250,50,290,70),this,103);
	title[4] = new CStatic;
	title[4]->Create("左",WS_VISIBLE|SS_CENTER,CRect(290,50,330,70),this,104);
	title[5] = new CStatic;
	title[5]->Create("大",WS_VISIBLE|SS_CENTER,CRect(250,240,290,260),this,105);
	title[6] = new CStatic;
	title[6]->Create("右",WS_VISIBLE|SS_CENTER,CRect(290,240,330,260),this,106);
	listbox = new CListBox;
	listbox->CreateEx(WS_EX_CLIENTEDGE,"ListBox",NULL,LBS_STANDARD|WS_VISIBLE|WS_CHILD,CRect(10,50,240,270),this,1); //建立 CListBox 物件
	CString item[6] = {"寶貝奇想曲","魔力狂飆五十音","新無敵炸彈超人","巴冷公主","陸戰英豪","雀戰時空"};  //選項名稱陣列
	DWORD num[6] = {0,1,2,3,4,5};  //選項的資料值
	int index;
	char filename[10];
	for(int i=0;i<=5;i++)
	{
		index = listbox->AddString(item[i]);   //加入各個選項
		listbox->SetItemData(index,num[i]);    //設定各個選項的資料值
		sprintf(filename,"s%d.wav",i);         //取得檔案名稱
		pSBuf[i] = createbuffer(filename);     //載入背景音樂
	}
	listbox->SelectString(0,"巴冷公主");       //設定預設選項
	stop = new CButton;
	stop->Create("停止",BS_PUSHBUTTON|WS_VISIBLE,CRect(10,280,80,310),this,2); //建立停止鈕
	play = new CButton;
	play->Create("播放",BS_PUSHBUTTON|WS_VISIBLE,CRect(90,280,160,310),this,3); //建立播放鈕
	loop = new CButton;
	loop->Create("循環",BS_PUSHBUTTON|WS_VISIBLE,CRect(170,280,240,310),this,4); //建立循環鈕
	volumn = new CSliderCtrl; //控制音量滑軸
	volumn->Create(WS_VISIBLE|TBS_VERT,CRect(250,70,290,240),this,5);
	volumn->SetRange(-2000,0,true);			//設定音量滑軸的範圍
	volumn->SetPos(-1000);					 //設定滑鈕位置
	channel = new CSliderCtrl; //控制聲道滑軸
	channel->Create(WS_VISIBLE|TBS_VERT,CRect(290,70,330,240),this,6);
	channel->SetRange(-10000,10000,true);	 //設定聲道滑軸的範圍
	channel->SetPos(0);						 //設定滑鈕位置
	return 0;
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

void canvasFrame::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	switch(pScrollBar->GetDlgCtrlID())   //取得滑軸控制項的識別碼
	{
	case 5:                              //捲動音量控制滑軸
		if(nPos != 0)
		{
			volume = nPos;               //設定音量值
			pSBuf[now]->SetVolume(nPos); //設定音量
		}
		break;
	case 6:							     //捲動聲道控制滑軸
		if(nPos != 0)
		{
			pan = nPos;                  //設定聲道值
			pSBuf[now]->SetPan(nPos);    //設定聲道
		}
		break;
	}
	CFrameWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void canvasFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if(nChar == VK_ESCAPE)
		PostMessage(WM_CLOSE);
	CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
}
