//========================================================
/**
*  @file      window.cpp
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

#include "stdafx.h"
#include "Window.h"


CWindow::CWindow(void):m_nNum(1),m_bBlend(FALSE),m_chBackTexFile(NULL),
					m_bVisible(TRUE),m_bBmpBack(FALSE)
{
	m_sRect=SRect(0,0,100,100);
	m_sColor=SColor(1.0f,1.0f,1.0f,1.0f);
}


CWindow::CWindow(SRect _rect):m_nNum(1),m_bBlend(FALSE),m_chBackTexFile(NULL),
					m_bVisible(TRUE),m_bBmpBack(FALSE)
{
	m_sRect=_rect;
	m_sColor=SColor(1.0f,1.0f,1.0f,1.0f);
}


/**\brief
 * 载入纹理
 *
 *\param	_filename	纹理文件名
 *\param	_num		地板砖纹理每行砖的块数
 *\param	_bBlend		是否透明处理
 */
void CWindow::LoadTexture(char* _filename,int _num,BOOL _bBlend)
{
	m_nNum=_num;
	m_bBmpBack=TRUE;
	m_chBackTexFile=_filename;
	m_bBlend=_bBlend;

	m_cTexture.InitTexture(_filename,m_bBlend);	//装入纹理
}


/**\brief
 * 窗口是否处于活动状态
 */
BOOL CWindow::IsActive(void)
{
	SPoint2 p=SYS_MOUSE->GetMousePos();

	if(		p.x>=m_sRect.x&&p.x<=m_sRect.x+m_sRect.width
		&&	p.y>=m_sRect.y&&p.y<=m_sRect.y+m_sRect.height)
		return TRUE;
	else return FALSE;
}


/**\brief
 * 从INI文件载入窗口数据
 */
BOOL CWindow::LoadIni(char* _filename,char* _index)
{
	CIniFile Ini(_filename);
	
	m_sRect.x=Ini.ReadInt(_index,"x");
	m_sRect.y=Ini.ReadInt(_index,"y");
	m_sRect.width=Ini.ReadInt(_index,"width");
	m_sRect.height=Ini.ReadInt(_index,"height");

	m_sColor.r=PERSENT(Ini.ReadInt(_index,"r"));
	m_sColor.g=PERSENT(Ini.ReadInt(_index,"g"));
	m_sColor.b=PERSENT(Ini.ReadInt(_index,"b"));
	m_sColor.a=PERSENT(Ini.ReadInt(_index,"a"));

	m_chBackTexFile =Ini.ReadText(_index,"BackTexFile");
	m_bBlend=Ini.ReadInt(_index,"bBlend");

	if(m_chBackTexFile==NULL)return FALSE;
	this->LoadTexture(m_chBackTexFile,1,m_bBlend);

	return TRUE;
}


/**\brief
 * 绘制窗口
 */
void CWindow::draw()
{
	if(m_bVisible==FALSE)return;

	if(m_bBmpBack)
	{
		glEnable(GL_TEXTURE_2D);
		m_cTexture.Bind(0);

		if(m_bBlend==TRUE)
		{
			glEnable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER , 0.0f);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		}

		glBegin(GL_QUADS);
		glColor4f(m_sColor.r,m_sColor.g,m_sColor.b,m_sColor.a);
			glTexCoord2f(0.0f,0.0f);
			glVertex2i(m_sRect.x,m_sRect.y);

			glTexCoord2f((float)m_nNum,0.0f);
			glVertex2i(m_sRect.x+m_sRect.width,m_sRect.y);

			glTexCoord2f((float)m_nNum,(float)m_nNum);
			glVertex2i(m_sRect.x+m_sRect.width,m_sRect.y+m_sRect.height);

			glTexCoord2f(0.0f,(float)m_nNum);
			glVertex2i(m_sRect.x,m_sRect.y+m_sRect.height);
		glEnd();


		if(m_bBlend==TRUE)
		{
			glDisable(GL_BLEND);
			glDisable(GL_ALPHA_TEST);
		}
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		if(m_bBlend==TRUE)glEnable(GL_BLEND);
		glBegin(GL_QUADS);
		glColor4f(m_sColor.r,m_sColor.g,m_sColor.b,m_sColor.a);
			glVertex2i(m_sRect.x,m_sRect.y);
			glVertex2i(m_sRect.x+m_sRect.width,m_sRect.y);
			glVertex2i(m_sRect.x+m_sRect.width,m_sRect.y+m_sRect.height);
			glVertex2i(m_sRect.x,m_sRect.y+m_sRect.height);
		glEnd();
		if(m_bBlend==TRUE)glDisable(GL_BLEND);
	}

	OnDraw();
}