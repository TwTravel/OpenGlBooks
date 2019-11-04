//=========================================================================
/**
*  @file      ShadowVolume.cpp
*
*  项目描述： 阴影体
*  文件描述:  具体实例类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-09-13	
*  修改日期： 2006-12-20
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
#include "ShadowVolume.h"						    

/** 光源位置 */
CVector4 lightPos(2.0f, 5.0f, 0.0f, 1.0f);

#define MAX_SHADOW_CASTER_POINTS 4  /**< 定义遮挡体顶点个数 */
bool displayVolume = false;         /**< 绘制阴影体标志 */  
float ExtendAmount = 5.0f;          /**< 延伸因子 */

/** 遮挡体顶点数组 */
CVector4 ShadowObject[MAX_SHADOW_CASTER_POINTS];


/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	CShadowVolume * demo = new CShadowVolume(class_name);
	return reinterpret_cast<GLApplication *>(demo);
}


/** 构造函数 */
CShadowVolume::CShadowVolume(const char * class_name) : GLApplication(class_name)
{
  /// 初始化用户自定义的程序变量
	m_Fps = 0;

}


/** 初始化OpenGL */
bool CShadowVolume::Init()									
{
/** 用户自定义的初始化过程 */
	glClearColor(0.2f, 0.2f, 0.2f, 0.5f);						
	glClearDepth(1.0f);											
	glEnable(GL_DEPTH_TEST);
	ResizeDraw(true);	                             /**< 改变OpenGL窗口大小，直接调用子类的函数 */

   /** 设置光源属性 */
   float ambientLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
   float diffuseLight[] = {1.0, 1.0, 1.0, 1.0}; 
   float specularLight[] = {1.0, 1.0, 1.0, 1.0};

   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
   glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
   glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
   
   /** 启用光照和光源 */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	
	/** 启用纹理 */
	glEnable(GL_TEXTURE_2D);
	
	if(!ground.Load("ground.tga"))
	{
		MessageBox(NULL,"载入纹理失败!","错误",MB_OK);
		exit(-1);
	}

	/** 初始化字体 */
	if(!m_Font.InitFont())
		MessageBox(NULL,"初始化字体失败!","错误",MB_OK);

	
   /** 初始化遮挡体位置 */
   ShadowObject[0] = CVector4(-1.0f, 2.5f, -1.0f);
   ShadowObject[1] = CVector4(-1.0f, 2.5f,  1.0f);
   ShadowObject[2] = CVector4(1.0f, 2.5f,  1.0f);
   ShadowObject[3] = CVector4(1.0f, 2.5f, -1.0f);

	return true;                                            /**< 成功返回 */
}

/** 用户自定义的卸载函数 */
void CShadowVolume::Uninit()									
{
	
}

/** 程序更新函数 */
void CShadowVolume::Update(DWORD milliseconds)						
{
	
	/** 按ESC键时退出 */
	if(m_Keys.IsPressed(VK_ESCAPE))
	{
		TerminateApplication();
	} 	
	

	/** 控制光源位置移动 */
	if(m_Keys.IsPressed(VK_UP))
		lightPos.z -= 0.01f;
	if(m_Keys.IsPressed(VK_DOWN))
		lightPos.z += 0.01f;

	if(m_Keys.IsPressed(VK_LEFT))
		lightPos.x -= 0.01f;

    if(m_Keys.IsPressed(VK_RIGHT))
		lightPos.x += 0.01f;
   
	/** 空格键切换是否绘制阴影体 */
	if(m_Keys.IsPressed(VK_SPACE))
	   displayVolume = !displayVolume;
     
	    
}

/** 计算帧速 */
void CShadowVolume::CaculateFrameRate()
{
	static float framesPerSecond    = 0.0f;	     /**< 保存显示帧数 */	
    static float lastTime			= 0.0f;	     /**< 记录上次时间 */						
    float currentTime = GetTickCount() * 0.001f; /**< 获得当前时间 */	 			

	++framesPerSecond;                           /**< 显示帧数递增1 */
    /** 如果时间差大于1.0秒 */
	if( currentTime - lastTime > 1.0f )          
    {
		
	    lastTime = currentTime;                   /**< 保存当前时间 */
		m_Fps = framesPerSecond;                  /**< 当前帧数传给m_Fps */
        framesPerSecond = 0;                      /**< 将帧数置零 */                    
    }
}

/** 输出文字信息 */
void  CShadowVolume::PrintText()
{
	char string[128];                               /**< 用于保存输出字符串 */
	glPushAttrib(GL_CURRENT_BIT);                   /**< 保存现有颜色属性信息 */
	glColor3f(0.0f,1.0f,0.0f);                      /**< 设置文字颜色 */
	
	/** 输出帧速 */
	CaculateFrameRate();                               /**< 计算帧速 */
    sprintf(string,"FPS:%3.0f",m_Fps);                 /**< 字符串赋值 */
	m_Font.PrintText(string, -5.0f,3.0f);              /**< 输出字符串 */
	glPopAttrib();
		
}

