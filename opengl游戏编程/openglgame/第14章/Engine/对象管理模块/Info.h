//========================================================
/**
*  @file      Info.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  对象实体的信息
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _INFO_H_
#define _INFO_H_

#include "mathlib.h"
#include "terrain.h"
#include <vector>

using namespace std;

#define TREE      0
#define HOUSE     1
#define MONSTER   2
#define ROLE      3
#define EARTH     4


///////////////////
/**\brief
 * 动态实体的状态
 */
enum OBJ_STATE
{
	STAND =0,
	PAIN,
	WALK,
	RUN,
	ATTACK,
	DEATH
};


/////////////////////////////
//! MD2模型动画的帧动作列表
#define STAND_START		1
#define STAND_END		10
#define WALK_START		136
#define WALK_END		142 
#define RUN_START		41
#define RUN_END			46
#define ATTACK_START	47
#define ATTACK_END		54
#define PAIN1_START		55
#define PAIN1_END		58
#define PAIN2_START		59 
#define PAIN2_END		63 
#define PAIN3_START		64 
#define PAIN3_END		68 
#define JUMP_START		69
#define JUMP_END		74
#define DEATH1_START	179
#define DEATH1_END		184
#define DEATH2_START	185
#define DEATH2_END		190
#define DEATH3_START	191
#define DEATH3_END		198



//////////////////////////////
/**\brief
 * 动态实体的信息结构
 */
struct SActiveObjPro
{

	SColor m_sColor;					//!< 颜色

	OBJ_STATE object_state,old_state;	//!< 当前状态、老状态

	int curr_frame,next_frame;			//!< 当前帧，下一帧

	float		t,t_add;				//!< 插值

	float		speed;					//!< 速度

	vector3d	vPosition;				//!< 3d位置

	float		r1,r2;					//!< 包围盒的半径,分别是x,y上的分量

	BOOL		m_bSelected;			//!< 是否被选中

	float		yrot;					//!< 旋转角度

	vector2d	orient;					//!< 步幅

	vector2d start_pos,end_pos,curr_pos;//!< 2d位置,开始、结束、当前位置

	int life,curr_life;					//!< 总生命、当前生命

	BOOL bArrive;						//!< 判断是否到达

	BOOL shadow;						//!< 是否绘阴影

	float dist_min,dist_max;			//!< 攻击距离

	SActiveObjPro();
	~SActiveObjPro(){}

	void ChangeCurrFrame();

	void SetObjectState(OBJ_STATE state)
	{
		if(object_state != state)
		{
			old_state = object_state;
			object_state = state;
			ChangeCurrFrame();
		}
	}


	void ReduceLife(int delta)
	{
		curr_life -= delta;
		if(curr_life < 0)curr_life = 0;
		if(object_state != DEATH)
			SetObjectState(PAIN);
	}


	void GetObjectPos(vector3d *pVector){*pVector=vPosition;}

	void SetObjectPos(vector3d &v){vPosition = v;}

	float GetLife()
	{
		float t = curr_life/(float)life;
		if(t > 1.0f)t = 1.0f;
		if(t < 0.0f)t = 0.0f;
		return t;
	}

	void Update();
};



////////////////////////////
/**\brief
 * 动态实体的信息管理
 */
class CInfo  
{
public:
	CInfo(){}
	~CInfo(){}


	/**\brief
	 * 将一个怪物的信息加入列表
	 */
	void PushMonster(SActiveObjPro *pProperty){m_pMonsterProperty.push_back(pProperty);}


	/**\brief
	 * 添加英雄的信息
	 */
	void PushRole(SActiveObjPro * pProperty){m_pRoleProperty = pProperty;}


	/**\brief
	 * 获得某一怪物的指针
	 */
	SActiveObjPro* GetMonsterPointer(int id){return m_pMonsterProperty[id];}


	/**\brief
	 * 获得英雄的指针
	 */
	SActiveObjPro* GetRolePointer(){return m_pRoleProperty;}


	/**\brief
	 * 获得怪物的个数
	 */
	unsigned int GetMonstersNum(void){return m_pMonsterProperty.size();}

private:
	//! 怪物的信息列表
	vector<SActiveObjPro*> m_pMonsterProperty;

	//! 英雄的信息
	SActiveObjPro *m_pRoleProperty;
};



#endif