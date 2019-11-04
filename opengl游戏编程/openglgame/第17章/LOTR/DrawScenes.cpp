
#include <windows.h>											
#include <math.h>			
#include <fstream.h>
#include <stdio.h>		
#include <stdarg.h>			
#include <gl\gl.h>												
#include <gl\glu.h>	
#include "mmsystem.h"
#include "GLFrame.h"
#include "linear.h"
#include "quaternions.h"
#include "graph.h"
#include "ASEloader.h"
#include "particles.h"

#pragma comment( lib, "opengl32.lib" )							
#pragma comment( lib, "glu32.lib" )								
#pragma comment( lib, "glaux.lib" )								
#pragma comment( lib, "ijl11.lib" )	
#pragma comment( lib, "winmm.lib" )							


#ifndef CDS_FULLSCREEN											
#define CDS_FULLSCREEN 4										
#endif														

GL_Window*	g_window;
Keys*		g_keys;

// 字体
GLuint	base;				
GLYPHMETRICSFLOAT gmf[256];	


int frames=0;
float time=0;
float fps;


#define PI 3.141592639

float rot=0;

// ASE模型
CModels ring;
CModels glamdring;
CModels finger;
CModels soldier;
CModels land;
GLint ring_DisplayList=0;
GLint glamdring_DisplayList=0;
GLint finger_DisplayList=0;
GLint soldier_DisplayList=0;
GLint land_DisplayList=0;


// 纹理
CTexture Textures;
#define FIRE_ANIMATION_WIDTH 256
#define FIRE_ANIMATION_HEIGHT 128

// 定义纹理名称
#define FIRE_TEXTURE 0
#define FIRE_FRONT_TEXTURE 1
#define FLAME_TEXTURE 2
#define GOLD_TEXTURE 3
#define TEXT_TEXTURE 4
#define TEXTGLOW_TEXTURE 5
#define GLAMDRING_TEXTURE 6
#define SILVER_TEXTURE 7
#define TITLE_TEXTURE 8
#define SOLDIER_TEXTURE 9
#define LAND_TEXTURE 10

// 粒子数组
#define FIRE_AMOUNT 100
particle fire_back[FIRE_AMOUNT];
particle fire_front[FIRE_AMOUNT];

#define SOLDIER_AMOUNT 1000
particle soldiers[SOLDIER_AMOUNT];


// 控制动画播放的变量
float ANIMC_time=0;
float ANIMC_firesize=0;
float ANIMC_spin1=1;
float ANIMC_spin2=1;
float ANIMC_text=0;
float ANIMC_glow=0;
float ANIMC_RingPos=-1;
int ANIMC_DisplayRing=0;
int ANIMC_DisplayGlamdring=1;
float ANIMC_titlefade=0;
int ANIMC_showtitle=0;
float ANIMC_blastscale=0;
float ANIMC_fall=0;
float ANIMC_land=1;

