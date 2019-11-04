//========================================================
/**
*  @file      SceneMgr.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  场景管理类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================


#ifndef __SCENE_MGR_H__
#define __SCENE_MGR_H__

#include "iscene.h"
#include "aviscene.h"
//#include "titlescene.h"
//#include "selcharscene.h"
#include "openairscene.h"
#include "role.h"
#include "monstersMgr.h"
#include "staticObjMgr.h"
#include "timer.h"


/**\brief
 * 场景管理类
 */
class SceneMgr
{
public:
	SceneMgr(void);
	~SceneMgr(void);


	/**\brief
	 * 初始化
	 */
	BOOL Init();


	/**\brief
	 * 从INI载入场景数据
	 */
	BOOL LoadIni(char* _filename,char* _index);


	/**\brief
	 * 设置当前场景
	 */
	void SetScene(GAME_SCENE _scene);


	/**\brief
	 * 获得当前运行的是哪类场景
	 */
	GAME_SCENE GetScene(void){return m_eSceneType;}


	/**\brief
	 * 场景的渲染
	 */
	void Render(void);

	/**\brief
	 * 取得当前场景管理对象的指针
	 */
	static SceneMgr* GetSysSceneMgr(void){return m_psceneMgr;}

private:
	//! 当前场景指针
	IScene* m_pScene;

	//! 当前的场景类型
	GAME_SCENE m_eSceneType;

	//! 场景管理对象的指针
	static SceneMgr* m_psceneMgr;

	/////////////////////////////////////////////
	//! 地形
	CTerrain Terrain;

	//! 点选数据缓冲区
	GLuint selectBuff[512];

	//! 静态实体
	CStaticObjMgr m_cStaticObj;

	//! 怪物
	CMonstersMgr m_cMonsters;

	//! 英雄
	CRole role;

	/**\brief
	 * 开始鼠标点选操作
	 */
	void StartPick(void);

	/**\brief
	 * 结束鼠标点选操作
	 */
	void EndPick(void);

	/**\brief
	 * 用鼠标进行对象点选
	 */
	void Pick(void);
	BOOL PickModel(int&type ,unsigned int &id);
};


#endif