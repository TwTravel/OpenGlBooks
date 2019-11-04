//=========================================================================
/**
*  @file      BitmapFont.h
*
*  项目描述： OpenGL字体
*  文件描述:  具体实例类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-12-02	
*  修改日期： 2007-07-10
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

#ifndef __BITMAPFONT_H__
#define __BITMAPFONT_H__

#include "stdafx.h"
#include "GLFrame.h"			/**< 包含基本的框架类 */
#include "GLFont.h"             /**< 包含字体定义类 */


/** 从GL_Application派生出一个子类 */
class BitmapFont : GLApplication								
{
public:
	bool	Init();							/**< 执行所有的初始化工作，如果成功函数返回true */
	void	Uninit();						/**< 执行所有的卸载工作 */
	void	Update(DWORD milliseconds);		/**< 执行所有的更新操作，传入的参数为两次操作经过的时间，以毫秒为单位 */
	void	Draw();							/**< 执行所有的绘制操作 */

private:
	friend class GLApplication;				/**< 父类为它的一个友元类，可以用来创建程序的实例，见函数GL_Application * GL_Application::Create(const char * class_name) */
	BitmapFont(const char * class_name);	/**< 构造函数 */

	/** 用户自定义的程序变量 */ 
	GLFont font;                /**< 定义一个字体实例 */
	float	cnt1;				/**< 字体移动计数器1 */
    float	cnt2;				/**< 字体移动计数器2 */
				
};


#endif	// __BITMAPFONT_H__