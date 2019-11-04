//========================================================
/**
*  @file      MapView.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  小地图
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
#include "mapview.h"

MapView::MapView(void)
{
}

MapView::~MapView(void)
{
}


/**\brief
 * 从INI文件载入小地图数据
 */
BOOL MapView::LoadIni(char* _filename,char* _index)
{
	CIniFile Ini(_filename);

	m_sRect.width=Ini.ReadInt(_index,"width");
	m_sRect.height=Ini.ReadInt(_index,"height");

	m_chBackTexFile =Ini.ReadText(_index,"BackTexFile");
	
	m_bBlend=TRUE;
	m_sColor=SColor(1.f,1.f,1.f,0.9f);
	m_sRect.x = SYS_WIDTH - m_sRect.width;
	m_sRect.y = SYS_HEIGHT - m_sRect.height;

	if(m_chBackTexFile==NULL)return FALSE;
	this->LoadTexture(m_chBackTexFile,1,m_bBlend);

	return TRUE;
}


/**\brief
 * 小地图的渲染
 */
void MapView::OnDraw(void)
{
	float x , y;
	vector3d pos=CRole::GetRolePointer()->GetPos();
	x = (pos.x /2048.0f * m_sRect.width) + m_sRect.x;
	y = -(pos.z /2048.0f * m_sRect.height) + m_sRect.y+m_sRect.height;
	glPointSize(5.0f);
		glColor3f(1.0f,1.0f,0.0f);
	glBegin(GL_POINTS);
	  glVertex2f(x,y);
	glEnd();

	for(unsigned int i=0;i<CMonstersMgr::GetSysMonstersMgr()->GetMonsterNum();++i)
	{
		pos = CMonstersMgr::GetSysMonstersMgr()->GetMonsterPos(i);
		x = (pos.x / 2048.0f * m_sRect.width) +m_sRect.x ;
		y =-(pos.z / 2048.0f * m_sRect.height)+m_sRect.y+m_sRect.height;
		if(CMonstersMgr::GetSysMonstersMgr()->GetMonsterState(i) == DEATH)
			glColor3f(0.0f , 0.0f , 0.0f);
		else
			glColor3f(1.0f , 0.0f , 0.0f);
		glBegin(GL_POINTS);
		glVertex2f(x,y);
		glEnd();
	}
	glPointSize(1.0f);
}