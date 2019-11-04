//=========================================================================
/**
*  @file  Example.cpp
*
*  项目描述： OpenGL程序框架演示
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

#include "Example.h"											/**< 包含头文件 */

#include <gl\gl.h>												/**< 包含OpenGL头文件 */
#include <gl\glu.h>												
#include <gl\glaux.h>											


#pragma comment(lib, "opengl32.lib")							/**< 包含OpenGL链接库文件 */
#pragma comment(lib, "glu32.lib")							
#pragma comment(lib, "glaux.lib")							


/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	Example * example = new Example(class_name);
	return reinterpret_cast<GLApplication *>(example);
}


/** 构造函数 */
Example::Example(const char * class_name) : GLApplication(class_name)
{
  /// 初始化用户自定义的程序变量
	m_Angle = 0.0f;												/**< 设置初始角度为0 */
}

/** 初始化OpenGL */
bool Example::Init()									
{
/** 用户自定义的初始化过程 */
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);						
	glClearDepth(1.0f);											
	glDepthFunc(GL_LEQUAL);										
	glEnable(GL_DEPTH_TEST);									
	glShadeModel(GL_SMOOTH);									
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			
	ResizeDraw(true);											/**< 改变OpenGL窗口大小，直接调用子类的函数 */

	return true;												/**< 成功返回 */
}

/** 用户自定义的卸载函数 */
void Example::Uninit()									
{
/** 用户自定义的卸载过程 */
///......
///......
}

/** 程序更新函数 */
void Example::Update(DWORD milliseconds)						
{
	if (m_Keys.IsPressed(VK_ESCAPE) == true)					/**< 按ESC退出 */
	{
		TerminateApplication();									
	}

	if (m_Keys.IsPressed(VK_F1) == true)						/**< 按F1切换窗口/全屏模式 */
	{
		ToggleFullscreen();										
	}
/** 用户自定义的更新过程 */
	m_Angle += (float)(milliseconds) / 15.0f;					/**< 更新旋转的角度 */
     
}

/** 绘制函数 */
void Example::Draw()											
{
/** 用户自定义的绘制过程 */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();											
	glTranslatef(0.0f, 0.0f, -6.0f);							
	glRotatef(m_Angle, 0.0f, 1.0f, 0.0f);						
	glRotatef(m_Angle * 0.7f, -1.0f, 0.0f, 0.0f);				

	for (int rot1 = 0; rot1 < 2; rot1++)						/**< 绘制四个交错的三角形（想的很巧妙啊）*/
	{
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);						
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);					
		for (int rot2 = 0; rot2 < 2; rot2++)					
		{
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);				
			glBegin(GL_TRIANGLES);								
				glColor3f(1.f, 0.f, 0.f);	glVertex3f( 0.0f,  1.0f, 0.0f);
				glColor3f(0.f, 1.f, 0.f);	glVertex3f(-1.0f, -1.0f, 1.0f);
				glColor3f(0.f, 0.f, 1.f);	glVertex3f( 1.0f, -1.0f, 1.0f);
			glEnd();											
		}
	}
	glFlush();													/**< 强制执行所有的OpenGL命令 */
}
