//=========================================================================
/**
*  @file      KeyboardDemo.cpp
*
*  项目描述： DirectInput键盘演示
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

#include "KeyboardDemo.h"											/**< 包含头文件 */

/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	CKeyboardDemo * demo = new CKeyboardDemo(class_name);
	return reinterpret_cast<GLApplication *>(demo);
}


/** 构造函数 */
CKeyboardDemo::CKeyboardDemo(const char * class_name) : GLApplication(class_name)
{ 
}

/** 初始化OpenGL */
bool CKeyboardDemo::Init()									
{
/** 用户自定义的初始化过程 */
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);						
	glClearDepth(1.0f);											
	glDepthFunc(GL_LEQUAL);										
	glEnable(GL_DEPTH_TEST);									
	glShadeModel(GL_SMOOTH);									
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			
	ResizeDraw(true);											

	/** 创建输入系统 */
	m_pInput = new CInputSystem();

	/** 输入系统初始化 */
	m_pInput->Init(this->m_Window.GetHwnd(),(HINSTANCE)GetModuleHandle(NULL),
		           true,IS_USEKEYBOARD);

	/** 初始化字体 */
	if(!m_hFont.InitFont())
		MessageBox(NULL,"初始化字体失败！","错误",MB_OK);

	return true;	/**< 成功返回 */										
}

/** 用户自定义的卸载函数 */
void CKeyboardDemo::Uninit()									
{
	/** 用户自定义的卸载过程 */
	if(m_pInput)
	{
		delete m_pInput;
		m_pInput = NULL;
	}
}

/** 程序更新函数 */
void CKeyboardDemo::Update(DWORD milliseconds)						
{
	/** 输入系统更新 */
	m_pInput->Update();

	/** 按ESC键时退出 */
	if(m_pInput->GetKeyboard()->KeyDown(DIK_ESCAPE))
	{
		TerminateApplication();
	}   	
	     
}

/** 绘制函数 */
void CKeyboardDemo::Draw()											
{
/** 用户自定义的绘制过程 */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();	
	glTranslatef(0.0f,0.0f,-10.0f);
	

	/** 检测键盘输入数据 */
	char string[50] = {"您没有按下任何键"};
   
	/** 检测A键按下？ */
	if(m_pInput->GetKeyboard()->KeyDown(DIK_A))
		sprintf(string,"您按下了 %s 键","A");
	
	/** 检测F1键按下？ */   
	if(m_pInput->GetKeyboard()->KeyDown(DIK_F1))
       sprintf(string,"您按下了 %s 键","F1");
	
	/** 检测Ctrl和A键同时按下？*/
	if(m_pInput->GetKeyboard()->KeyDown(DIK_A) && 
	      m_pInput->GetKeyboard()->KeyDown(DIK_LCONTROL))
       sprintf(string,"您按下了 %s 键","CTRL + A");
	
	/** 输出提示信息 */
	glColor3f(1.0f,0.0f,1.0f);
	m_hFont.PrintText(string,-2.0,0.0);

	glFlush();

}
