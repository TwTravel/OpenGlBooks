//=========================================================================
/**
*  @file  Material.cpp
*
*  项目描述： OPenGL材质演示
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

#include "Material.h"											/**< 包含头文件 */

#include <gl\gl.h>												/**< 包含OpenGL头文件 */
#include <gl\glu.h>												
#include <gl\glaux.h>											


#pragma comment(lib, "opengl32.lib")							/**< 包含OpenGL链接库文件 */
#pragma comment(lib, "glu32.lib")							
#pragma comment(lib, "glaux.lib")							


/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	Material * material = new Material(class_name);
	return reinterpret_cast<GLApplication *>(material);
}


/** 构造函数 */
Material::Material(const char * class_name) : GLApplication(class_name)
{
  /// 初始化用户自定义的程序变量
	m_Angle = 0.0f;												/**< 设置初始角度为0 */
	
}

/** 初始化OpenGL */
bool Material::Init()									
{
/** 用户自定义的初始化过程 */
	///定义光源GL_LIGHT0
	GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 }; /**< 环境光 */
	GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 }; /**< 漫射光 */
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };/**< 镜面光 */
	GLfloat position[] = { 0.0, 3.0, 2.0, 0.0 };/**< 光源位置 */
	
	///设置光源属性
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	///初始化
	glClearColor(0.0f, 0.2f, 0.2f, 0.0f);						
	glClearDepth(1.0f);	
	glDepthFunc(GL_LEQUAL);	
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	
	glEnable(GL_LIGHTING); /**< 启用光源 */
	glEnable(GL_LIGHT0);  /**< 启用0号光源 */                                  
	
	ResizeDraw(true);											/**< 改变OpenGL窗口大小，直接调用子类的函数 */

	return true;												/**< 成功返回 */
}

/** 用户自定义的卸载函数 */
void Material::Uninit()									
{
/** 用户自定义的卸载过程 */
///......
///......
}

/** 程序更新函数 */
void Material::Update(DWORD milliseconds)						
{
	if (m_Keys.IsPressed(VK_ESCAPE) == true)					/**< 按ESC退出 */
	{
		TerminateApplication();									
	}

	///if (m_Keys.IsPressed(VK_F1) == true)						/**< 按F1切换窗口/全屏模式 */
	///{
	///	ToggleFullscreen();										
	///}
  /** 用户自定义的更新过程 */
  
    m_Angle += (float)milliseconds / 30;  /**< 更新物体旋转角度 */

}

/** 绘制函数 */
void Material::Draw()											
{
     /** 用户自定义的绘制过程 */
	///定义材质属性值
	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 }; /** < 无材质颜色 */
	GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 }; /** < 环境颜色 */
	GLfloat mat_ambient_color[] = { 0.8, 0.6, 0.2, 1.0 };
	GLfloat mat_diffuse[] = { 0.2, 0.5, 0.8, 1.0 };/** < 散射颜色 */
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };/** < 镜面反射颜色 */
	GLfloat no_shininess[] = { 0.0 };              /** < 镜面反射指数为0 */
	GLfloat low_shininess[] = { 5.0 };             /** < 镜面反射指数为5.0 */
	GLfloat high_shininess[] = { 100.0 };          /** < 镜面反射指数为100.0 */
	GLfloat mat_emission[] = {0.3, 0.2, 0.3, 0.0}; /** < 发射光颜色 */

	//清除缓存并重置单位矩阵
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();	
	
	/** 第一行第一列绘制的球仅有漫反射光而无环境光和镜面光。*/
	glPushMatrix();
		glTranslatef (-2.0, 1.75, -6.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
		glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
		auxSolidSphere(0.5);
	glPopMatrix();

	/** 第一行第二列绘制的球有漫反射光和镜面光，并有低高光，而无环境光 。*/
	glPushMatrix();
		glTranslatef (-0.75, 1.75, -6.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
	auxSolidSphere(0.5);
	glPopMatrix();

	/** 第一行第三列绘制的球有漫反射光和镜面光，并有很亮的高光，而无环境光 。*/
	glPushMatrix();
		glTranslatef (0.75, 1.75, -6.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
		auxSolidSphere(0.5);
	glPopMatrix();

	/** 第一行第四列绘制的球有漫反射光和辐射光，而无环境和镜面反射光。*/
	glPushMatrix();
		glTranslatef (2.0, 1.75, -6.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
		glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
		auxSolidSphere(0.5);
	glPopMatrix();

	/** 第二行第一列绘制的球有漫反射光和环境光，而镜面反射光。*/
	glPushMatrix();
		glTranslatef (-2.0, 0.0, -6.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
		glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
		auxSolidSphere(0.5);
	glPopMatrix();

	/** 第二行第二列绘制的球有漫反射光、环境光和镜面光，且有低高光。*/
	glPushMatrix();
		glTranslatef (-0.75, 0.0, -6.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
		auxSolidSphere(0.5);
	glPopMatrix();

	/** 第二行第三列绘制的球有漫反射光、环境光和镜面光，且有很亮的高光。*/
	glPushMatrix();
		glTranslatef (0.75, 0.0, -6.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
		auxSolidSphere(0.5);
	glPopMatrix();

	/** 第二行第四列绘制的球有漫反射光、环境光和辐射光，而无镜面光。*/
	glPushMatrix();
		glTranslatef (2, 0.0, -6.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
		glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
		auxSolidSphere(0.5); 
	glPopMatrix();

	/** 第三行第一列绘制的球有漫反射光和有颜色的环境光，而无镜面光。*/
	glPushMatrix();
		glTranslatef (-2.0, -1.75, -6.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
		glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
		auxSolidSphere(0.5);
	glPopMatrix();

	/** 第三行第二列绘制的球有漫反射光和有颜色的环境光以及镜面光，且有低高光。*/
	glPushMatrix();
		glTranslatef (-0.75, -1.75, -6.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
		auxSolidSphere(0.5);
	glPopMatrix();

	/** 第三行第三列绘制的球有漫反射光和有颜色的环境光以及镜面光，且有很亮的高光。*/
	glPushMatrix();
		glTranslatef (0.75, -1.75, -6.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
		auxSolidSphere(0.5);
	glPopMatrix();

	/** 第三行第四列绘制的球有漫反射光和有颜色的环境光以及辐射光，而无镜面光。*/
	glPushMatrix();
		glTranslatef (2, -1.75, -6.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
		glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
		auxSolidSphere(0.5);
	glPopMatrix();

   glFlush();													/**< 强制执行所有的OpenGL命令 */
}
