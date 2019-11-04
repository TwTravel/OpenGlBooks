#include "ASELoader.h"
#include <fstream.h>
#include <string.h>
#include <math.h>
#include "Graph.h"

/**********************************************************************************
*CVertex Member Functions
***********************************************************************************/

/************************************************
*Function: Default constructor of CVertex
*Input: None
*Return: None
*Description: Sets x,y,z to 0 
************************************************/
CVertex::CVertex()
	{x=y=z=0;}

/************************************************
*Function: Overloaded constructor of CVertex
*Input: Three floats, one for each element of the vector.
*Return: None
*Description: Sets (x,y,z) to  (xt,yt,zt)
************************************************/
CVertex::CVertex(float xt, float yt, float zt)
	{x=xt; y=yt; z=zt;}

/************************************************
*Function: Set
*Input: Three floats, one for each element of the vector
*Return: None
*Description: Sets (x,y,z) to (xt,yt,zt)
************************************************/
void CVertex::set(float xt, float yt, float zt)
	{x=xt; y=yt; z=zt;}

/************************************************
*Function: Set overloaded for a vertex
*Input: One CVertex element
*Return: None
*Description: Sets (x,y,z) to (v.x,v.y,v.z)
************************************************/
void CVertex::set(CVertex v)
	{x=v.x; y=v.y; z=v.z;}

/************************************************
*Function: Scale
*Input: One float representing the scaling factor
*Return: None
*Description: Multiplies the vectex's 
*             elements by the scaling factor
************************************************/
void CVertex::scale(float sc)
	{x*=sc; y*=sc; z*=sc;}

/************************************************
*Function: Add
*Input: One CVertex
*Return: None
*Description: Adds two vertexes together and 
*             stores the result in the calling CVertex
************************************************/
void CVertex::add(CVertex v)
	{x+=v.x; y+=v.y; z+=v.z;}

/************************************************
*Function: dot
*Input: One CVertex
*Return: One float
*Description: Calculates the dot product between 
*             two vertexes and returns the result
************************************************/
float CVertex::dot(CVertex v)
	{ return (x * v.x) + (y * v.y) + (z * v.z);}

/************************************************
*Function: cross
*Input: One CVertex
*Return: One CVertex
*Description: Calculates the cross product between
*             two vertexes and return the result
************************************************/
CVertex CVertex::cross(CVertex v)
{
	CVertex dest;
	dest.x = (y * v.z) - (z * v.y);
	dest.y = (z * v.x) - (x * v.z);
	dest.z = (x * v.y) - (y * v.x);
	return dest;
}

/************************************************
*Function: Calculate Normal
*Input: Two other vertexes part of a triangle
*Return: One CVertex
*Description: Calculates the normal of triangle 
*             represented by this vertex and two others.
************************************************/
CVertex CVertex::CalcNormal(CVertex v1, CVertex v2)
{
	CVertex v0(x,y,z);
	v0.scale(-1);
	v1.add(v0);
	v2.add(v0);
	v0=v1.cross(v2);
	v0.normalize();
	return v0;
}

/************************************************
*Function: Calculate Smooth Normal
*Input: Four vertexes forming a cross around this one
*Return: A smoothed normal
*Description: Calculated a smoothed normal of a vertex,
*             used to display smooth shaded surfaces
************************************************/
CVertex CVertex::CalcSmoothNormal(CVertex v1, CVertex v2,CVertex v3, CVertex v4)
{
	CVertex v0(x,y,z), vt(0,0,0);
	v0.scale(-1);
	v1.add(v0);
	v2.add(v0);
	v3.add(v0);
	v4.add(v0);

	v0=v1.cross(v2);
	vt.add(v0);
	v0=v2.cross(v3);
	vt.add(v0);
	v0=v3.cross(v4);
	vt.add(v0);
	v0=v4.cross(v1);
	vt.add(v0);
			
	vt.scale(0.25);
	vt.normalize();
	return vt;
}

/************************************************
*Function: Magnitude
*Input: None
*Return: float
*Description: Calculates the magnitude (length) of a vertex
************************************************/
float CVertex::magnitude()
{
	return sqrt(x*x+y*y+z*z);
}

/************************************************
*Function: Normalize
*Input: None
*Return: None
*Description: Normalizes the vertex
************************************************/
void CVertex::normalize()
{
	float mag=magnitude();

	x/=mag;
	y/=mag;
	z/=mag;
}

