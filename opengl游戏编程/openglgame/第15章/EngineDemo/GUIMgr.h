//========================================================
/**
*  @file      GUIMgr.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  GUI界面管理类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef __GUI_MGR_H__
#define __GUI_MGR_H__

#include "ListView.h"
#include "edit.h"
#include "dialog.h"
#include "animate.h"
#include "window.h"
#include "ListView.h"
#include "edit.h"
#include "mapview.h"
#include "progressbar.h"
#include "SceneMgr.h"


/**\brief
 * 界面管理类
 */
class GUIMgr
{
public:
	GUIMgr(void);
	~GUIMgr(void);


	static GUIMgr* GetSysGUIMgr(){return m_pGUIMgr;}

	/**\brief
	 * 从INI文件载入光标数据
	 */
	BOOL LoadCurIni(char* _filename,char* _index);


	/**\brief
	 * 界面的渲染
	 */
	void Render(void);


	/**\brief
	 * 淡入淡出
	 */
	void Fade(void);

	/**\brief
	 * 开始出现淡入淡出效果
	 */
	void BeginFade(void){m_fFadeAlpha=1.f;}


private:
	static GUIMgr* m_pGUIMgr;

	CAnimate m_cCursor;
	CWindow bottom;

	ListView	m_cChatList;

	//! 聊天输入框
	Edit		m_cChatEdit;

	//! 主菜单
	Dialog		m_cMainMenu;

	//! 小地图
	MapView		m_cMapView;

	//! 是否弹出主对话框
	BOOL		m_bMainMenu;

	//! 上面的小菜单
	Dialog		m_cTopMenu;
	CAnimate	m_cTopAni;

	//! 用来控制淡入淡出的变量
	float		m_fFadeAlpha;

	/////////////////////////////////////////
	//! 英雄的血槽
	CProgressBar m_cRoleLife;

	//! 怪物的血槽
	CProgressBar m_cMonsterLife;

	//! 英雄属性窗口
	Dialog		m_cPropertyWnd;
	/**\brief
	 * 英雄的属性
	 */
	void Property(void);

	//! 英雄的物品栏窗口
	Dialog		m_cBagWnd;
	/**\brief
	 * 英雄的物品栏
	 */
	void Bag(void);

	///////////////////////////////////////////
	CTexture	m_cLifeTex;
	CTexture	m_cMagicTex;
	/**\brief
	 * 生命球和魔法球的绘制
	 */
	void Ball(CTexture tex,float x,float y,float pos);
};

#endif