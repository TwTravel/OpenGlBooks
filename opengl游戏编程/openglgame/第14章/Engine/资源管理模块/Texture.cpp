//========================================================
/**
*  @file      Texture.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  纹理类
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
#include "Texture.h"

#define POWER0 8


CTexture::CTexture(void)
{
	m_pImageData = NULL;
}

CTexture::~CTexture(void)
{
//	glDeleteTextures(3 , &tex[0]);
	if(m_pImageData)
	{
		delete m_pImageData;
		m_pImageData = NULL;
	}
}

BOOL CTexture::InitTexture(char *filename,BOOL bAlpha,BOOL bTga)
{
	GetImageData(filename,bAlpha,bTga);
	if(m_pImageData == NULL) return FALSE;
	glGenTextures(3 , &tex[0]);
	//create nearest filtered texture
	glBindTexture(GL_TEXTURE_2D , tex[0]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, color_type, m_iWidth , m_iHeight, 0, color_format, GL_UNSIGNED_BYTE, m_pImageData);
	//create linear filtered texture
	glBindTexture(GL_TEXTURE_2D , tex[1]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, color_type , m_iWidth , m_iHeight , 0, color_format, GL_UNSIGNED_BYTE, m_pImageData);
	//create MipMapped texture
	glBindTexture(GL_TEXTURE_2D, tex[2]);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, color_type , m_iWidth , m_iHeight , color_format , GL_UNSIGNED_BYTE, m_pImageData);

	if(m_pImageData)
	{
		delete m_pImageData;
		m_pImageData = NULL;
	}

	return TRUE;
}


int CTexture::GetPower(int iSize)
{
	if(iSize < POWER0) return -1;
	int iVal = POWER0;
	if(iSize == iVal) return iSize;
	while(iSize > iVal)
	{
		iVal = 2 * iVal;
	}

	return iVal;
}


void CTexture::GetImageData(char *filename,BOOL bAlpha,BOOL bTga)
{
	unsigned char r,g,b;
	r = 0;g=0;b=0;
	m_pImage = new CImage;

	if(m_pImage == NULL)return;
	
	if(bAlpha)
	{
		color_type = GL_RGBA8;
		color_format = GL_RGBA;
	}
	else
	{
		color_type = GL_RGB8;
		color_format = GL_RGB;
	}
	
	if(bTga)
	{
		if(bAlpha) m_pImage->LoadTgaWithAlpha(filename , RGB(r,g,b));
		else m_pImage->LoadTga(filename);
	}
	else
	{
		if(bAlpha) m_pImage->LoadBitmapWithAlpha(filename , RGB(r,g,b));
		else m_pImage->LoadBitmap(filename);
	}
	
	unsigned char *pImage = m_pImage->GetImageData();
	if(pImage == NULL) return;

	m_iWidth	= GetPower(m_pImage->GetWidth());
	m_iHeight	= GetPower(m_pImage->GetHeight());
	if(m_iWidth == m_pImage->GetWidth() && m_iHeight == m_pImage->GetHeight())
		bScale = FALSE;
	else
		bScale = TRUE;
	fScaleWidth	= ((float)m_pImage->GetWidth()) / ((float)m_iWidth);
	fScaleHeight= ((float)m_pImage->GetHeight())/((float)m_iHeight);
	if(!bScale)
	{

		fScaleWidth = 1.0f;
		fScaleHeight = 1.0f;
	}
	int iImageHeight;
	int iColor;
	if(bAlpha) iColor = 4;
	else iColor = 3;
	m_pImageData = new unsigned char[m_iWidth * m_iHeight * iColor];
	
	if(m_pImageData == NULL)return;

	unsigned char *p , *q;
	p = m_pImageData;
	q = pImage;
	for(iImageHeight = 0 ; iImageHeight < m_pImage->GetHeight() ; iImageHeight++)
	{
		memcpy(p , q , m_pImage->GetWidth() * iColor);
		p += m_iWidth * iColor;
		q += m_pImage->GetWidth() * iColor;
	}

	delete m_pImage;
}

