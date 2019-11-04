//========================================================
/**
*  @file      MapView.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  小地图
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef __MAP_VIEW_H__
#define __MAP_VIEW_H__

#include "window.h"
#include "texture.h"
#include "MonstersMgr.h"
#include "role.h"

/**\brief
 * 小地图类
 */
class MapView : public CWindow
{
public:
	MapView(void);
	~MapView(void);

	/**\brief
	 * 从INI文件载入小地图数据
	 */
	BOOL LoadIni(char* _filename,char* _index);





private:
	/**\brief
	 * 小地图的绘制
	 */
	void OnDraw(void);
};


#endif