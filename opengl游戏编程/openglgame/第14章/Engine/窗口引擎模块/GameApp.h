//========================================================
/**
*  @file      GameApp.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  应用程序类,完成程序的初始化、
*             窗口的创建，这是最顶层的类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================


#ifndef __GAME_APP_H__
#define __GAME_APP_H__

#include "GLEngine.h"
#include "Ime.h"
#include "IniFile.h"


/*! 应用程序类 */
class GameApp  
{
public:
	GameApp(void);
	~GameApp(void);

	BOOL	Init(SWindowInfo _sWinInfo);	//!< 初始化程序，窗口、输入等

	/**\brief
	 * 创建窗口
	 *
	 *\param	_sWinInfo	窗口信息
	 *\return	BOOL		创建是否成功
	 */
	BOOL	CreateGLWindow(SWindowInfo _sWinInfo);

	/**\brief
	 * 窗口消息处理函数
	 */
	LRESULT WINAPI MessageHandler(HWND hWnd,UINT message,WPARAM  wParam, LPARAM lParam);

	void Run(void);				//!< 程序的主循环


private:
	SWindowInfo	m_sWinInfo;		//!< 窗口信息

	MSG			msg;			//!< 消息
	
	BOOL		m_bActive;		//!< 窗口是否处于活动状态

	GLEngine	m_cEngine;		//!< 游戏的引擎实例

	void KillGLWindow(void);	//!< 销毁窗口
};


#endif