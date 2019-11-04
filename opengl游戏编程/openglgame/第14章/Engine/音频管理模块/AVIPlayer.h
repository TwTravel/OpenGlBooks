//========================================================
/**
*  @file      AVIPlayer.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  AVI播放类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef __AVI_PLAYER__
#define __AVI_PLAYER__

#include "stdafx.h"
#include <vfw.h>
#pragma comment(lib,"vfw32.lib")


/**\brief
 * AVI播放类
 */
class AVIPlayer
{
public:
	AVIPlayer(void);
	~AVIPlayer(void);


	/**\brief
	 * 初始化播放器
	 */
	BOOL Init(HWND _hWnd);
	
	
	/**\brief
	 * 载入AVI文件
	 */
	BOOL Load(char* _filename);

	
	/**\brief
	 * 播放影片
	 */
	void Play();


	/**\brief
	 * 暂停播放
	 */
	void Pause();


	/**\brief
	 * 停止播放
	 */
	void Stop();


private:
	//! 主窗口句柄
	HWND m_hWnd;

	//! MCI窗口句柄
	HWND m_hmmWnd;

};


////////////////////////////////////////////////////
//--------------inline函数实现----------------------

/**\brief
 * 载入AVI文件
 */
inline BOOL AVIPlayer::Load(char* _filename)
{
	if(!m_hmmWnd)
	{
		m_hmmWnd=MCIWndCreate(m_hWnd,(HINSTANCE)GetModuleHandle(NULL),WS_CHILD|WS_VISIBLE|MCIWNDF_NOMENU|MCIWNDF_NOPLAYBAR,_filename);
		RECT r; GetClientRect(m_hWnd,&r);	
		SetWindowPos(m_hmmWnd,0,0,0,r.right-r.left,r.bottom-r.top,SWP_NOZORDER);
	}

	return TRUE;
}


/**\brief
 * 播放影片
 */
inline void AVIPlayer::Play()
{
	if(m_hWnd)MCIWndPlay(m_hmmWnd);
}


/**\brief
 * 暂停播放
 */
inline void AVIPlayer::Pause()
{
	if(m_hWnd)MCIWndPause(m_hmmWnd);
}


/**\brief
 * 停止播放
 */
inline void AVIPlayer::Stop()
{
	if(m_hmmWnd)
	{
		MCIWndDestroy(m_hmmWnd);
		m_hmmWnd=NULL;
	}
}



#endif