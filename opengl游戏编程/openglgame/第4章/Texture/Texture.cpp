//=========================================================================
/**
*  @file      Texture.cpp
*
*  项目描述： 纹理映射
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

#include "Texture.h"						    


/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	Texture * test = new Texture(class_name);
	return reinterpret_cast<GLApplication *>(test);
}


/** 构造函数 */
Texture::Texture(const char * class_name) : GLApplication(class_name)
{
  /// 初始化用户自定义的程序变量
	rot = 0.0;	
   
}

/** 载入纹理数据 */
bool Texture::LoadTexture()
{
	/** 载入位图文件 */
	if(!texture1.Load("image.bmp"))                          /**< 载入位图文件 */
	{
		MessageBox(NULL,"装载位图文件失败！","错误",MB_OK);  /**< 如果载入失败则弹出对话框 */
		return false;
	}

	/** 载入TGA文件 */
	if(!texture2.Load("sphere.tga"))                         /**< 载入TGA文件 */
	{
		MessageBox(NULL,"装载TGA文件失败！","错误",MB_OK);  /**< 如果载入失败则弹出对话框 */
		return false;
	}
	
	/** 启用纹理映射 */
	glEnable(GL_TEXTURE_2D);

  	return true;
}

/** 设置光源 */
void Texture::SetLight()
{
    /** 定义光源的属性值 */
	GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f }; 	/**< 环境光参数 */
	GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };		/**< 漫射光参数 */
	GLfloat LightSpecular[]= { 1.0f, 1.0f, 1.0f, 1.0f };	/**< 镜面光参数 */
	GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };	/**< 光源位置 */

	/** 设置光源的属性值 */
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		/**< 设置环境光 */
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		/**< 设置漫射光 */
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);	/**< 设置漫射光 */
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);	/**< 设置光源位置 */
	
	/** 启用光源 */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1); 
}

/** 初始化OpenGL */
bool Texture::Init()									
{
/** 用户自定义的初始化过程 */
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);						
	glClearDepth(1.0f);											
	glDepthFunc(GL_LEQUAL);										
	glEnable(GL_DEPTH_TEST);									
	glShadeModel(GL_SMOOTH);									
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			
	ResizeDraw(true);	                             /**< 改变OpenGL窗口大小，直接调用子类的函数 */
	
		
	/** 载入纹理 */
	if(!LoadTexture())
		MessageBox(NULL,"载入纹理失败!","错误",MB_OK);
	
	/** 设置光源 */
	SetLight();

	return true;                                        /**< 成功返回 */
}

/** 用户自定义的卸载函数 */
void Texture::Uninit()									
{
/** 用户自定义的卸载过程 */
	texture1.FreeImage();              /** 释放纹理图像占用的内存 */
	glDeleteTextures(1, &texture1.ID); /**< 删除纹理对象 */

	texture2.FreeImage();              /** 释放纹理图像占用的内存 */
	glDeleteTextures(1, &texture2.ID); /**< 删除纹理对象 */

 }

/** 程序更新函数 */
void Texture::Update(DWORD milliseconds)						
{
	if (m_Keys.IsPressed(VK_ESCAPE) == true)					/**< 按ESC退出 */
	{
		TerminateApplication();									
	}


	if (m_Keys.IsPressed(VK_F1) == true)					/**< 按F1在窗口和全屏间切换 */
	{
		ToggleFullscreen();									
	}
	
	rot += milliseconds/20.0;

		    
}


/** 绘制球体 */
void Texture::DrawSphere()
{
	glPushMatrix();
	glTranslatef(2.0f,0.0f,-10.0f);
	glRotatef(rot,0.0f,1.0f,1.0f);
	
	/** 指定纹理 */
	glBindTexture(GL_TEXTURE_2D,texture2.ID);
	
      GLUquadricObj * sphere = gluNewQuadric();
      gluQuadricOrientation(sphere, GLU_OUTSIDE);
	  gluQuadricNormals(sphere,GLU_SMOOTH);
	  gluQuadricTexture(sphere,GL_TRUE);
	  gluSphere(sphere,1.5,50,50);
	  gluDeleteQuadric(sphere);
	glPopMatrix();
	
}

/** 绘制木箱 */
void Texture::DrawBox()
{
	/** 设置材质属性 */
	GLfloat mat_ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

	glPushMatrix();
	glTranslatef(-2.0f,0.0f,-10.0f);
	glRotatef(rot,1.0f,1.0f,0.0f);
	
	/** 选择纹理 */
	glBindTexture(GL_TEXTURE_2D, texture1.ID);
	
	/** 开始绘制四边形 */
	glBegin(GL_QUADS);												
		
	    /// 前侧面
		glNormal3f( 0.0f, 0.0f, 1.0f);								/**< 指定法线指向观察者 */
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	
		
		/// 后侧面
		glNormal3f( 0.0f, 0.0f,-1.0f);								/**< 指定法线背向观察者 */
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	
		
		/// 顶面
		glNormal3f( 0.0f, 1.0f, 0.0f);								/**< 指定法线向上 */
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	
		
		/// 底面
		glNormal3f( 0.0f,-1.0f, 0.0f);								/**< 指定法线朝下 */
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	
		
		/// 右侧面
		glNormal3f( 1.0f, 0.0f, 0.0f);								/**< 指定法线朝右 */
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	
		
		/// 左侧面
		glNormal3f(-1.0f, 0.0f, 0.0f);								/**< 指定法线朝左 */
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	
	glEnd();

	glPopMatrix();
}

/** 绘制函数 */
void Texture::Draw()											
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();	
	
	/** 绘制过程 */
    DrawSphere();
	DrawBox();
	
	/** 强制执行所有的OpenGL命令 */	
	glFlush();							    
}
