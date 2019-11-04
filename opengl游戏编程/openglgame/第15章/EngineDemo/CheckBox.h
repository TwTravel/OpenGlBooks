//========================================================
/**
*  @file      CheckBox.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  复选框类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================


#ifndef _CHECK_BOX_H_
#define _CHECK_BOX_H_

#include "StdAfx.h"
#include "Button.h"


/**\brief
 * 复选框类
 */
class CCheckBox : public CButton  
{
public:
	CCheckBox(){bChecked=FALSE;}
	~CCheckBox(){}


	/**\brief
	 * 从INI文件载入
	 */
	virtual BOOL LoadIni(char* filename,char* index);


	/**\brief
	 * 是否被选中
	 */
	BOOL	GetState(){return bChecked;}


	/**\brief
	 * 设置复选框的状态
	 */
	void	SetState(BOOL _bChecked){bChecked=_bChecked;}

private:
	//! 是否被选
	BOOL bChecked;


	/**\brief
	 *  绘制
	 */
	virtual void OnDraw(void);

};

#endif