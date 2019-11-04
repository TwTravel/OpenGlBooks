//========================================================
/**
*  @file      Dialog.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  对话框类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _CGW_DIALOG_H__
#define _CGW_DIALOG_H__

#include "window.h"
#include "button.h"


/**\brief
 * 对话框类
 */
class Dialog : public CWindow
{
public:
	Dialog(void);
	~Dialog(void);

	/**\brief
	 * 获得按钮数目
	 */
	unsigned int GetButtonNum(){return m_nButtonNum;}


	/**\brief
	 * 从INI文件载入对话框
	 */
	BOOL LoadIni(char* _filename,char* _index);


private:
	//! 菜单上的按钮数目
	unsigned int m_nButtonNum;

	//! 按钮指针
	CButton*	m_pButton;


	/**\brief
	 * 对话的渲染
	 */
	void OnDraw();
};

#endif