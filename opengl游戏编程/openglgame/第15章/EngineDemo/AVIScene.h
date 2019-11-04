//========================================================
/**
*  @file      AVIScene.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  视频动画场景类————用到片头动画和场间动画
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef __AVI_SCENE_H__
#define __AVI_SCENE_H__

#include "iscene.h"
#include "aviplayer.h"
#include "input.h"

/**\brief
 * 视频动画场景类
 */
class AVIScene : public IScene
{
public:
	AVIScene(void);
	~AVIScene(void);


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


protected:
	//! AVI播放器
	AVIPlayer m_cAVI;

	//! 要播放的AVI文件名
	char* m_strAVIFile;
};


#endif