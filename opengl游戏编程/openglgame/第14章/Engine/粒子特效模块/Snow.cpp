//========================================================
/**
*  @file      Snow.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  雪花渲染类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#include "stdafx.h"
#include "time.h"
#include "Snow.h"


BOOL CSnow::Init()
{
	m_width = 200;
	m_height = 100;
	m_depth = 200;
	if(CParticle::Init(500))
	{
		for(int loop = 0 ; loop < m_iMaxNum ; ++loop)
		{
			ResetPaticle(&pList_particle[loop]);
		}
		if(snow_texture.InitTexture("data/snow.bmp" , TRUE))
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
	
}



void CSnow::Render()
{
	//glDisable(GL_FOG);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	billboard.Update();
	snow_texture.Bind(0);
	glColor3f(1.f,1.f,1.f);
	for(int i = 0 ; i < m_iMaxNum ; ++i)
	{
		billboard.Render(pList_particle[i].m_pos , pList_particle[i].m_size);
	}
	for( i = 0 ; i < m_iMaxNum; ++i)
	{
		pList_particle[i].m_pos += pList_particle[i].m_velocity;
		pList_particle[i].m_velocity += pList_particle[i].m_gravity;
		if(pList_particle[i].m_pos.v[1] < -50.0f)
			ResetPaticle(&pList_particle[i]);
	}
	glDisable(GL_TEXTURE_2D);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_FOG);
}

#define FAND(x) ((float)((rand() - rand()) % (x)))


void CSnow::ResetPaticle(PARTICLE *particle)
{
	float x , y , z;
	vCameraPos=CRole::GetRolePointer()->GetPos();
	x = vCameraPos.x +FAND(30);
	y = vCameraPos.y +80.0f;
	z = vCameraPos.z - 80.0f + FAND(100)/10;
	particle->m_pos = vector3d(x , y , z);
	particle->m_velocity = vector3d(0.0f , 0.0f ,0.0f);
	particle->m_size = 0.2f;
	x = ((float)FAND(10)) / 1000.0f;
	y= ((float)(rand() % 50)) / 5000;
	if(y>0)y=-y;
	if(y==0) y =-0.001f;
	z = ((float)FAND(10)) / 1000.0f;
	particle->m_gravity  = vector3d(x , y , z);
}

