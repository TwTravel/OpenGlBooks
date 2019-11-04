//======================================================================
/**
*  @file      Sound.h
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

#ifndef __SOUND_H__
#define __SOUND_H__

#include "stdafx.h"

/** 定义WAV文件头结构 */
typedef struct sWaveHeader
{
	char  RiffID[4];          // 'RIFF'块 
	long  WaveformChunkSize;  // RIFF块的大小，4个字节 
	char  WaveID[4];          // .wav文件为‘WAVE’
	char  FormatID[4];        // 'fmt'块 后面有一个空格 
	long  FormatChunkSize;    //fmt块的大小，16个字节
	short Channels;           //声道数
	long  SampleRate;         //采样频率
	long  BytesPerSec;        //每秒采样字节数
	short BlockAlign;         //采样块对齐大小
	short BitsPerSample;      //bit位数
	char  DataID[4];          //data块
	long  DataSize;           //data块的大小，实际声音数据大小
}WaveFileHeader;


/** DirectSound音效类 */
class CSound
{
public:
	CSound();
	~CSound();
	
	/** 载入.wav文件 */
	bool LoadWaveFile(const char* fileName);
	
	/** 创建辅助缓冲区 */
	bool GetSecondaryBuffer(FILE* fp);

	/** 载入声音数据 */
	bool LoadData(FILE* fp);

	/** 播放 */
	bool  Play(bool bLoop);

	/** 停止 */
	bool  Stop();


private:
    /** DirectSoundBuffer接口对象 */
	IDirectSoundBuffer8*   m_pDirectSoundBuf;
};


/** 音频系统管理类 */
class CSoundManager
{
public:
	CSoundManager();
	~CSoundManager();

	/** 初始化 */
	bool Init(HWND  hwnd,DWORD dwCoopLevel = DSSCL_NORMAL);

	/** 设置主缓冲区 */
	bool SetPrimaryBuffer(WORD channels = 1,DWORD dwSamplesPerSec = 22050,
		                  WORD wBitsPerSample = 16);

	/** 获得当前CSoundManager类 */
	static CSoundManager*  GetSoundManager(){ return  m_pSoundManager; }

	/** 获得DirectSound8对象 */
	IDirectSound8*  GetDirectSound()        { return  m_pDirectSound; }


private:
	/** 当前全局音频系统类 */
	static CSoundManager*  m_pSoundManager;

	/** DirectSound8接口对象 */
	IDirectSound8*         m_pDirectSound;

};


#endif //__SOUND_H__