// 建立字体
GLvoid BuildFont(GLvoid)								
{
	HFONT	font;									
	base = glGenLists(256);								
	font = CreateFont(	-12,							// 字体高度
						0,								// 字体宽度
						0,								// 字体的旋转角度
						0,								// 字体底线的旋转角度
						FW_BOLD,						// 字体的重量
						FALSE,							// 是否使用斜体
						FALSE,							// 是否使用下划线
						FALSE,							// 是否使用删除线
						ANSI_CHARSET,					// 设置字符集
						OUT_TT_PRECIS,					// 输出精度
						CLIP_DEFAULT_PRECIS,			// 裁剪精度
						ANTIALIASED_QUALITY,			// 输出质量
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Arial");				        // 字体名称

	SelectObject(g_window->hDC, font);							

	wglUseFontOutlines(	g_window->hDC,						
						0,								
						255,							
						base,							
						10.0f,						
						0.01f,						
						WGL_FONT_POLYGONS,			
						gmf);							
}

GLvoid KillFont(GLvoid)									
{
  glDeleteLists(base, 256);							
}

GLvoid glPrint(const char *fmt, ...)					
{
	float		length=0;								
	char		text[256];								
	va_list		ap;										

	if (fmt == NULL)									
		return;											

	va_start(ap, fmt);									
	    vsprintf(text, fmt, ap);						
	va_end(ap);											

	for (unsigned int loop=0;loop<(strlen(text));loop++)	
	{
		length+=gmf[text[loop]].gmfCellIncX;			
	}

	glPushMatrix();
		glTranslatef(-length/2,0.0f,0.0f);					

		glPushAttrib(GL_LIST_BIT);							
		glListBase(base);									
		glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	
		glPopAttrib();										
	glPopMatrix();
}

// 初始化
BOOL Initialize (GL_Window* window, Keys* keys)					
{
	g_window	= window;
	g_keys		= keys;
	ScreenW=g_window->init.width;
	ScreenH=g_window->init.height;
	BuildFont();

	//载入ASE模型，并创建显示列表
	ring.Load("Models\\ring.ase");
	glamdring.Load("Models\\glamdring.ase");
	finger.Load("Models\\finger.ase");
	soldier.Load("Models\\soldier.ase");
	land.Load("Models\\land.ase");

	ring_DisplayList=glGenLists(1);
	glNewList(ring_DisplayList,GL_COMPILE);
	ring.Display();
	glEndList();

	glamdring_DisplayList=glGenLists(1);
	glNewList(glamdring_DisplayList,GL_COMPILE);
	glamdring.Display();
	glEndList();

	finger_DisplayList=glGenLists(1);
	glNewList(finger_DisplayList,GL_COMPILE);
	finger.Display();
	glEndList();

	soldier_DisplayList=glGenLists(1);
	glNewList(soldier_DisplayList,GL_COMPILE);
	soldier.Display();
	glEndList();

	land_DisplayList=glGenLists(1);
	glNewList(land_DisplayList,GL_COMPILE);
	land.Display();
	glEndList();

	//载入纹理
	Textures.Init(11); 
	Textures.Load("textures\\blank.jpg",FIRE_TEXTURE,0); 
	Textures.Load("textures\\blank.jpg",FIRE_FRONT_TEXTURE,0); 
	Textures.Load("textures\\flame.jpg",FLAME_TEXTURE,0); 
	Textures.Load("textures\\gold.jpg",GOLD_TEXTURE,0); 
	Textures.Load("textures\\onering.jpg",TEXT_TEXTURE,0); 
	Textures.Load("textures\\blank640.jpg",TEXTGLOW_TEXTURE,0);
	Textures.Load("textures\\glamdring_tex.jpg",GLAMDRING_TEXTURE,0);
	Textures.Load("textures\\silver.jpg",SILVER_TEXTURE,0); 
	Textures.Load("textures\\title.jpg",TITLE_TEXTURE,0); 
	Textures.Load("textures\\soldier.jpg",SOLDIER_TEXTURE,0); 
	Textures.Load("textures\\mountain.jpg",LAND_TEXTURE,0); 

    // 初始化士兵属性
	for(int i=0;i<SOLDIER_AMOUNT;i++)
	{
		float r=random(3,150,0);
		float theta=random(0,2*3.1415926539,0);
		soldiers[i].pos.set(r*cos(theta),0,r*sin(theta));
		soldiers[i].size=10;
		soldiers[i].r=r;
		soldiers[i].theta=theta;
		soldiers[i].time=1;
	}

	// OpenGL初始化设置
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);

	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);						
	glClearDepth (1.0f);										
	glDepthFunc (GL_LEQUAL);								
	glEnable (GL_DEPTH_TEST);

	GLfloat spec[]={1.0f,1.0f,1.0f,1.0f};
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMateriali(GL_FRONT, GL_SHININESS, 100);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_GEN_S);						
	glEnable(GL_TEXTURE_GEN_T);						

	PlaySound("BGM.wav",NULL,SND_ASYNC);

	return TRUE;												
}

// 卸载工作
void Deinitialize (void)										
{

	KillFont();
	glDeleteLists(ring_DisplayList,1);
	glDeleteLists(glamdring_DisplayList,1);
	glDeleteLists(finger_DisplayList,1);
	glDeleteLists(soldier_DisplayList,1);
	glDeleteLists(land_DisplayList,1);
	Textures.UnLoad(); 

}