/**********************************************************************************
*CFace Member Functions
***********************************************************************************/

/************************************************
*Function: Default Constructor
*Input: None
*Return: None
*Description: Initialises indexes to zero
************************************************/
CFace::CFace()
	{A=B=C=TA=TB=TC=0;}

/************************************************
*Function: Overloaded Constructor
*Input: six long
*Return: None
*Description: Sets the face vertex indexes and 
*             texture coordinates indexes
************************************************/
CFace::CFace(long xt, long yt, long zt,long txt, long tyt, long tzt)
	{A=xt; B=yt; C=zt;TA=txt; TB=tyt; TC=tzt;}

/************************************************
*Function: Overloaded Constructor
*Input: six long
*Return: None
*Description: Sets the face vertex indexes and 
*             texture coordinates indexes
************************************************/
void CFace::set(long xt, long yt, long zt,long txt, long tyt, long tzt)
	{A=xt; B=yt; C=zt;TA=txt; TB=tyt; TC=tzt;}

/**********************************************************************************
*CMaterials Member Functions
***********************************************************************************/

/************************************************
*Function: Default Constructor
*Input: None
*Return: None
*Description: Initialises elements
************************************************/
CMaterials::CMaterials()
{
	texture_name[0]=0;
	UOffset=VOffset=UTile=VTile=0;
	texture_index=0;
}

/**********************************************************************************
*CGeometry Member Functions
***********************************************************************************/

/************************************************
*Function: Default Constructor
*Input: None
*Return: None
*Description: Initialises elements
************************************************/
CGeometry::CGeometry()
{
	vertexes=0;
	textures=0;
	faces=0;
	material_index=0;
	vertex_number=texture_number=face_number=0;
	color.set(1,1,1);
}

/************************************************
*Function: Destructor
*Input: None
*Return: None
*Description: Deletes arrays of vertexes, textures and faces
************************************************/
CGeometry::~CGeometry()
{
	if(vertexes!=0)
		delete [] vertexes;
	if(textures!=0)
		delete [] textures;
	if(faces!=0)
		delete [] faces;
}

/**********************************************************************************
*CModels Member Functions
***********************************************************************************/

/************************************************
*Function: Default Constructor
*Input: None
*Return: None
*Description: Initialises array pointers and 
*             size variables to zero 
************************************************/
CModels::CModels()
{
	loaded=0;
	parts=0;
	part_number=0;
	loaded_textures=0;
	material_number=0;
}

/************************************************
*Function: Destructor
*Input: None
*Return: None
*Description: Deletes arrays of parts and materials
************************************************/
CModels::~CModels()
{
	if(parts!=0)
		delete [] parts;
//	if(material!=0)
//		delete [] material;
}

/************************************************
*Function: Search
*Input: fstream pointer and a text string
*Return: int (TRUE, FALSE)
*Description: Searches a file for the string given as argument
************************************************/
int CModels::Search(fstream& files, char* text)
{
	char buf[1024];
	int found=0;

	while(!files.eof()&&found==0)
	{
		files>>buf;
		if(strcmp(text,buf)==0)
		{
			found=1;
			return 1;
		}
	}
	return found;
}

