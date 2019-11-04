//========================================================
/**
*  @file      OpenAirScene.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  游戏室外场景类
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
#include "openairscene.h"

OpenAirScene::OpenAirScene(void)
{
}

OpenAirScene::~OpenAirScene(void)
{
}

/**\brief
 * 场景的初始化
 */
BOOL OpenAirScene::Init(void)
{
	if(!m_cSnow.Init())	return FALSE;
	if(!trees.LoadIni("ini/trees.ini","trees"))	return FALSE;
	if(!skybox.Init())	return FALSE;
	if(!water.Init())	return FALSE;

	return TRUE;
}


/**\brief
 * 从INI文件载入数据
 */
BOOL OpenAirScene::LoadIni(char* _filename,char* _index)
{
	return TRUE;
}


/**\brief
 * 更新场景的数据
 */
void OpenAirScene::UpdateData(void)
{
}


/**\brief
 * 场景的渲染
 */
void OpenAirScene::Render(void)
{	
	//water.Render();
	skybox.Render();
	m_cSnow.Render();
	trees.Render();
}