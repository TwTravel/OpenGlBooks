//========================================================
/**
*  @file      Animate.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  2D平面动画类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _ANIMATE_H_
#define _ANIMATE_H_

#include "StdAfx.h"
#include "Texture.h"
#include "IniFile.h"


/**\brief
 * 2D平面动画类
 */
class CAnimate  
{
public:
	//! 动画帧与帧之间的延时
	float	DeltaTime;

	//! 颜色
	SColor	color;

	//! 所在矩形
	SRect	rect;


	CAnimate();
	~CAnimate(){}


	/**\brief
	 * 是否透明处理
	 */
	BOOL GetBlendState(){return bBlend;}


	/**\brief
	 * 载入纹理
	 *
	 *\param	filename	纹理文件名
	 *\param	nFrameNum	总帧数
	 *\param	_bBlend		是否透明处理
	 *\return	BOOL		是否成功载入纹理
	 */
	BOOL LoadTexture(char* filename,int nFrameNum,BOOL _bBlend);


	/**\brief
	 * 通过INI文件初始化
	 */
	virtual BOOL LoadIni(char* filename,char* index);
	

	/**\brief
	 * 渲染动画
	 */
	void draw(void);


	/**\brief
	 * 移动动画
	 *
	 *\param	_dx	x上偏移
	 *\param	_dx	y上偏移
	 */
	void move(int _dx,int _dy){rect.x+=_dx;rect.y+=_dy;}



private:
	//! 动画的帧数
	unsigned int FrameNum;
	
	//! 是否透明处理
	BOOL		bBlend;

	//! 动画所用的纹理
	CTexture	texture;
	
	//! 正在播放第n帧（从0开始）
	unsigned int n;

	//! 两帧间的间隔时间
	float		m_fDeltaTime;
};


#endif