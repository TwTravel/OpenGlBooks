//=========================================================================
/**
*  @file      SoundDemo.h
*
*  项目描述： DirectSound声音演示
*  文件描述:  具体实例类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-09-13	
*  修改日期： 2006-10-10
*
*  在这个类中您必须重载如下几个虚函数
*																								
*	virtual bool Init();														
*		执行所有的初始化工作，如果成功函数返回true							
*																			
*	virtual void Uninit();													
*		执行所有的卸载工作										
*																			
*	virtual void Update(DWORD milliseconds);										
*		执行所有的更新操作，传入的参数为两次操作经过的时间，以毫秒为单位
*																			
*	virtual void Draw();															
*		执行所有的绘制操作
*/
//=========================================================================

#ifndef __SOUNDDEMO_H__
#define __SOUNDDEMO_H__

#include "stdafx.h"
#include "GLFrame.h"												
#include "Input.h"
#include "Font.h"
#include "Sound.h" 

/** 从GL_Application派生出一个子类 */
class CSoundDemo : GLApplication								
{
public:
	bool	Init();										/**< 初始化工作 */
	void	Uninit();									/**< 卸载工作 */
	void	Update(DWORD milliseconds);					/**< 更新操作 */
	void	Draw();										/**< 绘制操作 */

private:
	friend class GLApplication;							/**< 父类为它的一个友元类 */
	CSoundDemo(const char * class_name);			    /**< 构造函数 */
	GLFont         m_hFont;                             /**< 字体类 */								
    CInputSystem*  m_pInput;                            /**< 输入系统 */
	CSoundManager* m_pSoundManager;                     /**< 音频系统管理类 */
	CSound*        m_pSound1;                           /**< 声音1 */         
	CSound*        m_pSound2;                           /**< 声音2 */
	
};


#endif	// __KEYBOARDDEMO_H__