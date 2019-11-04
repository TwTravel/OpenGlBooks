//=========================================================================
/**
*  @file  Light.cpp
*
*  项目描述： OPenGL光照效果演示
*  文件描述:  具体实例类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-09-13	
*  修改日期： 2006-11-17
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

#include "Light.h"											/**< 包含头文件 */


/** 定义三个光源（红色 绿色和蓝色）的位置 */
static float lightPositionR[] = { 0.0f, 0.0f, 5.0f, 1.0f };
static float lightPositionG[] = { 0.0f, 0.0f, 5.0f, 1.0f };
static float lightPositionB[] = { 0.0f, 0.0f, 5.0f, 1.0f };

/** 定义三个光源的漫射光 */
static float diffuseLightR[] = { 1.0f, 0.0f, 0.0f, 1.0f };
static float diffuseLightG[] = { 0.0f, 1.0f, 0.0f, 1.0f };
static float diffuseLightB[] = { 0.0f, 0.0f, 1.0f, 1.0f };

/** 定义三个光源的镜面光 */
static float specularLightR[] = { 1.0f, 0.0f, 0.0f, 1.0f };
static float specularLightG[] = { 0.0f, 1.0f, 0.0f, 1.0f };
static float specularLightB[] = { 0.0f, 0.0f, 1.0f, 1.0f };

/** 再定义一个默认的光源 */
static float diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };	 
static float specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };	 
static float lightPosition[] = { 0.0f, 0.0f, 10.0f, 1.0f };


/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	Light * light = new Light(class_name);
	return reinterpret_cast<GLApplication *>(light);
}


/** 构造函数 */
Light::Light(const char * class_name) : GLApplication(class_name)
{
  /// 初始化用户自定义的程序变量
	m_Angle = 0.0f;												/**< 设置初始角度为0 */
	m_bRedX = 0.0f;
	m_bGreenX = 0.0f;
	m_bBlueX = 0.0f;
	m_bRedY = 0.0f;
	m_bGreenY = 0.0f;
	m_bBlueY = 0.0f;
	
}

/** 初始化OpenGL */
bool Light::Init()									
{
/** 用户自定义的初始化过程 */
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);						
	glClearDepth(1.0f);	
	glDepthFunc(GL_LEQUAL);	
	
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	
	glEnable(GL_LIGHTING);
	
	/** 设置0号光源 */
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	
	/** 设置1号红色光源 */
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLightR);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLightR);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPositionR);
	
	/** 设置2号绿色光源 */
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLightG);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specularLightG);
	glLightfv(GL_LIGHT2, GL_POSITION, lightPositionG);
	
	/** 设置3号蓝色光源 */
	glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuseLightB);
	glLightfv(GL_LIGHT3, GL_SPECULAR, specularLightB);
	glLightfv(GL_LIGHT3, GL_POSITION, lightPositionB);

	glEnable(GL_LIGHT0);                                        /**< 启用0号灰色光源,让物体可见 */
	
	ResizeDraw(true);											/**< 改变OpenGL窗口大小，直接调用子类的函数 */

	return true;												/**< 成功返回 */
}

/** 用户自定义的卸载函数 */
void Light::Uninit()									
{
/** 用户自定义的卸载过程 */
///......
///......
}

/** 程序更新函数 */
void Light::Update(DWORD milliseconds)						
{
	if (m_Keys.IsPressed(VK_ESCAPE) == true)					/**< 按ESC退出 */
	{
		TerminateApplication();									
	}

  /** 用户自定义的更新过程 */
   
	/**　更新红色光源 */
	if(m_Keys.IsPressed(0x31) == false)       /**< 当数字'1'键没有被按下时 */
	{
		glEnable(GL_LIGHT1);                  /**< 启用1号红色光源 */
		m_bRedX += (float)milliseconds / 30;  /**< 更新旋转角度 */
		m_bRedY += (float)milliseconds / 20;
	}
	else
		glDisable(GL_LIGHT1);                 /**< 当被按下时，禁用该光源 */
 
	/**　更新绿色光源 */
	if(m_Keys.IsPressed(0x32) == false)        /**< 当数字'2'键没有被按下时 */
	{
		glEnable(GL_LIGHT2);                   /**< 启用2号绿色光源 */
		m_bGreenX += (float)milliseconds / 20; /**< 更新旋转角度 */
		m_bGreenY += (float)milliseconds / 30;
	}
	else
		glDisable(GL_LIGHT2);                 /**< 当被按下时，禁用该光源 */

	/**　更新蓝色光源 */
	if(m_Keys.IsPressed(0x33) == false)       /**< 当数字'3'键没有被按下时 */
	{
		glEnable(GL_LIGHT3);                  /**< 启用3号蓝色光源 */
		m_bBlueX += (float)milliseconds / 30; /**< 更新旋转角度 */
		m_bBlueY += (float)milliseconds / 30;
	}
	else
		glDisable(GL_LIGHT3);               /**< 当被按下时，禁用该光源 */

     m_Angle += (float)milliseconds / 30;   /**< 更新物体旋转角度 */

}

