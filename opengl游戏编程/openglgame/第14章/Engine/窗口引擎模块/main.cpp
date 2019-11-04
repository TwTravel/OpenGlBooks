//========================================================
/**
*  @file      main.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  程序的入口
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
#include "resource.h"
#include "main.h"

//////////////////////////////////////////////////////////
//-----------------下面是全局变量的定义-------------------
//! 窗口信息结构
SWindowInfo* g_sWindowInfo;

//! 应用程序
GameApp*	 g_cGameApp;



//////////////////////////////////////////////////////////
//-----------------下面是全局函数的实现-------------------

/**\brief
 * 主函数，程序的入口
 */
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	g_cGameApp		= new GameApp();
	g_sWindowInfo	= new SWindowInfo;

	if(!g_sWindowInfo->LoadIni("ini\\init.ini","window"))
	{
		_ERROR("Window Info Init failed!");
		_DELETE(g_cGameApp);
		return 0;
	}

	if(!g_cGameApp->Init(*g_sWindowInfo))
	{
		_ERROR("App init failed!");
		_DELETE(g_cGameApp);
		return 0;
	}

	g_cGameApp->Run();

	_DELETE(g_cGameApp);
	_DELETE(g_sWindowInfo);

	return 0;
}



/**\brief
 * 窗口消息处理函数
 */
LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	return g_cGameApp->MessageHandler(hWnd,message,wParam,lParam);
}

