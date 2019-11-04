//========================================================
/**
*  @file      main.h
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


#ifndef __MAIN_H__
#define __MIAN_H__

#include "stdafx.h"
#include "gameapp.h"


/**\brief
 * 主函数，程序的入口
 */
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow);



/**\brief
 * 窗口消息处理函数
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



#endif