//=========================================================================
/**
*  @file     PlanarShadow.cpp
*
*  项目描述： 平面投射阴影
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
#include "stdafx.h"
#include "PlanarShadow.h"											/**< 包含头文件 */


/** 定义一些变量 */
float lightPos[4] = {0.0f, 0.0f, 13.0f, 1.0f};  /**< 光源位置 */
float direction = -0.06f;                       /**< X轴上移动增量 */

/** 下面这些变量控制3个球的位置 */
float objPos1;
float objPos2;
float objPos3;

float obj1 = -0.01f;
float obj2 = -0.02f;
float obj3 = -0.03f;


/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	CPlanarShadow * demo = new CPlanarShadow(class_name);
	return reinterpret_cast<GLApplication *>(demo);
}


/** 构造函数 */
CPlanarShadow::CPlanarShadow(const char * class_name) : GLApplication(class_name)
{
  /// 初始化用户自定义的程序变量
	m_Fps = 0;
	
}

/** 初始化OpenGL */
bool CPlanarShadow::Init()									
{
/** 用户自定义的初始化过程 */
	glClearColor(0.2f, 0.2f, 0.2f, 0.5f);						
	glClearDepth(1.0f);											
	glEnable(GL_DEPTH_TEST);
	ResizeDraw(true);	                             /**< 改变OpenGL窗口大小，直接调用子类的函数 */

	/** 设置光源属性 */
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	

	/** 初始化字体 */
	if(!m_Font.InitFont())
		MessageBox(NULL,"初始化字体失败!","错误",MB_OK);
 
	float size = 6.0f;	      /**< 定义墙的边长 */
	Wall wall;                /**< 定义结构体 */
	
	///创建左面
	wall.vVerts.clear();									/**< 清空顶点数组 */
	wall.vNormal = Vector3(1, 0, 0);						/**< 设置法向量 */
	wall.vVerts.push_back(Vector3(-size, size, size));		/**< 下面定义顶点 */
	wall.vVerts.push_back(Vector3(-size, -size, size));	
	wall.vVerts.push_back(Vector3(-size, -size, 0));		
	wall.vVerts.push_back(Vector3(-size, size, 0));		
	wall.vColor = Vector3(1, 0, 0);						    /**< 指定颜色 */
	walls.push_back(wall);								    /**< 添加该面到容器中 */

	///创建右面
	wall.vVerts.clear();									/**< 清空顶点数组 */
	wall.vNormal = Vector3(-1, 0, 0);						/**< 设置法向量 */
	wall.vVerts.push_back(Vector3(size, size, 0));			/**< 下面定义顶点 */
	wall.vVerts.push_back(Vector3(size, -size, 0));		
	wall.vVerts.push_back(Vector3(size, -size, size));		
	wall.vVerts.push_back(Vector3(size, size, size));		
	wall.vColor = Vector3(0, 0, 1);						    /**< 指定颜色 */
	walls.push_back(wall);								    /**< 添加该面到容器中 */
	
	
	///创建底面
	wall.vVerts.clear();									/**< 清空顶点数组 */
	wall.vNormal = Vector3(0, 1, 0);						/**< 设置法向量 */
	wall.vVerts.push_back(Vector3(size, -size, size));		/**< 下面定义顶点 */
	wall.vVerts.push_back(Vector3(size, -size, 0));		
	wall.vVerts.push_back(Vector3(-size, -size, 0));		
	wall.vVerts.push_back(Vector3(-size, -size, size));	
	wall.vColor = Vector3(0, 1, 0);						    /**< 指定颜色 */
	walls.push_back(wall);								    /**< 添加该面到容器中 */

	///创建顶面
	wall.vVerts.clear();									/**< 清空顶点数组 */
	wall.vNormal = Vector3(0, -1, 0);						/**< 设置法向量 */
	wall.vVerts.push_back(Vector3(size, size, size));		/**< 下面定义顶点 */
	wall.vVerts.push_back(Vector3(size, size, 0));		
	wall.vVerts.push_back(Vector3(-size,size, 0));		
	wall.vVerts.push_back(Vector3(-size,size, size));	
	wall.vColor = Vector3(0, 1, 1);						    /**< 指定颜色 */
	walls.push_back(wall);								    /**< 添加该面到容器中 */
	
	///创建后面
	wall.vVerts.clear();									/**< 清空顶点数组 */
	wall.vNormal = Vector3(0, 0, 1);						/**< 设置法向量 */
	wall.vVerts.push_back(Vector3(-size, size, 0));		    /**< 下面定义顶点 */
	wall.vVerts.push_back(Vector3(-size, -size, 0));		
	wall.vVerts.push_back(Vector3(size, -size, 0));		
	wall.vVerts.push_back(Vector3(size, size, 0));			
	wall.vColor = Vector3(1, 0, 1);						    /**< 指定颜色 */
	walls.push_back(wall);	                                /**< 添加该面到容器中 */
	

	return true;                                            /**< 成功返回 */
}

