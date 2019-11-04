//========================================================
/**
*  @file      Timer.h
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


/**
=========================使用方法=========================

#include "Timer.h"

CTimer*	timer;		// 高分辨率计时器

timer = new CTimer;
timer->Init();	// 记时器初始化

timer->GetElapsedSeconds();

delete timer;

=========================================================*/


#ifndef _TIMER_H_
#define _TIMER_H_

#include "StdAfx.h"


/**\brief
 * 高分辨率计时器类
 */
class CTimer
{
public:
	CTimer();
	~CTimer();

	//! 经过的时间，单位：秒
	float	m_fTime;

	//! 帧率
	float	m_fFPS;


	/**\brief
	 * 计时器的初始化
	 *
	 * 如果系统有高分辨率计时器，将保存tick频率，返回TRUE。
	 * 否则函数返回FALSE,不能使用该计时器。
	 */
	BOOL Init();


	/**\brief
	 * 获得经过的时间
	 */
	float GetElapsedSeconds();


	/**\brief
	 * 获取帧率
	 */
	float GetFPS(unsigned long elapsedFrames=1);


	/**\beief
	 * 锁定帧率
	 */
	float LockFPS(unsigned char targetFPS);


	/**\brief
	 * 刷新计时器
	 */
	void Update(){m_fTime=GetElapsedSeconds();m_fFPS=GetFPS();}


	/**\brief
	 * 取得当前计时器的指针
	 */
	static CTimer* GetSysTimer(){return m_pTimer;}


private:
	LARGE_INTEGER   m_startTime;
	LARGE_INTEGER   m_ticksPerSecond;

	static CTimer*	m_pTimer;
};

#endif