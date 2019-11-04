//=========================================================================
/**
*  @file      CameraRove.cpp
*
*  项目描述： 摄像机漫游
*  文件描述:  具体实例类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     BrightXu
*  电子邮件:  huoxini@hotmail.com
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

#include "CameraRove.h"						    


/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	CameraRove * test = new CameraRove(class_name);
	return reinterpret_cast<GLApplication *>(test);
}


/** 构造函数 */
CameraRove::CameraRove(const char * class_name) : GLApplication(class_name)
{
  /// 初始化用户自定义的程序变量
	m_Fps = 0;
   
}

bool CameraRove::LoadTexture()
{
	
	if(!m_Texture.LoadBitmap("image.bmp"))                   /**< 载入位图文件 */
	{
		MessageBox(NULL,"装载位图文件失败！","错误",MB_OK);  /**< 如果载入失败则弹出对话框 */
		return false;
	}
	glGenTextures(1, &m_Texture.ID);                        /**< 生成一个纹理对象名称 */
		
	glBindTexture(GL_TEXTURE_2D, m_Texture.ID);             /**< 创建纹理对象 */
	
	/** 控制滤波 */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	
	/** 创建纹理 */
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, m_Texture.imageWidth,
					m_Texture.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
					m_Texture.image);

	/** 启用纹理映射 */
	glEnable(GL_TEXTURE_2D);

  	return true;
}

/** 设置光源 */
void CameraRove::SetLight()
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
bool CameraRove::Init()									
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
	if(!m_Font.InitFont())
		MessageBox(NULL,"初始化字体失败!","错误",MB_OK);
	
	/** 载入纹理 */
	if(!LoadTexture())
		MessageBox(NULL,"载入纹理失败!","错误",MB_OK);
	
	/** 设置摄像机 */
	m_Camera.setCamera(0.0f,1.5f, 6.0f, 0.0f, 1.5f, 0.0f,0.0f, 1.0f, 0.0f);

	/** 设置光源 */
	SetLight();

	return true;                                        /**< 成功返回 */
}

/** 用户自定义的卸载函数 */
void CameraRove::Uninit()									
{
/** 用户自定义的卸载过程 */
	m_Texture.FreeImage();              /** 释放纹理图像占用的内存 */
	glDeleteTextures(1, &m_Texture.ID); /**< 删除纹理对象 */

 }
/** 更新摄像机 */
void CameraRove::UpdateCamera()
{
	m_Camera.setViewByMouse();
	
	/** 键盘按键响应 */
	if(m_Keys.IsPressed(VK_SHIFT))                      /**< 按下SHIFT键时加速 */
	{
		m_Camera.setSpeed(0.6f);
	}
	if(!m_Keys.IsPressed(VK_SHIFT))
	{
		m_Camera.setSpeed(0.2f);
	}
	if(m_Keys.IsPressed(VK_UP) || m_Keys.IsPressed('W'))   /**< 向上方向键或'W'键按下 */
		m_Camera.moveCamera(m_Camera.getSpeed());          /**< 移动摄像机 */

	if(m_Keys.IsPressed(VK_DOWN) || m_Keys.IsPressed('S')) /**< 向下方向键或'S'键按下 */
		m_Camera.moveCamera(-m_Camera.getSpeed());         /**< 移动摄像机 */

	if(m_Keys.IsPressed(VK_LEFT) || m_Keys.IsPressed('A')) /**< 向左方向键或'A'键按下 */
		m_Camera.yawCamera(-m_Camera.getSpeed());          /**< 移动摄像机 */

	if(m_Keys.IsPressed(VK_RIGHT) || m_Keys.IsPressed('D')) /**< 向右方向键或'D'键按下 */
		m_Camera.yawCamera(m_Camera.getSpeed());            /**< 移动摄像机 */

}
/** 程序更新函数 */
void CameraRove::Update(DWORD milliseconds)						
{
	if (m_Keys.IsPressed(VK_ESCAPE) == true)					/**< 按ESC退出 */
	{
		TerminateApplication();									
	}


	if (m_Keys.IsPressed(VK_F1) == true)					/**< 按F1在窗口和全屏间切换 */
	{
		ToggleFullscreen();									
	}

	/** 更新摄像机 */
	UpdateCamera();
	
	    
}