/** 用户自定义的卸载函数 */
void CPlanarShadow::Uninit()									
{
	
}

/** 程序更新函数 */
void CPlanarShadow::Update(DWORD milliseconds)						
{
	
	/** 按ESC键时退出 */
 	if(m_Keys.IsPressed(VK_ESCAPE))
    {
		TerminateApplication();
	} 
		    
}

/** 计算帧速 */
void CPlanarShadow::CaculateFrameRate()
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
void  CPlanarShadow::PrintText()
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

/** 绘制墙面 */
void CPlanarShadow::RenderWall(const Wall& wall)
{
	glPushMatrix();
	    /** 开始绘制 */
		glBegin(GL_QUADS);
			
			/** 指定法线 */
			glNormal3f(wall.vNormal.x, wall.vNormal.y, wall.vNormal.z);

			/** 指定颜色 */
			glColor4f(wall.vColor.x, wall.vColor.y, wall.vColor.z, 1.0);

			/** 指定顶点 */			 
			for(int i = 0; i < (int)wall.vVerts.size(); i++)
			{
				glVertex3f(wall.vVerts[i].x, wall.vVerts[i].y, wall.vVerts[i].z);
			}

		/** 绘制结束 */
		glEnd();

	glPopMatrix();

}

/** 绘制物体 */
void CPlanarShadow::RenderObjects()
{
	/** 创建一个二次对象指针 */
    GLUquadricObj *pObj = gluNewQuadric();			
	gluQuadricDrawStyle(pObj, GLU_FILL);
	

	/** 绘制球体1 */
	glColor4f(0.0f, 1.0f, 0.0f, 1.0);    /**< 指定颜色 */

	glPushMatrix();
		glTranslatef(-3.0f, objPos1, 4.0f);
		gluSphere(pObj, 0.5f, 75, 75);
	glPopMatrix();
	

	/** 绘制球体2 */
	glColor4f(1.0f, 1.0f, 0.1f, 1.0);   /**< 指定颜色 */

	glPushMatrix();
		glTranslatef(3, objPos2, 4);
		gluSphere(pObj, 0.5f, 75, 75);
	glPopMatrix();

	
	/** 绘制球体3 */
	glColor4f(0.0f, 0.0f, 1.0f, 1.0);   /**< 指定颜色 */
	glPushMatrix();
		glTranslatef(0, objPos3, 4);
		gluSphere(pObj, 0.5f, 75, 75);
	glPopMatrix();

	/** 下面是对物体位置进行限定 */
	///球体1
	if(objPos1 < -4.5f)
	{
		obj1 = obj1 * -1;
		objPos1 = -4.5f; 
	}
	if(objPos1 > 4.5f)
	{
		obj1 = obj1 * -1;
		objPos1 = 4.5f; 
	}
	
	///球体2
	if(objPos2 < -4.5f)
	{
		obj2 = obj2 * -1;
		objPos2 = -4.5f; 
	}
	if(objPos2 > 4.5f)
	{
		obj2 = obj2 * -1;
		objPos2 = 4.5f; 
	}	

	///球体3
	if(objPos3 < -4.5f)
	{
		obj3 = obj3 * -1;
		objPos3 = -4.5f; 
	}
	if(objPos3 > 4.5f)
	{
		obj3 = obj3 * -1;
		objPos3 = 4.5f; 
	}	
	
	/** 更新位置 */
	objPos1 += obj1;
	objPos2 += obj2;
	objPos3 += obj3;

	/** 恢复颜色 */
	glColor4f(1.0f, 1.0f, 1.0f, 1.0);

	/** 删除二次对象 */
	gluDeleteQuadric(pObj);

}

