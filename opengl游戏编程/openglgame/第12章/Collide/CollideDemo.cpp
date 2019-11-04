//=========================================================================
/**
*  @file      CollideDemo.cpp
*
*  项目描述： 碰撞检测演示
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

#include "CollideDemo.h"											/**< 包含头文件 */


///初始化摄像机参数
Vector3 dir(0,0,-10);                     /**< 方向 */
Vector3 pos(0,-50,1000);                  /**< 位置 */
float camera_rotation = 0;                /**< 旋转速度 */

///初始化球的属性
Vector3 veloc(0.5,-0.1,0.5);              /**< 速度 */
Vector3 accel(0,-0.05,0);                 /**< 重力加速度 */

Vector3 ArrayVel[10];                     /**< 保存各个球的速度 */
Vector3 ArrayPos[10];                     /**< 保存各个球的位置 */
Vector3 OldPos[10];                       /**< 保存上一次球的位置 */

int NrOfBalls;                            /**< 设置球的个数 */
double Time = 0.6;                        /**< 设置模拟的时间精度 */



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
  m_Fps = 0;
												
}

/** 初始化变量 */
void CDemo::InitVars()
{
	
	///构造平面 
	pl1._Position = Vector3(0,-300,0);
	pl1._Normal = Vector3(0,1,0);
	pl2._Position = Vector3(300,0,0);
	pl2._Normal = Vector3(-1,0,0);
	pl3._Position = Vector3(-300,0,0);
	pl3._Normal = Vector3(1,0,0);
	pl4._Position = Vector3(0,0,300);
	pl4._Normal = Vector3(0,0,-1);
	pl5._Position = Vector3(0,0,-300);
	pl5._Normal = Vector3(0,0,1);


	///构造圆柱体
	cyl1._Position = Vector3(0,0,0);
	cyl1._Axis = Vector3(0,1,0);
	cyl1._Radius = 60 + 20;
	cyl2._Position = Vector3(200,-300,0);
	cyl2._Axis = Vector3(0,0,1);
	cyl2._Radius = 60 + 20;
	cyl3._Position = Vector3(-200,0,0);
	cyl3._Axis = Vector3(0,1,1);
    cyl3._Axis.unit();
	cyl3._Radius = 30 + 20;
	
	///构造二次几何体
	cylinder_obj = gluNewQuadric();
	gluQuadricTexture(cylinder_obj, GL_TRUE);

    ///设置球的初始属性,并初始化爆炸数组
	NrOfBalls = 10;
	ArrayVel[0] = veloc;
	ArrayPos[0] = Vector3(199,180,10);
	ExplosionArray[0]._Alpha = 0;
	ExplosionArray[0]._Scale = 1;
	ArrayVel[1] = veloc;
	ArrayPos[1] = Vector3(0,150,100);
	ExplosionArray[1]._Alpha = 0;
	ExplosionArray[1]._Scale = 1;
	ArrayVel[2] = veloc;
	ArrayPos[2] = Vector3(-100,180,-100);
	ExplosionArray[2]._Alpha = 0;
	ExplosionArray[2]._Scale = 1;
	for (int i = 3; i < 10; i++)
	{
         ArrayVel[i] = veloc;
	     ArrayPos[i] = Vector3(-500 + i*75, 300, -500 + i*50);
		 ExplosionArray[i]._Alpha = 0;
	     ExplosionArray[i]._Scale = 1;
	}
	for (i = 10; i < 20; i++)
	{
         ExplosionArray[i]._Alpha = 0;
	     ExplosionArray[i]._Scale = 1;
	}
}