/************************************************
*Function: Load
*Input: filename string and a flag to indicate 
*       if textures are to be used or not
*Return: None
*Description: Reads a .ASE file and creates a model from it
************************************************/
void CModels::Load(char *filename, int loadtexture)
{

	fstream files;
	files.open(filename,ios::in|ios::nocreate);
	loaded=0;
	if(files.is_open())
	{
		if(!Search(files,"*MATERIAL_LIST"))
		{
				files.close();
				return;
		}

		Search(files,"*MATERIAL_COUNT");
		files>>material_number;

		material=new CMaterials[material_number];
		for(int i=0;i<material_number;i++)
		{
			if(!Search(files,"*MATERIAL"))
			{
				files.close();
				return;
			}
			Search(files,"*BITMAP");
			files>>material[i].texture_name;
				int stl=strlen(material[i].texture_name);
			for(int st=0;st<stl;st++)
				material[i].texture_name[st]=material[i].texture_name[st+1];
			material[i].texture_name[stl-2]=0;
				Search(files,"*UVW_U_OFFSET");
			files>>material[i].UOffset;
			Search(files,"*UVW_V_OFFSET");
			files>>material[i].VOffset;
			Search(files,"*UVW_U_TILING");
			files>>material[i].UTile;
			Search(files,"*UVW_V_TILING");
			files>>material[i].VTile;
		}

		part_number=0;
		while(Search(files,"*GEOMOBJECT"))
		{
			part_number++;
		}
		while(Search(files,"*LIGHTOBJECT"))
		{
			light_number++;
		}


		files.close();
		files.open(filename,ios::in|ios::nocreate);

		parts=new CGeometry[part_number];
		for(i=0;i<part_number;i++)
		{
				
			if(!Search(files,"*GEOMOBJECT"))
			{
				files.close();
				return;
			}
				
			Search(files,"*NODE_NAME");
			char tempname[255];
			files>>tempname;
			strncpy(parts[i].name,&tempname[1],strlen(tempname)-2);

			Search(files,"*NODE_TM");
			Search(files,"*TM_POS");
			files>>parts[i].pos.x;
			files>>parts[i].pos.z;
			files>>parts[i].pos.y;
				
			if(!Search(files,"*MESH"))
			{
				files.close();
				return;
			}
				
			Search(files,"*MESH_NUMVERTEX");
			files>>parts[i].vertex_number;

			Search(files,"*MESH_NUMFACES");
			files>>parts[i].face_number;

			parts[i].vertexes=new CVertex[parts[i].vertex_number];
			parts[i].faces=new CFace[parts[i].face_number];

			for(int j=0;j<parts[i].vertex_number;j++)
			{
				Search(files,"*MESH_VERTEX");
				int int_buf;
				files>>int_buf;
				files>>parts[i].vertexes[j].x;
				files>>parts[i].vertexes[j].y;
				files>>parts[i].vertexes[j].z;
			}

			for(j=0;j<parts[i].face_number;j++)
			{
				Search(files,"*MESH_FACE");
				int int_buf=0;
				char char_buf[24];

				files>>char_buf;
				files>>char_buf;
				files>>parts[i].faces[j].A;
				files>>char_buf;
				files>>parts[i].faces[j].C;
				files>>char_buf;
				files>>parts[i].faces[j].B;
			}

			if(loadtexture)
			{
				Search(files,"*MESH_NUMTVERTEX");
				files>>parts[i].texture_number;
				parts[i].textures=new CVertex[parts[i].texture_number];
					
				for(j=0;j<parts[i].texture_number;j++)
				{
					Search(files,"*MESH_TVERT");
					int int_buf;
					files>>int_buf;
					files>>parts[i].textures[j].x;
					files>>parts[i].textures[j].y;
					files>>parts[i].textures[j].z;
				}
				for(j=0;j<parts[i].face_number;j++)
				{
					Search(files,"*MESH_TFACE");
					int int_buf;
					files>>int_buf;
					files>>parts[i].faces[j].TA;
					files>>parts[i].faces[j].TC;
					files>>parts[i].faces[j].TB;
				}
			}

			for(j=0;j<parts[i].face_number;j++)
			{
				Search(files,"*MESH_FACENORMAL");
				int int_buf;
				files>>int_buf;
				files>>parts[i].faces[j].Normal.x;
				files>>parts[i].faces[j].Normal.y;
				files>>parts[i].faces[j].Normal.z;
				Search(files,"*MESH_VERTEXNORMAL");
				files>>int_buf;
				files>>parts[i].faces[j].NA.x;
				files>>parts[i].faces[j].NA.y;
				files>>parts[i].faces[j].NA.z;

				Search(files,"*MESH_VERTEXNORMAL");
				files>>int_buf;
				files>>parts[i].faces[j].NC.x;
				files>>parts[i].faces[j].NC.y;
				files>>parts[i].faces[j].NC.z;

				Search(files,"*MESH_VERTEXNORMAL");
				files>>int_buf;
				files>>parts[i].faces[j].NB.x;
				files>>parts[i].faces[j].NB.y;
				files>>parts[i].faces[j].NB.z;
			}
			if(loadtexture)
			{
				Search(files,"*MATERIAL_REF");
				files>>parts[i].material_index;
			}
			else
			{
	
				Search(files,"*WIREFRAME_COLOR");
				files>>parts[i].color.x;
				files>>parts[i].color.y;
				files>>parts[i].color.z;
			
			}

		}


		loaded=1;
		files.close();
	}
}

