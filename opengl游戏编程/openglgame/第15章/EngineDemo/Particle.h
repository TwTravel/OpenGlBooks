//========================================================
/**
*  @file      Particle.h
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

#ifndef _PATICLE_H_
#define _PATICLE_H_

#include "mathlib.h"


/**\brief
 * 粒子的结构
 */
struct PARTICLE
{
	//! 粒子的位置
	vector3d  m_pos;

	//! 粒子的速度
	vector3d  m_velocity;

	//! 粒子的生命期
	float   life;

	//! 粒子的大小
	float   m_size;
	
	//! 加速度
	vector3d   m_gravity;
	
	//! 颜色
	float   m_color[3];
};



/**\brief
 * 粒子系统类
 */
class CParticle  
{
public:
	CParticle();
	virtual ~CParticle();


	/**\brief
	 * 粒子系统的初始化
	 *
	 *\param	_num	粒子数
	 */
	virtual BOOL Init(int _num);
	
	
	/**\brief
	 * 粒子的渲染
	 */
	virtual void Render() = 0;


protected:
	//! 总粒子数
	int m_iMaxNum;

	//! 粒子的指针
	PARTICLE *pList_particle;
};


#endif