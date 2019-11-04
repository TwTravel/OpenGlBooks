//========================================================
/**
*  @file      Info.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  对象实体的信息
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
#include "Info.h"
#include "StaticObjMgr.h"
#include "Role.h"

CInfo ObjectInfo;


SActiveObjPro::SActiveObjPro():object_state(STAND),old_state(STAND),yrot(.0f),r1(4.f),r2(4.f),
				curr_frame(STAND_START),next_frame(STAND_START+1),t(0.f),t_add(.2f),bArrive(TRUE),
				orient(vector2d(1.0f,0.f)),dist_min(1000.f),dist_max(4000.f),m_bSelected(FALSE)
{
}

void SActiveObjPro::ChangeCurrFrame()
{
	switch(object_state)
	{
		case STAND:
			curr_frame = STAND_START;
			next_frame = curr_frame + 1;
			break;
		case WALK:
			curr_frame = WALK_START;
			next_frame = curr_frame + 1;
			break;
		case RUN:
			curr_frame = RUN_START;
			next_frame = curr_frame + 1;
			break;
		case ATTACK:
			curr_frame = ATTACK_START;
			next_frame = curr_frame + 1;
			break;
		case DEATH:
			curr_frame = DEATH1_START;
			next_frame = curr_frame + 1;
			break;
		case PAIN:
			curr_frame = PAIN1_START;
			next_frame = curr_frame + 1;
			break;
	}
	t = 0.0f;
}


void SActiveObjPro::Update()
{
	if((curr_life<=0) && (object_state != DEATH))
		SetObjectState(DEATH);

	if(object_state != DEATH && object_state != PAIN)
	{
		if(!bArrive)
		{
			curr_pos += orient * speed;
			
			if(start_pos.dist_sqr(curr_pos) >= start_pos.dist_sqr(end_pos))
			{
				curr_pos = end_pos;
				bArrive = TRUE;
			}
			for(unsigned int i=0;i<(CStaticObjMgr::GetSysStaticObjMgr()->GetObjNum());++i)
			{
				StaticObj* pObj=CStaticObjMgr::GetSysStaticObjMgr()->GetStaticObj(i);
				if(pObj->r1>0.f||pObj->r2>0.f)
					if( (fabsf(vPosition.x-pObj->GetPos().x)<fabsf(r1+2 * pObj->r1))
						&&(fabsf(vPosition.z-pObj->GetPos().z)<fabsf(r2+2 * pObj->r2)) )
					{
						bArrive = TRUE;
						continue;
						
					}
										
			}
			vPosition.x = curr_pos.x;
			vPosition.z = curr_pos.y;
		}
		else
		{
			if(object_state != STAND)
			{
				SetObjectState(STAND);
				ChangeCurrFrame();
				return;
			}
		}
	}
	vPosition.y = CTerrain::GetSysTerrain()->GetHeight(vPosition.x,vPosition.z);

	/////////////////////////////////
	if(t < 1.0f)
	{
		if(t + t_add < 1.0f)t += t_add;
		else t = 1.0f;
	}
	else
	{
		t = 0.0f;
		curr_frame = next_frame;
		++next_frame;
		switch(object_state)
		{
		case STAND:
			if(next_frame >= STAND_END)
				next_frame = STAND_START;
			break;
		case WALK:
			if(next_frame >= WALK_END)
				next_frame = WALK_START;
			break;
		case RUN:
			if(next_frame >= RUN_END)
				next_frame = RUN_START;
			break;
		case ATTACK:
			if(next_frame >= ATTACK_END)
				next_frame = ATTACK_START;
			break;
		case DEATH:
			if(next_frame >= DEATH1_END)
				next_frame -= 1;
			break;
		case PAIN:
			if(next_frame >= PAIN1_END)
				SetObjectState(old_state);
			break;
		}
	}
}

