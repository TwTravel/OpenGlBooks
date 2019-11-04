//========================================================
/**
*  @file      ProgressBar.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  进度条类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include "StdAfx.h"
#include "Window.h"


/**\brief
 * 进度条类
 */
class CProgressBar : public CWindow
{
public:
	CProgressBar();
	~CProgressBar(){}


	/**\brief
	 * 从INI文件载入窗口数据
	 */
	virtual BOOL LoadIni(char* filename,char* index);


	/**\brief
	 * 设置进度条的进度
	 */
	void	SetPos(float p){pos=p;}


	/**\brief
	 * 设置进度条边框的宽度
	 */
	void	SetBorderWidth(int b){border=b;}


	/**\brief
	 * 设置进度条的颜色
	 */
	void	SetBarColor(SColor c){BarColor=c;}
	

	/**\brief
	 * 获得进度条的进度
	 */
	float	GetPos(){return pos;}
	
	
	/**\brief
	 * 取得边框的宽度
	 */
	int		GetBorderWidth(){return border;}
	
	
	/**\brief
	 * 取得进度条的颜色
	 */
	SColor	GetBarColor(){return BarColor;}



private:
	//! 用来记录和设置进度条的进度
	float	pos;
	
	//边框宽度
	int		border;
	
	//条的颜色
	SColor	BarColor;
	
	
	/**\brief
	 * 绘制进度条
	 *
	 * 虚函数，由其父类CWindow的draw()来调用
	 */
	virtual void OnDraw(void);
};


#endif