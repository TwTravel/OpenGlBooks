//========================================================
/**
*  @file      GLEngine.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  游戏引擎类----游戏世界中的最上层的组织管理类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#include "stdafx.h"
#include "GLEngine.h"
#include "stdio.h"
#include "GameApp.h"


GLEngine::GLEngine(void){}

GLEngine::~GLEngine(void)
{
	_DELETE(m_pTimer);
	_DELETE(m_pInput);
	_DELETE(m_pFont);
}


/*! 初始化，主要是opengl方面的初始化*/
BOOL GLEngine::Init(void)
{
	m_hDC=::GetDC(SYS_HWND);
	if(!m_hDC)return FALSE;

	if(!SetDCPixelFormat()){return FALSE;}

	m_hRC= wglCreateContext(m_hDC);
	if (!m_hRC)return FALSE;

	if (!wglMakeCurrent(m_hDC,m_hRC))return FALSE;

	//--------------------------------------
	m_pFont=new HZFont;

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	SetProjection(45,1);
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
	glClearDepth(1.0);
	glClearStencil(0);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	char str[50] = {"载入数据中，请等待。。。"};
	BeginOrtho(800,600);
    m_pFont->Format(str);
	m_pFont->SetColor(SColor(1.0,0.0,0.0,0.0));
	m_pFont->PrintAt(300,300);
	EndOrtho();

	SwapBuffer();
	//--------------------------------------

	if(!InitObject())return FALSE;
	
	Sleep(500);
	
	return TRUE;
}


/*! 设置opengl象素格式 */
BOOL GLEngine::SetDCPixelFormat(void)
{
	int nPixelFormat;
	PIXELFORMATDESCRIPTOR pfd;

	memset(&pfd,sizeof(PIXELFORMATDESCRIPTOR),0);
	pfd.nSize		=sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	=1;
	pfd.dwFlags		=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.cColorBits	=24;
	pfd.cDepthBits	=16;
	pfd.cStencilBits=1;
	pfd.iLayerType	=PFD_MAIN_PLANE;
	pfd.iPixelType	=PFD_TYPE_RGBA;

	nPixelFormat	=ChoosePixelFormat(m_hDC, &pfd);

	if(!nPixelFormat)return FALSE;
	if(!SetPixelFormat(m_hDC,nPixelFormat,&pfd))return FALSE;

	return TRUE;
}

/*! 游戏中的实例对象的初始化 */
BOOL GLEngine::InitObject(void)
{
	m_pTimer = new CTimer;// 记时器初始化
	m_pTimer->Init();

	
	m_pInput=new CInput;
	m_pInput->Init(SYS_HWND,(HINSTANCE)GetModuleHandle(NULL),true,IS_USEKEYBOARD|IS_USEMOUSE);

	//dsound初始化
	CSound::InitDirectSound(SYS_HWND);
	CMusic::InitMusic();
	m_cMusic.LoadMusic("sound/title.mid");

	m_cGUIMgr.LoadCurIni("ini\\cursor.ini","normal");

	if(!m_cSceneMgr.Init())return FALSE;

	m_cMessage.Clear();
	//SYS_MESSAGE->Insert(SMessage(MSG_PLAYAVI,0,0,"ini\\AVI.ini","CG",NULL));
	SYS_MESSAGE->Insert(SMessage(MSG_OPENAIR,0,0,"ini\\OpenAir.ini","scene01",NULL));

	return TRUE;
}


/*! 渲染前的准备工作 */
void GLEngine::Prepare(void)
{
	m_pTimer->Update();
	m_pInput->Update();
	m_cMessage.Run();

	// 改变渲染模式
	if(m_pInput->GetKeyboard()->KeyDown(DIK_F9)) 
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);// 线框模式
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);// 纹理模式

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}


/*! 3D渲染 */
void GLEngine::Render(void)
{
	///////////////3D场景的渲染//////////////////
	glEnable(GL_DEPTH_TEST);
	if(SceneMgr::GetSysSceneMgr()->GetScene()==SCE_OPENAIR)camera.Update();

	m_cSceneMgr.Render();

	if(SceneMgr::GetSysSceneMgr()->GetScene()==SCE_AVI)
	{
		if(SYS_KEYBOARD->KeyDown(DIK_ESCAPE)||SYS_MOUSE->ButtonDown(1))
		{
			SYS_MESSAGE->Insert(SMessage(MSG_TITLE,0,0,"ini\\title.ini","menu",NULL));
			SYS_MESSAGE->Insert(SMessage(MSG_FADE,0,0,NULL,NULL,NULL));
			SYS_MESSAGE->Insert(SMessage(MSG_PLAY_MUSIC,0,0,"sound/title.mid",NULL,NULL));
		}
	}

	///////////////2D界面的绘制/////////////////
	glDisable(GL_DEPTH_TEST);
	m_cGUIMgr.Render();
}


/*! 关闭引擎 */
void GLEngine::Close(void)
{
	if(m_hRC)
	{
		if(!wglMakeCurrent(NULL,NULL))_ERROR("release  rc failed!");
		if(!wglDeleteContext(m_hRC))_ERROR("release rendering context failed!");
		m_hRC = NULL;
	}

	if(m_hDC)
	{
		if(!ReleaseDC(SYS_HWND,m_hDC))_ERROR("release dc failed!"); 
		m_hDC = NULL;
	}
}


/*! 设置矩阵 */
void GLEngine::SetProjection(int iFOVAngle, int iHeight)
{
	double Aspect;
	int t_iFOV=iFOVAngle;

	if(t_iFOV>90)t_iFOV=45;

	if (iHeight == 0) Aspect = 1.0;
	else Aspect = (double)SYS_WIDTH/(double)SYS_HEIGHT;

	glViewport(0, 0,SYS_WIDTH,SYS_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(t_iFOV,Aspect,0.001f,F3D_DEPTH);
}
