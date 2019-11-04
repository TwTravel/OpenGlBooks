//========================================================
/**
*  @file      MonstersMgr.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  怪物管理类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _MONSTERS_MGR_H_
#define _MONSTERS_MGR_H_

#include "monster.h"
#include "mathlib.h"
#include "Info.h"


/**\brief
 * 怪物对象管理类
 */
class CMonstersMgr
{
public:
	CMonstersMgr();
	~CMonstersMgr();


	/**\brief
	 * 得到某个怪物的状态
	 */
	OBJ_STATE GetMonsterState(int id){return m_pMonster[id].GetObjectState();}


	/**\brief
	 * 得到某个怪物的位置
	 */
	vector3d GetMonsterPos(int id){return m_pMonster[id].GetPos();}


	/**\brief
	 * 怪物是否显示阴影
	 */
	void SetShadow(BOOL b);


	/**\brief
	 * 在选择模式中渲染怪物
	 */
	void Render_Pick(int id){m_pMonster[id].Render_Pick();}


	/**\brief
	 * 取得怪物的数量
	 */
	unsigned int GetMonsterNum(){return m_nNumMonsters;}


	/**\brief
	 * 取得当前怪物管理对象的指针
	 */
	static CMonstersMgr* GetSysMonstersMgr(){return m_pMonstersMgr;}


	/**\brief
	 * 渲染怪物
	 */
	void Render();


	/**\brief
	 * 初始化怪物
	 */
	BOOL LoadIni(char* filename,char* index);


private:
	//! 怪物的数量
	unsigned int m_nNumMonsters;

	//! 存放怪物数据的指针
	CMonster* m_pMonster;

	//! 怪物管理对象的指针
	static CMonstersMgr* m_pMonstersMgr;
};



////////////////////////////////////////////////////
//----------------inline函数实现--------------------
inline void CMonstersMgr::Render()
{
	for(unsigned int i=0;i<m_nNumMonsters;++i)m_pMonster[i].Render();
}


#endif