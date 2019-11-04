//========================================================
/**
*  @file      CheckBox.cpp
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


#include "stdafx.h"
#include "CHeckBox.h"


/**\brief
 * 从INI文件载入
 */
BOOL CCheckBox::LoadIni(char* filename,char* index)
{
	CIniFile Ini(filename);

	m_sRect.x=Ini.ReadInt(index,"x");
	m_sRect.y=Ini.ReadInt(index,"y");
	m_sRect.width=Ini.ReadInt(index,"width");
	m_sRect.height=Ini.ReadInt(index,"height");

	bChecked=Ini.ReadInt(index,"style");

	return TRUE;
}



/**\brief
 *  绘制
 */
void CCheckBox::OnDraw(void)
{
	if(IsActive()==TRUE)//光标在按钮上
	{
		this->SetColor(color2);
		
		if(CInput::GetSysInput()->FirstMouseDownL())
		{
			m_cSound.LoadSoundFX(DOWNL_SOUND_NAME);
			m_cSound.Play();
		}
		
		if(CInput::GetSysInput()->Clicked())bChecked=!bChecked;
	}
	else this->SetColor(color1);//光标不在按钮上


	glLineWidth(2.f);
	////////////////////////////////////
	glColor3f(1.f,0.f,0.f);
	if(bChecked)//绘制对号
	{
		glBegin(GL_LINE_STRIP);
			glVertex2i(m_sRect.x,m_sRect.y+m_sRect.height/2);
			glVertex2i(m_sRect.x+m_sRect.width/2,m_sRect.y);
			glVertex2i(m_sRect.x+m_sRect.width,m_sRect.y+m_sRect.height);
		glEnd();
	}
	else//绘制错号
	{
		glBegin(GL_LINES);
			glVertex2i(m_sRect.x,m_sRect.y);
			glVertex2i(m_sRect.x+m_sRect.width,m_sRect.y+m_sRect.height);
			glVertex2i(m_sRect.x+m_sRect.width,m_sRect.y);
			glVertex2i(m_sRect.x,m_sRect.y+m_sRect.height);
		glEnd();
	}
	////////////////////////////////////
	glColor3f(1.f,0.f,0.f);	//绘制边框
	glBegin(GL_LINE_LOOP);
		glVertex2i(m_sRect.x,m_sRect.y);
		glVertex2i(m_sRect.x+m_sRect.width,m_sRect.y);
		glVertex2i(m_sRect.x+m_sRect.width,m_sRect.y+m_sRect.height);
		glVertex2i(m_sRect.x,m_sRect.y+m_sRect.height);
	glEnd();
	////////////////////////////////////
	glLineWidth(1.f);

}
