//======================================================================
/**
*  @file      Particle.cpp
*
*  项目描述： 粒子系统
*  文件描述:  粒子定义类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-12-18	
*  修改日期： 2006-12-19
*	
*/																			
//======================================================================
#include "Particle.h"

/** 构造函数 */
CParticle::CParticle():m_iNum(0),m_pList(NULL)
{	
}

/** 析构函数 */
CParticle::~CParticle()
{
	if(m_pList != NULL)
	{
		delete []m_pList;
		m_pList = NULL;
	}
	
}

/** 粒子的初始化 */
bool CParticle::Init(int _num)
{
	m_iNum = _num;
	m_pList = new Particle[m_iNum];
	if(m_pList == NULL)
		return false;
	return true;
}

/** 粒子的渲染 */
void CParticle::Render()
{
}

/** 粒子的更新 */
void CParticle::Update()
{
}
