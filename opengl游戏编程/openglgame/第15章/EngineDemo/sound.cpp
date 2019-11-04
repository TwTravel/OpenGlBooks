//========================================================
/**
*  @file      sound.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  DirectSound音效类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#include "stdafx.h"
#include "Sound.h"

#define WAV_HDR_SIZE  44
#define SND_GATE  0
#define SND_PING  1
#define SND_STATIC 2
#define SND_WALK 3


BOOL CSound::bExist=FALSE;
BOOL CSound::bEnable=FALSE;
LPDIRECTSOUND CSound::lpDSound=NULL;
LPDIRECTSOUNDBUFFER CSound::lpPrimarySoundBuffer=NULL;


CSound::CSound():DSSoundFX(NULL)
{
}

CSound::~CSound()
{
	if( lpDSound && DSSoundFX )
		_RELEASE( DSSoundFX );
}


/**\brief
 * 初始化声音设备
 *
 *\param	_hWnd	窗口句柄
 *\return	BOOL	初始化是否成功
 */
BOOL CSound::InitDirectSound(HWND _hWnd)	
{	
	if( bExist || bEnable ) return TRUE;
	
	if( DirectSoundCreate(NULL,&lpDSound,NULL)!=DS_OK )
	{
		MessageBox(_hWnd,"Create DirectSound Failed!\n初始化DirectSound过程中出错！\
			请检查你是否正确的安装了DirectX 5.0以上版本，或有无其他设备正占用声卡！\
			否则你将无法听到声音。","error",MB_OK);

		bEnable=FALSE;
		return FALSE;
	}

	if( lpDSound->SetCooperativeLevel(_hWnd,DSSCL_NORMAL)!=DS_OK)
	{
		MessageBox(_hWnd,"error on set DirectSound CooperativeLevel\n初始化DirectSound\
			过程中出错！请检查你是否正确的安装了DirectX 5.0以上版本，或有无其他设备正占\
			用声卡！否则你将无法听到声音。","error",MB_OK);

		bEnable=FALSE;
		return FALSE;
	}
	bEnable = TRUE;
	bExist=TRUE;
	return TRUE;  // 初始化成功
}



/**\brief
 * 创建主声音缓冲区
 */
BOOL CSound::CreatePrimarySoundBuffer()
{
	if( bExist || !bEnable ) 
		return TRUE;

    DSBUFFERDESC	dsbdesc;
	WAVEFORMATEX	pcmwf;

    // 设置wave格式结构
    memset(&pcmwf, 0, sizeof(WAVEFORMATEX));
    pcmwf.wFormatTag = WAVE_FORMAT_PCM;
    pcmwf.nChannels = 1;
    pcmwf.nSamplesPerSec = 8000;
    pcmwf.nBlockAlign = 4;
    pcmwf.nAvgBytesPerSec = pcmwf.nSamplesPerSec * pcmwf.nBlockAlign;
    pcmwf.wBitsPerSample = 8;
	pcmwf.cbSize = 0;

    // 设置 DSBUFFERDESC 结构
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); // Zero it out.
    dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

    // 主缓冲区必须是0
    dsbdesc.dwBufferBytes = 0; 
    dsbdesc.lpwfxFormat = NULL; // 主缓冲区必须是NULL

	if( (lpDSound->CreateSoundBuffer(&dsbdesc, &lpPrimarySoundBuffer, NULL)) == DS_OK)
	{
		if( (lpPrimarySoundBuffer->SetFormat(&pcmwf)) == DS_OK )
			return TRUE;
	}
	
	return FALSE;
}



/**\brief
 * 释放声音设备
 */
void CSound::ReleaseDirectSound(void)
{
	if( !bExist ) 
		return;
	bExist=FALSE;

	if( lpDSound != NULL )
	{
		_RELEASE( lpPrimarySoundBuffer );
		_RELEASE( lpDSound );
	}
}


/**\brief
 * 装入声音
 *
 *\param	filename	声音文件名
 *\return	BOOL		载入是否成功
 */
BOOL CSound::LoadSoundFX(char *filename)
{
	if( !bEnable ) return TRUE;

	DSBUFFERDESC dsbd;	//结构
	LPWAVEFORMATEX wav;	//声音格式
	DWORD len; 			//文件长度
	BYTE *buf; 			//临时缓冲区
	LPVOID ptr1;		//指针1
	LPVOID ptr2;		//指针2
	DWORD size1;		
	DWORD size2;
	DWORD BufferSize;	//DirectSoundBuffer 大小

	FILE *f;
	long WavLong;
	if( (f=fopen(filename,"rb"))==NULL )
	{
		MessageBox(NULL,"error open file",filename,MB_OK);
		return FALSE;
	}
	fseek(f,0x04,SEEK_SET);
	fread(&WavLong,sizeof(long),1,f);	//读取声音数据长度
	len=WavLong+WAV_HDR_SIZE;			//文件长度
	BufferSize=len-WAV_HDR_SIZE-800;	//设置缓冲区大小

	buf=(BYTE *)malloc(len);
	if( buf==NULL )

	{
		MessageBox(NULL,"error buf",filename,MB_OK);
		return FALSE;
	}

	//读取声音数据到临时缓冲区
	fseek(f,0l,SEEK_SET);
	fread(buf,len,1,f);
	fclose(f);

	ptr1=buf+20;
	wav=(LPWAVEFORMATEX)(ptr1);
	
	memset(&dsbd,0,sizeof(dsbd));
	dsbd.dwSize=sizeof(dsbd);
	dsbd.dwFlags=DSBCAPS_STATIC;
	dsbd.dwBufferBytes=len-WAV_HDR_SIZE;
	dsbd.lpwfxFormat=wav;

	_RELEASE( DSSoundFX );

	if(lpDSound->CreateSoundBuffer(&dsbd,&DSSoundFX,NULL)!=DS_OK)
	{
		MessageBox(NULL,"error createsoundbuffer","error",MB_OK);
		return FALSE;
	}

	if( (DSSoundFX->Lock(0,BufferSize,&ptr1,&size1,&ptr2,&size2,0))!=DS_OK)
	{
		MessageBox(NULL,"error Lock","error",MB_OK);
		return FALSE;
	}
	memcpy(ptr1,buf+WAV_HDR_SIZE,size1); //拷贝数据到声音缓冲区
	if(size2)
		memcpy(ptr2,buf+size1+WAV_HDR_SIZE,size2);//拷贝数据到声音缓冲区
	if( DSSoundFX->Unlock(ptr1,size1,ptr2,size2)!=DS_OK)
	{
		MessageBox(NULL,"error UnLock","error",MB_OK);
		return FALSE;
	}
	
	_FREE(buf);

	
	return TRUE;
}



