//=========================================================================
/**
*  @file      BitmapFont.cpp
*
*  项目描述： OpenGL字体
*  文件描述:  具体实例类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-12-02	
*  修改日期： 2007-05-26
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

#include "BitmapFont.h"						    


/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	BitmapFont * test = new BitmapFont(class_name);
	return reinterpret_cast<GLApplication *>(test);
}


/** 构造函数 */
BitmapFont::BitmapFont(const char * class_name) : GLApplication(class_name)
{
  
	/// 初始化用户自定义的程序变量
	cnt1 = 0.0;
	cnt2 = 0.0;	
   
}


/** 初始化OpenGL */
bool BitmapFont::Init()									
{
/** 用户自定义的初始化过程 */
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);						
	glClearDepth(1.0f);											
	glDepthFunc(GL_LEQUAL);										
	glEnable(GL_DEPTH_TEST);									
	glShadeModel(GL_SMOOTH);									
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			
	ResizeDraw(true);	                             /**< 改变OpenGL窗口大小，直接调用子类的函数 */
			
	/** 初始化字体 */
	if(!font.InitFont(wglGetCurrentDC(),"Arial",60)) 
	   return false;

	return true;                                        /**< 成功返回 */
}

/** 用户自定义的卸载函数 */
void BitmapFont::Uninit()									
{
}

/** 程序更新函数 */
void BitmapFont::Update(DWORD milliseconds)						
{
	if (m_Keys.IsPressed(VK_ESCAPE) == true)					/**< 按ESC退出 */
	{
		TerminateApplication();									
	}


    cnt1 += milliseconds/1500.0;  /**< 更新计数器值 */
	cnt2 += milliseconds/1000.0;  /**< 更新计数器值 */
		    
}


/** 绘制函数 */
void BitmapFont::Draw()											
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	 /**< 清除缓存 */		
	glLoadIdentity();	
	
    /** 绘制字符过程 */
	glTranslatef(0.0f, 0.0f, -10.0f);                              /**< 移入屏幕 */
	glColor3f(cos(cnt1),sin(cnt2),1.0f - 0.5f * cos(cnt1 + cnt2)); /**< 指定颜色 */
    
	/** 调用方法输出字符串 */
	font.PrintText("Hello,OpenGL!",-3.5f + 2.0 * cos(cnt1), 3.5f * sin(cnt2)); 
	
	
	/** 强制执行所有的OpenGL命令 */	
	glFlush();							    
}
