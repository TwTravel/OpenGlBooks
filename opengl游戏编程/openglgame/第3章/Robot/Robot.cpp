//=========================================================================
/**
*  @file     robot.cpp
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

#include "Robot.h"											    /**< 包含头文件 */

#include <gl\gl.h>												/**< 包含OpenGL头文件 */
#include <gl\glu.h>												
#include <gl\glaux.h>											


#pragma comment(lib, "opengl32.lib")							/**< 包含OpenGL链接库文件 */
#pragma comment(lib, "glu32.lib")							
#pragma comment(lib, "glaux.lib")							


/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	Robot * example = new Robot(class_name);
	return reinterpret_cast<GLApplication *>(example);
}


/** 构造函数 */
Robot::Robot(const char * class_name) : GLApplication(class_name)
{
  /// 初始化用户自定义的程序变量
	angle = 0.0f;                    /**< 设置初始角度为0 */
	legAngle[0] = legAngle[1] = 0.0f;				
	armAngle[0] = armAngle[1] = 0.0f;
}

/** 初始化OpenGL */
bool Robot::Init()									
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
void Robot::Uninit()									
{
/** 用户自定义的卸载过程 */
///......
///......
}

/** 程序更新函数 */
void Robot::Update(DWORD milliseconds)						
{
	if (m_Keys.IsPressed(VK_ESCAPE) == true)					/**< 按ESC退出 */
	{
		TerminateApplication();									
	}

	if (m_Keys.IsPressed(VK_F1) == true)						/**< 按F1切换窗口/全屏模式 */
	{
		ToggleFullscreen();										
	}
	
	//角度更新
	angle = angle + 0.05f;					
	if (angle >= 360.0f)					
		angle = 0.0f;

     
}

/** 绘制立方体 */
void Robot::DrawCube(float xPos, float yPos, float zPos)
{
	glPushMatrix();
		glTranslatef(xPos, yPos, zPos);
		glBegin(GL_POLYGON);
			
		    /** 顶面 */
		    glVertex3f(0.0f, 0.0f, 0.0f);	 
			glVertex3f(0.0f, 0.0f, -1.0f);
			glVertex3f(-1.0f, 0.0f, -1.0f);
			glVertex3f(-1.0f, 0.0f, 0.0f);
			
            /** 前面 */
			glVertex3f(0.0f, 0.0f, 0.0f);	 
			glVertex3f(-1.0f, 0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 0.0f);
			glVertex3f(0.0f, -1.0f, 0.0f);
			
			/** 右面 */
			glVertex3f(0.0f, 0.0f, 0.0f);	
			glVertex3f(0.0f, -1.0f, 0.0f);
			glVertex3f(0.0f, -1.0f, -1.0f);
			glVertex3f(0.0f, 0.0f, -1.0f);
			
			/** 左面*/
			glVertex3f(-1.0f, 0.0f, 0.0f);	
			glVertex3f(-1.0f, 0.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, 0.0f);
			
			/** 底面 */
			glVertex3f(0.0f, 0.0f, 0.0f);	
			glVertex3f(0.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, 0.0f);
				
			
			/** 后面 */
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(-1.0f, 0.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(0.0f, -1.0f, -1.0f);
		glEnd();
	glPopMatrix();
}

/** 绘制一个手臂 */
void Robot::DrawArm(float xPos, float yPos, float zPos)
{
	glPushMatrix();
	    glColor3f(1.0f, 0.0f, 0.0f);    /**< 红色 */	
		glTranslatef(xPos, yPos, zPos);
		glScalef(1.0f, 4.0f, 1.0f);	    /**< 手臂是1x4x1的立方体 */
		DrawCube(0.0f, 0.0f, 0.0f);
	glPopMatrix();
}

/** 绘制头部 */
void Robot::DrawHead(float xPos, float yPos, float zPos)
{
	glPushMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);	/**< 白色 */
		glTranslatef(xPos, yPos, zPos);
		glScalef(2.0f, 2.0f, 2.0f);		/**<头部是 2x2x2长方体 */
		DrawCube(0.0f, 0.0f, 0.0f);
	glPopMatrix();
}

/** 绘制机器人的躯干 */ 
void Robot::DrawTorso(float xPos, float yPos, float zPos)
{
	glPushMatrix();
		glColor3f(0.0f, 0.0f, 1.0f);	 /**< 蓝色 */
		glTranslatef(xPos, yPos, zPos);
		glScalef(3.0f, 5.0f, 2.0f);	     /**< 躯干是3x5x2的长方体 */
		DrawCube(0.0f, 0.0f, 0.0f);
	glPopMatrix();
}

