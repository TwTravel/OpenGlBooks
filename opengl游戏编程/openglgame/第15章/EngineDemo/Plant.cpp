//========================================================
/**
*  @file      Plant.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  树的渲染
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
#include "Plant.h"
#include "inifile.h"


Plant::Plant():m_szTree(1.f),m_height(30.f),m_fBranchWidth(30.f)
{
}

Plant::~Plant()
{
}

/**\brief
 * 载入纹理
 */
BOOL Plant::Load(char* body,char* branch)
{
	if(!treebody.InitTexture(body))return FALSE;
	if(!treebranch.InitTexture(branch, TRUE))return FALSE;

	return TRUE;
}


/**\brief
 * 从INI文件载入树的参数
 */
BOOL Plant::LoadIni(char* filename,char* index)
{
	CIniFile Ini(filename);

	Load(Ini.ReadText(index,"body"),Ini.ReadText(index,"branch"));
	m_sPos.x		=(float)Ini.ReadInt(index,"x");
	m_sPos.z		=(float)Ini.ReadInt(index,"y");
	m_sPos.y		=CTerrain::GetSysTerrain()->GetHeight(m_sPos.x,m_sPos.z);
	m_szTree		=PERSENT(Ini.ReadInt(index,"size"));
	m_fBranchWidth	=PERSENT(Ini.ReadInt(index,"BranchWidth"));
	m_height		=PERSENT(Ini.ReadInt(index,"height"));

	return TRUE;
}

/**\brief
 * 植物的渲染
 */
void Plant::OnRender()
{
	glPushMatrix();
	glTranslatef(m_sPos.x,m_sPos.y,m_sPos.z);
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f , 1.0f , 1.0f);
	if(m_szTree!=0.f)
	{
		treebody.Bind(0);
		RenderTreebody();
	}
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER , 0.0f);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	treebranch.Bind(0);
	RenderBranch();
	glDisable(GL_ALPHA_TEST);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}


/**\brief
 * 绘制树身
 */
void Plant::RenderTreebody()
{
	glBegin(GL_QUAD_STRIP);
	    for(float i=0;i<5;++i)
		{
	        glTexCoord2f(i*0.3f,0);    
			glVertex3f(-m_szTree*sinf(i*72*0.0174533f),0,-m_szTree*cosf(i*72*0.0174533f));
	        glTexCoord2f(i*0.3f+0.3f,8);    
			glVertex3f(-m_szTree*sinf(i*72*0.0174533f)*0.1f,m_height,-m_szTree*cosf(i*72*0.0174533f)*0.1f);
		}
	    glTexCoord2f(2.4f,0); 
	    glVertex3f(0,0,-m_szTree);
	    glTexCoord2f(2.7f,8);
		glVertex3f(0,m_height,-m_szTree*0.1f);
    glEnd();
}


/**\brief
 * 绘制树枝
 */
