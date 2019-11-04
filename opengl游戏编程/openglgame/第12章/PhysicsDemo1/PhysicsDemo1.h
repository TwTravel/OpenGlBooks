//=========================================================================
/**
*  @file      PhysicsDemo1.h
*
*  项目描述： 匀速运动演示
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

#ifndef __DEMO_H__
#define __DEMO_H__

#include "stdafx.h"
#include "GLFrame.h"												/**< 包含基本的框架类 */
#include "Input.h"

/** 从GL_Application派生出一个子类 */
class CDemo : GLApplication								
{
public:
	bool	Init();								    /**< 初始化工作 */
	void	Uninit();								/**< 卸载工作 */
	void	Update(DWORD milliseconds);				/**< 更新操作 */
	void	Draw();									/**< 绘制操作 */


	void    DrawGrid();                             /**< 绘制网格地面 */
	void    DrawSphere(float x,float y);            /**< 在指定位置绘制球体 */

private:
	friend class GLApplication;					    /**< 父类为它的一个友元类 */
	CDemo(const char * class_name);				    /**< 构造函数 */
   
	/** 用户自定义的程序变量 */
	CInputSystem*  m_pInput;                         /**< 输入系统 */
	float x,y,vx;                                    /**< 小球的位置和X方向速度 */
	      
};


#endif	// __DEMO_H__