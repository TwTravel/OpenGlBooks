//========================================================
/**
*  @file      StaticObj.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  游戏中的静态实体
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef __STATIC_OBJ_H__
#define __STATIC_OBJ_H__

#include "object.h"
#include "MS3DFile.h"
#include "terrain.h"

class StaticObj : public Object
{
public:
	StaticObj(void);
	~StaticObj(void);

	void SetDir(float _dir)		{m_fDir=_dir;}
	float GetDir()				{return m_fDir;}

	BOOL LoadIni(char* filename,char* index);

	virtual void OnRender();

	void SetSelecte(BOOL b)		{m_bSelected=b;}

private:
	CMS3DFile ms3d;			//!< MS3D文件操作

	float		m_fDir;		//!< 方向

	BOOL		m_bSelected;//!< 是否被选中
};


#endif