//========================================================
/**
*  @file      Role.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  游戏中的英雄
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
#include "Role.h"
#include <fstream>
#include <string>
#include "IniFile.h"
#include "timer.h"
using namespace std;

CRole* CRole::m_pRole = NULL;


CRole::CRole(void):bRightMouseDown(TRUE),pick_state(EARTH),
		bPick(FALSE),m_pMonster(NULL),energy(1.f),stamina(1.f)
{
	m_pRole = this;
	property = new SActiveObjPro;
}


BOOL CRole::Init(void)
{
	LoadIni("ini\\role.ini","role");

	if(!fireball.Init("data/star.bmp" , 4.0f))return FALSE;
	fireball.SetColor(1.0f , 1.0f , 0.0f);
	fireball.SetSpeed(property->speed * 3);

	return TRUE;
}


BOOL CRole::LoadIni(char* _filename,char* _index)
{
	CIniFile Ini(_filename);
	if(! property)return FALSE;

	float t_scale=PERSENT(Ini.ReadInt(_index,"scale"));
	float t_h	 =PERSENT(Ini.ReadInt(_index,"h"));
	if(!CActiveObj::Init(Ini.ReadText(_index,"ModelFile"),Ini.ReadText(_index,"TexFile"),t_scale,t_h))return FALSE;
	property->shadow=Ini.ReadInt(_index,"shadow");

	property->m_sColor.r=PERSENT(Ini.ReadInt(_index,"r"));
	property->m_sColor.g=PERSENT(Ini.ReadInt(_index,"g"));
	property->m_sColor.b=PERSENT(Ini.ReadInt(_index,"b"));
	property->m_sColor.a=PERSENT(Ini.ReadInt(_index,"a"));

	property->r1=PERSENT(Ini.ReadInt(_index,"r1"));
	property->r2=PERSENT(Ini.ReadInt(_index,"r2"));
	property->speed=PERSENT(Ini.ReadInt(_index,"speed"));
	m_sPos.x=(float)Ini.ReadInt(_index,"x");
	m_sPos.y = 0.0f;
	m_sPos.z=(float)Ini.ReadInt(_index,"z");
	property->start_pos.x = m_sPos.x;
	property->start_pos.y = m_sPos.z;
	property->curr_pos = property->start_pos;

	property->life=Ini.ReadInt(_index,"life");
	property->curr_life=Ini.ReadInt(_index,"curr_life");

	ObjectInfo.PushRole(property);

	return TRUE;
}

void CRole::EndRender(void)
{
	vector3d vector;
	vector2d orient1;
	vector2d dest;
	float theta1 , theta2;

	//这里可以加走路时的声音/////////
	if(property->object_state==RUN)
	{
		if(IsRun==TRUE)
		{
			run.LoadSoundFX("sound\\run.wav");
			run.Play(1);
		}
		IsRun=FALSE;
	}
	else{run.Stop();IsRun=TRUE;}
	//////////////////////////////


	if(!property->bArrive)//体力值减少/////////
	{
		stamina-=0.0003f;if(stamina<=0.f)stamina=0.f;
	}
	///////////随着时间各个属性值都有一定的增加////////////
	static float dtime=0.f;
	dtime-=CTimer::GetSysTimer()->m_fTime;
	if(dtime<=0.f)
	{
		stamina+=0.0015f;if(stamina>=1.f)stamina=1.f;
		energy+=0.0015f;if(energy>=1.f)energy=1.f;
		dtime=1.f;
	}


	if(property->object_state != DEATH)
	{
		if(m_bActive&&SYS_MOUSE->ButtonDown(0))
		{
			vector = CInput::GetSysInput()->GetMousePosition();
			dest.x = vector.x;
			dest.y = vector.z;
			////////////////////////////
			property->end_pos = dest;
			orient1 = property->end_pos - property->curr_pos;
			orient1.normalize();
			theta1 = property->orient.getradian();
			theta2 = orient1.getradian();
			property->bArrive = FALSE;
			if(property->object_state != RUN)SetObjectState(RUN);
			property->start_pos = property->curr_pos;
			property->yrot += theta1 - theta2;
			if(property->yrot >=360.0f)property->yrot -= 360.0f;
			if(property->yrot <0.0f)property->yrot += 360.0f;
			property->orient = orient1;
			//////////////////////////////
		}

		if(pick_state == MONSTER)
		{
			bPick = TRUE;
			pickid = objectid;
			m_pMonster = ObjectInfo.GetMonsterPointer(objectid);
			m_pMonster->m_bSelected=TRUE;
		}
		else
		{
			bPick = FALSE;
			m_pMonster = NULL;
		}
		if(m_bActive&&SYS_MOUSE->ButtonDown(1))
		{
			if(!bRightMouseDown)
			{
				if(!property->bArrive)
				{
					property->bArrive = TRUE;
				}
				if(!fireball.isRun())
				{
					energy-=0.03f;if(energy<=0.f)energy=0.f;//精力值减少////////

					vector = CInput::GetSysInput()->GetMousePosition();
					dest.x = vector.x;
					dest.y = vector.z;
					orient1 = dest - property->curr_pos;
					orient1.normalize();
					float theta1 = property->orient.getradian();
					float theta2 = orient1.getradian();
					property->yrot += theta1 - theta2;
					if(property->yrot >=360.0f)property->yrot -= 360.0f;
					if(property->yrot <0.0f)property->yrot += 360.0f;
					property->orient = orient1;
					fireball.Set(property->vPosition , vector);

					bRightMouseDown = TRUE;
				}
			}
		}
		else
		{
			bRightMouseDown = FALSE;
		}
		if(fireball.canReduceLife()&&bPick)
		{
			if(m_pMonster) m_pMonster->ReduceLife(15);
			//这是可以加挨打时发出的声音
			sound.LoadSoundFX("sound\\dead.wav");
			sound.Play();
		}
	}
	CActiveObj::EndRender();
}


void CRole::Render_Pick(void)
{
	property->t_add = CTimer::GetSysTimer()->m_fTime*13.f;

	glPushMatrix();
	glTranslatef(property->vPosition.x,property->vPosition.y,property->vPosition.z);
	glRotatef(property->yrot,0.0f,1.0f,0.0f);
	md2.RenderFrame(property->curr_frame,property->next_frame,property->t);
	glPopMatrix();
}


BOOL CRole::GetMonsterInfo(char *pInfo,float& t)
{
	if(bPick)
	{
		sprintf(pInfo,"monster%d",pickid);
		t = m_pMonster->GetLife();
		return TRUE;
	}
	return FALSE;
}


void CRole::SetSpeed(float s)
{
	property->speed =s;
	fireball.SetSpeed(property->speed*3);
}
