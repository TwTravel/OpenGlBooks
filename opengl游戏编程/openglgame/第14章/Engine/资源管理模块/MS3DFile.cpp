//========================================================
/**
*  @file      MS3DFile.cpp
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

#include "stdafx.h"
#include "MS3DFile.h"
#include "math.h"
#include "inifile.h"


CMS3DFile::CMS3DFile():m_pMeshes(NULL),m_pTriangles(NULL),m_pVertices(NULL),m_pTexture(NULL),
 m_fScale(vector3d(1.f,1.f,1.f)),m_numMeshes(0),m_numTriangles(0),m_numVertices(0),m_numTexture(0)
{
}

CMS3DFile::~CMS3DFile()
{
	unsigned short i;
	for ( i = 0; i < m_numMeshes; ++i )
		delete[] m_pMeshes[i].m_pTriangleIndices;

	_DELETE_ARRAY(m_pMeshes);
	_DELETE_ARRAY(m_pTriangles);
	_DELETE_ARRAY(m_pVertices);
	_DELETE_ARRAY(m_pTexture);
}


void CMS3DFile::Render()
{
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.f,1.f,1.f);
	int triangleIndex;
	int index;
	for (unsigned short i = 0; i < m_numMeshes; ++i )
	{
		m_pTexture[m_pMeshes[i].m_textureIndex].Bind(0);
		glBegin( GL_TRIANGLES );
			for (unsigned short j = 0; j < m_pMeshes[i].m_numTriangles; ++j )
			{
				triangleIndex = m_pMeshes[i].m_pTriangleIndices[j];
			    for (unsigned int k = 0; k < 3; ++k )
				{
					index = m_pTriangles[triangleIndex].m_vertexIndices[k];
					glNormal3fv( m_pTriangles[triangleIndex].m_normal );
					glTexCoord2f( m_pTriangles[triangleIndex].m_u[k], m_pTriangles[triangleIndex].m_v[k] );
					glVertex3fv( m_pVertices[index].m_location );
				}
			}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}


BOOL CMS3DFile::Load(char *pModelFile,vector3d& scale)
{
	m_fScale = scale;

	unsigned short i;
	FILE* file;
	//Open the .ms3d model file
	if((file= fopen(pModelFile, "rb"))==NULL)return false;

	MS3DHeader Header ;
	fread(&Header.m_ID, sizeof(char), 10, file);
	fread(&Header.m_version, sizeof(int), 1, file);
	if ( strncmp( Header.m_ID, "MS3D000000", 10 ) != 0 )
	{ 
		MessageBox(NULL,"id error","ERROR",MB_OK|MB_ICONEXCLAMATION);
		fclose(file);
		return FALSE; // "Not a valid Milkshape3D model file."
	}
	if ( Header.m_version < 3 ||Header.m_version > 4 )
	{
		MessageBox(NULL,"version error","ERROR",MB_OK|MB_ICONEXCLAMATION);
		fclose(file);
		return FALSE; // "Unhandled file version. Only Milkshape3D Version 1.3 and 1.4 is supported." );
	}

	fread(&m_numVertices, sizeof(unsigned short), 1, file);


	m_pVertices = new Vertex[m_numVertices];
	MS3DVertex ms3dVertex;
	for (i = 0; i < m_numVertices; ++i )
	{
        fread( &ms3dVertex.m_flags,    sizeof(unsigned char),1,file);
        fread(  ms3dVertex.m_vertex,   sizeof(float),        3,file);
        fread( &ms3dVertex.m_refCount, sizeof(unsigned char),1,file);
        fread( &ms3dVertex.m_boneID,   sizeof(char),         1,file);
        /////////////data copy
		m_pVertices[i].m_location[0]=ms3dVertex.m_vertex[0];
		m_pVertices[i].m_location[1]=ms3dVertex.m_vertex[1];
		m_pVertices[i].m_location[2]=ms3dVertex.m_vertex[2];
		///////////////zoom
		m_pVertices[i].m_location[0] *=scale.x;
		m_pVertices[i].m_location[1] *=scale.y;
		m_pVertices[i].m_location[2] *=scale.z;

	
	}
	
    ///////////read Triangles///////////////////////////
    fread(&m_numTriangles, sizeof(unsigned short), 1, file);
	m_pTriangles = new Triangle[m_numTriangles];
    MS3DTriangle ms3dTriangle;

	for ( i = 0; i < m_numTriangles; ++i )
	{
        fread(&ms3dTriangle.m_flags,            sizeof(unsigned short),1,file);
        fread( ms3dTriangle.m_vertexIndices,    sizeof(unsigned short),3,file);
        fread( ms3dTriangle.m_vertexNormals[0], sizeof(float),3,file);
        fread( ms3dTriangle.m_vertexNormals[1], sizeof(float),3,file);
        fread( ms3dTriangle.m_vertexNormals[2], sizeof(float),3,file);

        fread( ms3dTriangle.m_u,                sizeof(float), 3, file);
        fread( ms3dTriangle.m_v,                sizeof(float), 3, file);
  
        fread(&ms3dTriangle.m_smoothingGroup,   sizeof(unsigned char), 1, file);
        fread(&ms3dTriangle.m_groupIndex  ,     sizeof(unsigned char), 1, file);

        //////////////////////////////////////////////////////////////////////
		int v1,v2,v3;
		v1=ms3dTriangle.m_vertexIndices[0];
		v2=ms3dTriangle.m_vertexIndices[1];
		v3=ms3dTriangle.m_vertexIndices[2];

        vector3d normal=GetNormal(m_pVertices[v1].m_location,
			                          m_pVertices[v2].m_location,
						              m_pVertices[v3].m_location);

		m_pTriangles[i].m_normal[0]=normal.x;
		m_pTriangles[i].m_normal[1]=normal.y;
		m_pTriangles[i].m_normal[2]=normal.z;

		memcpy( m_pTriangles[i].m_u, ms3dTriangle.m_u, sizeof( float )*3 );
		memcpy( m_pTriangles[i].m_v, ms3dTriangle.m_v, sizeof( float )*3 );
		memcpy( m_pTriangles[i].m_vertexIndices, ms3dTriangle.m_vertexIndices, sizeof(unsigned short )*3 );

	}
    //////////////read Meshes/////////////
    fread(&m_numMeshes, sizeof(unsigned short), 1, file);
	m_pMeshes = new Mesh[m_numMeshes];

	MS3DMesh ms3dMesh ;
	////////////allocate maximum memory : m_numTriangles
	ms3dMesh.m_TriangleIndices = new unsigned short [m_numTriangles];

	for ( i = 0; i < m_numMeshes; ++i )
	{
		fread(&ms3dMesh.m_flags,		sizeof(unsigned char),	1,	file);
		fread( ms3dMesh.m_name,			sizeof(char),			32,	file);
		fread(&ms3dMesh.m_numTriangles,	sizeof(unsigned short ),1,	file);

		fread( ms3dMesh.m_TriangleIndices, sizeof(unsigned short), ms3dMesh.m_numTriangles, file);
		fread(&ms3dMesh.m_MaterialIndex, sizeof(char),  1, file);
        //////////////////////copy data
		m_pMeshes[i].m_pTriangleIndices=new unsigned short [ms3dMesh.m_numTriangles];
		memcpy( m_pMeshes[i].m_pTriangleIndices,ms3dMesh.m_TriangleIndices, sizeof( unsigned short )*ms3dMesh.m_numTriangles );

	    m_pMeshes[i].m_textureIndex=ms3dMesh.m_MaterialIndex;
	    m_pMeshes[i].m_numTriangles=ms3dMesh.m_numTriangles;
	}
	delete [] ms3dMesh.m_TriangleIndices;

    ////////////////read texture/////////////
    fread(&m_numTexture, sizeof(unsigned short), 1, file);
	m_pTexture=new CTexture[m_numTexture];
    
	MS3DMaterial ms3dMaterial;
	for ( i = 0; i < m_numTexture; ++i )
	{
        fread( ms3dMaterial.m_name,            sizeof(char),   32, file);	
        fread( ms3dMaterial.m_ambient,         sizeof(float),   4, file);	
        fread( ms3dMaterial.m_diffuse,         sizeof(float),   4, file);	
        fread( ms3dMaterial.m_specular,        sizeof(float),   4, file);	
        fread( ms3dMaterial.m_emissive,        sizeof(float),   4, file);	
        fread(&ms3dMaterial.m_shininess,       sizeof(float),   1, file);	
        fread(&ms3dMaterial.m_transparency,    sizeof(float),   1, file);	
        fread(&ms3dMaterial.m_mode,            sizeof(char),    1, file);	
        fread( ms3dMaterial.m_texture,         sizeof(char),   128,file);	
        fread( ms3dMaterial.m_alphamap,        sizeof(char),   128,file);	
        /*
		if ( strncmp(ms3dMaterial.m_texture, ".\\", 2 ) == 0 ) 
		{
			char texname[128];
			strcpy( texname, "model\\ms3d\\");
			strcpy( texname+11, ms3dMaterial.m_texture+2 );
            char bmpname[128];
	
			int  n=0;
			while(texname[n] != '.' && texname[n] != ' ' && n<100)
			{
				bmpname[n]=texname[n];
				n++;
			}
			strcpy( &bmpname[n], ".bmp" );
			if(!m_pTexture[i].InitTexture(bmpname))return false;     
		}*/
	}

    fclose(file);
	return TRUE;
}

BOOL CMS3DFile::LoadTexFromIni(char* filename,char* index)
{
	CIniFile Ini(filename);
	int t_num=Ini.ReadInt(index,"TexFileNum");
	if(t_num!=m_numTexture)
	{
		_ERROR("有个StaticObj的纹理数量不对!");
		m_numTexture=1;
	}
	char t_name[10];
	for(unsigned int i=0;i<m_numTexture;++i)
	{
		sprintf(t_name,"TexFile%d",i+1);
		if(!m_pTexture[i].InitTexture(Ini.ReadText(index,t_name)))return FALSE;
	}

	return TRUE;
}