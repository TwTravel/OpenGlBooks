//========================================================
/**
*  @file      MD2Loader.h
*
*  项目描述： MD2文件载入
*  文件描述:  MD2文件载入类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-12-06	
*  修改日期： 2006-12-13
*
*/     
//========================================================
#ifndef __MD2LOADER_H__
#define __MD2LOADER_H__


#include "vector.h"
#include "CBMPLoader.h"
#include <string>
#include <vector>                    /** 包含向量容器 */
using namespace std;    

/** 下面定义一些最大值 */
#define MD2_MAX_TRIANGLES		4096
#define MD2_MAX_VERTICES		2048
#define MD2_MAX_TEXCOORDS		2048
#define MD2_MAX_FRAMES			512
#define MD2_MAX_SKINS			32
#define MD2_MAX_FRAMESIZE		(MD2_MAX_VERTICES * 4 + 128)

/** MD2文件头 */
struct tMd2Header
{ 
   int magic;					/**< 文件标志 */
   int version;					/**< 文件版本号 */
   int skinWidth;				/**< 纹理宽度 */
   int skinHeight;				/**< 纹理高度 */
   int frameSize;				/**< 每一帧的字节数 */
   int numSkins;				/**< 纹理数目 */
   int numVertices;				/**< 顶点数目(每一帧中) */
   int numTexCoords;			/**< 纹理坐标数目 */
   int numTriangles;			/**< 三角行数目 */
   int numGlCommands;			/**< gl命令数目 */
   int numFrames;				/**< 总帧数 */
   int offsetSkins;				/**< 纹理的偏移位置 */
   int offsetTexCoords;			/**< 纹理坐标的偏移位置 */
   int offsetTriangles;			/**< 三角形索引的偏移位置 */
   int offsetFrames;			/**< 第一帧的偏移位置 */
   int offsetGlCommands;		/**< OPenGL命令的偏移位置 */
   int offsetEnd;				/**< 文件结尾偏移位置 */
};

/** 顶点结构 */
struct tMd2AliasTriangle
{
   BYTE vertex[3];
   BYTE lightNormalIndex;
};

/** 三角形结构 */  
struct tMd2Triangle
{
   float vertex[3];
   float normal[3];
};

/** 面结构 */
struct tMd2Face
{
   short vertexIndices[3];
   short textureIndices[3];
};

/** 纹理坐标结构 */
struct tMd2TexCoord
{
   short u, v;
};

/** 帧结构 */
struct tMd2AliasFrame
{
   float scale[3];
   float translate[3];
   char name[16];
   tMd2AliasTriangle aliasVertices[1];
};

/** 关键帧结构 */
struct tMd2Frame
{
   char strName[16];
   tMd2Triangle *pVertices;
};


/** 面信息 */
struct tFace
{
	int vertIndex[3];			/**< 顶点索引 */
	int coordIndex[3];			/**< 纹理坐标索引 */
};

/** 材质信息结构体 */
struct tMaterialInfo
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

/** 动作信息结构体 */
struct tAnimationInfo
{
	char strName[255];			/**< 帧的名称 */
	int startFrame;				/**< 开始帧 */
	int endFrame;				/**< 结束帧 */
};

/** 对象信息结构体 */
struct t3DObject	
{	
	int  numOfVerts;		      /**< 模型中顶点的数目 */
	int  numOfFaces;			  /**< 模型中面的数目 */
	int  numTexVertex;			  /**< 模型中纹理坐标的数目 */
	int  materialID;			  /**< 纹理ID */
	bool bHasTexture;			  /**< 是否具有纹理映射 */
	char strName[255];			  /**< 对象的名称 */
	Vector3  *pVerts;			  /**< 对象的顶点 */
	Vector3  *pNormals;		      /**< 对象的法向量 */
	Vector2  *pTexVerts;		  /**< 纹理UV坐标 */
	tFace    *pFaces;		      /**< 对象的面信息 */
};

/** 模型信息结构体 */
struct t3DModel	
{	int numOfObjects;			        /**< 模型中对象的数目 */
	int numOfMaterials;			        /**< 模型中材质的数目 */
	int numOfAnimations;                /**< 模型中动作的数目 */
	int currentAnim;                    /**< 帧索引 */
	int currentFrame;                   /**< 当前帧 */
	vector<tAnimationInfo> pAnimations; /**< 帧信息链表 */
	vector<tMaterialInfo> pMaterials;	/**< 材质链表信息 */
	vector<t3DObject> pObject;	        /**< 模型中对象链表信息 */
};

/** 纹理名字 */
typedef char tMd2Skin[64];

/** 控制一帧播放速度 */
#define kAnimationSpeed			5.0f

/** 纹理最大数目 */
#define MAX_TEXTURES 100

/** MD2文件载入类 */
class CMD2Loader
{

public:
	/** 构造函数 */
	CMD2Loader();	

	~CMD2Loader();

    /** 返回3D模型结构 */
	t3DModel& GetModel() { return m_3DModel; }

	/** 初始化 */
	void  Init(char* strFileName, char *strTexture);

	/** 播放模型动画 */
	void AnimateMD2Model();
    
	/** 模型结构 */
	t3DModel                m_3DModel;                  
	
private:
	
	/** 载入MD2文件 */
	bool ImportMD2(t3DModel *pModel, char *strFileName, char *strTexture);
	
	/** 读取MD2文件数据 */
	void ReadMD2Data();

	/**  从文件中创建纹理 */
	void LoadTexture(char* filename, GLuint textureArray[], GLuint textureID);

	/** 保存动作信息 */
	void ParseAnimations(t3DModel *pModel);

	/** 将MD2文件结构转化为模型结构 */
	void ConvertDataStructures(t3DModel *pModel);

	/** 释放内存 */
	void CleanUp();

	/** 返回当前时间 */
	float ReturnCurrentTime(t3DModel *pModel, int nextFrame);

	FILE *m_FilePointer;                                 /**< 文件指针 */
	tMd2Header				m_Header;			         /**< 文件头 */
	tMd2Skin				*m_pSkins;			         /**< 纹理数据指针 */
	tMd2TexCoord			*m_pTexCoords;		         /**< 纹理坐标指针 */
	tMd2Face				*m_pTriangles;		         /**< 三角形面指针 */
	tMd2Frame				*m_pFrames;			         /**< 关键帧 */
	CBMPLoader              m_BMPTexture;                /**< 载入位图 */
	GLuint                  m_textures[MAX_TEXTURES];    /**< 纹理数组 */
	
};

#endif