//======================================================================
/**
*  @file      Sound.cpp
*
*  项目描述： DirectSound声音演示
*   
*  文件描述:  DirectSound音频系统
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-24	
*  修改日期： 2007-07-24
*/																			
//======================================================================

#include "sound.h"

CSoundManager* CSoundManager::m_pSoundManager = NULL;

CSound::CSound()
{
}

CSound::~CSound()
{
	if(m_pDirectSoundBuf)
		m_pDirectSoundBuf->Release();

}

/** 载入wav文件 */
bool CSound::LoadWaveFile(const char* fileName)
{
	/** 打开wav文件 */
	FILE*  fp;
	if((fp = fopen(fileName,"rb")) == NULL)
	{
		MessageBox(NULL,"打开wav文件失败!","错误",MB_OK);
		return false;
	}

	/** 根据文件创建辅助缓冲区 */
	if(!GetSecondaryBuffer(fp))
	{
		MessageBox(NULL,"创建辅助缓冲失败!","错误",MB_OK);
		fclose(fp);
		return false;
	}

	/** 载入声音文件数据 */
	if(!LoadData(fp))
	{
		MessageBox(NULL,"载入声音数据失败!","错误",MB_OK);
		fclose(fp);
		return false;
	}

	/** 关闭文件 */
	fclose(fp);
	return true;
    
}

/** 创建辅助缓冲区 */
bool CSound::GetSecondaryBuffer(FILE* fp)
{
	/** 读取wav文件的头部信息 */
	WaveFileHeader* pHeader = new WaveFileHeader();
	fseek(fp,0,SEEK_SET);
	fread(pHeader,1,sizeof(WaveFileHeader),fp);

	/** 检查是否为wav文件 */
	if(memcmp(pHeader->RiffID,"RIFF",4)    ||
		memcmp(pHeader->WaveID,"WAVE",4)   ||
		memcmp(pHeader->FormatID,"fmt ",4) ||
		memcmp(pHeader->DataID,"data",4))
	{
		MessageBox(NULL,"该文件不是wav格式的文件","错误",MB_OK);
		return false;
	}
	
	/** 声音格式结构体 */
	WAVEFORMATEX   waveFormat;
	ZeroMemory(&waveFormat,sizeof(WAVEFORMATEX));

	/** 设置声音格式 */
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = pHeader->Channels;          /**< 声道数 */
	waveFormat.nSamplesPerSec = pHeader->SampleRate;   /**< 播放频率 */
	waveFormat.wBitsPerSample = pHeader->BitsPerSample;/**< 位数 */
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) 
							* waveFormat.nChannels;     
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * 
								waveFormat.nBlockAlign;

	/** 填充缓冲区描述结构体 */
	DSBUFFERDESC  bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DSBUFFERDESC));
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS
						| DSBCAPS_STATIC;
	bufferDesc.dwBufferBytes = pHeader->DataSize;
	bufferDesc.lpwfxFormat = &waveFormat;

	/** 获得DirectSoundBuffer接口对象 */
	IDirectSoundBuffer*  pDirectSoundBuf = NULL;
	if(FAILED(CSoundManager::GetSoundManager()->GetDirectSound()->
				CreateSoundBuffer(&bufferDesc,&pDirectSoundBuf,NULL)))
	
		return false;
	
	/** 再次获取DirectSoundBuffer接口对象 */
	if(FAILED(pDirectSoundBuf->QueryInterface(IID_IDirectSoundBuffer8,
		                       (void** )&m_pDirectSoundBuf)))
	{
        MessageBox(NULL,"创建DirectSoundBuffer接口对象失败!","错误",MB_OK);
		m_pDirectSoundBuf->Release();
		return false;
	}

	/** 删除指针，并成功返回 */
	delete pHeader;
	return true;

}

