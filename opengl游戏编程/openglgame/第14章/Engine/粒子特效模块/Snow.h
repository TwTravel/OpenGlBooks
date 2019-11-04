//========================================================
/**
*  @file      Snow.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  雪花渲染类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _SNOW_H_
#define _SNOW_H_

#include "Particle.h"
#include "texture.h"
#include "billboard.h"
#include "role.h"


/**\brief
 * 雪的渲染类
 */
class CSnow : public CParticle  
{
public:
	void Render();
	BOOL Init();
	CSnow(){}
	~CSnow(){}

private:
	void ResetPaticle(PARTICLE *particle);
	CTexture snow_texture;
	int m_width , m_height , m_depth;
	vector3d vCameraPos;
	CBillBoard billboard;
};

#endif