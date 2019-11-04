//=========================================================================
/**
*  @file      MultiTexture.cpp
*
*  项目描述： 纹理映射
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
#include "MultiTexture.h"						    


/** 多重纹理函数 */
PFNGLMULTITEXCOORD2FARBPROC     glMultiTexCoord2fARB = NULL;
PFNGLACTIVETEXTUREARBPROC       glActiveTextureARB = NULL;


/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	MultiTexture * test = new MultiTexture(class_name);
	return reinterpret_cast<GLApplication *>(test);
}


/** 构造函数 */
MultiTexture::MultiTexture(const char * class_name) : GLApplication(class_name)
{
  /// 初始化用户自定义的程序变量
	multitexturing = true;
	sp = false;
}

/** 检查是否支持扩展 */
bool MultiTexture::isExtensionSupported(const char* string)
{
	char *extension;			/**< 指向扩展字符串的指针 */
	char *end;				    /**< 最后一个字符指针 */
	int idx;

	extension = (char*)glGetString(GL_EXTENSIONS);
	if (extension == NULL)
		return false;	
	
	/** 得到最后一个字符 */
	end = extension + strlen(extension);	

	/** 循环查找字符串string */
	while (extension < end)
	{
		/** 找到空格前的一个字符串 */
		idx = strcspn(extension, " ");
		
		/** 检查是否找到字符串string */
		if ( (strlen(string) == idx) && (strncmp(string, extension, idx) == 0))
		{
			return true;
		}
		
		/** 当前串不是string,继续下一个字符串 */
		extension += (idx + 1);
	}
	/** 没有找到,则返回false */
	return false;   
}

/** 初始化 */
bool MultiTexture::initMultiTexture()
{
	/** 检查是否支持扩展 */
	if (isExtensionSupported("GL_ARB_multitexture"))
	{
				
		/** 获得函数指针地址 */
		glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
		glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
		
		return true;
	}
	else 
		return false;
}

/** 载入纹理数据  */
bool MultiTexture::loadTexture()
{
	/// 文件名 
	char* fileName[4] ={ "wall.bmp","lightmap.bmp","bitmap.bmp","fog.bmp"};
		
	/// 载入四幅位图 
	for(int i=0; i<4; i++)
	{
		if(!m_texture[i].Load(fileName[i]))
		{
			MessageBox(NULL,"载入位图文件失败!","错误",MB_OK);
			exit(-1);
		}		 
		
	}
	return true;

}

/** 初始化OpenGL */
bool MultiTexture::Init()									
{
/** 用户自定义的初始化过程 */
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);						
	glClearDepth(1.0f);											
	glDepthFunc(GL_LEQUAL);										
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);									
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			
	ResizeDraw(true);	                             /**< 改变OpenGL窗口大小，直接调用子类的函数 */
	

	/** 初始化 */
	if(!initMultiTexture())
	{
		MessageBox(NULL,"不支持多重纹理!","错误",MB_OK);
		exit(-1);
	}

	/** 载入纹理 */
	if(!loadTexture())
	{
		MessageBox(NULL,"载入纹理错误!","错误",MB_OK);
		exit(-1);
	}
			
		return true;                                        /**< 成功返回 */
}

/** 用户自定义的卸载函数 */
void MultiTexture::Uninit()									
{
	for(int i=0; i<4; i++)
	{
		m_texture[i].FreeImage();
		glDeleteTextures(1,&m_texture[i].ID);
	}
}

/** 程序更新函数 */
void MultiTexture::Update(DWORD milliseconds)						
{
	if (m_Keys.IsPressed(VK_ESCAPE) == true)					/**< 按ESC退出 */
	{
		TerminateApplication();									
	}

/** 用户自定义的更新过程 */

	/** 当按下空格键时,开启或关闭多重纹理 */
	if(m_Keys.IsPressed(VK_SPACE) && !sp)
	{
		sp = true;
		multitexturing = !multitexturing;
	}
	if(!m_Keys.IsPressed(VK_SPACE))
		sp = false;
	
	    
}


/** 绘制函数 */
void MultiTexture::Draw()											
{
/** 用户自定义的绘制过程 */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();
	
	glTranslatef(0.0f,0.0f,-10.0f);

	/** 激活纹理0,并绑定纹理 */
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D); 
	glBindTexture(GL_TEXTURE_2D,  m_texture[0].ID);

	/** 激活纹理1,并绑定纹理 */
	glActiveTextureARB(GL_TEXTURE1_ARB);

	/** 如果多重纹理启用,则启用该纹理 */
	if (multitexturing) 
		glEnable(GL_TEXTURE_2D); 
	else 
		glDisable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,  m_texture[1].ID);

	/** 绘制一个四方形墙面 */
	glPushMatrix();
		glTranslatef(-2.5, 0, 0);
		glScalef(2.0f,2.0f,2.0f);
		glBegin(GL_QUADS);

			/** 左上点 */
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 1.0f);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 1.0f);
			glVertex3f(-1, 1, 0);

			/** 左下点 */
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f);
			glVertex3f(-1, -1, 0);

			/** 右下点 */
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 0.0f);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 0.0f);
			glVertex3f(1, -1, 0);

			/** 右上点 */
			glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 1.0f);
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f, 1.0f);
			glVertex3f(1, 1, 0);

		glEnd();    /**< 绘制结束 */											
	glPopMatrix();

	
	/** 激活纹理0,并绑定纹理 */
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,  m_texture[2].ID);

	/** 激活纹理1,并绑定纹理 */
	glActiveTextureARB(GL_TEXTURE1_ARB);
	
	/** 如果多重纹理启用,则启用该纹理 */
	if (multitexturing) 
		glEnable(GL_TEXTURE_2D); 
	else 
		glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,  m_texture[3].ID);

	static float wrap = 0;      /**< 用于雾的流动 */     

	glTranslatef(2.5, 0, 0);
	glScalef(2.0f,2.0f,2.0f);
	glBegin(GL_QUADS);

		/** 左上点 */
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 1.0f);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f - wrap, 1.0f);
		glVertex3f(-1, 1, 0);

		/** 左下点 */
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f - wrap, 0.0f);
		glVertex3f(-1, -1, 0);

		/** 右下点 */
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 0.0f);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f - wrap, 0.0f);
		glVertex3f(1, -1, 0);

		/** 右上点 */
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 1.0f, 1.0f);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 1.0f - wrap, 1.0f);
		glVertex3f(1, 1, 0);
	glEnd();											

	wrap += 0.001f;                   /**< 递增 */

			
	glFlush();	                     /**< 强制执行所有的OpenGL命令 */
}
