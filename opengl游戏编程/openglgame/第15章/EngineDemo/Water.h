//========================================================
/**
*  @file      Water.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  水的渲染
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _WATER_H_
#define _WATER_H_

#include "texture.h"


/**\brief
 * 水的渲染类
 */
class CWater  
{
public:
	CWater();
	~CWater();

	/**\brief
	 * 初始化,主要是纹理的载入
	 */
	BOOL Init();
	

	/**\brief
	 *  水的渲染
	 */
	void Render();



private:
	//! 纹理
	CTexture texture;
	
	//! 纹理移动的最小单位
	float delta;

};

#endif
