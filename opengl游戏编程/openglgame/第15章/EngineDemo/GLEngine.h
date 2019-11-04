//========================================================
/**
*  @file      GLEngine.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  游戏引擎类----游戏世界中的最上层的组织管理类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================


#ifndef _GL_ENGINE_H_
#define _GL_ENGINE_H_

#include "input.h"
#include "font.h"
#include "camera.h"
#include "timer.h"
#include "sound.h"
#include "music.h"
#include "StaticObjMgr.h"
#include "role.h"
#include "monstersMgr.h"
#include "SceneMgr.h"
#include "GUIMgr.h"
#include "Message.h"


/*! 引擎类*/
class GLEngine
{
public:
	GLEngine(void);
	~GLEngine(void);

	BOOL Init(void);				//!< 初始化，包括opengl方面的初始化

	void SetProjection(int iFOVAngle,int iHeight);//!< 设置矩阵

	void Prepare(void);				//!< 渲染前的准备工作

	void Render(void);				//!< 3D渲染

	void SwapBuffer(void){::SwapBuffers(m_hDC);}//!< 交换缓冲区

	void Close(void);				//!< 关闭引擎

private:
	BOOL InitObject(void);			//!< 游戏中的实例对象的初始化

	BOOL SetDCPixelFormat(void);	//!< 设置opengl象素格式


	HDC			m_hDC;		//!< 设备句柄

	HGLRC		m_hRC;		//!< 渲染句柄

	HZFont*		m_pFont;	//!< 字体

	CInput*		m_pInput;	//!< 输入系统

	CTimer*		m_pTimer;	//!< 计时器

	CCamera		camera;		//!< 摄像机

	CMusic		m_cMusic;	//!< 背景音乐

	SceneMgr	m_cSceneMgr;//!< 场景管理

	GUIMgr		m_cGUIMgr;	//!< 界面管理

	Message		m_cMessage;	//!< 消息处理
};


#endif 