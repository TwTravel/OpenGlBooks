//========================================================
/**
*  @file      SkyBox.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  天空类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _SKY_BOX_H_
#define _SKY_BOX_H_

#include "camera.h"
#include "texture.h"


/**\brief
 * 天空盒类
 */
class CSkyBox  
{
public:
	CSkyBox();
	~CSkyBox();

	BOOL Init();			//!< 天空的初始化

	void Render();			//!< 渲染天空


private:
	CTexture texture;		//!< 天空纹理

	float yrot;				//!< 围绕Y轴的旋转角度

	vector3d m_sCameraPos;	//!< 摄像机的位置
};

#endif