/** 计算帧速 */
void CameraRove::CaculateFrameRate()
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
void  CameraRove::PrintText()
{
	char string[128];                               /**< 用于保存输出字符串 */
	glPushAttrib(GL_CURRENT_BIT);                   /**< 保存现有颜色属性信息 */
	glColor3f(0.0f,1.0f,0.0f);                      /**< 设置文字颜色 */
	sprintf(string,"当前位置:X=%3.1f  Y=%3.1f Speed =%3.1f ",   
		     m_Camera.getView().x,-m_Camera.getView().z ,m_Camera.getSpeed()); /**< 字符串赋值 */
	m_Font.PrintText(string,-5.0f,3.5f);

	/** 输出帧速 */
	CaculateFrameRate();                               /**< 计算帧速 */
    sprintf(string,"FPS:%3.0f",m_Fps);                 /**< 字符串赋值 */
	m_Font.PrintText(string, -5.0f,3.0f);              /**< 输出字符串 */
	glPopAttrib();
		
}

/** 绘制网格地面 */
void CameraRove::DrawGrid()
{
    /** 获得场景中一些状态  */
	GLboolean  lp,tp;
	glGetBooleanv(GL_LIGHTING,&lp);  
	glGetBooleanv(GL_TEXTURE_2D,&tp);
	
	/** 关闭纹理和光照 */
	glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
	
	/** 绘制过程 */
	glPushAttrib(GL_CURRENT_BIT);   /**< 保存当前属性 */
    glPushMatrix();                 /**< 压入堆栈 */
	glTranslatef(0.0f,0.0f,0.0f);  
	glColor3f(0.0f, 0.0f, 1.0f);    /**< 设置颜色 */

	/** 在X,Z平面上绘制网格 */
	for(float i = -50; i <= 50; i += 1)
	{
		/** 绘制线 */
		glBegin(GL_LINES);

		    /** X轴方向 */
			glVertex3f(-50, 0, i);
			glVertex3f(50, 0, i);

			/** Z轴方向 */
			glVertex3f(i, 0, -50);
			glVertex3f(i, 0, 50);

		glEnd();
	}
	glPopMatrix();
	glPopAttrib();
	
	/** 恢复场景状态 */
	if(tp)
	  glEnable(GL_TEXTURE_2D);
	if(lp)
      glEnable(GL_LIGHTING);
}

/** 绘制球体 */
void CameraRove::DrawSphere()
{
	/** 设置材质属性 */
	GLfloat mat_ambient[] = { 0.9f, 0.5f, 0.8f, 1.0f };
    GLfloat mat_diffuse[] = { 0.9f, 0.5f, 0.8f, 1.0f };
	GLfloat mat_shininess[] = { 100.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    
	/** 获得纹理启用状态 */
	GLboolean tp;
	glGetBooleanv(GL_TEXTURE_2D,&tp);
    glDisable(GL_TEXTURE_2D);                   /**< 关闭纹理 */
    
	/** 绘制过程 */
	glPushMatrix();
    glTranslatef(-5.0f,2.0f,-10.0f);
    GLUquadricObj * sphere = gluNewQuadric();
    gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricNormals(sphere,GLU_SMOOTH);
	gluSphere(sphere,2.0,50,50);
	gluDeleteQuadric(sphere);
    glPopMatrix();
    
	/** 恢复状态 */
	if(tp)
	   glEnable(GL_TEXTURE_2D);
	
}

/** 绘制木箱 */
void CameraRove::DrawBox()
{
	/** 设置材质属性 */
	GLfloat mat_ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

	glPushMatrix();
	glTranslatef(5.0f,2.0f,-10.0f);
	glScalef(2.0f,2.0f,2.0f);
	
	/** 选择纹理 */
	glBindTexture(GL_TEXTURE_2D, m_Texture.ID);
	
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
void CameraRove::Draw()											
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();	
	
	/** 放置摄像机 */	
	m_Camera.setLook();
	
	/** 绘制过程 */
	DrawGrid();
    DrawSphere();
	DrawBox();
	
	/** 输出文字信息 */
	PrintText();
	
	/** 强制执行所有的OpenGL命令 */	
	glFlush();							    
}
