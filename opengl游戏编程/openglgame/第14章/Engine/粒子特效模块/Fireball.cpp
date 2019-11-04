//========================================================
/**
*  @file      FireBall.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  爆炸火花类
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
#include "Fireball.h"


CFireball::CFireball():bRun(FALSE),speed(0.6f),canReduce(FALSE),isArrive(TRUE)
{
	i = 0;
	r= 1.0f;
	g = 1.0f;
	b = 1.0f;
	vpos.x = 0.0f;
	vpos.y = 0.0f;
	vpos.z = 0.0f;
}


BOOL CFireball::Init(char *filename , float size)
{
	if(!texture.InitTexture(filename , TRUE))
		return FALSE;

	m_iSize = size;
	return TRUE;
}


void CFireball::Set(vector3d start, vector3d end)
{
	start_pos = start;
	start_pos.y += 4.0f;
	end_pos = end;
	vpos = start_pos;
	orient = end_pos - start_pos;
	if(orient.mag_sqr() <= MAX_LENGTH * MAX_LENGTH)
		bRun = TRUE;
	orient.normalize();
}


void CFireball::Render()
{
	glDisable(GL_FOG);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0f);
	if(bRun)
	{
		texture.Bind(0);
		glColor3f(r,g,b);
		billboard.Update();
		billboard.Render(vpos,m_iSize);
		if(!isArrive)
		{
			bRun = FALSE;
			isArrive = TRUE;
			canReduce = FALSE;
			goto close;
		}
		orient = end_pos - vpos;
		orient.normalize();
		vpos += orient * speed ;
		if(vpos.dist_sqr(end_pos) <= 4.0f)
		{
			isArrive = FALSE;
			canReduce = TRUE;
			vpos = end_pos;
		}
	}

close:	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_FOG);
}
