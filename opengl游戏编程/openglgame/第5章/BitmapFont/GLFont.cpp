/**
*  @file      GLFont.cpp
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

#include<windows.h>              /**< 包含windows头文件 */
#include<gl/gl.h>                /**< 包含gl头文件 */

#include "GLFont.h"

GLFont::GLFont()
{
}

/** 析构函数 */
GLFont::~GLFont()
{
	KillGLFont();          /**< 调用KillFont()删除字体 */
}

/** 初始化字体 */
bool GLFont::InitFont(HDC hDC, char *fontName,int Size )
{
   HFONT oldFont;              /**< 保存旧的字体句柄 */
   HFONT hFont;                /**< 字体句柄 */
   
   /** 创建96个显示列表 */
   listBase = glGenLists(96);
   if(listBase == 0)          /**< 分配失败,则返回 */
	   return false;
   
    /** 创建字体 */
    hFont = CreateFont(Size,					/**< 字体高度 */
						0,						/**< 字体宽度 */
						0,						/**< 字体的旋转角度 Angle Of Escapement */
						0,						/**< 字体底线的旋转角度Orientation Angle */
						FW_BOLD,				/**< 字体的重量 */
						FALSE,					/**< 是否使用斜体 */
						FALSE,					/**< 是否使用下划线 */
						FALSE,					/**< 是否使用删除线 */
						ANSI_CHARSET,			/**< 设置字符集 */
						OUT_TT_PRECIS,			/**< 输出精度 */
						CLIP_DEFAULT_PRECIS,	/**< 裁剪精度 */
						ANTIALIASED_QUALITY,	/**< 输出质量 */
				FF_DONTCARE|DEFAULT_PITCH,		/**< Family And Pitch */
						fontName);				/**< 字体名称 */
	 if(!hFont)
		return false;                           /**< 创建字体失败则返回 */
 
	oldFont = (HFONT)SelectObject(hDC, hFont);  /**< 选择我们需要的字体 */
	wglUseFontBitmaps(hDC, 32, 96, listBase);   /**< 创建96个显示列表，绘制从ASCII码为32-128的字符 */
	 
   return true;
}



/** 在指定位置输出字符串 */
void GLFont::PrintText(char *string, float x, float y)
{
   /** 错误检查.如果listBase为0或string为NULL则不用绘制直接返回 */
   if((listBase == 0 || string == NULL))
      return ;

   glPushMatrix();
	 glDisable(GL_DEPTH_TEST); /**< 关闭深度测试 */
	 glDisable(GL_LIGHTING);   /**< 关闭光照 */

	 glRasterPos2f(x, y);      /**< 将光栅位置移动到指定点 */
	
	 /** 将字符串输出到屏幕上 */
	glPushAttrib(GL_LIST_BIT);                       /**< 把显示列表属性压入属性堆栈 */
		glListBase(listBase - 32);                   /**< 设置显示列表的基值 */
		
		/** 执行多个显示列表输出字符串 */
		glCallLists((GLsizei)strlen(string), GL_UNSIGNED_BYTE, string); 
	glPopAttrib();                                   /**< 把显示列表属性弹出属性堆栈 */
    
	/** 恢复原来状态 */
	glEnable(GL_LIGHTING);    /**< 启用光照 */
	glEnable(GL_DEPTH_TEST);  /**< 启用深度测试 */
   glPopMatrix();
 
}

/**< 删除字体 */
void GLFont::KillGLFont()
{
   /** 删除字体显示列表 */
   if(listBase != 0)
      glDeleteLists(listBase, 96);
}

