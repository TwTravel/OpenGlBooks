//========================================================
/**
*  @file      Monster.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  游戏中的怪物
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _MONSTER_H_
#define _MONSTER_H_

#include "ActiveObj.h"
#include "terrain.h"
#include "fireball.h"
#include "info.h"
#include "sound.h"

extern CInfo ObjectInfo;


/**\brief
 * 游戏中的怪物
 */
class CMonster : public CActiveObj
{
public:
	CMonster(void);
	~CMonster(void){_DELETE(property);}


	/**\brief
	 * 设置速度
	 */
	void SetSpeed(float s);
	void Render_Pick(void);


	/**\brief
	 * 设置怪物的属性
	 */
	void SetMonsterInfo(SActiveObjPro& info);
	void SetShadow(BOOL b){property->shadow=b;}

	/**\brief
	 * 渲染怪物
	 */
	virtual void OnRender(void){CActiveObj::OnRender();fireball.Render();}


	/**\brief
	 * 渲染后的数据更新
	 */
	virtual void EndRender(void);


	/**\brief
	 * 初始化一个怪物
	 */
	BOOL Init(char* modelname,char* texturename,float scale,float h);


	/**\brief
	 * 从INI文件初始化一个怪物
	 */
	BOOL LoadIni(char* filename,char* index);


private:
	//! 魔法
	CFireball fireball;

	int time_elapse;

	CSound sound;
};

#endif