/** 绘制函数 */
void Light::Draw()											
{
     /** 用户自定义的绘制过程 */
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();	
	
	glTranslatef(0.0f, 0.0f, -10.0f); /**< 移入屏幕 */
	
	/** 红色光源 */
	glPushMatrix();
		/** 旋转红色光源球 */
		glRotatef(m_bRedX, 1.0f, 0.0f, 0.0f);
		glRotatef(m_bRedY, 0.0f, 1.0f, 0.0f);
		
		/** 设置红色光源位置 */
		glLightfv(GL_LIGHT1, GL_POSITION, lightPositionR);

		/**< 在光源位置处绘制一个球代表光源 */
		glTranslatef(lightPositionR[0], lightPositionR[1], lightPositionR[2]);
		glColor3f(1.0f, 0.0f, 0.0f);	/**< 红色 */
		
		glPushAttrib(GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);	/**< 关闭光照 */
			  auxSolidSphere(0.1);
			glEnable(GL_LIGHTING);  /**< 启用光照 */
		glPopAttrib();	
	glPopMatrix();
	
	/** 绿色光源 */
	glPushMatrix();
		/** 旋转绿色光源球 */
		glRotatef(m_bGreenX, 1.0f, 0.0f, 0.0f);
		glRotatef(m_bGreenY, 0.0f, 1.0f, 0.0f);
		
		/** 设置绿色光源位置 */
		glLightfv(GL_LIGHT2, GL_POSITION, lightPositionG);
	
		/**< 在光源位置处绘制一个球代表光源 */
		glTranslatef(lightPositionG[0], lightPositionG[1], lightPositionG[2]);
		glColor3f(0.0f, 1.0f, 0.0f);	/**< 绿色 */
		
		glPushAttrib(GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);	/**< 关闭光照 */
			  auxSolidSphere(0.1);
			glEnable(GL_LIGHTING);  /**< 启用光照 */
		glPopAttrib();	
	glPopMatrix();
	
	/** 蓝色光源 */
	glPushMatrix();
		
	    /** 旋转蓝色光源球 */
		glRotatef(m_bBlueX, 1.0f, 0.0f, 0.0f);
		glRotatef(m_bBlueY, 0.0f, 1.0f, 0.0f);

		/** 设置蓝色光源位置 */
		glLightfv(GL_LIGHT3, GL_POSITION, lightPositionB);
	
		/**< 在光源位置处绘制一个球代表光源 */
		glTranslatef(lightPositionB[0], lightPositionB[1], lightPositionB[2]);
		glColor3f(0.0f, 0.0f, 1.0f);	/**< 蓝色 */
		
		glPushAttrib(GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);	/**< 关闭光照 */
			  auxSolidSphere(0.1);
			glEnable(GL_LIGHTING);  /**< 启用光照 */
		glPopAttrib();	
	glPopMatrix();
	
	/** 绘制一个球体 */
	glPushMatrix();
		/**< 设置旋转 */
		glRotatef(m_Angle, 1.0f, 0.0f, 0.0f);
		glRotatef(m_Angle, 0.0f, 1.0f, 0.0f);
		glRotatef(m_Angle, 0.0f, 0.0f, 1.0f);
		glColor3f(1.0, 1.0, 1.0);
		auxSolidSphere(2.5);
	glPopMatrix();

   glFlush();													/**< 强制执行所有的OpenGL命令 */
}
