//=========================================================================
/**
*  @file      PlanarShadow.h
*
*  项目描述： 平面投射阴影
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


#include "GLFrame.h"												/**< 包含基本的框架类 */
#include "Font.h"
#include "Vector.h"
#include <vector>
using namespace std;

/** 墙结构体 */
struct Wall
{
	vector<Vector3> vVerts;	/**< 顶点数组 */
	Vector3 vNormal;		/**< 法向量 */
	Vector3 vColor;			/**< 颜色 */
};

/** 墙四面体的信息数组 */
vector<Wall> walls;


/** 从GL_Application派生出一个子类 */
class CPlanarShadow : GLApplication								
{
public:
	bool	Init();							   /**< 执行所有的初始化工作，如果成功函数返回true */
	void	Uninit();						   /**< 执行所有的卸载工作 */
	void	Update(DWORD milliseconds);		   /**< 执行所有的更新操作，传入的参数为两次操作经过的时间，以毫秒为单位 */
	void	Draw();							   /**< 执行所有的绘制操作 */
	
	
	void    CaculateFrameRate();               /**< 计算帧速 */
	void    PrintText();                       /**< 输出文字信息 */
    void    RenderWall(const Wall& wall);      /**< 绘制墙四面体 */
	void    RenderObjects();                   /**< 绘制物体 */
	void    RenderLight();                     /**< 绘制光源 */
	
	/** 创建投影矩阵 */
	void    CreateShadowMatrix(float m[16], Vector3 point, 
		                       Vector3 normal, float lp[4]);
	/** 绘制物体和阴影 */
	void    Render();                          
	
private:
	friend class GLApplication;				   /**< 父类为它的一个友元类，可以用来创建程序的实例，见函数GL_Application * GL_Application::Create(const char * class_name) */
	CPlanarShadow(const char * class_name);	   /**< 构造函数 */

	/** 用户自定义的程序变量 */ 
	GLFont         m_Font;                        /**< 字体类 */
  	float          m_Fps;                         /**< 帧速 */

};


#endif	// __DEMO_H__