/**\brief
 * 播放声音
 *
 *\param	loop	为-1时可以循环播放
 */
void CSound::Play(DWORD loop)
{
	if( bEnable==FALSE || !DSSoundFX ) 
		return;
	
	DWORD *Status1=NULL;
	if( (DSSoundFX->GetStatus(Status1))!=DS_OK ) 
		goto _next;
	if( (*Status1 & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING  ) 
		return;

_next:
	DSSoundFX->SetCurrentPosition(0);
	if( loop==-1 ) 
		loop=DSBPLAY_LOOPING;	//循环播放
	DSSoundFX->Play(0,0,loop);
}



/**\brief
 * 停止播放
 */
void CSound::Stop()
{
	if( bEnable==FALSE || DSSoundFX==NULL ) 
		return;

	DSSoundFX->Stop();
}



/**\brief
 * 缓冲区是否正在播放
 */
BOOL CSound::IsPlaying()
{
	if( bEnable==FALSE || DSSoundFX==NULL )
		return TRUE;

	DWORD pos, wpos;
	DSBCAPS dsbc;
	
	memset(&dsbc,0,sizeof(DSBCAPS));
	dsbc.dwSize=sizeof(DSBCAPS);
	if( (DSSoundFX->GetCaps(&dsbc)) != DS_OK ) 
	{
		MessageBox(NULL, "DSSoundFX->GetCaps","error",MB_OK);
		return TRUE;
	}
	pos=dsbc.dwBufferBytes;
	
	if( (DSSoundFX->GetCurrentPosition(&wpos,NULL)) != DS_OK )
	{
		MessageBox(NULL, "DSSoundFX->GetCurrentPosition","error",MB_OK);
		return TRUE;
	}

	if( wpos >= pos/7.16 ) 
		return FALSE;
	return TRUE;
}



/**\brief
 * 填充一个WAV声音缓冲区
 *
 *\param	lpSoundBuffer	声音缓冲区
 *\param	filename		声音文件名
 *\return	BOOL			填充是否成功
 */
BOOL CSound::LoadSoundBuffer(LPDIRECTSOUNDBUFFER lpSoundBuffer, char *filename)
{
	if( bEnable==FALSE ) return FALSE;
	
	FILE *f;
	DSBUFFERDESC dsbd;		//结构
	LPWAVEFORMATEX wav;  	//声音格式
	DWORD FileLen; 			//文件长度
	BYTE *Buf;	 			//临时缓冲区
	long BufLong;			//声音缓冲区长度

	if( (f=fopen(filename,"rb"))==NULL )
	{
		MessageBox(NULL,"Not found the sound file!", filename, MB_OK);
		return FALSE;
	}
	fseek(f,0x28,SEEK_SET);
	fread(&BufLong,sizeof(long),1,f);	//读取声音数据长度
	FileLen=BufLong+WAV_HDR_SIZE;		//文件长度

	Buf=(BYTE *)malloc(FileLen);
	if( Buf==NULL )
	{
		_ERROR("error alloc soundbuffer");
		return FALSE;
	}
	
	//读取声音数据到临时缓冲区
	fseek(f,0x00,SEEK_SET);
	fread(Buf,FileLen,1,f);
	fclose(f);

	wav=(LPWAVEFORMATEX)(Buf+20);

	memset(&dsbd,0,sizeof(dsbd));
	dsbd.dwSize=sizeof(dsbd);
	dsbd.dwFlags=DSBCAPS_STATIC;
	dsbd.dwBufferBytes=BufLong;
	dsbd.lpwfxFormat=wav;
	
	//创建缓冲区并读取数据
	if(	lpDSound->CreateSoundBuffer(&dsbd, &lpSoundBuffer, NULL) )
	{		
		LPVOID ptr1;		//指针1
		LPVOID ptr2;		//指针2
		DWORD size1;		
		DWORD size2;

		if( ( lpSoundBuffer->Lock(0,BufLong,&ptr1,&size1,&ptr2,&size2,0))!=DS_OK)
		{
			_ERROR("error Lock DSoundBuffer");
			return FALSE;
		}
		
		//拷贝数据到声音缓冲区
		memcpy(ptr1,Buf+WAV_HDR_SIZE,size1);
		if(size2) memcpy(ptr2,Buf+size1+WAV_HDR_SIZE,size2);
		
		if( lpSoundBuffer->Unlock(ptr1,size1,ptr2,size2)!=DS_OK)
		{
			_ERROR("error UnLock SoundBuffer");
			return FALSE;
		}
	}
	_FREE(Buf);
	return TRUE;//返回
}
