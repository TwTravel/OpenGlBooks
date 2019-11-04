//========================================================
/**
*  @file      MonstersMgr.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  怪物管理类
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
#include "MonstersMgr.h"
#include "inifile.h"

CMonstersMgr* CMonstersMgr::m_pMonstersMgr=NULL;

CMonstersMgr::CMonstersMgr()
{
	m_pMonster = NULL;
	m_pMonstersMgr=this;
}


CMonstersMgr::~CMonstersMgr()
{
	if(m_pMonster != NULL)
	{
		delete [] m_pMonster;
		m_pMonster = NULL;
	}
}


BOOL CMonstersMgr::LoadIni(char* filename,char* index)
{
	CIniFile Ini(filename);
	m_nNumMonsters =Ini.ReadInt(index,"MonsterNum");
	m_pMonster = new CMonster[m_nNumMonsters];

	//初始化怪物
	char* strFile;
	char* strIndex;
	for(unsigned int i=0; i<m_nNumMonsters; ++i)
	{
		strFile = Ini.ReadText(index, i);
		strIndex = Ini.ReadData(index,i);
		m_pMonster[i].LoadIni(strFile, strIndex);
		_FREE(strFile);
		_FREE(strIndex);
	}

	return TRUE;
}


void CMonstersMgr::SetShadow(BOOL b)
{
	for(unsigned int i = 0; i < m_nNumMonsters; ++i)
	{
		m_pMonster[i].SetShadow(b);
	}
}

