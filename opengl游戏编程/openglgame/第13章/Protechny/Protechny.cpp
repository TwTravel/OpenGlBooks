//======================================================================
/**
*  @file      Protechny.cpp
*
*  项目描述： 粒子系统
*  文件描述:  喷泉类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-12-18	
*  修改日期： 2006-12-19
*	
*/																			
//======================================================================

#include "stdafx.h"
#include "Protechny.h"

/** 喷泉粒子的初始化 */
bool CProtechny::Init(int num)
{
	/** 初始化粒子数目为num个 */
	if(CParticle::Init(num))
	{
		for(int i=0; i<m_iNum; i++)
		{
			/** 初始化位置 */
			float x,y,z,vx,vy,vz;

			x = 0.005f * (rand()%9);
		    y = 0.005f * (rand()%9)-1;
		    z = 0.005f * (rand()%6);
			m_pList[i].position = Vector3(x,y,z);

			/** 初始化速度 */
			vx = 0.0000007f * (rand()%9000-rand()%9000);
		    vy = 0.0000042f * (rand()%9000);
		    vz = 0.0000001f * (rand()%9000);
			m_pList[i].velocity = Vector3(vx,vy,vz);

			/** 初始化加速度 */
			m_pList[i].acceleration = Vector3(0.0,-0.00025f,0.0);

			/** 初始化粒子生命时间 */
			m_pList[i].lifetime = 100;
			
			/** 初始化粒子的尺寸 */
			m_pList[i].size = 0.01f;

			/** 初始化粒子的消失速度 */
			m_pList[i].dec = 0.05 * (rand()%50);

			/** 初始化粒子的颜色 */
			m_pList[i].color[0] = 255.0f;
			m_pList[i].color[1] = 255.0f;
			m_pList[i].color[2] = 255.0f;

		}
		
		/** 载入粒子纹理 */
		if(!m_texture.Load("Data/flare.bmp"))
		{
			MessageBox(NULL,"载入粒子纹理失败!","错误",MB_OK);
			return false;
		}
		else
			return true;
	}
	else
		return false;
}

/** 喷泉粒子的渲染 */
void CProtechny::Render()
{
	/** 绑定纹理 */
	glBindTexture(GL_TEXTURE_2D,m_texture.ID);
	
	/** 开始渲染喷泉 */
	for(int i=0; i<m_iNum; ++i)
	{
		float x = m_pList[i].position.x;
		float y = m_pList[i].position.y;
		float z = m_pList[i].position.z;
		float size = m_pList[i].size;

		glColor3fv(m_pList[i].color);
		glNormal3f(0.0f,0.0f,1.0f);     /**< 定义法线方向 */
		
		/** 画出粒子 */
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f,0.0f);glVertex3f(x-size,y-size,z);
			glTexCoord2f(1.0f,0.0f);glVertex3f(x-size,y+size,z);
			glTexCoord2f(1.0f,1.0f);glVertex3f(x+size,y+size,z);
			glTexCoord2f(0.0f,1.0f);glVertex3f(x+size,y-size,z);
		glEnd();
				
	}
	
	/** 更新粒子属性 */
	Update();
}

/** 喷泉粒子的更新 */
void CProtechny::Update()
{
	for(int i = 0; i<m_iNum; ++i)
	{
		/** 更新位置 */
		m_pList[i].position.x += m_pList[i].velocity.x;
		m_pList[i].position.y += m_pList[i].velocity.y;
		m_pList[i].position.z += m_pList[i].velocity.z;

		/** 更新速度 */
		m_pList[i].velocity.y += m_pList[i].acceleration.y;

		/** 更新生存时间 */
		m_pList[i].lifetime -= m_pList[i].dec;

		/** 如果粒子消失或生命结束 */
		if (m_pList[i].position.y <= -1 || m_pList[i].lifetime <= 0)
		{
			/** 初始化位置 */
			float x,y,z,vx,vy,vz;
			x = 0.005f * (rand()%9);
		    y = 0.005f * (rand()%9)-1;
		    z = 0.000001f * (rand()%9000);
			m_pList[i].position = Vector3(x,y,z);

			/** 初始化速度 */
			vx = 0.0000007f * (rand()%9000-rand()%9000);
			vy = 0.0000042f * (rand()%9000);
			vz = 0.0000001f * (rand()%90000);
			m_pList[i].velocity = Vector3(vx,vy,vz);

			/** 初始化加速度 */
			m_pList[i].acceleration = Vector3(0.0,-0.00025f,0.0);

			/** 初始化生命时间 */
			m_pList[i].lifetime = 100;

			/** 初始化消失速度 */
			m_pList[i].dec = 0.05*(rand()%50);
		}

	}
	
}

