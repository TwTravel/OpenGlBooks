//=========================================================================
/**
*  @file      MultiTexture.h
*
*  项目描述： 纹理映射
*  文件描述:  具体实例类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-09-13	
*  修改日期： 2006-12-11
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

#ifndef __MULTITEXTURE_H__
#define __MULTITEXTURE_H__

#include "stdafx.h"
#include "CBMPLoader.h"
#include "GLFrame.h"	



/** 从GL_Application派生出一个子类 */
class MultiTexture : GLApplication								
{
public:
	bool	Init();							   /**< 执行所有的初始化工作 */
	void	Uninit();						   /**< 执行所有的卸载工作 */
	void	Update(DWORD milliseconds);		   /**< 执行所有的更新操作 */
	void	Draw();							   /**< 执行所有的绘制操作 */

	bool    isExtensionSupported(const char *string);  /**< 检查是否支持扩展 */
	bool    initMultiTexture();                        /**< 初始化 */
	bool    loadTexture();                             /**< 装载纹理 */

private:
	friend class GLApplication;				   
	MultiTexture(const char * class_name);	           /**< 构造函数 */

	/** 用户自定义的程序变量 */ 
	CBMPLoader   m_texture[4];            /**< 载入4幅纹理 */
	bool         multitexturing;          /**< 标志是否使用多重纹理 */
	bool         sp;                      /**< 判断空格键是否释放 */
			
};

#endif	// __MULTITEXTURE_H__