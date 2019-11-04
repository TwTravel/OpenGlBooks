//========================================================
/**
*  @file      Font.h
*
*  项目描述： OpenGL显示中文
*  文件描述:  字体类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-09-13	
*  修改日期： 2006-11-24
*
*/     
//========================================================
#ifndef	__FONT_H__
#define	__FONT_H__

#pragma warning(disable: 4267)  /**< 禁止编译器出现类型转换的警告 */
#pragma warning(disable: 4244)

/** 定义字体类 */
class GLFont															
{
public:
    
	/** 构造函数和析构函数 */
	GLFont();
	~GLFont();
     
	/** 初始化字体 */
	bool InitFont(); 
	
	/** 在(x,y)处输出string内容 */
	void PrintText(char *string, float x, float y); 
	
protected:
	HFONT m_hFont;     /**< 字体句柄 */
		
};

#endif	// __GLFONT_H__