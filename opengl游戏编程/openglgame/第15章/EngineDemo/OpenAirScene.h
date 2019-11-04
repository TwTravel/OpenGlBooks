//========================================================
/**
*  @file      OpenAirScene.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  游戏室外场景类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================


#ifndef __OPEN_AIR_SCENE_H__
#define __OPEN_AIR_SCENE_H__

#include "iscene.h"
#include "snow.h"
#include "plantmgr.h"
#include "SkyBox.h"
#include "water.h"

/**\brief
 * 游戏主场景类
 */
class OpenAirScene : public IScene
{
public:
	OpenAirScene(void);
	~OpenAirScene(void);


	/**\brief
	 * 场景的初始化
	 */
	BOOL Init(void);


	/**\brief
	 * 从INI文件载入数据
	 */
	BOOL LoadIni(char* _filename,char* _index);


	/**\brief
	 * 更新场景的数据
	 */
	void UpdateData(void);


	/**\brief
	 * 场景的渲染
	 */
	void Render(void);


	CTreeMgr* GetTreeMgr(){return &trees;}

private:
	//! 雪的渲染对象
	CSnow m_cSnow;

	//! 天空
	CSkyBox skybox;
	
	//! 树
	CTreeMgr trees;

	//! 水
	CWater water;
};


#endif