/** 载入纹理 */
bool CDemo::LoadTexture()
{
	glEnable(GL_TEXTURE_2D);                                      /**< 启用纹理映射 */
	char* file[] = {"data/marble.bmp","data/spark.bmp","data/boden.bmp","data/wand.bmp"};
	
	/** 循环读入位图纹理 */
	for(int i = 0; i < 4; i++)
	{
		if(!m_texture[i].Load(file[i]))                          /**< 载入位图文件 */
		{
			MessageBox(NULL,"装载位图文件失败！","错误",MB_OK);  /**< 如果载入失败则弹出对话框 */
			return false;
		}
		
	}
	return true;
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
	ResizeDraw(true);											

	
	/** 初始化输入系统 */
	m_pInput = new CInputSystem();
	m_pInput->Init(this->m_Window.GetHwnd(),(HINSTANCE)GetModuleHandle(NULL),
		           true,IS_USEKEYBOARD|IS_USEMOUSE);

    /** 设置光源参数 */
	GLfloat spec[] = { 1.0, 1.0 ,1.0 ,1.0 };      
	GLfloat posl[] = { 0, 400, 0, 1 };               
	GLfloat amb[]  = { 0.2f, 0.2f, 0.2f ,1.0f };   
	GLfloat amb2[] = { 0.3f, 0.3f, 0.3f ,1.0f };  
	float df=100.0;

	glClearDepth(1.0f);									
	glEnable(GL_DEPTH_TEST);							
	glDepthFunc(GL_LEQUAL);		
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);						
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	

	/** 设置材质属性 */
	glMaterialfv(GL_FRONT,GL_SPECULAR,spec);
	glMaterialfv(GL_FRONT,GL_SHININESS,&df);

	/** 设置光源 */
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0,GL_POSITION,posl);
	glLightfv(GL_LIGHT0,GL_AMBIENT,amb2);
	glEnable(GL_LIGHT0);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
   
	/** 开启混合 */
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
   	
	/** 开启纹理映射，并载入纹理 */
	glEnable(GL_TEXTURE_2D);
    LoadTexture();

	/** 用两个互相垂直的平面模拟爆炸效果 */
    glNewList( dlist = glGenLists(1), GL_COMPILE);  /**< 创建显示列表 */
    glBegin(GL_QUADS);
	glRotatef(-45,0,1,0);
	glNormal3f(0,0,1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-50,-40,0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(50,-40,0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(50,40,0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-50,40,0);
    glNormal3f(0,0,-1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-50,40,0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(50,40,0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(50,-40,0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-50,-40,0);

	glNormal3f(1,0,0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0,-40,50);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0,-40,-50);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0,40,-50);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0,40,50);
    glNormal3f(-1,0,0);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0,40,50);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0,40,-50);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0,-40,-50);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0,-40,50);
	glEnd();
    glEndList();                                   /**< 结束显示列表定义 */

	/** 初始化变量 */
	InitVars();    

	/** 初始化字体 */
	if(!m_Font.InitFont())
		MessageBox(NULL,"初始化字体失败!","错误",MB_OK);

	
	return true;											
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

 for(int i=0; i < 4; i++) 
	{
		m_texture[i].FreeImage();               /**< 释放纹理图像占用的内存 */
	    glDeleteTextures(1, &m_texture[i].ID);  /**< 删除纹理对象 */
	}
	
	/** 释放二次对象 */
	if(cylinder_obj != NULL)
		gluDeleteQuadric(cylinder_obj);
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
	
}

/** 计算帧速 */
void CDemo::CaculateFrameRate()
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
void  CDemo::PrintText()
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

/** 检测是否与平面碰撞 */
int CDemo::TestIntersionPlane(const Plane& plane,const Vector3& position,const Vector3& direction, double& lamda, Vector3& pNormal)
{
	double dotProduct = direction.dot(plane._Normal);
	double l2;

    ///判断是否平行于平面
    if ((dotProduct < ZERO) && (dotProduct > -ZERO)) 
		return 0;

    l2 = (plane._Normal.dot(plane._Position - position))/dotProduct;

	///交点在射线相反方向，此时不发生碰撞
    if (l2 < -ZERO) 
		return 0;

    pNormal = plane._Normal;
	lamda = l2;
    return 1;
}

