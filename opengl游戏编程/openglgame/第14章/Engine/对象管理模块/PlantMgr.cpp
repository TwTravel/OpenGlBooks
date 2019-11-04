//========================================================
/**
*  @file      PlantMgr.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  树木管理类
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
#include "PlantMgr.h"
#include "inifile.h"


BOOL CTreeMgr::LoadIni(char* filename,char* index)
{
	CIniFile Ini(filename);
	unsigned int t_num=Ini.ReadInt(index,"TreeNum");
	//初始化植物
	char* strFile;
	char* strIndex;
	for(unsigned int i=0; i<t_num; ++i)
	{
		strFile = Ini.ReadText(index, i);
		strIndex = Ini.ReadData(index,i);
		tree.LoadIni(strFile, strIndex);
		m_pTrees.push_back(tree);
		_FREE(strFile);
		_FREE(strIndex);
	}

	return TRUE;
}


void CTreeMgr::Render()
{
	glDisable(GL_CULL_FACE);
	for(unsigned int i=0;i<m_pTrees.size();++i)
	{
		m_pTrees[i].Render();
	}
	glEnable(GL_CULL_FACE);
}

