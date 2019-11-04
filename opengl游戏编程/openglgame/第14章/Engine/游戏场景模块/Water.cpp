//========================================================
/**
*  @file      Water.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  水的渲染
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
#include "Water.h"


CWater::CWater()
{
	delta = 0.0f;
}

CWater::~CWater()
{
}


BOOL CWater::Init()
{
	if(!texture.InitTexture("data/water.tga",FALSE,TRUE))
		return FALSE;
	return TRUE;
}


void CWater::Render()
{
	//glDisable(GL_FOG);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	texture.Bind(0);
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f+delta,0.0f+delta);glVertex3f(-100.0f, -1.5f , -100.0f);
	glTexCoord2f(0.0f+delta,1.0f+delta);glVertex3f(-100.0f, -1.5f , 613.0f);
	glTexCoord2f(1.0f+delta,1.0f+delta);glVertex3f(613.0f , -1.5f , 613.0f);
	glTexCoord2f(0.0f+delta,1.0f+delta);glVertex3f(613.0f , -1.5f , -100.0f);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_FOG);

	delta += 0.001f;
	if(delta >= 1.0f)delta -= 1.0f;
}
