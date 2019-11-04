//========================================================
/**
*  @file      Music.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  DirectMusic音乐类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _MUSIC_H_
#define _MUSIC_H_

#include "StdAfx.h"
#include <dmusicc.h>
#include <dmusici.h>


/**\brief
 * DMusic　音乐播放类
 */
class CMusic
{
public:
	CMusic();
	~CMusic();

	static IDirectMusicSegment* LoadMIDISegment(IDirectMusicLoader* pLoader, WCHAR *wszMidiFileName );

	/**\brief
	 * 初始化DMUSIC
	 */
	static BOOL InitMusic();

	/**\brief
	 * 载入MIDI音乐文件
	 */
	BOOL LoadMusic(WCHAR *);
	BOOL LoadMusic(char* filename){return LoadMusic(WChar(filename));}

	/**\brief
	 * 开始播放
	 */
	BOOL Play(int start=0);

	/**\brief
	 * 停止播放
	 */
	BOOL Stop();

	/**\brief
	 * 暂停播放
	 */
	BOOL Pause();

	/**\brief
	 * 暂停后用来继续播放
	 */
	BOOL Resume();

	/**\brief
	 * 是否正在播放
	 */
	BOOL IsPlay();

	/**\brief
	 * 得到当前音乐对象的指针
	 */
	static CMusic* GetSysMusic(){return m_pMusic;}

private:
	//是否有音乐
	static bool bMusic;

	//主缓冲是否已存在
	static bool bExist;
	
	//是否播放
	static bool bPlay;
	static IDirectMusicPerformance* g_pPerf;
	static IDirectMusicLoader* g_pLoader;
	IDirectMusicSegment* g_pMIDISeg;

	//暂停点
	MUSIC_TIME PauseTime;

	//文件
	WCHAR MusicFile[MAX_PATH];

	//! 对象指针
	static CMusic* m_pMusic;

	static IDirectMusicPerformance* CreatePerformance(void);
	static IDirectMusicLoader* CreateLoader(void);
};

#endif