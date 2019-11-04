//========================================================
/**
*  @file      GLFont.h
*
*  项目描述： OpenGL字体
*  文件描述:  位图字体类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-09-13	
*  修改日期： 2006-11-21
*
*/     
//========================================================
#ifndef	__GLFONT_H__
#define	__GLFONT_H__


/** 定义位图字体类 */
class GLFont															
{
public:
    
	/** 构造函数和析构函数 */
	GLFont();
	~GLFont();
    
	/** 成员方法 */
	bool InitFont(HDC hDC, char *fontName,int Size );  /**< 初始化字体 */
	void PrintText(char *string, float x, float y);    /**< 在位置(x,y)处绘制字符串string */
	void KillGLFont();                                 /**< 删除字体 */

protected:
	unsigned int listBase;                             /**< 绘制字体的显示列表的开始位置 */
	
};

#endif	// __GLFONT_H__