//========================================================
/**
*  @file      BillBoard.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  公告板类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _BILL_BORAD_H_
#define _BILL_BORAD_H_

#include "mathlib.h"


/**\brief
 * 公告板描述类
 */
class CBillBoard  
{
public:
	CBillBoard():rotate(0.f),bRotate(TRUE){}
	~CBillBoard(){}

	void SetRotate(BOOL isRotate){bRotate = isRotate;}
	void Render(vector3d vpos,float m_iSize);
	void Update();


private:
	vector3d	right;
	vector3d	up;
	float		rotate;
	BOOL		bRotate;
};


#endif