// 绘制火焰
void DrawFire(particle* fires)
{
	for(int i=0;i<FIRE_AMOUNT;i++)
	{
		glPushMatrix();							
			fires[i].Display();
		glPopMatrix();
	}
}

// 移动火焰
void MoveFire(particle *fires,float time)
{
	for(int i=0;i<FIRE_AMOUNT;i++)
	{
		fires[i].Move(time);
		if(fires[i].time<=0&&ANIMC_firesize>0) 
		{
			fires[i].pos.set(random(0,FIRE_ANIMATION_WIDTH,0),FIRE_ANIMATION_HEIGHT+10,0);
			fires[i].dir.set(random(0,10,1),random(-5,-50,0),0);
			fires[i].time=random(0.1,ANIMC_firesize,0);
			fires[i].size=random(25,50,0);
			fires[i].roll=random(0,360,1);
		}
	}
}

// 动画播放时间序列
void AnimationSequence(float timet)
{
	if(ANIMC_time>=155) TerminateApplication (g_window);

	if(ANIMC_time>130)
	{
		ANIMC_showtitle=1;
		ANIMC_titlefade+=0.1*timet;
		 if(ANIMC_titlefade>=1) ANIMC_titlefade=1;
	}

	if(ANIMC_time>=110)
	{
		ANIMC_RingPos+=0.05*timet;
	}
	else if(ANIMC_time>=34)
	{
		ANIMC_DisplayGlamdring=0;
		ANIMC_DisplayRing=1;
		ANIMC_RingPos+=0.05*timet;
		if(ANIMC_RingPos>=0) ANIMC_RingPos=0;
	}

	if(ANIMC_time>=96)
	{
		 ANIMC_spin1=1;
	}
	if(ANIMC_time>=84)
	{
	//	 ANIMC_glow+=0.1*timet;
	//	 if(ANIMC_glow>=0.5) 
		ANIMC_glow=0.5;
	}
	else if(ANIMC_time>=72)
	{
		 ANIMC_spin1=0;
		
		 ANIMC_text+=0.1*timet;
		 if(ANIMC_text>=1) ANIMC_text=1;
	}
	
	if(ANIMC_time>=124)
	{
		ANIMC_firesize-=0.2*timet;
		if(ANIMC_firesize<=0) ANIMC_firesize=0;
	}
	else if(ANIMC_time>=60)
	{
		ANIMC_firesize-=0.4*timet;
		 if(ANIMC_firesize<=1) ANIMC_firesize=1;
	}
	else if(ANIMC_time>=30)
	{
		ANIMC_firesize=2;
	}
	else if(ANIMC_time>=28)
	{
		ANIMC_firesize=4;
	}
	else if(ANIMC_time>=15)
	{
		ANIMC_firesize=2;
		ANIMC_land=0;
		
	}
	else if(ANIMC_time>=10)
	{
		ANIMC_firesize=4;
		ANIMC_land-=0.2*timet;
	}
	
	if(ANIMC_time>=5)
	{
		ANIMC_blastscale+=.2*timet;
		ANIMC_fall+=20*timet;
	}

	if(ANIMC_time>=125)
		ANIMC_glow=0;
	
}

// 更新
void Update (DWORD milliseconds)								
{
	if (g_keys->keyDown [VK_ESCAPE] == TRUE)					
	{
		TerminateApplication (g_window);						
	}

	if (g_keys->keyDown [VK_F1] == TRUE)						
	{
	//	ToggleFullscreen (g_window);							
	}

	float ms=milliseconds*0.001;
	rot+=ms;

	MoveFire(fire_back,ms);
	MoveFire(fire_front,ms);

	time+=milliseconds;

	ANIMC_time+=ms;

	AnimationSequence(ms);

}

void ViewOrtho(int w,int h)							
{
	glMatrixMode(GL_PROJECTION);					
	glPushMatrix();							
	glLoadIdentity();						
	glOrtho( 0, w , h , 0, -1, 1 );				
	glMatrixMode(GL_MODELVIEW);					
	glPushMatrix();							
	glLoadIdentity();						
}