void Plant::RenderBranch()
{
	glPushMatrix();	
    //////////////////////////////////
	if(m_szTree!=0.f)
	{
		glTranslatef(0,m_height*0.3f,0);
		glRotatef(0,  0.0f,1.0f,0.0f);
		RenderSingleBranch(m_fBranchWidth*0.2f,m_height*0.2f);

		glTranslatef(0,m_height*0.04f,0);
		glRotatef(170,  0.0f,1.0f,0.0f);
		RenderSingleBranch(m_fBranchWidth*0.25f,m_height*0.25f);

		glTranslatef(0,m_height*0.04f,0);
		glRotatef(80,  0.0f,1.0f,0.0f);
		RenderSingleBranch(m_fBranchWidth*0.3f,m_height*0.3f);
  
		glTranslatef(0,m_height*0.04f,0);
		glRotatef(130,  0.0f,1.0f,0.0f);
		RenderSingleBranch(m_fBranchWidth*0.32f,m_height*0.32f);
 
		glTranslatef(0,m_height*0.045f,0);
		glRotatef(200,  0.0f,1.0f,0.0f);
		RenderSingleBranch(m_fBranchWidth*0.32f,m_height*0.32f);

		glTranslatef(0,m_height*0.04f,0);
		glRotatef(90,  0.0f,1.0f,0.0f);
		RenderSingleBranch(m_fBranchWidth*0.28f,m_height*0.28f);

		glTranslatef(0,m_height*0.045f,0);
		glRotatef(150,  0.0f,1.0f,0.0f);
		RenderSingleBranch(m_fBranchWidth*0.25f,m_height*0.25f);

		glTranslatef(0,m_height*0.05f,0);
		glRotatef(80,  0.0f,1.0f,0.0f);
		RenderSingleBranch(m_fBranchWidth*0.23f,m_height*0.23f);
 
		glTranslatef(0,m_height*0.05f,0);
		glRotatef(90,  0.0f,1.0f,0.0f);
		RenderSingleBranch(m_fBranchWidth*0.2f,m_height*0.2f);
 
		glTranslatef(0,m_height*0.05f,0);
		glRotatef(180,  0.0f,1.0f,0.0f);
		RenderSingleBranch(m_fBranchWidth*0.2f,m_height*0.2f);
 
		glTranslatef(0,m_height*0.04f,0);
		glRotatef(90,  0.0f,1.0f,0.0f);
		RenderSingleBranch(m_fBranchWidth*0.16f,m_height*0.16f);

		glTranslatef(0,m_height*0.04f,0);
		glRotatef(180,  0.0f,1.0f,0.0f);
		RenderSingleBranch(m_fBranchWidth*0.15f,m_height*0.15f);
	}
	
	RenderTop(m_fBranchWidth*0.3f,m_height*0.3f);
   
	////////////////////////////
	glPopMatrix();
}

/**\brief
 * 绘制一个树枝
 */
void Plant::RenderSingleBranch(float width ,float height)
{
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f,0.0f); glVertex3f(0.0f ,0.0f  ,0.f);
	glTexCoord2f(0.0f,0.0f); glVertex3f(width,0.0f  ,0.f);
	glTexCoord2f(0.0f,1.0f); glVertex3f(width,height,0.f);
    glTexCoord2f(1.0f,1.0f); glVertex3f(0.0f ,height,0.f);
	glEnd();
}


/**\brief
 * 绘制树的头部
 */
void Plant::RenderTop(float width, float height)
{

	if(m_szTree!=0.f)
	{
		glBegin(GL_QUADS);
			glTexCoord2f(1,0);
			glVertex3f(0,0,0);
			glTexCoord2f(0,0);
			glVertex3f(0.7071f*width,0.5f*height,0);
			glTexCoord2f(0,1);
			glVertex3f(0,height,0);
			glTexCoord2f(1,1);
			glVertex3f(-0.7071f*width,0.5f*height,0);
			///////////////
			glTexCoord2f(1,0);
			glVertex3f(0,0,0);
			glTexCoord2f(0,0);
			glVertex3f(0,height*0.5f,0.7071f*width);
			glTexCoord2f(0,1);
			glVertex3f(0,height,0);
			glTexCoord2f(1,1);
			glVertex3f(0,height*0.5f,-0.7071f*width);
		glEnd();
	}
	else
	{
		glBegin(GL_QUAD_STRIP);
			glTexCoord2f(1.f,1.f);
			glVertex3f(0.5f*width,height,0.f);
			glTexCoord2f(1.f,0.f);
			glVertex3f(0.5f*width,0.f,0.f);
			glTexCoord2f(.5f,1.f);
			glVertex3f(0.f,height,-0.35f*width);
			glTexCoord2f(.5f,0.f);
			glVertex3f(0.f,0.f,-0.35f*width);
			glTexCoord2f(0.f,1.f);
			glVertex3f(-0.5f*width,height,0.f);
			glTexCoord2f(0.f,0.f);
			glVertex3f(-0.5f*width,0.f,0.f);
		glEnd();
	}
}
