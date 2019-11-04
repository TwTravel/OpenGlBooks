//========================================================
/**
*  @file      FireBall.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  爆炸火花类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _FIRE_BALL_H_
#define _FIRE_BALL_H_

#include "texture.h"
#include "billboard.h"

#define MAX_LENGTH 100.0f

/** 爆炸火花类 */
class CFireball  
{
public:
	CFireball();
	~CFireball(){}

	void SetSpeed(float s){speed = s;}
	BOOL canReduceLife(){return canReduce;}
	void Change(vector3d& dest){end_pos = dest;}
	void SetColor(float _r,float _g,float _b){r=_r;g=_g;b=_b;}
	BOOL isRun(){return bRun;}
	void Render();
	void Set(vector3d start , vector3d end);
	BOOL Init(char *filename , float size);



private:
	CTexture texture;
	CBillBoard billboard;

	float m_iSize;
	vector3d start_pos,end_pos,vpos;
	float speed;
	BOOL bRun;
	vector3d orient;
	float r,g,b;
	BOOL canReduce;
	BOOL isArrive;
	int i;
};

#endif