void ViewPerspective()							
{
	glMatrixMode( GL_PROJECTION );				
	glPopMatrix();							
	glMatrixMode( GL_MODELVIEW );			
	glPopMatrix();							
}

// 主要的绘制函数
void Draw (void)
{
	
   //FPS
	if(time>1000)
	{
		fps=(frames*1000.0)/time;
		time=0;
		frames=0;
	}
	else
	{
		frames++;
	}
//	glClearColor (1.0f, 1.0f, 1.0f, 0.5f);	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glColor3f(1,1,1);
	glLoadIdentity ();
	


		glPushMatrix();
			
			glEnable(GL_TEXTURE_2D);
			glEnable (GL_DEPTH_TEST);	

			// 渲染火焰层并拷贝到纹理
			
			glViewport (0, 0,FIRE_ANIMATION_WIDTH, FIRE_ANIMATION_HEIGHT);				
			
			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_GEN_S);						
			glDisable(GL_TEXTURE_GEN_T);	

			ViewOrtho(FIRE_ANIMATION_WIDTH,FIRE_ANIMATION_HEIGHT);
			
			glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			glColor3f(1,1,1);
			Textures.Bind(FLAME_TEXTURE);
			DrawFire(fire_back);

			Textures.Bind(FIRE_TEXTURE);
			glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,FIRE_ANIMATION_WIDTH,FIRE_ANIMATION_HEIGHT);

			glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		

			Textures.Bind(FLAME_TEXTURE);
			DrawFire(fire_front);

			Textures.Bind(FIRE_FRONT_TEXTURE);
			glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,FIRE_ANIMATION_WIDTH,FIRE_ANIMATION_HEIGHT);

			glDisable(GL_BLEND);
			ViewPerspective();
			
			// 重置视口
			glViewport (0, 0, 512, 512);				
			glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			
			glPushMatrix();
				glTranslatef(0,0,-300);
				glRotatef(-90,1,0,0);
				glRotatef(ANIMC_spin1*(rot*15),1,0.5,0);
				glRotatef(-ANIMC_spin2*(rot*15),0,0,1);

				glColor4f(1,0,0,1);
				Textures.Bind(TEXT_TEXTURE);
				glCallList(ring_DisplayList);

				Textures.Bind(TEXTGLOW_TEXTURE);
				glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,512,512);
		
			glPopMatrix();
            
			// 重置视口
			glViewport (0, 0, ScreenW, ScreenH);				
			glClearColor (0.0f, 0.0f, 0.0f, 0.5f);	
			glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			Textures.Bind(FIRE_TEXTURE);
			glColor3f(1,1,1);
			glBegin(GL_QUADS);
				glTexCoord2f(0,1);
				glVertex3f(-400,200,-480);
				glTexCoord2f(1,1);
				glVertex3f(400,200,-480);
				glTexCoord2f(1,0);
				glVertex3f(400,-200,-480);
				glTexCoord2f(0,0);
				glVertex3f(-400,-200,-480);		
			glEnd();
			
			// 开始主要的渲染
			GLfloat LightAmbient[]    = { 0.3f, 0.3f, 0.31f, 1.0f };
			GLfloat LightDiffuse[]    = { 1.0f, 1.0f, 1.0f, 1.0f };
		    //GLfloat LightSpecular[]    = { 0.0f, 0.0f, 0.0f, 0.1f };
			GLfloat LightPosition[]    = { 75.0f, 75.0f, 75.0f, 0.0f };

			glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
			glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
			//glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
			glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
			
			//glEnable(GL_LIGHTING);
			glEnable(GL_LIGHTING);	
			glEnable(GL_LIGHT0);
			
			if(ANIMC_land>0)
			{
				if(ANIMC_land<1&&ANIMC_land>=0)
					glEnable(GL_BLEND);

				// 渲染士兵
				glPushMatrix();
				glPushAttrib(GL_ENABLE_BIT);
					Textures.Bind(SOLDIER_TEXTURE);
					glColor4f(ANIMC_land,ANIMC_land,ANIMC_land,ANIMC_land);
					glTranslatef(0,0,-150);
					glRotatef(30,1,0,0);
					
					for(int sold=0;sold<SOLDIER_AMOUNT;sold++)
					{
						glPushMatrix();
							if(ANIMC_fall>soldiers[sold].r&&soldiers[sold].fall<1)
							{
								soldiers[sold].fall+=0.1;
							}
							glTranslatef(soldiers[sold].pos.x,soldiers[sold].pos.y,soldiers[sold].pos.z);
							glRotatef(90-soldiers[sold].theta/3.1415926539*180.0,0,1,0);
							glRotatef(soldiers[sold].fall*90,1,0,0);
							glCallList(soldier_DisplayList);

						glPopMatrix();
					}
					
					
					Textures.Bind(LAND_TEXTURE);
					glCallList(land_DisplayList);

					glEnable(GL_BLEND);
					glDisable(GL_LIGHTING);
					glDisable(GL_DEPTH_TEST);
				    //glBlendFunc(GL_ONE_MINUS_SRC_COLOR,GL_ONE_MINUS_DST_COLOR);
					glColor3f(ANIMC_land*0.5,ANIMC_land*0.5,ANIMC_land*0.5);
					glRotatef(90,1,0,0);
					glScalef(ANIMC_blastscale,ANIMC_blastscale,0.25);
					Textures.Bind(LAND_TEXTURE);
					glCallList(ring_DisplayList);

				glPopAttrib();
				glPopMatrix();
			    //glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			}

			if(ANIMC_DisplayGlamdring)
			{
				//渲染圣剑
				glPushMatrix();
				glPushAttrib(GL_ENABLE_BIT);
					glTranslatef(0,75-rot*3,-25);
					glRotatef(-90,1,0,0);
					glRotatef(180+rot*5,0,1,0);
					glRotatef((rot*45),0,0,1);
					
			 /*		glDisable(GL_TEXTURE_2D);
					glColor4f(0,0,0,1);
					glCallList(glamdring_DisplayList);*/
					
					
					glEnable(GL_TEXTURE_2D);
					glDisable(GL_LIGHTING);			

					glColor4f(0.5,0.5,0.5,1);
					Textures.Bind(GLAMDRING_TEXTURE);
					glCallList(glamdring_DisplayList);

					glEnable(GL_BLEND);
					glEnable(GL_TEXTURE_GEN_S);	
					glEnable(GL_TEXTURE_GEN_T);	

					glColor4f(.5,.5,.5,.7);
					Textures.Bind(SILVER_TEXTURE);
					glCallList(glamdring_DisplayList);

					glColor4f(1,1,1,0.7);
					Textures.Bind(FIRE_FRONT_TEXTURE);
					glCallList(glamdring_DisplayList);
				
				glPopAttrib();
				glPopMatrix();

				// 绘制四个掉落的手指
				glPushAttrib(GL_ENABLE_BIT);
				
					glEnable(GL_TEXTURE_2D);
					glEnable(GL_LIGHTING);

				glPushMatrix();
					glTranslatef(rot*0.25,75-rot*3.5,-30);
					glRotatef(-90,1,0,0);
					glRotatef(180+rot*35,0,1,0);
					glRotatef((rot*45),0,0,1);				

					glColor4f(1,1,1,1);
					Textures.Bind(SILVER_TEXTURE);
					glCallList(finger_DisplayList);
				glPopMatrix();

				glPushMatrix();
					glTranslatef(-rot*0.25,75-rot*3.2,-25);
					glRotatef(-90,1,0,0);
					glRotatef(-rot*35,0,1,0);
					glRotatef((rot*25),0,0,1);				

					glColor4f(1,1,1,1);
					Textures.Bind(SILVER_TEXTURE);
					glCallList(finger_DisplayList);
				glPopMatrix();

				glPushMatrix();
					glTranslatef(rot*0.15,75-rot*3.8,-40);
					glRotatef(-90,1,0,0);
					glRotatef(-rot*15,0,1,0);
					glRotatef((-rot*55),0,0,1);				

					glColor4f(1,1,1,1);
					Textures.Bind(SILVER_TEXTURE);
					glCallList(finger_DisplayList);
				glPopMatrix();
				
				glPushMatrix();
					glTranslatef(rot*0.25,75-rot*3,-30);
					glRotatef(-90,1,0,0);
					glRotatef(rot*65,0,1,0);
					glRotatef((rot*25),0,0,1);				

					glColor4f(1,1,1,1);
					Textures.Bind(SILVER_TEXTURE);
					glCallList(finger_DisplayList);
				glPopMatrix();

				glPopAttrib();
			}

			if(ANIMC_DisplayRing)
			{
				// 绘制指环
				glPushMatrix();
					glTranslatef(0,ANIMC_RingPos*200,-300);
					glRotatef(-90,1,0,0);
					glRotatef(ANIMC_spin1*(rot*15),1,0.5,0);
					glRotatef(-ANIMC_spin2*(rot*15),0,0,1);
					
					// 高光层
					glDisable(GL_TEXTURE_2D);
					glColor4f(0,0,0,1);
					glCallList(ring_DisplayList);
					
					glEnable(GL_BLEND);
					glEnable(GL_TEXTURE_2D);
					glDisable(GL_LIGHTING);			

					// 文字层
					glColor4f(1,0,0,ANIMC_text);
					Textures.Bind(TEXT_TEXTURE);
					glCallList(ring_DisplayList);

					glEnable(GL_TEXTURE_GEN_S);	
					glEnable(GL_TEXTURE_GEN_T);	

					// 反射层
					glColor4f(1,1,1,.7);
					Textures.Bind(GOLD_TEXTURE);
					glCallList(ring_DisplayList);

					// 火焰反射层
					glColor4f(1,1,1,0.7);
					Textures.Bind(FIRE_FRONT_TEXTURE);
					glCallList(ring_DisplayList);

				glPopMatrix();
			}

		   // 前背景
			glDisable(GL_TEXTURE_GEN_S);	
			glDisable(GL_TEXTURE_GEN_T);	
			glDisable(GL_LIGHTING);	
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);

			//绘制前层火焰
			Textures.Bind(FIRE_FRONT_TEXTURE);
			glColor4f(1,1,1,1);
			glBegin(GL_QUADS);
				glTexCoord2f(0,1);
				glVertex3f(-400,200,-480);
				glTexCoord2f(1,1);
				glVertex3f(400,200,-480);
				glTexCoord2f(1,0);
				glVertex3f(400,-200,-480);
				glTexCoord2f(0,0);
				glVertex3f(-400,-200,-480);		
			glEnd();

			ViewOrtho(ScreenW,ScreenH);
		//	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
			Textures.Bind(TEXTGLOW_TEXTURE);
			glColor4f(1,1,1,1);
			
			for(int z=1;z<2 && ANIMC_glow!=0;z++)
			{
				float zoom=ANIMC_glow/z;
				glColor4f(1,1,1,1-zoom);
				glBegin(GL_QUADS);
					glTexCoord2f(0+zoom,1-zoom);
					glVertex3f(0,0,0);
					glTexCoord2f(1-zoom,1-zoom);
					glVertex3f(ScreenW,0,0);
					glTexCoord2f(1-zoom,0+zoom);
					glVertex3f(ScreenW,ScreenH,0);
					glTexCoord2f(0+zoom,0+zoom);
					glVertex3f(0,ScreenH,0);		
				glEnd();
			}

			if(ANIMC_showtitle)
			{
				Textures.Bind(TITLE_TEXTURE);
				glColor4f(1,1,1,ANIMC_titlefade);
				glBegin(GL_QUADS);
					glTexCoord2f(0,1);
					glVertex3f(0,0,0);
					glTexCoord2f(1,1);
					glVertex3f(ScreenW,0,0);
					glTexCoord2f(1,0);
					glVertex3f(ScreenW,ScreenH,0);
					glTexCoord2f(0,0);
					glVertex3f(0,ScreenH,0);		
				glEnd();
			}

			ViewPerspective();
		glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glColor4f(.6,.6,.6,1);
	glTranslatef(-10.5,9,-25);
	glPrint("FPS: %.2f",fps);

	glFlush ();													
}





