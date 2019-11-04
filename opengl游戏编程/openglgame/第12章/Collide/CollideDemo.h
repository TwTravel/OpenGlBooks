//=========================================================================
/**
*  @file      CollideDemo.h
*
*  项目描述： 碰撞检测演示
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
#include "Font.h"
#include "Vector.h"
#include "BMPLoader.h"
#include "Tray.h"


/** 平面结构 */
struct Plane
{
	Vector3 _Position;
	Vector3 _Normal;
};
                                         
/** 圆柱结构 */
struct Cylinder
{                          
	Vector3 _Position;
    Vector3 _Axis;
    double _Radius;
};
                                          
/** 爆炸结构 */
struct Explosion
{
	Vector3 _Position;
	float   _Alpha;
	float   _Scale;
};



/** 从GL_Application派生出一个子类 */
class CDemo : GLApplication								
{
public:
	bool	Init();								    /**< 初始化工作 */
	void	Uninit();								/**< 卸载工作 */
	void	Update(DWORD milliseconds);				/**< 更新操作 */
	void	Draw();									/**< 绘制操作 */

	void    CaculateFrameRate();                    /**< 计算帧速 */
	void    PrintText();                            /**< 输出文字信息 */

	void    InitVars();                             /**< 初始化变量 */
	bool    LoadTexture();                          /**< 载入纹理 */

	/** 检测与平面是否碰撞 */
	int TestIntersionPlane(const Plane& plane,const Vector3& position,
		                   const Vector3& direction, double& lamda, 
						   Vector3& pNormal);
    
	/** 检测是否与圆柱体碰撞 */
	int TestIntersionCylinder(const Cylinder& cylinder,const Vector3& position,
		                      const Vector3& direction, double& lamda, 
							  Vector3& pNormal,Vector3& newposition);
    
	/** 找到任两个球在当前时间片的碰撞点 */
	int FindBallCol(Vector3& point, double& TimePoint, double Time2, 
		            int& BallNr1, int& BallNr2);
  
	/** 主要的处理过程 */
	void    Process();      
	
private:
	friend class GLApplication;				   /**< 父类为它的一个友元类 */
	CDemo(const char * class_name);			   /**< 构造函数 */
   
	/** 用户自定义的程序变量 */
	CInputSystem*  m_pInput;                   /**< 输入系统 */
    CBMPLoader     m_texture[4];               /**< 纹理数组 */
	GLUquadricObj  *cylinder_obj;              /**< 使用二次几何体渲染圆柱 */
	GLuint         dlist;                      /**< 保存显示列表 */
	Plane          pl1,pl2,pl3,pl4,pl5;        /**< 房间的5个平面 */
    Cylinder       cyl1,cyl2,cyl3;             /**< 3个圆柱 */
    Explosion      ExplosionArray[20];         /**< 保存爆炸效果 */
 	GLFont         m_Font;                     /**< 字体类 */
  	float          m_Fps;                      /**< 帧速 */
	      
};


#endif	// __DEMO_H__