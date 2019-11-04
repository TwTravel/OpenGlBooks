//=========================================================================
/**
*  @file      SnowDemo.h
*
*  项目描述： 粒子系统
*  文件描述:  具体实例类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-09-13	
*  修改日期： 2006-12-02
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

#ifndef __SNOWDEMO_H__
#define __SNOWDEMO_H__

#include "stdafx.h"
#include "GLFrame.h"												/**< 包含基本的框架类 */
#include "Font.h"
#include "Snow.h"

/** 从GL_Application派生出一个子类 */
class SnowDemo : GLApplication								
{
public:
	bool	Init();							/**< 执行所有的初始化工作 */
	void	Uninit();						/**< 执行所有的卸载工作 */
	void	Update(DWORD milliseconds);		/**< 执行所有的更新操作 */
	void	Draw();							/**< 执行所有的绘制操作 */
	
	void    CaculateFrameRate();            /**< 计算帧速 */
	void    PrintText();                    /**< 输出文字信息 */
	void    DrawGround();                   /**< 绘制地面 */
		
	
private:
	friend class GLApplication;				/**< 父类为它的一个友元类 */
	SnowDemo(const char * class_name);	    /**< 构造函数 */

	/** 用户自定义的程序变量 */ 
	CSnow       m_Snow;                      /**< 雪花实例 */
	GLFont      m_Font;                      /**< 字体类 */
	CBMPLoader  m_TextureGround;             /**< 地面纹理 */
	float       m_Fps;                       /**< 帧速 */
	float       m_RotAngle;                  /**< 地面的旋转角度 */
	
};


#endif	// __SNOWDEMO_H__