/** 检测是否与圆柱体碰撞 */
int CDemo::TestIntersionCylinder(const Cylinder& cylinder,const Vector3& position,const Vector3& direction, double& lamda, Vector3& pNormal,Vector3& newposition)
{
	Vector3 RC;
	double d;
	double t,s;
	Vector3 n,D,O;
	double ln;
	double in,out;
	

	Vector3::subtract(position,cylinder._Position,RC);
	Vector3::cross(direction,cylinder._Axis,n);

    ln = n.mag();

	if ( (ln<ZERO)&&(ln>-ZERO) ) return 0;

	n.unit();

	d =  fabs( RC.dot(n) );

    if (d <= cylinder._Radius)
	{
		Vector3::cross(RC,cylinder._Axis,O);
		t =  - O.dot(n)/ln;
		Vector3::cross(n,cylinder._Axis,O);
		O.unit();
		s =  fabs( sqrt(cylinder._Radius*cylinder._Radius - d*d) / direction.dot(O) );

		in = t-s;
		out = t+s;

		if (in<-ZERO){
			if (out<-ZERO) return 0;
			else lamda = out;
		}
		else
        if (out<-ZERO) {
			      lamda = in;
		}
		else
		if (in<out) lamda = in;
		else lamda = out;

    	newposition = position+direction*lamda;
		Vector3 HB = newposition-cylinder._Position;
		pNormal = HB - cylinder._Axis*(HB.dot(cylinder._Axis));
		pNormal.unit();

		return 1;
	}
    
	return 0;
}

int CDemo::FindBallCol(Vector3& point, double& TimePoint, double Time2, int& BallNr1, int& BallNr2)
{
	Vector3 RelativeV;
	TRay rays;
	double MyTime = 0.0, Add = Time2/150.0, Timedummy = 10000, Timedummy2 = -1;
	Vector3 posi;
	
	///判断球和球是否相交
	for (int i = 0;i<NrOfBalls-1;i++)
	{
	 for (int j = i+1;j<NrOfBalls;j++)
	 {	
		    RelativeV = ArrayVel[i]-ArrayVel[j];
			rays = TRay(OldPos[i],Vector3::unit(RelativeV));
			MyTime = 0.0;

			if ( (rays.dist(OldPos[j])) > 40) continue; 

			while (MyTime<Time2)
			{
			   MyTime += Add;
			   posi = OldPos[i]+RelativeV*MyTime;
			   if (posi.dist(OldPos[j]) <= 40) {
										   point = posi;
										   if (Timedummy>(MyTime-Add)) Timedummy = MyTime-Add;
										   BallNr1 = i;
										   BallNr2 = j;
										   break;
										}
			
			}
	 }

	}

	if (Timedummy != 10000) { TimePoint = Timedummy;
	                        return 1;
	}

	return 0;
}