/** 渲染场景中的物体 */
void CShadowVolume::render()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
  	glTranslatef(0.0f, -2.0f, -10.0f);
	
	/** 绘制地面 */
	glDisable(GL_LIGHTING);
	glPushMatrix();

	   /** 指定纹理 */
	   glBindTexture(GL_TEXTURE_2D, ground.ID);

       ///绘制四边形
      glBegin(GL_QUADS);
		 glNormal3f(0.0f, 1.0f, 0.0f); /**< 指定法线 */
         glTexCoord2f(0.0, 0.0); glVertex3f(15.0f, -0.01f, 15.0f);
         glTexCoord2f(1.0, 0.0); glVertex3f(-15.0f, -0.01f, 15.0f);
         glTexCoord2f(1.0, 1.0); glVertex3f(-15.0f, -0.01f, -15.0f);
         glTexCoord2f(0.0, 1.0); glVertex3f(15.0f, -0.01f, -15.0f);
      glEnd();
	glPopMatrix();
	glEnable(GL_LIGHTING);
	
	/** 绘制圆环 */
	glPushMatrix();
        glDisable(GL_TEXTURE_2D);
		glTranslatef(-2.0f, 0.8f, -0.2f);
		auxSolidTorus(0.2f, 0.4f);
	glPopMatrix();

   /** 绘制立方体 */
	glPushMatrix();
		glTranslatef(0.0f, 0.8f, -0.2f );
		auxSolidCube(1.0f);
	glPopMatrix();

   /** 绘制球体 */
	glPushMatrix();
		glTranslatef(2.0f, 0.8f, -0.2f);
		auxSolidSphere(0.7f);
    glPopMatrix();

   /** 绘制遮挡体 */
   glPushMatrix();
     glBegin(GL_QUADS);
  			glNormal3f(0, 1, 0);
        	glVertex3f(ShadowObject[0].x, ShadowObject[0].y, ShadowObject[0].z);
        	glVertex3f(ShadowObject[1].x, ShadowObject[1].y, ShadowObject[1].z);
			glVertex3f(ShadowObject[2].x, ShadowObject[2].y, ShadowObject[2].z);
			glVertex3f(ShadowObject[3].x, ShadowObject[3].y, ShadowObject[3].z);
	glEnd();
  glPopMatrix();


   /** 绘制光源 */
	glDisable(GL_LIGHTING);
	glPushMatrix();

	    /** 设置光源位置 */
		float lp[4];		
		
		lp[0] = lightPos.x; 
		lp[1] = lightPos.y;
		lp[2] = lightPos.z; 
		lp[3] = lightPos.w;
		glLightfv(GL_LIGHT0, GL_POSITION, lp);
	
		/** 开始绘制 */
		glTranslatef(lightPos.x,lightPos.y, lightPos.z);
		glColor3f(1.0f, 1.0f, 0.5f);
		auxSolidSphere(0.1);
		glColor3f(1.0f,1.0f,1.0f);  
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	
}

/** 绘制阴影体 */
void CShadowVolume::DrawSilhouette(CVector4 lightPos, float Extend)
{
   CVector4 Extended;     
   int A, B;

   /** 关闭光照 */
   glDisable(GL_LIGHTING);

   /** 设置颜色 */
   glColor3f(0.5f, 0.7f, 0.5f);

   /** 对每个顶点都延伸 */
   for(int i = 0; i < MAX_SHADOW_CASTER_POINTS; i++)
      {
          
          A = i; B = i + 1;

          /** 边界检查 */
          if(B >= MAX_SHADOW_CASTER_POINTS) B = 0;

         /** 绘制四边形 */
         glBegin(GL_QUADS);

            /** 原始顶点A */
            glVertex3f(ShadowObject[A].x, ShadowObject[A].y, ShadowObject[A].z);

            /** 顶点A延伸后的点 */
            Extended.ExtendVertexPos(ShadowObject[A], lightPos, Extend);
            glVertex3f(Extended.x, Extended.y, Extended.z);

            /** 顶点B延伸后的点 */
            Extended.ExtendVertexPos(ShadowObject[B], lightPos, Extend);
            glVertex3f(Extended.x, Extended.y, Extended.z);

            /** 原始顶点B */
            glVertex3f(ShadowObject[B].x, ShadowObject[B].y, ShadowObject[B].z);

         glEnd();
      }

   glEnable(GL_LIGHTING);
}

/** 绘制函数 */
void CShadowVolume::Draw()											
{
/** 用户自定义的绘制过程 */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);			
	glLoadIdentity();
    
	/** 关闭颜色缓存写入 */
	 glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	 
	 /** 渲染场景 */
	 render();
	

	/** 指定模版缓存操作 */
    glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);   
    glDepthMask(GL_FALSE);
    glStencilFunc(GL_ALWAYS, 0, 0);

	/** 绘制阴影体,模版值加1 */   
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glCullFace(GL_BACK);
	DrawSilhouette(lightPos, ExtendAmount);

	/** 再次绘制阴影体,模版值减1 */
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glCullFace(GL_FRONT);
	DrawSilhouette(lightPos, ExtendAmount);

	///恢复以前的属性
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	/** 绘制阴影部分 */
    glDisable(GL_LIGHT0);
	glStencilFunc(GL_EQUAL, 1, 1);
	render();

	/** 绘制阴影体外的部分 */
    glEnable(GL_LIGHT0);
	glStencilFunc(GL_EQUAL, 0, 1);
	render();

	/** 关闭模版测试 */
	glDepthFunc(GL_LESS);
	glDisable(GL_STENCIL_TEST);

    /** 绘制阴影体 */
	if(displayVolume == true)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);   /**< 指定线框模式 */
		DrawSilhouette(lightPos, ExtendAmount);      /**< 绘制阴影体 */
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);   /**< 恢复填充模式 */
	}

	/** 输出屏幕信息 */ 
	PrintText();
		
	glFlush();	                 /**< 强制执行所有的OpenGL命令 */
}
