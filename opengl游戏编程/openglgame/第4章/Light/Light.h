//=========================================================================
/**
*  @file  Light.h
*
*  项目描述： OPenGL光照效果演示
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

#ifndef __LIGHT_H__
#define __LIGHT_H__


#include "GLFrame.h"									/**< 包含基本的框架类 */

/** 包含OpenGL头文件 */
#include <gl\gl.h>											
#include <gl\glu.h>												
#include <gl\glaux.h>											

/** 包含OpenGL链接库文件 */
#pragma comment(lib, "opengl32.lib")						
#pragma comment(lib, "glu32.lib")							
#pragma comment(lib, "glaux.lib")							


/** 从GL_Application派生出一个子类 */
class Light : GLApplication								
{
public:
	bool	Init();										/**< 执行所有的初始化工作 */
	void	Uninit();									/**< 执行所有的卸载工作 */
	void	Update(DWORD milliseconds);				    /**< 执行所有的更新操作 */
	void	Draw();										/**< 执行所有的绘制操作 */

private:
	friend class GLApplication;							/**< 父类为它的一个友元类 */
	Light(const char * class_name);						/**< 构造函数 */
/** 用户自定义的程序变量 */
	 
	float m_Angle;                                      /**< 用来旋转物体 */
    float m_bRedX;                                      /**< 红色球的旋转角度 */
	float m_bRedY;
	float m_bGreenX;                                    /**< 绿色球的旋转角度 */
	float m_bGreenY;
	float m_bBlueX;                                     /**< 蓝色球的旋转角度 */
	float m_bBlueY; 

};


#endif	// __LIGHT_H__