/** 主要的处理过程 */
void CDemo::Process()
{
	 double rt,rt2,rt4,lamda = 10000;
  Vector3 norm,uveloc;
  Vector3 normal,point,time;
  double RestTime,BallTime;
  Vector3 Pos2;
  int BallNr = 0,dummy = 0,BallColNr1,BallColNr2;
  Vector3 Nc;

  ///如果没有锁定到球上，旋转摄像机
 // if (!hook_toball1)
 // {
	  camera_rotation += 0.1f;
	  if (camera_rotation>360)
		  camera_rotation = 0;
 // }
 
	  RestTime = Time;
	  lamda = 1000;

	///计算重力加速度
	for (int j = 0;j<NrOfBalls;j++)
	  ArrayVel[j] += accel*RestTime;

	///如果在一步的模拟时间内(如果来不及计算，则跳过几步)
	while (RestTime>ZERO)
	{
	   lamda = 10000;   
	
	   ///对于每个球，找到它们最近的碰撞点
   	   for (int i = 0;i<NrOfBalls;i++)
	   {
		      ///计算新的位置和移动的距离
			  OldPos[i] = ArrayPos[i];
			  Vector3::unit(ArrayVel[i],uveloc);
			  ArrayPos[i] = ArrayPos[i]+ArrayVel[i]*RestTime;
			  rt2 = OldPos[i].dist(ArrayPos[i]);

			  ///测试是否和墙面碰撞
			  if (TestIntersionPlane(pl1,OldPos[i],uveloc,rt,norm))
			  {  
				  ///计算碰撞的时间
				  rt4 = rt*RestTime/rt2;

				  ///如果小于当前保存的碰撞时间，则更新它
				  if (rt4 <= lamda)
				  { 
				    if (rt4 <= RestTime+ZERO)
						 if (! ((rt <= ZERO)&&(uveloc.dot(norm)>ZERO)) )
						  {
							normal = norm;
							point = OldPos[i]+uveloc*rt;
							lamda = rt4;
							BallNr = i;
						  }
				  }
			  }
			  
			  if (TestIntersionPlane(pl2,OldPos[i],uveloc,rt,norm))
			  {
				   rt4 = rt*RestTime/rt2;

				  if (rt4 <= lamda)
				  { 
				    if (rt4 <= RestTime+ZERO)
						if (! ((rt <= ZERO)&&(uveloc.dot(norm)>ZERO)) )
						 {
							normal = norm;
							point = OldPos[i]+uveloc*rt;
							lamda = rt4;
							BallNr = i;
							dummy = 1;
						 }
				  }
				 
			  }

			  if (TestIntersionPlane(pl3,OldPos[i],uveloc,rt,norm))
			  {
			      rt4 = rt*RestTime/rt2;

				  if (rt4 <= lamda)
				  { 
				    if (rt4 <= RestTime+ZERO)
						if (! ((rt <= ZERO)&&(uveloc.dot(norm)>ZERO)) )
						 {
							normal = norm;
							point = OldPos[i]+uveloc*rt;
							lamda = rt4;
							BallNr = i;
						 }
				  }
			  }

			  if (TestIntersionPlane(pl4,OldPos[i],uveloc,rt,norm))
			  {
				  rt4 = rt*RestTime/rt2;

				  if (rt4 <= lamda)
				  { 
				    if (rt4 <= RestTime+ZERO)
						if (! ((rt <= ZERO)&&(uveloc.dot(norm)>ZERO)) )
						 {
							normal = norm;
							point = OldPos[i]+uveloc*rt;
							lamda = rt4;
							BallNr = i;
						 }
				  }
			  }

			  if (TestIntersionPlane(pl5,OldPos[i],uveloc,rt,norm))
			  {
				  rt4 = rt*RestTime/rt2;

				  if (rt4 <= lamda)
				  { 
				    if (rt4 <= RestTime+ZERO)
						if (! ((rt <= ZERO)&&(uveloc.dot(norm)>ZERO)) )
						 {
							normal = norm;
							point = OldPos[i]+uveloc*rt;
							lamda = rt4;
							BallNr = i;
						 }
				  }
			  }

              ///测试是否与三个圆柱相碰
			  if (TestIntersionCylinder(cyl1,OldPos[i],uveloc,rt,norm,Nc))
			  {
				  rt4 = rt*RestTime/rt2;

				  if (rt4 <= lamda)
				  { 
				    if (rt4 <= RestTime+ZERO)
						if (! ((rt <= ZERO)&&(uveloc.dot(norm)>ZERO)) )
						 {
							normal = norm;
							point = Nc;
							lamda = rt4;
							BallNr = i;
						 }
				  }
				 
			  }
			  if (TestIntersionCylinder(cyl2,OldPos[i],uveloc,rt,norm,Nc))
			  {
				  rt4 = rt*RestTime/rt2;

				  if (rt4 <= lamda)
				  { 
				    if (rt4 <= RestTime+ZERO)
						if (! ((rt <= ZERO)&&(uveloc.dot(norm)>ZERO)) )
						 {
							normal = norm;
							point = Nc;
							lamda = rt4;
							BallNr = i;
						 }
				  }
				 
			  }
			  if (TestIntersionCylinder(cyl3,OldPos[i],uveloc,rt,norm,Nc))
			  {
				  rt4 = rt*RestTime/rt2;

				  if (rt4 <= lamda)
				  { 
				    if (rt4 <= RestTime+ZERO)
						if (! ((rt <= ZERO)&&(uveloc.dot(norm)>ZERO)) )
						 {
							normal = norm;
							point = Nc;
							lamda = rt4;
							BallNr = i;
						 }
				  }
				 
			  }
	   }


	   ///计算每个球之间的碰撞，如果碰撞时间小于与上面的碰撞，则替换它们
	   if (FindBallCol(Pos2,BallTime,RestTime,BallColNr1,BallColNr2))
	   {
						      
				  if ( (lamda == 10000) || (lamda>BallTime) )
				  {
					  RestTime = RestTime-BallTime;

					  Vector3 pb1,pb2,xaxis,U1x,U1y,U2x,U2y,V1x,V1y,V2x,V2y;
					  double a,b;

					  pb1 = OldPos[BallColNr1]+ArrayVel[BallColNr1]*BallTime;
					  pb2 = OldPos[BallColNr2]+ArrayVel[BallColNr2]*BallTime;
					  xaxis = (pb2-pb1).unit();

					  a = xaxis.dot(ArrayVel[BallColNr1]);
					  U1x = xaxis*a;
					  U1y = ArrayVel[BallColNr1]-U1x;

					  xaxis = (pb1-pb2).unit();
					  b = xaxis.dot(ArrayVel[BallColNr2]);
      				  U2x = xaxis*b;
					  U2y = ArrayVel[BallColNr2]-U2x;

					  V1x = (U1x+U2x-(U1x-U2x))*0.5;
					  V2x = (U1x+U2x-(U2x-U1x))*0.5;
					  V1y = U1y;
					  V2y = U2y;

					  for (j = 0;j<NrOfBalls;j++)
					  ArrayPos[j] = OldPos[j]+ArrayVel[j]*BallTime;

					  ArrayVel[BallColNr1] = V1x+V1y;
					  ArrayVel[BallColNr2] = V2x+V2y;

					  ///更新爆炸数组
                      for(j = 0;j<20;j++)
					  {
						  if (ExplosionArray[j]._Alpha <= 0)
						  {
							  ExplosionArray[j]._Alpha = 1;
                              ExplosionArray[j]._Position = ArrayPos[BallColNr1];
							  ExplosionArray[j]._Scale = 1;
							  break;
						  }
					  }

					  continue;
				  }
		}
  
	    ///最后的测试，替换下次碰撞的时间，并更新爆炸效果的数组
		if (lamda!= 10000)
		{		 
			RestTime -= lamda;

			for (j = 0;j<NrOfBalls;j++)
			ArrayPos[j] = OldPos[j]+ArrayVel[j]*lamda;

			rt2 = ArrayVel[BallNr].mag();
			ArrayVel[BallNr].unit();
			ArrayVel[BallNr] = Vector3::unit( (normal*(2*normal.dot(-ArrayVel[BallNr]))) + ArrayVel[BallNr] );
			ArrayVel[BallNr] = ArrayVel[BallNr]*rt2;
				
			for(j = 0;j<20;j++)
			{
			  if (ExplosionArray[j]._Alpha <= 0)
			  {
				  ExplosionArray[j]._Alpha = 1;
                  ExplosionArray[j]._Position = point;
				  ExplosionArray[j]._Scale = 1;
				  break;
			  }
			}
		}
		else
			RestTime = 0;

	}
}


