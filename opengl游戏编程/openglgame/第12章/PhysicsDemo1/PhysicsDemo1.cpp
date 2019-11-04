//=========================================================================
/**
*  @file      PhysicsDemo1.cpp
*
*  项目描述： 匀速运动演示
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

#include "PhysicsDemo1.h"											/**< 包含头文件 */

/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	CDemo * demo = new CDemo(class_name);
	return reinterpret_cast<GLApplication *>(demo);
}


/** 构造函数 */
CDemo::CDemo(const char * class_name) : GLApplication(class_name)
{
  /// 初始化用户自定义的程序变量
	x = -5.0;
	y = 0.5;
	vx = 0.01;

												
}

/** 初始化OpenGL */
bool CDemo::Init()									
{
/** 用户自定义的初始化过程 */
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);						
	glClearDepth(1.0f);											
	glDepthFunc(GL_LEQUAL);										
	glEnable(GL_DEPTH_TEST);									
	glShadeModel(GL_SMOOTH);									
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			
	ResizeDraw(true);											/**< 改变OpenGL窗口大小，直接调用子类的函数 */


	/** 定义光源的属性值 */
	GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f }; 	/**< 环境光参数 */
	GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };		/**< 漫射光参数 */
	GLfloat LightSpecular[]= { 1.0f, 1.0f, 1.0f, 1.0f };	/**< 镜面光参数 */
	GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };	/**< 光源位置 */


    /** 设置光源的属性值 */
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		/**< 设置环境光 */
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		/**< 设置漫射光 */
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);	/**< 设置漫射光 */
	
	
	/** 启用光源和纹理 */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1); 	
	
	/** 初始化输入系统 */
	m_pInput = new CInputSystem();
	m_pInput->Init(this->m_Window.GetHwnd(),(HINSTANCE)GetModuleHandle(NULL),
		           true,IS_USEKEYBOARD|IS_USEMOUSE);
	
	return true;											/**< 成功返回 */
}

/** 用户自定义的卸载函数 */
void CDemo::Uninit()									
{
/** 用户自定义的卸载过程 */
 if(m_pInput)
 {
	 delete m_pInput;
	 m_pInput = NULL;
 }
}

/** 程序更新函数 */
void CDemo::Update(DWORD milliseconds)						
{

/** 用户自定义的更新过程 */
	m_pInput->Update();
	if(m_pInput->GetKeyboard()->KeyDown(DIK_ESCAPE))
	{
		TerminateApplication();
	} 
    
	/** 更新小球的位置 */
    x += vx;
	if(x > 5.0 || x<-5.0)
		vx = -vx;
     
}


/** 绘制网格地面 */
void CDemo::DrawGrid()
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
void CDemo::DrawSphere(float x,float y)
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
    glTranslatef(x,y,-5.0f);
    GLUquadricObj * sphere = gluNewQuadric();
    gluQuadricOrientation(sphere, GLU_OUTSIDE);
	gluQuadricNormals(sphere,GLU_SMOOTH);
	gluSphere(sphere,0.25,20,20);
	gluDeleteQuadric(sphere);
    glPopMatrix();
    
	/** 恢复状态 */
	if(tp)
	   glEnable(GL_TEXTURE_2D);
	
}

/** 绘制函数 */
void CDemo::Draw()											
{
/** 用户自定义的绘制过程 */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();	

	/** 设置视点 */
	gluLookAt(0.0f,1.5f, 5.0f, 
		      0.0f, 1.5f, 0.0f,
			  0.0f, 1.0f, 0.0f);
	
	/** 绘制网格地面 */
	DrawGrid();

	/** 绘制小球 */
    DrawSphere(x,y);


		

}
