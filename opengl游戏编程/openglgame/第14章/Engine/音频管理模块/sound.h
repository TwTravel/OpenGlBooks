//========================================================
/**
*  @file      sound.h
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

#ifndef _SOUND_H_
#define _SOUND_H_

#include "StdAfx.h"

#define DIRECTSOUND_VERSION 0x0800
#include <dmusicc.h>
#include <dmusici.h>
#include <d3d8types.h>


/**\brief
 *  音效类
 */
class CSound
{
public:
	//! 后台缓冲区
	LPDIRECTSOUNDBUFFER DSSoundFX;

public:
	CSound();
	~CSound();

	
	/**\brief
	 * 初始化声音设备
	 *
	 *\param	_hWnd	窗口句柄
	 *\return	BOOL	初始化是否成功
	 */
	static BOOL InitDirectSound(HWND _hWnd);


	/**\brief
	 * 创建主声音缓冲区
	 */
	static BOOL CreatePrimarySoundBuffer();


	/**\brief
	 * 释放声音设备
	 */
	static void ReleaseDirectSound(void);


	/**\brief
	 * 音效开关
	 */
	static void EnableSound(BOOL sw){CSound::bEnable=sw;}


	/**\brief
	 * 装入声音
	 *
	 *\param	filename	声音文件名
	 *\return	BOOL		载入是否成功
	 */
	BOOL LoadSoundFX(char *filename);


	/**\brief
	 * 播放声音
	 *
	 *\param	loop	为-1时可以循环播放
	 */
	void Play(DWORD loop=0);


	/**\brief
	 * 停止播放
	 */
	void Stop();


	/**\brief
	 * 缓冲区是否正在播放
	 */
	BOOL IsPlaying();


	/**\brief
	 * 填充一个WAV声音缓冲区
	 *
	 *\param	lpSoundBuffer	声音缓冲区
	 *\param	filename		声音文件名
	 *\return	BOOL			填充是否成功
	 */
	BOOL LoadSoundBuffer(LPDIRECTSOUNDBUFFER lpSoundBuffer,char *filename);


private:
	//! 允许音效
	static BOOL bEnable;

	//! 是否已经初始化
	static BOOL bExist;

	//! 设备
	static LPDIRECTSOUND lpDSound;

	//! 主缓冲区
	static LPDIRECTSOUNDBUFFER lpPrimarySoundBuffer;
};


#endif