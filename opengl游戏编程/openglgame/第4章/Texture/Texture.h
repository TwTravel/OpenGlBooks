//=========================================================================
/**
*  @file      Texture.h
*
*  项目描述： 纹理映射
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

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "stdafx.h"
#include "GLFrame.h"												/**< 包含基本的框架类 */
#include "CBMPLoader.h"
#include "TGALoader.h" 


/** 从GL_Application派生出一个子类 */
class Texture : GLApplication								
{
public:
	bool	Init();							/**< 执行所有的初始化工作，如果成功函数返回true */
	void	Uninit();						/**< 执行所有的卸载工作 */
	void	Update(DWORD milliseconds);		/**< 执行所有的更新操作，传入的参数为两次操作经过的时间，以毫秒为单位 */
	void	Draw();							/**< 执行所有的绘制操作 */
	
	
	bool    LoadTexture();                  /**< 载入纹理 */
	void    SetLight();                     /**< 设置光源 */
	void    DrawSphere();                   /**< 绘制球体 */
	void    DrawBox();                      /**< 绘制木箱 */
private:
	friend class GLApplication;				/**< 父类为它的一个友元类，可以用来创建程序的实例，见函数GL_Application * GL_Application::Create(const char * class_name) */
	Texture(const char * class_name);	    /**< 构造函数 */

	/** 用户自定义的程序变量 */ 
	CBMPLoader texture1;                     /**< 位图载入类 */
	CTGALoader texture2;                     /**< TGA文件载入类 */
	float  rot;					             /**< 用于旋转物体 */	
				
};


#endif	// __TEXTURE_H__