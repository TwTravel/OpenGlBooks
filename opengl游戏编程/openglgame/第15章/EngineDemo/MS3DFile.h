//========================================================
/**
*  @file      MS3DFile.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  MS3D文件载入类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================
#ifndef _MS3D_H_
#define _MS3D_H_

#include "texture.h"
#include "mathlib.h"


/**\brief
 * MS3D文件载入类
 */
class CMS3DFile  
{
	typedef struct MS3DHeader_TYP
	{
		char m_ID[10];
		int  m_version;
	} MS3DHeader;

	/*\brief
	 *  MS3D模型中的点结构
	 */
	typedef struct MS3DVertex_TYP
	{
		unsigned char	m_flags;
		float			m_vertex[3];
		char			m_boneID;
		unsigned char	m_refCount;
	} MS3DVertex;


	/*\brief
	 *  MS3D模型中的三角形结构
	 */
	typedef struct MS3DTriangle_TYP
	{
		unsigned short	m_flags;
		unsigned short	m_vertexIndices[3];
		float			m_vertexNormals[3][3];
		float			m_u[3];
		float			m_v[3];
		unsigned char	m_smoothingGroup;
		unsigned char	m_groupIndex;
	} MS3DTriangle;


	/*\brief
	 */
	typedef struct MS3DMesh_TYP
	{
		unsigned char	m_flags;
		char			m_name[32];
		unsigned short	m_numTriangles;
		unsigned short*	m_TriangleIndices;
		char			m_MaterialIndex;
	} MS3DMesh;


	/*\brief
	 *  MS3D模型中用到的材质结构
	 */
	typedef struct MS3DMaterial_TYP
	{
		char m_name[32];
		float m_ambient[4];
		float m_diffuse[4];
		float m_specular[4];
		float m_emissive[4];
		float m_shininess;		// 0.0f - 128.0f
		float m_transparency;	// 0.0f - 1.0f
		char m_mode;			// 0, 1, 2 is unused now
		char m_texture[128];
		char m_alphamap[128];
	} MS3DMaterial;

	
	struct Vertex
	{
		float m_location[3];
	};

	struct Triangle//	Triangle structure
	{
		float m_normal[3];
		float m_u[3], m_v[3];
		unsigned short   m_vertexIndices[3];
	};

	struct Mesh//	Mesh
	{
		unsigned int   m_textureIndex;
		unsigned short m_numTriangles;
		unsigned short *m_pTriangleIndices;
	};


public:	
	CMS3DFile();
	virtual ~CMS3DFile();

	BOOL Load(char * pModelFile,vector3d& scale);

	BOOL LoadTexFromIni(char* filename,char* index);

	void Render();

private:
	    
	unsigned short m_numMeshes;
	unsigned short m_numTriangles;	
	unsigned short m_numVertices;	
	unsigned short m_numTexture;

	Mesh*		m_pMeshes;
	Triangle*	m_pTriangles;			//Triangles used
	Vertex*		m_pVertices;			//Vertices Used
	CTexture*	m_pTexture;

	vector3d	m_fScale;


	//////////////////////////////////////////////////
	vector3d GetTwoNormalProduct(vector3d n1,vector3d n2)
	{
		vector3d v;
		v.x=n1.y*n2.z-n2.y*n1.z;
		v.y=-(n1.x*n2.z-n2.x*n1.z);
		v.z=n1.x*n2.y-n2.x*n1.y;
		v.normalize();
		return v;
	}

	vector3d  GetNormal(float *v1,float *v2,float *v3)
	{
		vector3d n1,n2;
		n1.x=v2[0]-v1[0];
		n1.y=v2[1]-v1[1];
		n1.z=v2[2]-v1[2];

		n2.x=v3[0]-v1[0];
		n2.y=v3[1]-v1[1];
		n2.z=v3[2]-v1[2];

		return GetTwoNormalProduct(n1,n2);
	}
};


#endif