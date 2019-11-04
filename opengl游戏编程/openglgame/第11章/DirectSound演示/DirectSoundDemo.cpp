//=========================================================================
/**
*  @file      SoundDemo.cpp
*
*  项目描述： DirectSound声音演示
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

#include "DirectSoundDemo.h"											/**< 包含头文件 */

/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	CSoundDemo * demo = new CSoundDemo(class_name);
	return reinterpret_cast<GLApplication *>(demo);
}


/** 构造函数 */
CSoundDemo::CSoundDemo(const char * class_name) : GLApplication(class_name)
{ 
}

/** 初始化OpenGL */
bool CSoundDemo::Init()									
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

	/** 创建音频系统和声音对象 */
	m_pSoundManager = new CSoundManager();
	m_pSound1 = new CSound();
	m_pSound2 = new CSound();

	/** 输入系统初始化 */
	m_pInput->Init(this->m_Window.GetHwnd(),(HINSTANCE)GetModuleHandle(NULL),
		           true,IS_USEKEYBOARD);


    /** 音频系统初始化 */
	m_pSoundManager->Init(this->m_Window.GetHwnd());
	
	/** 设置主缓冲区格式 */
	m_pSoundManager->SetPrimaryBuffer();

	/** 载入声音并播放 */
	m_pSound1->LoadWaveFile("airplane.wav");
	m_pSound1->Play(true);

	m_pSound2->LoadWaveFile("explorer.wav");
	m_pSound2->Play(true);

	/** 初始化字体 */
	if(!m_hFont.InitFont())
	{
		MessageBox(NULL,"初始化字体失败!","错误",MB_OK);
		return false;
	}
	
	return true;	/**< 成功返回 */										
}

/** 用户自定义的卸载函数 */
void CSoundDemo::Uninit()									
{
	/** 用户自定义的卸载过程 */
	if(m_pInput)
	{
		delete m_pInput;
		m_pInput = NULL;
	}
	if(m_pSound1)
	   delete m_pSound1;
		if(m_pSound2)
	   delete m_pSound2;

	if(m_pSoundManager)
		delete m_pSoundManager;

}

/** 程序更新函数 */
void CSoundDemo::Update(DWORD milliseconds)						
{
	/** 输入系统更新 */
	m_pInput->Update();

	/** 按ESC键时退出 */
	if(m_pInput->GetKeyboard()->KeyDown(DIK_ESCAPE))
	{
		TerminateApplication();
	} 

	/** 按下‘A’键停止飞机声音播放 */
	if(m_pInput->GetKeyboard()->KeyDown(DIK_A))
	{
		m_pSound1->Stop();
	} 

	/** 按下‘S’键停止爆炸声音播放 */
	if(m_pInput->GetKeyboard()->KeyDown(DIK_S))
	{
		m_pSound2->Stop();
	} 
	     
}

/** 绘制函数 */
void CSoundDemo::Draw()											
{
/** 用户自定义的绘制过程 */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();	
	glTranslatef(0.0f,0.0f,-10.0f);

	/** 输出提示信息 */
	m_hFont.PrintText("按下‘A’键关闭飞机声音",-2.5,1.0);
	m_hFont.PrintText("按下‘S’键关闭爆炸声音",-2.5,0.0);
	m_hFont.PrintText("按下‘ESC’键退出",-2.5,-1.0);
	
	glFlush();

}