/************************************************
*Function: Load Material
*Input: None
*Return: None
*Description: Load textures from the material array
************************************************/
void CModels::LoadMaterials()
{
	if(material_number!=0)
	{
		textures=new unsigned int[material_number];
		glGenTextures(material_number,textures);

		for(int i=0;i<material_number;i++)
		{
			glBindTexture(GL_TEXTURE_2D,textures[i]);
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

			int numpx,numpy;
			unsigned char* bmp=GetJPEG(material[i].texture_name,numpx,numpy);
			if(bmp!=0)
			{
				glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, numpx, numpy, 0, GL_RGB, GL_UNSIGNED_BYTE, bmp);
				delete [] bmp;
				loaded_textures=1;
			}
			else loaded_textures=0;
		}		
	}
}

/************************************************
*Function: Unload Materials
*Input: None
*Return: None
*Description: Deletes loaded textures
************************************************/
void CModels::UnLoadMaterials()
{
	if(loaded_textures)
		glDeleteTextures(material_number,textures);
}

/************************************************
*Function: Display
*Input: Three flags
*Return: None
*Description: Displays the model to the screen with different
*             Parameters depending on the flags:
*             texture = Textures on/off
*             normal = normal are smoothed or faced
*             lighting enbled or disabled
************************************************/
void CModels::Display(int textures_flag,int normal_flag,int lighting_flag)
{
	if(loaded)
	{
		for(int p=0;p<part_number;p++)
		{
		//	glColor3f(parts[p].color.x,parts[p].color.y,parts[p].color.z);
			if(textures_flag&&loaded_textures)
				glBindTexture(GL_TEXTURE_2D,textures[parts[p].material_index]);
			glBegin(GL_TRIANGLES);
				for(int i=0;i<parts[p].face_number;i++)
				{
					if(!normal_flag) 
						glNormal3f(parts[p].faces[i].Normal.x, parts[p].faces[i].Normal.y, parts[p].faces[i].Normal.z);
			
					if(textures_flag)
						glTexCoord2f(parts[p].textures[parts[p].faces[i].TA].x*material[parts[p].material_index].UTile+material[parts[p].material_index].UOffset, parts[p].textures[parts[p].faces[i].TA].y*material[parts[p].material_index].VTile+material[parts[p].material_index].VOffset);
					if(normal_flag)
						glNormal3f(parts[p].faces[i].NA.x, parts[p].faces[i].NA.y, parts[p].faces[i].NA.z);
					glVertex3f(parts[p].vertexes[parts[p].faces[i].A].x, parts[p].vertexes[parts[p].faces[i].A].y, parts[p].vertexes[parts[p].faces[i].A].z);
					if(textures_flag)
						glTexCoord2f(parts[p].textures[parts[p].faces[i].TB].x*material[parts[p].material_index].UTile+material[parts[p].material_index].UOffset, parts[p].textures[parts[p].faces[i].TB].y*material[parts[p].material_index].VTile+material[parts[p].material_index].VOffset);
					if(normal_flag)
						glNormal3f(parts[p].faces[i].NB.x, parts[p].faces[i].NB.y, parts[p].faces[i].NB.z);
					glVertex3f(parts[p].vertexes[parts[p].faces[i].B].x, parts[p].vertexes[parts[p].faces[i].B].y, parts[p].vertexes[parts[p].faces[i].B].z);

					if(textures_flag)
						glTexCoord2f(parts[p].textures[parts[p].faces[i].TC].x*material[parts[p].material_index].UTile+material[parts[p].material_index].UOffset, parts[p].textures[parts[p].faces[i].TC].y*material[parts[p].material_index].VTile+material[parts[p].material_index].VOffset);
					if(normal_flag)
						glNormal3f(parts[p].faces[i].NC.x, parts[p].faces[i].NC.y, parts[p].faces[i].NC.z);
					glVertex3f(parts[p].vertexes[parts[p].faces[i].C].x, parts[p].vertexes[parts[p].faces[i].C].y, parts[p].vertexes[parts[p].faces[i].C].z);
				}
			glEnd();
		}
	}
}
