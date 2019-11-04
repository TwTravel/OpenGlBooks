//========================================================
/**
*  @file      Texture.h
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


#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "image.h"


/**\brief
 * 纹理处理类
 */
class CTexture
{
public:
	CTexture(void);
	~CTexture(void);

	unsigned int GetTextureId(int i);

	void SetTexCoord(float u , float v);
	
	void Bind(int i);
	
	BOOL InitTexture(char * filename,BOOL bAlpha=FALSE,BOOL bTga=FALSE);



private:
	void GetImageData(char *filename , BOOL bAlpha , BOOL bTga);
	
	int GetPower(int iSize);
	
	unsigned int tex[3];
	
	int m_iWidth;
	
	int m_iHeight;

	float fScaleWidth;
	
	float fScaleHeight;

	unsigned char *m_pImageData;
	
	GLint color_type;
	
	GLenum color_format;
	
	BOOL bScale;
	
	CImage *m_pImage;

};





//////////////////////////////////////////////////////////
//--------------------inline函数实现---------------------

inline unsigned int CTexture::GetTextureId(int i)
{
	if(i<0)i=0;if(i>2)i=2;
	return tex[i];
}


inline void CTexture::SetTexCoord(float u , float v)
{
	u = u * fScaleWidth;
	v = v * fScaleHeight;
	glTexCoord2d(u ,v);
}


inline void CTexture::Bind(int i)
{
	if(i<0)i=0;
	if(i>2)i=2;
	glBindTexture(GL_TEXTURE_2D ,tex[i]);
}


#endif