/** 载入声音数据 */
bool  CSound::LoadData(FILE* fp)
{
	/** 获得wav文件的大小 */
	WaveFileHeader* pHeader = new WaveFileHeader();
	fseek(fp,0,SEEK_SET);
	fread(pHeader,1,sizeof(WaveFileHeader),fp);

	/** 锁定缓冲区 */
	BYTE*  pSoundPtr1;
	BYTE*  pSoundPtr2;
	DWORD  dwSoundSize1,dwSoundSize2;

	if(FAILED(m_pDirectSoundBuf->Lock(0,pHeader->DataSize,
		               (void** )&pSoundPtr1,&dwSoundSize1, 
					   (void** )&pSoundPtr2,&dwSoundSize2,0)))
	{
		MessageBox(NULL,"锁定缓冲区失败!","错误",MB_OK);
	    return false;
	}

	/**读数据到缓冲区 */
	fread(pSoundPtr1,1,dwSoundSize1,fp);
	if(pSoundPtr2 != NULL)
		fread(pSoundPtr2,1,dwSoundSize2,fp);

	/** 解锁 */
	m_pDirectSoundBuf->Unlock(pSoundPtr1,dwSoundSize1,
		                      pSoundPtr2,dwSoundSize2);
	
	/** 删除指针，并成功返回 */
	delete pHeader;
	return true;

}

/** 播放 */
bool CSound::Play(bool bLoop)
{
	if(m_pDirectSoundBuf == NULL)
		return false;

	m_pDirectSoundBuf->SetCurrentPosition(0);

	if(FAILED(m_pDirectSoundBuf->Play(0,0,bLoop?true:false)))
		return false;
	return true;
}

/** 停止 */
bool CSound::Stop()
{
	if(FAILED(m_pDirectSoundBuf->Stop()))
		return false;
	return true;
}




/////////////////////////
CSoundManager::CSoundManager()
{
	m_pDirectSound = NULL;
	m_pSoundManager = this;
}

CSoundManager::~CSoundManager()
{
	if(m_pDirectSound)
		m_pDirectSound->Release();

}

/** 初始化 */
bool CSoundManager::Init(HWND hwnd,DWORD dwCoopLevel)
{
	/** 创建DirectSound对象 */
	if(FAILED(DirectSoundCreate8(NULL,&m_pDirectSound,NULL)))
		return false;

	/** 设置协调级别 */
	if(FAILED(m_pDirectSound->SetCooperativeLevel(hwnd,dwCoopLevel)))
		return false;
	return true;

}

/** 设置主缓冲区 */
bool CSoundManager::SetPrimaryBuffer(WORD channels,DWORD SamplePerSec,WORD BitsPerSample)
{
	if(m_pDirectSound == NULL)
		return false;

	/** 填充主缓冲区的描述结构体 */
	DSBUFFERDESC  bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(DSBUFFERDESC));
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	bufferDesc.dwBufferBytes = 0;   /**< 主缓冲区必须设置为0 */
	bufferDesc.lpwfxFormat = NULL;  /**< 主缓冲区必须设置为NULL */

	/** 取得主缓冲区 */
	IDirectSoundBuffer*  pDirectSoundBuf = NULL;
	if(FAILED(m_pDirectSound->CreateSoundBuffer(&bufferDesc,&pDirectSoundBuf,NULL)))
		return false;

	/** 设置主缓冲区自定义格式 */
	WAVEFORMATEX   waveFormat;
	ZeroMemory(&waveFormat,sizeof(WAVEFORMATEX));

	/** 设置声音格式 */
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = channels;          
	waveFormat.nSamplesPerSec = SamplePerSec;   
	waveFormat.wBitsPerSample = BitsPerSample;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) *
							 waveFormat.nChannels;     
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * 
								waveFormat.nBlockAlign;

	if(FAILED(pDirectSoundBuf->SetFormat(&waveFormat)))
		return false;

	/** 释放主缓冲区接口对象 */
	pDirectSoundBuf->Release();
	return true;

}
