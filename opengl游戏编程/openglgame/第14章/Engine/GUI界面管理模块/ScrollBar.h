//========================================================
/**
*  @file      ScrollBar.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  滚动条类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _SCROLL_BAR_H_
#define _SCROLL_BAR_H_

#include "StdAfx.h"
#include "button.h"
#include "IniFile.h"


/**\brief
 * 滚动条类
 */
class CScrollBar  
{
public:
	CScrollBar();
	~CScrollBar();


	//! TRUE:水平　FALSE:垂直
	BOOL	style;
	
	//位置：左下角坐标
	int		x,y;
	
	//滚动条宽度
	int		width;
	
	//滚动条的长度
	int		MaxLength;
	
	//滑块的长度
	int		CellLength;


	/**\brief
	 * 设置滑块位置
	 */
	void	SetPos(float p){pos=p;}
	
	
	/**\brief
	 * 获得滑块位置（pos值）
	 */
	float	GetPos(){return pos;}
	
	
	/**\brief
	 * 从INI文件载入
	 */
	BOOL	LoadIni(char* filename,char* index);
	
	
	/**\brief
	 * 绘制滚动条
	 */
	void	draw(void);
	
	
	/**\brief
	 * 滚动条是否处于活动状态，即光标是否在其上
	 */
	BOOL	IsActive(void);


private:
	//! 滑块的位置，用百分数表示
	float	pos;

	//! 滑块每次移动的最小距离
	float	MinPos;

	//! 向上（前）按钮
	CButton*	btn1;

	//! 向下（后）按钮
	CButton*	btn2;

	//! 滑块
	CButton*	CenBtn;

};


#endif