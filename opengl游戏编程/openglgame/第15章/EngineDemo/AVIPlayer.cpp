//========================================================
/**
*  @file      AVIPlayer.cpp
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

#include "stdafx.h"
#include "aviplayer.h"


AVIPlayer::AVIPlayer(void)
{
	m_hWnd=NULL;
	m_hmmWnd=NULL;
}


AVIPlayer::~AVIPlayer(void)
{
	Stop();
	m_hWnd=NULL;
}


BOOL AVIPlayer::Init(HWND _hWnd)
{
	if(_hWnd)
	{
		m_hWnd=_hWnd;
		return TRUE;
	}
	else 
		return FALSE;
}