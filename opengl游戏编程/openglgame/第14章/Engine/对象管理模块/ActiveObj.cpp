//========================================================
/**
*  @file      ActiveObj.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  游戏中的活动实体
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
#include "ActiveObj.h"
#include "inifile.h"


void CActiveObj::SetObjectState(OBJ_STATE state)
{
	if(property->object_state != state)
	{
		property->old_state = property->object_state;
		property->object_state = state;
		property->ChangeCurrFrame();
	}
}


void CActiveObj::OnRender(void)
{
	glPushMatrix();
	property->vPosition=m_sPos;
	glTranslatef(property->vPosition.x , property->vPosition.y , property->vPosition.z);
	glRotatef(property->yrot,0.0f,1.0f,0.0f);
    
	if(property->shadow)
	{
		float m[16];
		float lp[4]={100.0f,100.0f,0.0f,0.0f};
		glPushMatrix();
		glLoadIdentity();
		glRotatef(-property->yrot,0.0f,1.0f,0.0f);
		glGetFloatv(GL_MODELVIEW_MATRIX,m);
		vMatMult(m,lp);
		glTranslatef(-property->vPosition.x,-property->vPosition.y,-property->vPosition.z);
		glPopMatrix();
		md2.CastShadow(lp,property->curr_frame);
	}

	if(property->m_bSelected)
	{
		glColor3f(1.f,0.f,0.f);
		glBegin(GL_LINE_LOOP);
			glVertex3f(-property->r1,1,-property->r2);
			glVertex3f(-property->r1,1, property->r2);
			glVertex3f( property->r1,1, property->r2);
			glVertex3f( property->r1,1,-property->r2);
		glEnd();
	}
	glColor4f(property->m_sColor.r,property->m_sColor.g,property->m_sColor.b,property->m_sColor.a);
	md2.RenderFrame(property->curr_frame,property->next_frame,property->t);
	glPopMatrix();	
	
	EndRender();
}



BOOL CActiveObj::Init(char *filename,char*texfilename,float scale,float h)
{
	if(!md2.Init(filename,texfilename,scale,h))
		return FALSE;

	return TRUE;
}


void CActiveObj::vMatMult(float M[], float v[])
{
    float res[4];										
	res[0]=M[ 0]*v[0]+M[ 4]*v[1]+M[ 8]*v[2]+M[12]*v[3];
	res[1]=M[ 1]*v[0]+M[ 5]*v[1]+M[ 9]*v[2]+M[13]*v[3];
	res[2]=M[ 2]*v[0]+M[ 6]*v[1]+M[10]*v[2]+M[14]*v[3];
	res[3]=M[ 3]*v[0]+M[ 7]*v[1]+M[11]*v[2]+M[15]*v[3];
	v[0]=res[0];									
	v[1]=res[1];
	v[2]=res[2];
	v[3]=res[3];
}

