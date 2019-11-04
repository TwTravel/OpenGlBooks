//=========================================================================
/**
*  @file      Robot.h
*
*  项目描述： OpenGL变换
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

#ifndef __ROBOT_H__
#define __ROBOT_H__


#include "GLFrame.h"												/**< 包含基本的框架类 */

/** 从GL_Application派生出一个子类 */
class Robot : GLApplication								
{
public:
	bool	Init();										   /**< 初始化e */
	void	Uninit();									   /**< 卸载 */
	void	Update(DWORD milliseconds);					   /**< 更新 */
	void	Draw();										   /**< 绘制 */

	void    DrawRobot(float xPos, float yPos, float zPos); /**< 绘制机器人 */

private:
	void    DrawHead(float xPos,float yPos,float zPos);    /**< 绘制头部 */
	void    DrawTorso(float xPos, float yPos, float zPos); /**< 绘制躯干 */
	void    DrawLeg(float xPos, float yPos, float zPos);   /**< 绘制腿 */
	void    DrawArm(float xPos, float yPos, float zPos);   /**< 绘制胳膊 */
   
	void    DrawCube(float xPos, float yPos, float zPos);  /**< 绘制立方体 */

private:
	friend class GLApplication;							  /**< 父类为它的一个友元类 */
	Robot(const char * class_name);						  /**< 构造函数 */

		
	float angle;            /**< 机器人绕视点旋转的角度 */
	float legAngle[2];		/**< 腿的当前旋转角度 */
    float armAngle[2];      /**< 胳膊的当前旋转角度 */

};


#endif	// __ROBOT_H__