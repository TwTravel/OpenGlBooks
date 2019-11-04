//========================================================
/**
*  @file      MD2File.cpp
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

#include "stdafx.h"
#include "stdio.h"
#include "MD2File.h"
#include <fstream>
#include <string>
#include<stdlib.h>


CMD2File::CMD2File(void):
		m_iFrameSize(0),
		m_iNumFrames(0),
		m_iNumGlCommands(0),
		m_iNumTriangles(0),
		m_iNumVertices(0),
		m_fInterpolation(0.f),
		glCommands(NULL),
		m_pFrames(NULL),
		m_fScale(1.f),
		m_fHeight(0.f){}

CMD2File::~CMD2File(void)
{
	if(glCommands)
	{
		delete [] glCommands;
		glCommands = NULL;
	}

	if(m_pFrames)
	{
		for(int i = 0;i<m_iNumFrames;++i)
		{
			delete [] m_pFrames[i].m_pVertices;
			m_pFrames[i].m_pVertices = NULL;
		}
		delete [] m_pFrames;
		m_pFrames = NULL;
	}
}


BOOL CMD2File::Load(char *filename)
{
	FILE *fp;
	int i,j;
	float scale = 1.0f/32.0f;
	MD2_HEADER md2Header;
	fp = fopen(filename,"rb");
	if(fp == NULL)return FALSE;
	fread(&md2Header,1,sizeof(MD2_HEADER),fp);

	m_iNumVertices	= md2Header.m_iNumVertices;
	m_iNumFrames	= md2Header.m_iNumFrames;
	m_iNumGlCommands= md2Header.m_iNumGLCommands;
	m_iFrameSize	= md2Header.m_iFrameSize;
	m_iNumTriangles	= md2Header.m_iNumTriangles;


	unsigned char frame[2000];
	//read frames and vertices
	fseek(fp,md2Header.m_iOffsetFrames,SEEK_SET);
    m_pFrames = new MD2_FRAME[m_iNumFrames];
	
	for(i=0;i<m_iNumFrames;++i)
	{
		MD2_FRAME_TEM *frame_tem=(MD2_FRAME_TEM*)frame;
		fread(frame_tem,1,m_iFrameSize,fp);
		m_pFrames[i].m_pVertices=new MD2_VERTEX[m_iNumVertices];
		for(j=0;j<m_iNumVertices;++j)
		{
			m_pFrames[i].m_pVertices[j].m_fVert[0] = frame_tem->md2_vertex_tem[j].vertex[0]*
				frame_tem->m_fScale[0] + frame_tem->m_fTrans[0];
			m_pFrames[i].m_pVertices[j].m_fVert[2] = frame_tem->md2_vertex_tem[j].vertex[1]*
				frame_tem->m_fScale[1] + frame_tem->m_fTrans[1];
			m_pFrames[i].m_pVertices[j].m_fVert[1] = frame_tem->md2_vertex_tem[j].vertex[2]*
				frame_tem->m_fScale[2] + frame_tem->m_fTrans[2];
			
			m_pFrames[i].m_pVertices[j].m_fVert[0] = m_pFrames[i].m_pVertices[j].m_fVert[0]*scale*m_fScale;
			m_pFrames[i].m_pVertices[j].m_fVert[1] = m_pFrames[i].m_pVertices[j].m_fVert[1]*scale*m_fScale+m_fHeight;
			m_pFrames[i].m_pVertices[j].m_fVert[2] = -m_pFrames[i].m_pVertices[j].m_fVert[2]*scale*m_fScale;
		}
	}

	//read triangles
	fseek(fp , md2Header.m_iOffsetTriangles,SEEK_SET);
	m_pTriangles= new MD2_TRIANGLE[m_iNumTriangles];
	fread(m_pTriangles , sizeof(MD2_TRIANGLE) , m_iNumTriangles , fp);

	//read gl commands
	glCommands = new int[md2Header.m_iNumGLCommands];
	fseek(fp , md2Header.m_iOffsetGlCommands,SEEK_SET);
	fread(glCommands , sizeof(int) , md2Header.m_iNumGLCommands , fp);
	
	if(fp)fclose(fp);

	m_pPlanes = new MD2_PLANE[m_iNumFrames * m_iNumTriangles];
	memset(m_pPlanes , 0 , sizeof(MD2_PLANE) * m_iNumFrames * m_iNumTriangles);

	return TRUE;
}



BOOL CMD2File::Init(char *filename,char *texfilename,float fScale,float h)
{
	m_fScale = fScale;
	m_fHeight = h;
	if(filename == NULL || texfilename == NULL)
		return FALSE;
	if(!Load(filename))
	{
		MessageBox(NULL,"load md2 file failed!","error",MB_OK);
		return FALSE;
	}
	if(!LoadTexture(texfilename))
	{
		MessageBox(NULL,"load md2 file's texture failed!","error",MB_OK);
		return FALSE;
	}

	CalSize();
	SetConnectivity();
	CalPlane();
	
	return TRUE;
}




void CMD2File::RenderFrame(int currFrame, int nextFrame, float fInterpolation)
{
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	texture.Bind(0);
	int i=0;
	float s,t;
	int val = glCommands[i++];
	float x1,y1,z1,x2,y2,z2;
	int count;
	int index;
	while(val != 0)
	{
		if(val > 0)
		{
			glBegin(GL_TRIANGLE_STRIP);
			count = val;
		}
		else
		{
			glBegin(GL_TRIANGLE_FAN);
			count = -val;
		}

		while(count--)
		{
			s = *(float*)&glCommands[i++];
			t = *(float*)&glCommands[i++];
			index = glCommands[i++];

			texture.SetTexCoord(s,t);		

			x1 = m_pFrames[currFrame].m_pVertices[index].m_fVert[0];
			y1 = m_pFrames[currFrame].m_pVertices[index].m_fVert[1];
			z1 = m_pFrames[currFrame].m_pVertices[index].m_fVert[2];

			x2 = m_pFrames[nextFrame].m_pVertices[index].m_fVert[0];
			y2 = m_pFrames[nextFrame].m_pVertices[index].m_fVert[1];
			z2 = m_pFrames[nextFrame].m_pVertices[index].m_fVert[2];
			glVertex3f((x1 + fInterpolation * (x2 - x1)) , (y1 + fInterpolation * (y2 - y1)) , 
				       (z1 + fInterpolation * (z2 - z1)));
		}
		glEnd();

		val = glCommands[i++];
	}
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
}


void CMD2File::CalSize(void)
{
	float min_x , max_x;
	float min_z , max_z;
	min_x = 9999.0f;
	max_x = -9999.0f;
	min_z = 9999.0f;
	max_z = -9999.0f;
	for(int i = 0 ; i < m_iNumVertices ; ++i)
	{
		if(m_pFrames[0].m_pVertices[i].m_fVert[0]<min_x)
			min_x = m_pFrames[0].m_pVertices[i].m_fVert[0];
		if(m_pFrames[0].m_pVertices[i].m_fVert[0]>max_x)
			max_x = m_pFrames[0].m_pVertices[i].m_fVert[0];

		
		if(m_pFrames[0].m_pVertices[i].m_fVert[2]<min_z)
			min_z = m_pFrames[0].m_pVertices[i].m_fVert[2];
		if(m_pFrames[0].m_pVertices[i].m_fVert[2]>max_z)
			max_z = m_pFrames[0].m_pVertices[i].m_fVert[2];
	}
	size = sqrtf((max_x-min_x)*(max_x-min_x)+(min_z-max_z)*(min_z-max_z));
}



void CMD2File::CalPlane(void)
{
	MD2_VERTEX v[4];

	int i , k , n;
	for(n = 0 ; n < m_iNumFrames ; ++n)
	for(i = 0  ; i < m_iNumTriangles; ++i)
	{
		for(k = 0 ; k < 3 ; ++k)
		{
			v[k+1].x = m_pFrames[n].m_pVertices[m_pTriangles[i].vertIndex[k]].m_fVert[0];
			v[k+1].y = m_pFrames[n].m_pVertices[m_pTriangles[i].vertIndex[k]].m_fVert[1];
			v[k+1].z = m_pFrames[n].m_pVertices[m_pTriangles[i].vertIndex[k]].m_fVert[2];
		}
		m_pPlanes[n*m_iNumTriangles +i].PlaneEq.a = v[1].y*(v[2].z-v[3].z) + v[2].y*(v[3].z-v[1].z) + v[3].y*(v[1].z-v[2].z);
		m_pPlanes[n*m_iNumTriangles +i].PlaneEq.b = v[1].z*(v[2].x-v[3].x) + v[2].z*(v[3].x-v[1].x) + v[3].z*(v[1].x-v[2].x);
		m_pPlanes[n*m_iNumTriangles +i].PlaneEq.c = v[1].x*(v[2].y-v[3].y) + v[2].x*(v[3].y-v[1].y) + v[3].x*(v[1].y-v[2].y);
		m_pPlanes[n*m_iNumTriangles +i].PlaneEq.d = -( v[1].x*(v[2].y*v[3].z - v[3].y*v[2].z) +
					  v[2].x*(v[3].y*v[1].z - v[1].y*v[3].z) +
					  v[3].x*(v[1].y*v[2].z - v[2].y*v[1].z) );
	}
}

void CMD2File::SetConnectivity(void)
{
	unsigned int p1i, p2i, p1j, p2j;
	unsigned int P1i, P2i, P1j, P2j;
	int i,j,ki,kj;
	for(i = 0 ; i < m_iNumTriangles - 1 ; ++i)
	{
		for( j = i + 1 ; j < m_iNumTriangles ; ++j)
		{
			for(ki = 0 ; ki < 3 ; ++ki)
			{
				if(!m_pPlanes[i].neigh[ki])
				{
					for(kj = 0 ; kj < 3 ; ++kj)
					{
						p1i=ki;
						p1j=kj;
						p2i=(ki+1)%3;
						p2j=(kj+1)%3;

						p1i=m_pTriangles[i].vertIndex[p1i];
						p2i=m_pTriangles[i].vertIndex[p2i];
						p1j=m_pTriangles[j].vertIndex[p1j];
						p2j=m_pTriangles[j].vertIndex[p2j];

						P1i=((p1i+p2i)-abs((int)(p1i-p2i)))/2;
						P2i=((p1i+p2i)+abs((int)(p1i-p2i)))/2;
						P1j=((p1j+p2j)-abs((int)(p1j-p2j)))/2;
						P2j=((p1j+p2j)+abs((int)(p1j-p2j)))/2;

						if((P1i==P1j) && (P2i==P2j))
						{  
							m_pPlanes[i].neigh[ki] = j+1;	  
							m_pPlanes[j].neigh[kj] = i+1;
						}
					}
				}
			}
		}
	}
	int n , k;
	for(n = 1 ; n < m_iNumFrames ; ++n)
	{
		for(i = 0 ; i < m_iNumTriangles ; ++i)
		{
			for(k = 0 ; k < 3 ; ++k)
			m_pPlanes[n*m_iNumTriangles + i].neigh[k] = m_pPlanes[i].neigh[k];
		}
	}
}



void CMD2File::CastShadow(float lp[], int nframe)
{
	int	i, j, k, jj;
	unsigned int	p1, p2;
	MD2_VERTEX		v1, v2;
	float			side;
	//set visual parameter
	for (i=0;i<m_iNumTriangles;++i)
	{
		// chech to see if light is in front or behind the plane (face plane)
		side =	m_pPlanes[nframe*m_iNumTriangles +i].PlaneEq.a*lp[0]+
				m_pPlanes[nframe*m_iNumTriangles +i].PlaneEq.b*lp[1]+
				m_pPlanes[nframe*m_iNumTriangles +i].PlaneEq.c*lp[2]+
				m_pPlanes[nframe*m_iNumTriangles +i].PlaneEq.d*lp[3];
		if (side >0) m_pPlanes[i].bVisible = TRUE;
				else m_pPlanes[i].bVisible = FALSE;
	}
	MD2_VERTEX *points;
	points = m_pFrames[nframe].m_pVertices;
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_STENCIL_TEST);
	glColorMask(0, 0, 0, 0);
	glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

	// first pass, stencil operation increases stencil value
	glFrontFace(GL_CCW);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	for (i=0; i<m_iNumTriangles;++i){
		if (m_pPlanes[i].bVisible)
			for (j=0;j<3;++j){
				k = m_pPlanes[i].neigh[j];
				if ((!k) || (!m_pPlanes[k-1].bVisible)){
					// here we have an edge, we must draw a polygon
					p1 = m_pTriangles[i].vertIndex[j];
					jj = (j+1)%3;
					p2 = m_pTriangles[i].vertIndex[jj];

					//calculate the length of the vector
					v1.x = (points[p1].x - lp[0])*100;
					v1.y = (points[p1].y - lp[1])*100;
					v1.z = (points[p1].z - lp[2])*100;

					v2.x = (points[p2].x - lp[0])*100;
					v2.y = (points[p2].y - lp[1])*100;
					v2.z = (points[p2].z - lp[2])*100;
					
					//draw the polygon
					glBegin(GL_TRIANGLE_STRIP);
						glVertex3f(points[p1].x,
									points[p1].y,
									points[p1].z);
						glVertex3f(points[p1].x + v1.x,
									points[p1].y + v1.y,
									points[p1].z + v1.z);

						glVertex3f(points[p2].x,
									points[p2].y,
									points[p2].z);
						glVertex3f(points[p2].x + v2.x,
									points[p2].y + v2.y,
									points[p2].z + v2.z);
					glEnd();
				}
			}
	}

	// second pass, stencil operation decreases stencil value
	glFrontFace(GL_CW);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	for (i=0; i<m_iNumTriangles;++i){
		if (m_pPlanes[i].bVisible)
			for (j=0;j<3;++j){
				k = m_pPlanes[i].neigh[j];
				if ((!k) || (!m_pPlanes[k-1].bVisible)){
					// here we have an edge, we must draw a polygon
					p1 = m_pTriangles[i].vertIndex[j];
					jj = (j+1)%3;
					p2 = m_pTriangles[i].vertIndex[jj];

					//calculate the length of the vector
					v1.x = (points[p1].x - lp[0])*100;
					v1.y = (points[p1].y - lp[1])*100;
					v1.z = (points[p1].z - lp[2])*100;

					v2.x = (points[p2].x - lp[0])*100;
					v2.y = (points[p2].y - lp[1])*100;
					v2.z = (points[p2].z - lp[2])*100;
					
					//draw the polygon
					glBegin(GL_TRIANGLE_STRIP);
						glVertex3f(points[p1].x,
									points[p1].y,
									points[p1].z);
						glVertex3f(points[p1].x + v1.x,
									points[p1].y + v1.y,
									points[p1].z + v1.z);

						glVertex3f(points[p2].x,
									points[p2].y,
									points[p2].z);
						glVertex3f(points[p2].x + v2.x,
									points[p2].y + v2.y,
									points[p2].z + v2.z);
					glEnd();
				}
			}
	}

	glFrontFace(GL_CCW);
	glColorMask(1, 1, 1, 1);

	//draw a shadowing rectangle covering the entire screen
	glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glStencilFunc(GL_NOTEQUAL, 0, 0xffffffff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(-0.1f, 0.1f,-0.10f);
		glVertex3f(-0.1f,-0.1f,-0.10f);
		glVertex3f( 0.1f, 0.1f,-0.10f);
		glVertex3f( 0.1f,-0.1f,-0.10f);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
	
	glShadeModel(GL_SMOOTH);
	glColor3f(1.0f , 1.0f  ,  1.0f);
}
