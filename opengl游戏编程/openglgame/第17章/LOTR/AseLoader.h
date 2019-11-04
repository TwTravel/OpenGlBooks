#ifndef _ASELOADER_H
#define _ASELOADER_H

#include <fstream.h>

/**********************************************
*CVertex
**********************************************
*Vertex descriptor containing the coordinates
*of a vertex.
**********************************************/
class CVertex
{
	public:
		float x,y,z;

		CVertex();
		CVertex(float xt, float yt, float zt);
		void set(float xt, float yt, float zt);
		void set(CVertex v);
		void scale(float sc);
		void add(CVertex v);
		float dot(CVertex v);
		CVertex cross(CVertex v);
		CVertex CalcNormal(CVertex v1, CVertex v2);
		CVertex CalcSmoothNormal(CVertex v1, CVertex v2,CVertex v3, CVertex v4);
		float magnitude();
		void normalize();
};

/**********************************************
*CFace  
***********************************************
*Describes a triangle who's corners are pointed
*to by the indexes A,B,C and texture coordinates
*TA, TB, TC.
*Face and smoothed normals are also contained 
*in the CFace class
**********************************************/
class CFace
{
	public:
		long A,B,C;			//Vertex Numbers
		long TA,TB,TC;	//texture numbers
		CVertex Normal;		//Face Normal
		CVertex NA,NB,NC;	//Vertex Normal

		CFace();
		CFace(long xt, long yt, long zt,long txt, long tyt, long tzt);
		void set(long xt, long yt, long zt,long txt, long tyt, long tzt);
};

/**********************************************
*CMaterials
***********************************************
*Material descriptor containing the textures 
*filename and loaded index
**********************************************/

class CMaterials
{
	public:
		char texture_name[256];
		float UOffset;
		float VOffset;
		float UTile;
		float VTile;

		int texture_index;

		CMaterials();
};

/**********************************************
*CGeometry
***********************************************
*Representing a full model in itself, CGeometry
*contains a list of vertices, texture coodinates and face
**********************************************/

class CGeometry
{
	public:
	
		char name[255];
		CVertex pos;
		CVertex* vertexes;
		CVertex* textures;
		CFace* faces;

		CVertex color;

		long vertex_number;
		long texture_number;
		long face_number;

		long material_index;

		CGeometry();
		~CGeometry();
};

/**********************************************
*CModels
***********************************************
*An arangment of CGeometry and textures creating a full model
**********************************************/

class CModels
{
	private:
		int Search(fstream& files, char* text);
	
	public:
		long part_number;
		long light_number;
		long material_number;
		int loaded_textures;
		unsigned int* textures;
		CGeometry* parts;
		CMaterials* material;
		int loaded;


		CModels();
		~CModels();
		void Load(char *filename, int loadtexture=1);
		void LoadMaterials();
		void UnLoadMaterials();
		void Display(int textures_flag=1,int normal_flag=1,int lighting_flag=1);
};

#endif