/** 绘制光源 */
void CPlanarShadow::RenderLight()
{
	/** 关闭光照 */
	glDisable(GL_LIGHTING);

	glPushMatrix();
		GLUquadricObj *pObj = gluNewQuadric();				/**< 创建二次对象 */
		gluQuadricDrawStyle(pObj, GLU_FILL);				

		///在光源位置处绘制一个黄色小球
		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
		gluSphere(pObj, 0.03f, 25, 25);
		
		gluDeleteQuadric(pObj);								/**< 删除二次对象 */
	glPopMatrix();

	/** 开启光照 */
	glEnable(GL_LIGHTING);

	/** 对光源位置移动位置和方向进行更新 */
	if(lightPos[0] < -8.0f)
	{
		direction = direction * -1;
		lightPos[0] = -8.0f; 
	}
	if(lightPos[0] > 8.0f)
	{
		direction = direction * -1;
		lightPos[0] = 8.0f; 
	}	

	/** 更新光源位置 */
	lightPos[0] += direction;
	glLightfv( GL_LIGHT0, GL_POSITION, lightPos );
}

/** 创建投射矩阵 */
void CPlanarShadow::CreateShadowMatrix(float m[16], Vector3 point, Vector3 normal, float lp[4])
{
	/** 计算顶点到平面的距离 */
	float d = - ((normal.x * point.x) + (normal.y * point.y) + (normal.z * point.z));
	
	/** 计算光源向量和法向量的点积 */
	float dot = normal.x*lp[0]  + normal.y*lp[1] + normal.z*lp[2] + d*lp[3];

	/** 设置矩阵元素值 */
	m[0]  = dot - lp[0]*normal.x;  m[1]  =	   -lp[1]*normal.x;	 m[2]  =      -lp[2]*normal.x;  m[3]  =	    -lp[3]*normal.x;
	m[4]  =		 -lp[0]*normal.y;  m[5]  = dot -lp[1]*normal.y;  m[6]  =	  -lp[2]*normal.y;  m[7]  =	    -lp[3]*normal.y;
	m[8]  =		 -lp[0]*normal.z;  m[9]  =	   -lp[1]*normal.z;	 m[10] = dot  - lp[2]*normal.z; m[11] =	    -lp[3]*normal.z;
	m[12] =		 -lp[0]*d;	       m[13] =	   -lp[1]*d;		 m[14] =	  -lp[2]*d;	        m[15] = dot -lp[3]*d;

}

/** 渲染墙面和阴影 */
void  CPlanarShadow::Render()
{
	glClear(GL_STENCIL_BUFFER_BIT);							/**< 清除模版缓存 */
	glEnable(GL_STENCIL_TEST);								/**< 开始模版测试 */

	/** 循环处理每个墙面 */
	for(int i = 0; i < (int)walls.size(); i++)
	{
		glStencilFunc(GL_ALWAYS, 1, 1);						/**< 设置每个像素模版值为1 */
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);			

		/** 绘制当前墙面 */
		glDisable(GL_LIGHT0);							
		RenderWall(walls[i]);								
		glEnable(GL_LIGHT0);                          

		glDisable(GL_DEPTH_TEST);							/**< 关闭深度测试 */
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);/**< 禁止颜色缓存写 */
		glStencilFunc(GL_EQUAL, 1, 1);                      /**< 绘制模版值为1的象素 */
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);             /**< 模版值加1 */

		/** 开始投射阴影 */
		glPushMatrix();

			/** 创建投射矩阵 */
			float m[16] = {0};								
			CreateShadowMatrix(m, walls[i].vVerts[0], walls[i].vNormal, lightPos);
			glMultMatrixf(m);

			/** 绘制物体,得到阴影 */
			glPushMatrix();
			    RenderObjects();
			glPopMatrix();

		glPopMatrix();

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);	/**< 恢复颜色缓存写 */

		RenderWall(walls[i]);								/**< 绘制除阴影外的部分 */
		glEnable(GL_DEPTH_TEST);							/**< 开始深度测试 */
	}

	/** 关闭模版测试 */
	glDisable(GL_STENCIL_TEST);  				

}
/** 绘制函数 */
void CPlanarShadow::Draw()											
{
/** 用户自定义的绘制过程 */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();
    glTranslatef(0.0f,0.0f,-18.0f);
	
	/** 绘制物体 */
	RenderObjects();
	
	/** 渲染墙面和阴影 */
	Render();

	/** 绘制光源 */
	RenderLight();

	/** 输出屏幕信息 */ 
	PrintText();
		
	glFlush();	                 /**< 强制执行所有的OpenGL命令 */
}