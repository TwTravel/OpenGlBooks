//========================================================
/**
*  @file      BillBoard.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  公告板类
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
#include "BillBoard.h"


void CBillBoard::Update()
{
	float m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX , m);
	right.set(m[0],m[4],m[8]);
	up.set(m[1],m[5],m[9]);
}

void CBillBoard::Render(vector3d vpos , float m_iSize)
{
	glPushMatrix();
	glBegin(GL_TRIANGLE_STRIP);
	  glTexCoord2f(0.0f , 1.0f); 
	  glVertex3fv((vpos + (up - right)*(m_iSize) ).v);
	  glTexCoord2f(0.0f , 0.0f); 
	  glVertex3fv((vpos + (right + up)*(-m_iSize) ).v);
      glTexCoord2f(1.0f , 1.0f); 
	  glVertex3fv((vpos + (right + up)*(m_iSize) ).v);
	  glTexCoord2f(1.0f , 0.0f);
	  glVertex3fv((vpos + (right - up)*(m_iSize) ).v);
	glEnd();
	glPopMatrix();
}

