//========================================================
/**
*  @file      Object.h
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

#ifndef __CGW_OBJECT_H__
#define __CGW_OBJECT_H__

#include "mathlib.h"
#include "terrain.h"

/**\brief
 * 所有游戏实体类的基类
 */
class Object
{
public:
	Object(void);
	~Object(void);

	float	r1,r2;		//!< 包围盒的半径,分别是x,y上的分量

	void Render();

	void SetPos(vector3d v)		{m_sPos=v;m_sPos.y = CTerrain::GetSysTerrain()->GetHeight(m_sPos.x ,m_sPos.z);}

	vector3d GetPos(void)		{return m_sPos;}

	void SetHeightAt(float _h)	{m_sPos.y=_h;}


private:
	virtual void OnRender(){}


protected:
	vector3d   m_sPos;

};

#endif