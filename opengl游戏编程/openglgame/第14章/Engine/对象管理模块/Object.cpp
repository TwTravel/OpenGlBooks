//========================================================
/**
*  @file      Object.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  对象实体类的基类
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
#include "object.h"
#include "role.h"

Object::Object(void):m_sPos(vector3d(0.f,0.f,0.f)),r1(24.f),r2(20.f)
{
}


Object::~Object(void)
{
}


void Object::Render()
{
	vector3d v=CRole::GetRolePointer()->GetPos();
	if((fabsf(m_sPos.x-v.x)<450)&&(fabsf(m_sPos.z-v.z)<450) )
	{
		OnRender();
	}
}