//========================================================
/**
*  @file      Timer.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  高分辨率计时器,这是整个游戏的时间系统  
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
#include "Timer.h"

CTimer* CTimer::m_pTimer=NULL;

CTimer::CTimer(){m_pTimer=this;}
CTimer::~CTimer(){}

/**\brief
 * 计时器的初始化
 *
 * 如果系统有高分辨率计时器，将保存tick频率，返回TRUE。
 * 否则函数返回FALSE,不能使用该计时器。
 */
BOOL CTimer::Init()
{
	if (!QueryPerformanceFrequency(&m_ticksPerSecond))
	{
		return FALSE;// 系统不支持高分辨率计时器
	}

	else
	{
		QueryPerformanceCounter(&m_startTime);
		return TRUE;
	}
}



/**\brief
 * 获得经过的时间
 */
float CTimer::GetElapsedSeconds()
{
	static LARGE_INTEGER s_lastTime = m_startTime;
	LARGE_INTEGER currentTime;

	QueryPerformanceCounter(&currentTime);

	float seconds=(float)(currentTime.QuadPart-s_lastTime.QuadPart)/(float)m_ticksPerSecond.QuadPart;

	// 记时器复位
	s_lastTime = currentTime;

	return seconds;
}


/**\brief
 * 获取帧率
 */
float CTimer::GetFPS(unsigned long elapsedFrames)
{
	static LARGE_INTEGER s_lastTime = m_startTime;
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	float fps = (float)elapsedFrames * (float)m_ticksPerSecond.QuadPart / ((float)currentTime.QuadPart - (float)s_lastTime.QuadPart);
	// 重置定时器
	s_lastTime = currentTime;
	return fps;
} 


/**\beief
 * 锁定帧率
 */
float CTimer::LockFPS(unsigned char targetFPS)
{
	if (targetFPS == 0)targetFPS = 1;

	static LARGE_INTEGER s_lastTime = m_startTime;
	LARGE_INTEGER currentTime;
	float   fps;
	// 延时以保证固定的帧率
	do
	{
		QueryPerformanceCounter(&currentTime);
		fps=(float)m_ticksPerSecond.QuadPart/((float)(currentTime.QuadPart-s_lastTime.QuadPart));
	}
	while (fps > (float)targetFPS);
	// 重置定时器
	s_lastTime = m_startTime;

	return fps;
}
