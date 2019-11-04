//========================================================
/**
*  @file      Font.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  字体类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================


#include "stdafx.h"
#include "Font.h"


/**\brief
 * 初始化字体
 */
GLvoid Font::InitFont(void)
{
	HFONT font;
	HFONT oldfont;

	HDC m_hdc = wglGetCurrentDC();
	base = glGenLists(96);
	font = CreateFont( -24,-14,0,0,
					   FW_NORMAL,
					   FALSE,FALSE,FALSE,
					   ANSI_CHARSET,OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS,
					   ANTIALIASED_QUALITY,
					   FF_DONTCARE|DEFAULT_PITCH,
					   "Courier New");

	oldfont = (HFONT)SelectObject(m_hdc,font);
	wglUseFontBitmaps(m_hdc,32,96,base);
	SelectObject(m_hdc , oldfont);
	DeleteObject(font);
}


/**\brief
 * 显示字
 *
 *\param	x,y		字所显示的位置
 *\param	string	要显示的字符串
 */
void Font::Print(int x,int y,char *string)
{
	glColor3f(color.r,color.g,color.b);
	glRasterPos2i(x,y);
	glPushAttrib(GL_LIST_BIT);
	glListBase(base - 32);
	glCallLists(strlen(string), GL_UNSIGNED_BYTE, string);	
	glPopAttrib();
	glColor3f(1.0f , 1.0f , 1.0f);
}


/**\brief
 * 释放资源
 */
void Font::KillFont(GLvoid)
{
	glDeleteLists(base , 96);
}



/////////////////////////////////////////////

HZFont* HZFont::m_pFont=NULL;
CharacterBinary HZFont::cb;

HZFont::HZFont(void):color(SColor(0.f,0.f,0.f,0.f))
{
	m_pFont=this;
}


HZFont::~HZFont(void)
{
}
