//======================================================================
/**
*  @file      Particle.h
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

#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "stdafx.h"
#include "Vector.h"



/** 粒子结构 */
struct Particle
{
	Vector3   position;		       /**< 粒子的位置 */
	
	Vector3   velocity;            /**< 粒子的速度 */
	
	Vector3   acceleration;        /**< 粒子的加速度 */
	
	float     lifetime;            /**< 粒子生命值 */
	
	float     dec;		           /**< 粒子消失的速度 */
	
	float     size;		           /**< 粒子尺寸 */
	
	float     color[3];            /**< 粒子的颜色 */
	
};


/** 粒子类 */
class CParticle
{

public:

	CParticle();                  /**< 构造函数 */

	virtual ~CParticle();         /**< 析构函数 */
	
	/** 粒子的初始化 */
	virtual bool Init(int _num);

	/** 粒子的渲染 */
	virtual void Render() = 0;

	/** 粒子的更新 */
	virtual void Update() = 0;

protected:

	int           m_iNum;	              /**< 粒子总数目 */
	Particle*     m_pList;				  /**< 粒子指针 */

};

#endif