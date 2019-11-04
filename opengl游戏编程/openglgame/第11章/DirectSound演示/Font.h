//========================================================
/**
*  @file      Font.h
*
*  项目描述： DirectSound声音演示
*  文件描述:  字体类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-09-13	
*  修改日期： 2006-12-02
*
*/     
//========================================================
#ifndef	__GLFONT_H__
#define	__GLFONT_H__

#include "stdafx.h"

/** 定义字体类 */
class GLFont															
{
public:
    /** 构造函数和析构函数 */
	GLFont();
	~GLFont();
    ///成员方法
	bool InitFont();  /**< 初始化字体 */
	void PrintText(char *string, float x, float y); /**< 在(x,y)处输出string内容 */
	
protected:
	HFONT m_hFont;  /**< 字体句柄 */
		
};

#endif	// __GLFONT_H__