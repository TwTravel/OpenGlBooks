//========================================================
/**
*  @file      structdef.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  常用结构定义  
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
#include "structdef.h"
#include "IniFile.h"



/** 下面是SWindowInfo结构的实现  */

//! SWindowInfo结构的静态变量初始化
SWindowInfo*	SWindowInfo::m_pInfo=NULL;
BOOL			SWindowInfo::m_bFullScreen=FALSE;
int				SWindowInfo::m_iWidth=640;
int				SWindowInfo::m_iHeight=480;
int				SWindowInfo::m_iBits=32;
HINSTANCE		SWindowInfo::m_hInstance=NULL;
HWND			SWindowInfo::m_hWnd=NULL;
char*			SWindowInfo::m_strWinName=NULL;

/**\brief
 * 从INI文件载入窗口信息
 */
BOOL SWindowInfo::LoadIni(char* _filename,char* _index)
{
	CIniFile t_cIni(_filename);
	m_iWidth		=t_cIni.ReadInt(_index,"width");
	m_iHeight		=t_cIni.ReadInt(_index,"height");
	m_iBits			=t_cIni.ReadInt(_index,"bits");
	m_bFullScreen	=t_cIni.ReadInt(_index,"FullScreen");
	m_strWinName	=t_cIni.ReadText(_index,"WindowName");

	m_hInstance = GetModuleHandle(NULL);

	return TRUE;
}