/** 绘制一条腿 */
void Robot::DrawLeg(float xPos, float yPos, float zPos)
{
	glPushMatrix();
		glColor3f(1.0f, 1.0f, 0.0f);	/**< 黄色 */
		glTranslatef(xPos, yPos, zPos);
		glScalef(1.0f, 5.0f, 1.0f);		/**< 腿是1x5x1长方体 */
		DrawCube(0.0f, 0.0f, 0.0f);
	glPopMatrix();
}

/** 在指定位置绘制机器人 */
void Robot::DrawRobot(float xPos, float yPos, float zPos)
{
	static bool leg1 = true;		/**< 机器人腿的状态，true向前，flase向后 */
	static bool leg2 = false;		
	
	static bool arm1 = true;
	static bool arm2 = false;

	glPushMatrix();	

		glTranslatef(xPos, yPos, zPos);	/**< 定位 */

	    ///绘制各个部分
		DrawHead(1.0f, 2.0f, 0.0f);     /**< 绘制头部 */		
		DrawTorso(1.5f, 0.0f, 0.0f);   /**< 绘制躯干 */
		
		///绘制胳膊
		glPushMatrix();
			///如果胳膊正在向前运动，则递增角度，否则递减角度 
			if (arm1)
				armAngle[0] = armAngle[0] + 0.1f;
			else
				armAngle[0] = armAngle[0] - 0.1f;

			///如果胳膊达到其最大角度则改变其状态
			if (armAngle[0] >= 15.0f)
					arm1 = false;
			if (armAngle[0] <= -15.0f)
					arm1 = true;

			///平移并旋转后绘制胳膊
			glTranslatef(0.0f, -0.5f, 0.0f);
			glRotatef(armAngle[0], 1.0f, 0.0f, 0.0f);
			DrawArm(2.5f, 0.0f, -0.5f);
		glPopMatrix();

		///同上arm0
		glPushMatrix();
			
			if (arm2)
				armAngle[1] = armAngle[1] + 0.1f;
			else
				armAngle[1] = armAngle[1] - 0.1f;

			
			if (armAngle[1] >= 15.0f)
					arm2 = false;
			if (armAngle[1] <= -15.0f)
					arm2 = true;

			
			glTranslatef(0.0f, -0.5f, 0.0f);
			glRotatef(armAngle[1], 1.0f, 0.0f, 0.0f);
			DrawArm(-1.5f, 0.0f, -0.5f);
		glPopMatrix();

		///绘制腿部
		glPushMatrix();					

			///如果腿正在向前运动，则递增角度，否则递减角度 
			if (leg1)
				legAngle[0] = legAngle[0] + 0.1f;
			else
				legAngle[0] = legAngle[0] - 0.1f;

			///如果腿达到其最大角度则改变其状态
			if (legAngle[0] >= 15.0f)
					leg1 = false;
			if (legAngle[0] <= -15.0f)
					leg1 = true;

			///平移并旋转后绘制胳膊
			glTranslatef(0.0f, -0.5f, 0.0f);
			glRotatef(legAngle[0], 1.0f, 0.0f, 0.0f);
			DrawLeg(-0.5f, -5.0f, -0.5f);

		glPopMatrix();

		///同上leg1
		glPushMatrix();

			if (leg2)
				legAngle[1] = legAngle[1] + 0.1f;
			else
				legAngle[1] = legAngle[1] - 0.1f;

			if (legAngle[1] >= 15.0f)
				leg2 = false;
			if (legAngle[1] <= -15.0f)
				leg2 = true;

			glTranslatef(0.0f, -0.5f, 0.0f);
			glRotatef(legAngle[1], 1.0f, 0.0f, 0.0f);
			DrawLeg(1.5f, -5.0f, -0.5f);

		glPopMatrix();
	glPopMatrix();
}

/** 绘制函数 */
void Robot::Draw()											
{
/** 用户自定义的绘制过程 */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();											
	
	///进行模型变换并绘制机器人
	glPushMatrix();
	  glTranslatef(0.0f, 0.0f, -30.0f);	
	  glRotatef(angle, 0.0f, 1.0f, 0.0f);
	  DrawRobot(0.0f, 0.0f, 0.0f);
    glPopMatrix();

	
	glFlush();													/**< 强制执行所有的OpenGL命令 */
}
