//========================================================
/**
*  @file      AVIScene.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  视频动画场景类————用到片头动画和场间动画
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
#include "aviscene.h"
#include "inifile.h"

AVIScene::AVIScene(void):m_strAVIFile(NULL)
{
}

AVIScene::~AVIScene(void)
{
	m_cAVI.Stop();
	_DELETE(m_strAVIFile);
}


/**\brief
 * 场景的初始化
 */
BOOL AVIScene::Init(void)
{
	if(!m_cAVI.Init(CInput::GetSysHWnd()))return FALSE;

	return TRUE;
}


/**\brief
 * 从INI文件载入数据
 */
BOOL AVIScene::LoadIni(char* _filename,char* _index)
{
	CIniFile Ini(_filename);
	m_strAVIFile=Ini.ReadText(_index,"AVIFile");

	return TRUE;
}


/**\brief
 * 更新场景数据
 */
void AVIScene::UpdateData(void)
{
}


/**\brief
 * 场景的渲染
 */
void AVIScene::Render(void)
{
	if(!m_strAVIFile)return;

	m_cAVI.Load(m_strAVIFile);
	m_cAVI.Play();
	if(SYS_KEYBOARD->KeyDown(DIK_ESCAPE)||SYS_MOUSE->ButtonDown(1))m_cAVI.Stop();

}
