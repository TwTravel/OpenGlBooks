//========================================================
/**
*  @file      Message.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  消息处理类
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
#include "message.h"
#include "sceneMgr.h"
#include "guimgr.h"
#include "music.h"

Message* Message::m_pMessage=NULL;

Message::Message(void)
{
	m_pMessage=this;
}


Message::~Message(void)
{
}

/**\brief
 * 执行当前消息
 */
void Message::Run()
{
	if(GetMsgNum()<=0)return;

	switch(m_dQueue[0].msg)
	{
	case MSG_PLAY_MUSIC:
		if(CMusic::GetSysMusic()->IsPlay())CMusic::GetSysMusic()->Stop();
		CMusic::GetSysMusic()->LoadMusic(m_dQueue[0].str1);
		CMusic::GetSysMusic()->Play();
		Delete();
		break;
	case MSG_STOP_MUSIC:
		if(CMusic::GetSysMusic()->IsPlay())CMusic::GetSysMusic()->Stop();
		Delete();
		break;
	case MSG_PAUSE_MUSIC:
		if(CMusic::GetSysMusic()->IsPlay())CMusic::GetSysMusic()->Pause();
		Delete();
		break;
	case MSG_RESUME_MUSIC:
		if(!CMusic::GetSysMusic()->IsPlay())CMusic::GetSysMusic()->Resume();
		Delete();
		break;

	case MSG_FADE:
		GUIMgr::GetSysGUIMgr()->BeginFade();
		Delete();
		break;

	case MSG_PLAYAVI:
		SceneMgr::GetSysSceneMgr()->SetScene(SCE_AVI);
		SceneMgr::GetSysSceneMgr()->LoadIni(m_dQueue[0].str1,m_dQueue[0].str2);
		Delete();
		break;

	case MSG_TITLE:
		SceneMgr::GetSysSceneMgr()->SetScene(SCE_TITLE);
		SceneMgr::GetSysSceneMgr()->LoadIni(m_dQueue[0].str1,m_dQueue[0].str2);
		Delete();
		break;

	case MSG_SELCHAR:
		SceneMgr::GetSysSceneMgr()->SetScene(SCE_SELCHAR);
		SceneMgr::GetSysSceneMgr()->LoadIni(m_dQueue[0].str1,m_dQueue[0].str2);
		Delete();
		break;

	case MSG_OPENAIR:
		SceneMgr::GetSysSceneMgr()->SetScene(SCE_OPENAIR);
		SceneMgr::GetSysSceneMgr()->LoadIni(m_dQueue[0].str1,m_dQueue[0].str2);
		Delete();
		break;

	case MSG_EXIT:
		PostQuitMessage(0);
		Delete();
		break;
	}

}