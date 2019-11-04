//========================================================
/**
*  @file      IScene.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  场景类的基类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================


#ifndef __I_SCENE_H__
#define __I_SCENE_H__


/**\brief
 * 游戏中的场景
 */
enum GAME_SCENE
{
	//! 空场景
	SCE_NULL=0,

	//! 视频动画场景
	SCE_AVI,

	//! 开始时的字幕场景
	SCE_TITLE,

	//! 人物角色选择场景
	SCE_SELCHAR,

	//! 室外场景
	SCE_OPENAIR,

	//! 室内场景
	SCE_ROOM
};


/**\brief
 * 场景类的基类(纯虚)
 */
class IScene
{
public:
	IScene(void);
	virtual ~IScene(void);


	/**\brief
	 * 场景的初始化
	 */
	virtual BOOL Init(void)=0;


	/**\brief
	 * 从IIN文件载入数据
	 */
	virtual BOOL LoadIni(char* _filename,char* _index)=0;


	/**\brief
	 * 数据的更新
	 */
	virtual void UpdateData(void)=0;


	/**\brief
	 * 场景的渲染
	 */
	virtual void Render(void)=0;

};


#endif