/** 绘制函数 */
void CDemo::Draw()											
{
/** 用户自定义的绘制过程 */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);			
	glLoadIdentity();	

	Process();  /** 处理模拟 */

	int i;
	 	
	///设置摄像机朝向固定点
	gluLookAt(pos.x,pos.y,pos.z, pos.x+dir.x,pos.y+dir.y,pos.z+dir.z, 0,1.0,0.0);
	
	glRotatef(camera_rotation,0,1,0);

	///渲染球
	for (i = 0;i<NrOfBalls;i++)
	{
		switch(i)
		{
			case 1: glColor3f(1.0f,1.0f,1.0f);
					break;
			case 2: glColor3f(1.0f,1.0f,0.0f);
					break;
			case 3: glColor3f(0.0f,1.0f,1.0f);
					break;
			case 4: glColor3f(0.0f,1.0f,0.0f);
					break;
			case 5: glColor3f(0.0f,0.0f,1.0f);
					break;
			case 6: glColor3f(0.65f,0.2f,0.3f);
					break;
			case 7: glColor3f(1.0f,0.0f,1.0f);
					break;
			case 8: glColor3f(0.0f,0.7f,0.4f);
					break;
			default: glColor3f(1.0f,0,0);
		}
		glPushMatrix();
		glTranslated(ArrayPos[i].x,ArrayPos[i].y,ArrayPos[i].z);
		gluSphere(cylinder_obj,20,20,20);
		glPopMatrix();
	}
	
	/** 开启纹理映射 */
	glEnable(GL_TEXTURE_2D);
	///渲染墙面
	glBindTexture(GL_TEXTURE_2D, m_texture[3].ID); 
	glColor3f(1, 1, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(320,320,320);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(320,-320,320);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-320,-320,320);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-320,320,320);
        
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-320,320,-320);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-320,-320,-320);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(320,-320,-320);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(320,320,-320);
    
	glTexCoord2f(1.0f, 0.0f); glVertex3f(320,320,-320);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(320,-320,-320);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(320,-320,320);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(320,320,320);
	
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-320,320,320);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-320,-320,320);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-320,-320,-320);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-320,320,-320);
	glEnd();

	///渲染地面
	glBindTexture(GL_TEXTURE_2D, m_texture[2].ID); 
    glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-320,-320,320);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(320,-320,320);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(320,-320,-320);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-320,-320,-320);
	glEnd();

    ///渲染圆柱
	glBindTexture(GL_TEXTURE_2D, m_texture[0].ID);   
	glColor3f(0.5,0.5,0.5);
    glPushMatrix();
	glRotatef(90, 1,0,0);
	glTranslatef(0,0,-500);
	gluCylinder(cylinder_obj, 60, 60, 1000, 20, 2);
	glPopMatrix();

  	glPushMatrix();
  	glTranslatef(200,-300,-500);
	gluCylinder(cylinder_obj, 60, 60, 1000, 20, 2);
	glPopMatrix();

	glPushMatrix();
    glTranslatef(-200,0,0);
	glRotatef(135, 1,0,0);
	glTranslatef(0,0,-500);
	gluCylinder(cylinder_obj, 30, 30, 1000, 20, 2);
	glPopMatrix();
	
	///渲染球的爆炸效果
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBindTexture(GL_TEXTURE_2D, m_texture[1].ID);   
	for(i = 0; i < 20; i++)
	{
		if(ExplosionArray[i]._Alpha >= 0)
		{
		   glPushMatrix();
           ExplosionArray[i]._Alpha -= 0.01f;
		   ExplosionArray[i]._Scale += 0.03f;
		   glColor4f(1,1,0,ExplosionArray[i]._Alpha);	 
		   glScalef(ExplosionArray[i]._Scale,ExplosionArray[i]._Scale,ExplosionArray[i]._Scale);
           glTranslatef((float)ExplosionArray[i]._Position.x/ExplosionArray[i]._Scale, 
			            (float)ExplosionArray[i]._Position.y/ExplosionArray[i]._Scale, 
						(float)ExplosionArray[i]._Position.z/ExplosionArray[i]._Scale);
           glCallList(dlist);
	       glPopMatrix();
		}
	}

	/** 恢复一些属性 */
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
   
	/** 输出屏幕信息 */ 
	PrintText();
		

}
