//========================================================
/**
*  @file      window.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  GUI图形控件的基类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "stdafx.h"
#include "texture.h"
#include "Input.h"
#include "IniFile.h"


/**\brief
 *  窗口类
 */
class CWindow  
{
public:
	CWindow(void);
	CWindow(SRect _rect);
	virtual ~CWindow(void){}

	/**\brief
	 * 取得窗口退出时带回的值
	 */
	int GetState(void){return m_iState;}


	/**\brief
	 * 清除窗口状态
	 */
	void ClearState(void){m_iState=0;}

	/**\brief
	 * 设置是否显示
	 */
	void SetVisible(BOOL _visible){m_bVisible=_visible;}


	/**\brief
	 * 取得是否显示
	 */
	BOOL GetVisible(void){return m_bVisible;}


	/**\brief
	 * 设置是否透明
	 */
	void SetBlend(BOOL _blend){m_bBlend=_blend;}


	/**\brief
	 * 是否透明处理
	 */
	BOOL GetBlend(){return m_bBlend;}


	/**\brief
	 * 设置窗口颜色,可以用color.a来控制窗口的透明度
	 */
	void SetColor(SColor _color){m_sColor=_color;}


	/**\brief
	 * 设置窗口所在矩形
	 */
	void SetRect(SRect _rect){m_sRect=_rect;}


	/**\brief
	 * 获得背景颜色
	 */
	SColor GetColor(void){return m_sColor;}


	/**\brief
	 * 获得窗口所在矩形
	 */
	SRect GetRect(void){return m_sRect;}


	/**\brief
	 * 窗口是否处于活动状态
	 */
	BOOL IsActive(void);
	


	/**\brief
	 * 移动窗口
	 */
	void move(int _dx,int _dy){m_sRect.x+=_dx;m_sRect.y+=_dy;}


	/**\brief
	 * 载入纹理
	 *
	 *\param	_filename	纹理文件名
	 *\param	_num		地板砖纹理每行砖的块数
	 *\param	_bBlend		是否透明处理
	 */
	void LoadTexture(char* _filename,int _num,BOOL _bBlend);


	/**\brief
	 * 绘制窗口
	 */
	void draw(void);


	/**\brief
	 * 从INI文件载入窗口数据
	 */
	virtual BOOL LoadIni(char* _filename,char* _index);


private:
	//! 是否用纹理背景
	BOOL	m_bBmpBack;

	//! 地板砖纹理每行砖的块数
	int		m_nNum;
	
	//! 窗口的纹理
	CTexture m_cTexture;
	
	
	/**\brief
	 * 不光担负着绘制窗口的责任，还有窗口的主循环
	 */
	virtual void OnDraw(void){}


protected:
	//! 窗口背景颜色
	SColor	m_sColor;

	//! 窗口所在矩形
	SRect	m_sRect;

	//! 是否透明处理（使黑色部分透明）
	BOOL	m_bBlend;

	//! 是否显示
	BOOL	m_bVisible;

	//! 纹理文件名
	char*	m_chBackTexFile;

	//! 窗口的状态，可以代回一个值
	int m_iState;

};


#endif