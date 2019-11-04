//========================================================
/**
*  @file      PlantMgr.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  树木管理类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _TREE_MANAGE_H_
#define _TREE_MANAGE_H_

#define TREE_NUM   10

#include "Plant.h"
#include "terrain.h"
#include <vector>

using namespace std;
class CTreeMgr
{
public:
	CTreeMgr(){}
	~CTreeMgr(){}
	int GetTreeNum(){return m_pTrees.size();}
	void Render_Pick(int id){m_pTrees[id].Render();}
	void Render();
	BOOL LoadIni(char* filename,char* index);


private:
	Plant tree;
	vector<Plant> m_pTrees;
};

#endif