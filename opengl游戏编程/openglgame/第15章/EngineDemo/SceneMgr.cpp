//========================================================
/**
*  @file      SceneMgr.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  场景管理类
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
#include "scenemgr.h"


SceneMgr* SceneMgr::m_psceneMgr=NULL;

SceneMgr::SceneMgr(void):m_pScene(NULL),m_eSceneType(SCE_NULL)
{
	m_psceneMgr=this;
}


SceneMgr::~SceneMgr(void)
{
	_DELETE(m_pScene);
}


/**\brief
 * 初始化
 */
BOOL SceneMgr::Init(void)
{
	//Terrain.Init(64,"scene/ground01.bmp");
	Terrain.Load("scene/ter02.ter");
	if(!m_cStaticObj.LoadIni("ini/StaticObj.ini","StaticObj"))return FALSE;
	if(!m_cMonsters.LoadIni("ini/monsters.ini","monsters"))return FALSE;
	if(!role.Init())return FALSE;

	return TRUE;
}

/**\brief
 * 从INI载入场景数据
 */
BOOL SceneMgr::LoadIni(char* _filename,char* _index)
{
	if(!m_pScene)return FALSE;
	if(!m_pScene->LoadIni(_filename,_index))return FALSE;
	if(m_pScene->Init())return FALSE;

	return TRUE;
}


/**\brief
 * 设置当前场景
 */
void SceneMgr::SetScene(GAME_SCENE _scene)
{
	m_eSceneType=_scene;
	if(m_pScene)_DELETE(m_pScene);

	switch(m_eSceneType)
	{
	case SCE_AVI	:m_pScene=new AVIScene;break;
//	case SCE_TITLE	:m_pScene=new TitleScene;break;
//	case SCE_SELCHAR:m_pScene=new SelCharScene;break;
	case SCE_OPENAIR:m_pScene=new OpenAirScene;break;
	default			:m_pScene=NULL;break;
	}
}


/**\brief
 * 场景的渲染
 */
void SceneMgr::Render(void)
{
	if(!m_pScene)return;
	m_pScene->Render();

	if(m_eSceneType==SCE_OPENAIR||m_eSceneType==SCE_ROOM)
	{
		Terrain.Render();

		if(SYS_MOUSE->ButtonDown(0)||SYS_MOUSE->ButtonDown(1))Pick();

		m_cStaticObj.Render();
		role.Render();
		m_cMonsters.Render();
	}
}


void SceneMgr::StartPick(void)
{
	GLint	viewport[4];
    SPoint2 p= SYS_MOUSE->GetMousePos();

	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(512, selectBuff);

	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix( (double)(p.x),(double)(/*viewport[3]-*/p.y),5.0,5.0,viewport);
	gluPerspective(45.0f, (GLfloat)(viewport[2]-viewport[0])/(GLfloat)(viewport[3]-viewport[1]), 0.001f, F3D_DEPTH);
	glMatrixMode(GL_MODELVIEW);
}


void SceneMgr::EndPick(void)
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}


BOOL SceneMgr::PickModel(int &type,unsigned int &id)
{
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);
	glPushMatrix();
	CCamera::GetCameraPointer()->FrameMove();

	unsigned int modelnum = 0;
	unsigned int i=0;

	for(i = 0 ; i < m_cStaticObj.GetObjNum(); ++i)
	{
		glLoadName(modelnum++);
		m_cStaticObj.Render_Pick(i);
	}
	for(i = 0 ; i < m_cMonsters.GetMonsterNum();++i)
	{
		glLoadName(modelnum++);
		m_cMonsters.Render_Pick(i);
	}
	glLoadName(modelnum);
	role.Render_Pick();
	glPopMatrix();
	GLint hits = glRenderMode(GL_RENDER);

	if(hits > 0)
	{
		int choose = selectBuff[3];
		int depth = selectBuff[1];

		for(int i=0;i<hits;++i)
		{
			if(selectBuff[i*4 + 1] < (GLuint)depth)
			{
				choose = selectBuff[i*4 + 3];
				depth  = selectBuff[i*4 + 1];
			}
		}
		id = choose;
		if(id==m_cStaticObj.GetObjNum()+m_cMonsters.GetMonsterNum())
		{
			type = ROLE;
			return TRUE;
		}
		if(id >=m_cStaticObj.GetObjNum())
		{
			type = MONSTER;
			id = id-m_cStaticObj.GetObjNum();
			return TRUE;
		}

		type = HOUSE;
		return TRUE;
	}
	
	return FALSE;
}


void SceneMgr::Pick(void)
{
	StartPick();
	unsigned int id;
	int type;
	char buffer[16];

	//除掉所有实体上的红框框
	ObjectInfo.GetRolePointer()->m_bSelected=FALSE;
	for(unsigned int i=0;i<ObjectInfo.GetMonstersNum();++i)
	{
		ObjectInfo.GetMonsterPointer(i)->m_bSelected=FALSE;
	}

	if(PickModel(type , id))
	{
		if(type == ROLE)
		{
			sprintf(buffer,"role");
			role.SetPickState(ROLE,id);
			role.SetSelecte(TRUE);
		}
		if(type == MONSTER)
		{
			sprintf(buffer,"monster%d",id);
			role.SetPickState(MONSTER,id);
		}
		if(type == HOUSE)
		{
			sprintf(buffer,"house%d",id);
			role.SetPickState(HOUSE,id);
		}
	}
	else
	{
		sprintf(buffer,"earth");
		role.SetPickState(EARTH ,id);
	}
	::SetWindowText(SYS_HWND,buffer);

	EndPick();
}