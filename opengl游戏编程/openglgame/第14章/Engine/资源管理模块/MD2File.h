//========================================================
/**
*  @file      MD2File.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  MD2文件载入类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _MD2_FILE_H_
#define _MD2_FILE_H_

#include "texture.h"
#include "mathlib.h"
#include <vector>
using namespace std;


/**\brief
 * MD2文件载入类
 */
class CMD2File
{
	struct MD2_HEADER
	{
		int m_iMagicNum;	//一般为0x49445032
		int m_iVersion;		//一般为8
		int m_iSkinWidthPx;
		int m_iSkinHeightPx;
		int m_iFrameSize;
		int m_iNumSkins;
		int m_iNumVertices;
		int m_iNumTexCoords;
		int m_iNumTriangles;
		int m_iNumGLCommands;
		int m_iNumFrames;
		int m_iOffsetSkins;
		int m_iOffsetTexcoords;
		int m_iOffsetTriangles;
		int m_iOffsetFrames;
		int m_iOffsetGlCommands;
		int m_iFileSize;
	};

	struct MD2_VERTEX
	{
		union
		{
			struct{float x,y,z;};
			float m_fVert[3];
		};
	};

	struct MD2_VERTEX_TEM
	{
		unsigned char vertex[3];
		unsigned char lightNormalIndex;
	} ;

	struct MD2_FRAME_TEM
	{
		float m_fScale[3];
		float m_fTrans[3];
		char m_caName[16];
		MD2_VERTEX_TEM md2_vertex_tem[1];
	};

	struct MD2_FRAME
	{
		MD2_VERTEX*	m_pVertices;
		MD2_FRAME()	{m_pVertices = NULL;}
		~MD2_FRAME(){if(m_pVertices){delete [] m_pVertices;m_pVertices = NULL;}}
	};

	struct MD2_TRIANGLE
	{
		short vertIndex[3];
		short textureIndex[3];
	};

	struct PLANE_EQ
	{
		float a,b,c,d;
	};

	struct MD2_PLANE
	{
		PLANE_EQ PlaneEq;
		unsigned int neigh[3];
		BOOL bVisible;
	};


	////////////////////////////////////////////////////////

public:	
	CMD2File(void);
	virtual ~CMD2File(void);

	BOOL Init(char *filename,char *texfilename,float fScale,float h);

	void RenderFrame(int currFrame,int nextFrame,float fInterpolation);

	void CastShadow(float lp[4],int nframe);

	float GetSize(){return size;}


private:
	void SetConnectivity(void);

	void CalPlane(void);

	void CalSize(void);

	BOOL LoadTexture(char *filename){if(!texture.InitTexture(filename))return FALSE;return TRUE;}

	BOOL Load(char*filename);

	int m_iNumGlCommands;

	int m_iFrameSize;

	int m_iNumTriangles;

	int m_iNumVertices;

	int m_iNumFrames;

	int *glCommands;

	MD2_FRAME*		m_pFrames;

	MD2_PLANE*		m_pPlanes;

	MD2_TRIANGLE*	m_pTriangles;

	int m_iCurrFrame;

	float m_fScale;

	float m_fInterpolation;

	CTexture texture;

	float m_fHeight;

	float size;

};


#endif