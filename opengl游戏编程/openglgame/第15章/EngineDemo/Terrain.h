//========================================================
/**
*  @file      Terrain.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  地形类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================



#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include "texture.h"
#include "camera.h"


/**\brief
 * 地形类
 */
class CTerrain
{
public:
	CTerrain();
	~CTerrain(){delete [] heightMap; }
	
	BOOL Init(int _width,char* TexFile);
	BOOL Load(char* filename);

	static CTerrain* GetSysTerrain(){return m_pTerrain;}
	
	/**\brief
	 * 取得地形的宽
	 */
	int GetWidth(){return m_nWidth;}

	int GetCellWidth(){return m_nCellWidth;}

	void Render(void);

	float GetHeight(float x, float z);

private:
	//! 地形大小为m_iWidth*m_iWidth，最好是2^n
	unsigned int m_nWidth;

	//! 动态高程映射
	short*        heightMap;      

	//! 每个格子的宽度
	unsigned int m_nCellWidth;

	// 地形上的多重纹理
	CTexture terrainTex;

	// 雾的颜色
	float fogColor[4];

	float scanDepth;
	
	static CTerrain* m_pTerrain;
};


#endif