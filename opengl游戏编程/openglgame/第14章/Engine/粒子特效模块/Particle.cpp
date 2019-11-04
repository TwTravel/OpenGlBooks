//========================================================
/**
*  @file      Particle.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  粒子系统类
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
#include "Particle.h"
#include "time.h"


CParticle::CParticle()
{
	pList_particle = NULL;	
}

CParticle::~CParticle()
{
	if(pList_particle != NULL)
	{
		delete [] pList_particle;
		pList_particle = NULL;
	}
}


/**\brief
 * 粒子系统的初始化
 *
 *\param	_num	粒子数
 */
BOOL CParticle::Init(int _num)
{
	m_iMaxNum = _num;
	pList_particle = new PARTICLE[m_iMaxNum];
	if(pList_particle == NULL)
		return FALSE;
	return TRUE;
}


/**\brief
 * 粒子的渲染
 */
void CParticle::Render()
{	
}