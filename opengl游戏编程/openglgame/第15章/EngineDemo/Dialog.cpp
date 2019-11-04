//========================================================
/**
*  @file      Dialog.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  对话框类
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
#include "dialog.h"

Dialog::Dialog(void):m_pButton(NULL),m_nButtonNum(0)
{
}

Dialog::~Dialog(void)
{
	_DELETE_ARRAY(m_pButton);
}



/**\brief
 * 从INI文件载入对话框
 */
BOOL Dialog::LoadIni(char* _filename,char* _index)
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

	if(m_chBackTexFile!=NULL)this->LoadTexture(m_chBackTexFile,1,m_bBlend);

	m_nButtonNum=Ini.ReadInt(_index, "ButtonNum");
	m_pButton=new CButton[m_nButtonNum];

	//初始化按钮
	char* strFile;
	char* strIndex;
	for(unsigned int i=0; i<m_nButtonNum; ++i)
	{
		strFile = Ini.ReadText(_index, i);
		strIndex = Ini.ReadData(_index,i);
		m_pButton[i].LoadIni(strFile, strIndex);
		_FREE(strFile);
		_FREE(strIndex);
	}

	return TRUE;
}



/**\brief
 * 对话的渲染
 */
void Dialog::OnDraw()
{
	//---------------------------------
	//绘制按钮
	for(unsigned int i=0;i<m_nButtonNum;++i)m_pButton[i].draw();

	for(unsigned int i=0;i<m_nButtonNum;++i)
	{
		if(m_pButton[i].GetState()==BUTTON_DOWNL)this->m_iState=-1*(i+1);
		if(m_pButton[i].GetState()==BUTTON_CLICK)this->m_iState=i+1;
	}
}