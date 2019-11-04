//========================================================
/**
*  @file      3DSLoader.h
*
*  项目描述： 3DS文件载入
*  文件描述:  3DS文件载入类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-12-06	
*  修改日期： 2006-12-12
*
*/     
//========================================================

#ifndef __3DSLOADER_H__
#define __3DSLOADER_H__

#include "vector.h"
#include "CBMPLoader.h"
#include <math.h>
#include <vector>                    /** 包含向量容器 */
using namespace std;         

/** 下面是定义一些块的ID号 */

/**  基本块(Primary Chunk)，位于文件的开始 */
#define PRIMARY       0x4D4D

/**  主块(Main Chunks) */
#define OBJECTINFO    0x3D3D		/**< 网格对象的版本号 */
#define VERSION       0x0002		/**< .3ds文件的版本 */
#define EDITKEYFRAME  0xB000		/**< 所有关键帧信息的头部 */

/**<  对象的次级定义 */
#define MATERIAL	  0xAFFF		/** 纹理信息 */
#define OBJECT		  0x4000		/** 对象的面、顶点等信息 */

/**  材质的次级定义 */
#define MATNAME       0xA000		/**< 材质名称 */
#define MATDIFFUSE    0xA020		/**< 对象/材质的颜色 */
#define MATMAP        0xA200		/**< 新材质的头部 */
#define MATMAPFILE    0xA300		/**< 保存纹理的文件名 */
#define OBJ_MESH	  0x4100		/**< 新的网格对象 */


/** 网格对象的次级定义 */
#define OBJ_VERTICES  0x4110		/**< 对象顶点 */
#define OBJ_FACES	  0x4120		/**< 对象的面 */
#define OBJ_MATERIAL  0x4130		/**< 对象的材质 */
#define OBJ_UV		  0x4140		/**< 对象的UV纹理坐标 */


/** 面的结构定义 */
struct tFace		
{	
	int vertIndex[3];			    /**< 顶点索引 */
	int coordIndex[3];			    /**< 纹理坐标索引 */
};

/** 材质信息结构体 */
struct tMatInfo
{	
	char  strName[255];			   /**< 纹理名称 */
	char  strFile[255];			   /**< 纹理文件名称 */
	BYTE  color[3];				   /**< 对象的RGB颜色 */
	int   texureId;				   /**< 纹理ID */
	float uTile;				   /**< u 重复 */
	float vTile;				   /**< v 重复 */
	float uOffset;			       /**< u 纹理偏移 */
	float vOffset;				   /**< v 纹理偏移 */
} ;

/** 对象信息结构体 */
struct t3DObject	
{	
	int  numOfVerts;		      /**<	模型中顶点的数目 */
	int  numOfFaces;			  /**< 模型中面的数目 */
	int  numTexVertex;			  /**< 模型中纹理坐标的数目 */
	int  materialID;			  /**< 纹理ID */
	bool bHasTexture;			  /**< 是否具有纹理映射 */
	char strName[255];			  /**< 对象的名称 */
	Vector3  *pVerts;			  /**< 对象的顶点 */
	Vector3  *pNormals;		      /**< 对象的法向量 */
	Vector2  *pTexVerts;		  /**< 纹理UV坐标 */
	tFace *pFaces;				  /**< 对象的面信息 */
};

/** 模型信息结构体 */
struct t3DModel	
{	int numOfObjects;			  /**< 模型中对象的数目 */
	int numOfMaterials;			  /**< 模型中材质的数目 */
	vector<tMatInfo>pMaterials;	  /**< 材质链表信息 */
	vector<t3DObject> pObject;	  /**< 模型中对象链表信息 */
};

/** 块信息的结构 */
struct tChunk	
{	
	unsigned short int ID;		  /**< 块的ID */
	unsigned int length;		  /**< 块的长度 */
	unsigned int bytesRead;		  /**< 需要读的块数据的字节数 */
};

#define MAX_TEXTURES  100

/** 3DS文件载入类 */
class C3DSLoader
{
public:
	/** 构造函数 */
	C3DSLoader();								
	virtual ~C3DSLoader();
	void Draw();//显示3ds模型
	void Init(char *filename);

private:
	/** 读一个字符串 */
	int  GetString(char *);								
	
	/** 装载3ds文件到模型结构中 */
	bool Import3DS(t3DModel *pModel, char *strFileName);

	void LoadTexture(char* filename, GLuint textureArray[], GLuint textureID);//  从文件中创建纹理
	
	/** 读取一个块 */
	void ReadChunk(tChunk *);		

	/** 读取下一个块 */
	void ReadNextChunk(t3DModel *pModel, tChunk *);

	/** 读取下一个对象 */
	void ReadNextObjChunk(t3DModel *pModel,t3DObject *pObject,tChunk *);

	/** 读取下一个材质块 */
	void ReadNextMatChunk(t3DModel *pModel, tChunk *);	

	/** 读取对象颜色的RGB值 */
	void ReadColor(tMatInfo *pMaterial, tChunk *pChunk);

	/** 读取对象的顶点信息 */
	void ReadVertices(t3DObject *pObject, tChunk *);

	/** 读取对象的面信息 */
	void ReadVertexIndices(t3DObject *pObject,tChunk *);

	/** 读取对象的纹理坐标 */
	void ReadUVCoordinates(t3DObject *pObject,tChunk *);

	/** 读取赋予对象的材质 */
	void ReadObjMat(t3DModel *pModel,t3DObject *pObject,tChunk *pPreChunk);

	/** 计算对象顶点的法向量 */
	void ComputeNormals(t3DModel *pModel);	

	/** 释放内存,关闭文件 */
	void CleanUp();										

	FILE	     *m_FilePointer;			  /**< 文件指针 */					
	tChunk	     *m_CurrentChunk;             /**< 读取过程中当前块 */
	tChunk	     *m_TempChunk;                /**< 临时块 */
    CBMPLoader   m_BMPTexture;                /**< 载入位图 */
	GLuint       m_textures[MAX_TEXTURES];    /**< 纹理 */
	t3DModel     m_3DModel;                   /**< 模型 */
};

#endif