//========================================================
/**
*  @file      Terrain.cpp
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

#include "stdafx.h"
#include "terrain.h"
#include "IniFile.h"

CTerrain Terrain;

CTerrain* CTerrain::m_pTerrain = NULL;

CTerrain::CTerrain()
{
	m_nWidth = 64.0;
	scanDepth = 16.0;
	m_nCellWidth = 32;
	fogColor[0] = 0.75f;
	fogColor[1] = 0.9f;
	fogColor[2] = 1.0f;
	fogColor[3] = 1.0f;
	m_pTerrain = this;
}

BOOL CTerrain::Init(int _width,char* TexFile)
{
	m_nWidth=_width;

	// 生成地形
	heightMap = new short[(m_nWidth+1)*(m_nWidth+1)];

	for(unsigned int j=0;j<=m_nWidth;++j)
		for(unsigned int i=0;i<=m_nWidth;++i)
	{
		heightMap[i*m_nWidth+j]=0;
		//heightMap[i*m_nWidth+j]=20*sin(j)*cos(i*i);
	}

	// 装入纹理
	terrainTex.InitTexture(TexFile,TRUE);

	return TRUE;
}

BOOL CTerrain::Load(char* filename)
{
	FILE* fp;
	fp=fopen(filename,"rb");
	if(fp==NULL)return FALSE;
	fread(&m_nWidth,sizeof(unsigned int),1,fp);
	if(m_nWidth<=0)return FALSE;
	fread(&m_nCellWidth,sizeof(unsigned int),1,fp);
	if(m_nCellWidth<=0)return FALSE;

	heightMap=new short[(m_nWidth+1)*(m_nWidth+1)];
	fread(heightMap,sizeof(short),(m_nWidth+1)*(m_nWidth+1),fp);

	char t_chTexFile[20];
	strcpy(t_chTexFile,"scene/");
	char str[15];
	fread(str,sizeof(char),15,fp);
	strcat(t_chTexFile,str);
	terrainTex.InitTexture(t_chTexFile,TRUE);

	fclose(fp);
	return TRUE;
}

float CTerrain::GetHeight(float x, float z)
{
	float projCameraX, projCameraZ;

	projCameraX = x / m_nCellWidth;
	projCameraZ = z / m_nCellWidth;
	// 计算高程坐标(Col0, Row0)，(Col1, Row1)
	int col0 = int(projCameraX);
	int row0 = int(projCameraZ);
	unsigned int col1 = col0 + 1;
	unsigned int row1 = row0 + 1;
	// 确保单元坐标不超过高程以外
	if (col1 > m_nWidth)col1 = 0;
	if (row1 > m_nWidth)row1 = 0;

	// 获取单元的四个角的高度
	float h00 = (float)(heightMap[col0 + row0*m_nWidth]);
	float h01 = (float)(heightMap[col1 + row0*m_nWidth]);
	float h11 = (float)(heightMap[col1 + row1*m_nWidth]);
	float h10 = (float)(heightMap[col0 + row1*m_nWidth]);

	// 计算机摄像机相对于单元格的位置
	float tx = projCameraX - float(col0);
	float ty = projCameraZ - float(row0);

	// 下一步是进行双线性插值
	float txty = tx * ty;

	float final_height	= h00 * (1.0f - ty - tx + txty)
						+ h01 * (tx - txty)
						+ h11 * txty
						+ h10 * (ty - txty);
	return final_height;
}


void CTerrain::Render(void)
{
	int z, x;
	vector3d camera_pos;
	CCamera::GetCameraPointer()->GetCameraPos(&camera_pos);
/**/
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_START, scanDepth * 0.4f);
	glFogf(GL_FOG_END, scanDepth * 40.f);
	glHint(GL_FOG_HINT, GL_FASTEST);
	glEnable(GL_FOG);
/**/
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.f,1.f,1.f);

	float dist_to_role=CCamera::GetCameraPointer()->GetDestToRole();
	for(z=(int)((camera_pos.z-dist_to_role)/m_nCellWidth-scanDepth),z=z<0?0:z;
		(z<(camera_pos.z-dist_to_role)/m_nCellWidth+scanDepth)&&(z<m_nWidth);
		++z)
	{
		terrainTex.Bind(2);
		glBegin(GL_TRIANGLE_STRIP);
		for(x=(int)(camera_pos.x/m_nCellWidth-scanDepth),x=x<0?0:x;(x<camera_pos.x/m_nCellWidth+scanDepth)&&x<m_nWidth;++x)
		{
			glTexCoord2f((float)x/(float)m_nWidth,(float)z/(float)m_nWidth);
			glVertex3i(x*m_nCellWidth,		heightMap[x+z*m_nWidth],		z*m_nCellWidth);

			glTexCoord2f((float)x/(float)m_nWidth,(float)(z+1)/(float)m_nWidth);
			glVertex3i(x*m_nCellWidth,		heightMap[x+(z+1)*m_nWidth],	(z+1)*m_nCellWidth);

			glTexCoord2f((float)(x+1)/(float)m_nWidth,(float)z/(float)m_nWidth);
			glVertex3i((x+1)*m_nCellWidth,	heightMap[x+1+z*m_nWidth],	z*m_nCellWidth);

			glTexCoord2f((float)(x+1)/(float)m_nWidth,(float)(z+1)/(float)m_nWidth);
			glVertex3i((x+1)*m_nCellWidth,	heightMap[x+1+(z+1)*m_nWidth],(z+1)*m_nCellWidth);
		}
		glEnd();
	}
}
