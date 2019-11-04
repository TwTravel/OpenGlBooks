//=========================================================================
/**
*  @file      ProtechnyDemo.cpp
*
*  项目描述： 粒子系统
*  文件描述:  具体实例类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-09-13	
*  修改日期： 2006-12-02
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
#include "stdafx.h"
#include "ProtechnyDemo.h"						    


/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	ProtechnyDemo * demo = new ProtechnyDemo(class_name);
	return reinterpret_cast<GLApplication *>(demo);
}


/** 构造函数 */
ProtechnyDemo::ProtechnyDemo(const char * class_name) : GLApplication(class_name)
{
  /// 初始化用户自定义的程序变量
	m_Fps = 0;
	m_RotAngle = 0.0;
		    
}


/** 初始化OpenGL */
bool ProtechnyDemo::Init()									
{
/** 用户自定义的初始化过程 */
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);						
	glClearDepth(1.0f);											
	glDepthFunc(GL_LEQUAL);										
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);									
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			
	ResizeDraw(true);	                             


	glEnable(GL_TEXTURE_2D);             /**< 开启纹理映射 */   
	
	/** 设置混合因子获得半透明效果 */
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);    
	glEnable(GL_BLEND);				     /**< 启用混和 */
	
	/** 初始化字体 */
	if(!m_Font.InitFont())
		MessageBox(NULL,"初始化字体失败!","错误",MB_OK);

	/** 载入地面纹理 */
	if(!m_TextureGround.Load("Data/wall.bmp"))
		MessageBox(NULL,"地面纹理载入失败!","错误",MB_OK);
	
	
	/** 初始化喷泉实例 */	
	if(!m_Protechny.Init(5000))
	{
		MessageBox(NULL,"雪花系统初始化失败!","错误",MB_OK);
		exit(-1);
	}
	
	return true;                                      
}

/** 用户自定义的卸载函数 */
void ProtechnyDemo::Uninit()	
{
}

/** 程序更新函数 */
void ProtechnyDemo::Update(DWORD milliseconds)						
{
	if (m_Keys.IsPressed(VK_ESCAPE) == true)					/**< 按ESC退出 */
	{
		TerminateApplication();									
	}
	
}

/** 计算帧速 */
void ProtechnyDemo::CaculateFrameRate()
{
	static float framesPerSecond    = 0.0f;	     /**< 保存显示帧数 */	
    static float lastTime			= 0.0f;	     /**< 记录上次时间 */						
    float currentTime = GetTickCount() * 0.001f; /**< 获得当前时间 */	 			

	framesPerSecond++;                           /**< 显示帧数递增1 */
    /** 如果时间差大于1.0秒 */
	if( currentTime - lastTime > 1.0f )          
    {
		
	    lastTime = currentTime;                   /**< 保存当前时间 */
		m_Fps = framesPerSecond;                  /**< 当前帧数传给m_Fps */
        framesPerSecond = 0;                      /**< 将帧数置零 */                    
    }

}

/** 输出文字信息 */
void  ProtechnyDemo::PrintText()
{
	char string[128];                               /**< 用于保存输出字符串 */
	glPushAttrib(GL_CURRENT_BIT);                   /**< 保存现有颜色属性信息 */
	glColor3f(0.0f,1.0f,1.0f);                      /**< 设置文字颜色 */

	/** 输出帧速 */
    CaculateFrameRate();                               /**< 计算帧速 */
    sprintf(string,"FPS:%d",(int)m_Fps);               /**< 字符串赋值 */
	m_Font.PrintText(string, -5.0f,3.5f);              /**< 输出字符串 */
	glPopAttrib();
		
}

/** 绘制地面 */
void ProtechnyDemo::DrawGround()
{
	glPushMatrix();
	glRotatef(m_RotAngle,0.0f,1.0f,0.0f);   

	/** 指定纹理 */
	glBindTexture(GL_TEXTURE_2D,m_TextureGround.ID);
	glColor4ub(255,255,255,255);
	glNormal3f(0.0f,1.0f,0.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);glVertex3f(-2.5f,-1.0f,2.5f);
		glTexCoord2f(1.0f,0.0f);glVertex3f(-2.5f,-1.0f,-2.5f);
		glTexCoord2f(1.0f,1.0f);glVertex3f(2.5f,-1.0f,-2.5f);
		glTexCoord2f(0.0f,1.0f);glVertex3f(2.5f,-1.0f,2.5f);
	glEnd();
	
	glPopMatrix();
	
	/** 地面转动的角度 */
	m_RotAngle += 0.005f;
}



/** 绘制函数 */
void ProtechnyDemo::Draw()											
{
/** 用户自定义的绘制过程 */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-6.0f);

	/** 绘制地面 */
	DrawGround();
    
	/** 渲染喷泉 */
	m_Protechny.Render();

	/** 输出屏幕信息 */ 
	PrintText();
		
	glFlush();	                 /**< 强制执行所有的OpenGL命令 */
}
