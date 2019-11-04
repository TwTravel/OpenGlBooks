//========================================================
/**
*  @file      Role.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  游戏中的英雄
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================


#ifndef _GAME_ROLE_H_
#define _GAME_ROLE_H_

#include "mathlib.h"
#include "ActiveObj.h"
#include "input.h"
#include "fireball.h"
#include "sound.h"

extern CInfo ObjectInfo;


/**\brief
 * 游戏中的英雄
 */
class CRole:public CActiveObj
{
public:
	CRole(void);
	~CRole(void){_DELETE(property)}

	void SetSpeed(float s);

	BOOL GetMonsterInfo(char *pInfo,float& t);

	void SetShadow(BOOL b){property->shadow=b;}
	void SetSelecte(BOOL b)		{property->m_bSelected=b;}
	void SetPickState(int state,int id){pick_state=state;objectid=id;}

	virtual void Render_Pick(void);

	virtual void EndRender(void);

	virtual void OnRender(void){CActiveObj::OnRender();fireball.Render();}

	BOOL Init(void);

	static CRole* GetRolePointer(){return m_pRole;}
	
	BOOL LoadIni(char* _filename,char* _index);

	float energy;//精力
	float stamina;//体力


private:
	MD2_ANIMATION *m_pAnimations;
	CFireball fireball;
	int pick_state;
	BOOL bPick;

	int objectid,pickid;

	static CRole* m_pRole;
	SActiveObjPro* m_pMonster;

	BOOL bRightMouseDown;

	CSound sound;
	CSound run;
	BOOL IsRun;
};


#endif