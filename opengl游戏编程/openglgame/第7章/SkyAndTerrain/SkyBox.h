//========================================================
/**
*  @file      SkyBox.h
*
*  项目描述： 构造天空和地面
*  文件描述:  天空盒类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-05-17	
*  
*
*/     
//========================================================
#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "stdafx.h"
#include "CBMPLoader.h"
#include "Vector.h"
#include "Camera.h"

#define GL_CLAMP_TO_EDGE	0x812F

/** 天空盒类 */
class CSkyBox
{
public:
	/** 构造函数 */
	CSkyBox();
	~CSkyBox();

	/** 初始化 */
	bool init();
	
	/** 渲染 */
	void render();

private:
	
	CBMPLoader  m_texture[5];   /**< 天空盒纹理   */
	Vector3     m_CameraPos;    /**< 当前摄像机位置 */
	float       length;         /**< 长度 */
	float       width;          /**< 宽度 */
	float       height;         /**< 高度 */
	float       yRot;           /**< 绕Y轴旋转 */
		
};


#endif ///__SKYBOX_H__