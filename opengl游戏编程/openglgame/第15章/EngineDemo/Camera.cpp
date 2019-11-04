//========================================================
/**
*  @file      Camera.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  摄像机类
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
#include "Camera.h"
#include "mathlib.h"
#include "role.h"


//! 正在使用的摄像机的指针
CCamera* CCamera::m_pCamera = NULL;


CCamera::CCamera(void):direction(0.0f),pitch(21.f),
				dist_to_role(80.0f),role_pos(vector3d(2.0f,0.4f,-0.5f))
{
	m_pCamera = this;
}



/**\brief
 * 更新摄像的参数，把它放到主循环中
 */
void CCamera::Update(void)
{
	if(CInput::GetSysInput()->GetMouse()->GetWheelState()>0)
	{
		pitch += 1.0f;
		//if(pitch>90.0f)	pitch=90.0f;
	}
	if(CInput::GetSysInput()->GetMouse()->GetWheelState()<0)
	{
		pitch -= 1.0f;
		//if(pitch<0.f)pitch=0.f;
	}

	SPoint2 p=SYS_MOUSE->GetMousePos();
	if(p.x <=5)
	{
		if(CRole::GetRolePointer()->IsArrive())direction+=1.0f;
	}

	if(p.x >=SYS_WIDTH-5)
	{
		if (CRole::GetRolePointer()->IsArrive())direction-=1.0f;
	}
	if(p.y>SYS_HEIGHT-5)
	{
		dist_to_role += 0.5f ;
		//if(dist_to_role>120.0f)dist_to_role=120.0f;
	}
	if(p.y<5)
	{
		dist_to_role -= 0.5f;
		//if(dist_to_role<80.0f)dist_to_role=80.0f;
	}

	role_pos=CRole::GetRolePointer()->GetPos();
	position = role_pos ;
	position.z += dist_to_role;
	position.y = role_pos.y+15.0f;

	FrameMove();
}


/**\brief
 * 设置变幻矩阵
 */
void CCamera::FrameMove(void)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f,0.0f,-dist_to_role);
	glRotatef(pitch,1.0f,0.0f,0.f);
	glRotatef(direction,0.0f,1.0f,0.0f);

    glTranslatef(0.0f,0.0f,dist_to_role);

	glTranslatef(-position.x,-position.y,-position.z);
}

