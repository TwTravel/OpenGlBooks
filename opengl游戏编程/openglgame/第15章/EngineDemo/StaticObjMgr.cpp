//========================================================
/**
*  @file      StaticObjMgr.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  静态实体对象管理类
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
#include "StaticObjMgr.h"
#include "IniFile.h"


CStaticObjMgr* CStaticObjMgr::m_pStaticObjMgr=NULL;

CStaticObjMgr::CStaticObjMgr()
{
	m_pStaticObjMgr=this;
}

CStaticObjMgr::~CStaticObjMgr()
{
	_DELETE_ARRAY(m_pObjArray);
}


/**\brief
 * 初始化
 */
BOOL CStaticObjMgr::LoadIni(char* filename,char* index)
{
	CIniFile Ini(filename);
	m_nObjNum=Ini.ReadInt(index,"ObjNum");
	m_pObjArray=new StaticObj[m_nObjNum];

	//初始化模型
	char* strFile;
	char* strIndex;
	for(unsigned int i=0; i<m_nObjNum; ++i)
	{
		strFile = Ini.ReadText(index, i);
		strIndex = Ini.ReadData(index,i);
		m_pObjArray[i].LoadIni(strFile, strIndex);
		_FREE(strFile);
		_FREE(strIndex);
	}

	return TRUE;
}

