//========================================================
/**
*  @file      Image.cpp
*
*  项目描述： 构造游戏引擎
*  文件描述:  纹理文件载入类
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
#include "Image.h"

CImage::CImage(void)
{
	m_iWidth = 0;
	m_iHeight = 0;
	m_pImageData = NULL;
}

CImage::~CImage(void)
{
	if(m_pImageData)
	{
		delete [] m_pImageData;
		m_pImageData = NULL;
	}
}


/**\brief
 * 载入bmp文件
 */
void CImage::LoadBitmap(char *filename)
{
	BITMAPFILEHEADER  bitmapFileHeader;
	BITMAPINFOHEADER  bitmapInfoHeader;
	FILE *fp;
	fp = fopen(filename , "rb");
	if(fp == NULL)
		return ;
	fread(&bitmapFileHeader,sizeof(BITMAPFILEHEADER),1,fp);
	if(bitmapFileHeader.bfType != 0x4d42)
	{
		fclose(fp);
		return ;
	}
	fread(&bitmapInfoHeader,sizeof(BITMAPINFOHEADER),1,fp);
	m_iWidth = bitmapInfoHeader.biWidth;
	m_iHeight = bitmapInfoHeader.biHeight;
	unsigned char *pPalette;
	if(bitmapInfoHeader.biBitCount == 8)
	{
		pPalette = new unsigned char[256 * 4];
	}
	m_pImageData = new unsigned char[m_iWidth * m_iHeight * 3];
	if(m_pImageData == NULL)
	{
		fclose(fp);
		return;
	}
	unsigned char temp;
	if(bitmapInfoHeader.biBitCount == 24)
	{
		fread(m_pImageData , 1 , m_iWidth * m_iHeight * 3 , fp);
		for(int i =0 ; i < m_iWidth * m_iHeight * 3 ; i += 3)
		{
			temp = m_pImageData[i];
			m_pImageData[i] = m_pImageData[i + 2];
			m_pImageData[i + 2] = temp;
		}
	}
	else if(bitmapInfoHeader.biBitCount == 8)
	{
		fread(pPalette , 1 , 1024 , fp);
		for(int j = 0 ; j < m_iWidth * m_iHeight * 3 ; j += 3)
		{
			fread(&temp , 1 , 1 , fp);
			m_pImageData[j] = pPalette[temp * 4 + 2];
			m_pImageData[j + 1] = pPalette[temp * 4 + 1];
			m_pImageData[j + 2] = pPalette[temp * 4];
		}
		delete [] pPalette;
	}
	
	fclose(fp);
}


/**\brief
 * 载入带alpha值的bmp文件
 */
void CImage::LoadBitmapWithAlpha(char *filename,DWCOLOR keycolor)
{
	LoadBitmap(filename);
	if(m_pImageData == NULL)
		return;
	unsigned char r , g , b;
	r = GetRValue(keycolor);
	g = GetGValue(keycolor);
	b = GetBValue(keycolor);
	unsigned char *pImage;
	pImage = new unsigned char[m_iWidth * m_iHeight * 4];
	int src , dst;
	for(src =0 , dst = 0 ; src < m_iWidth * m_iHeight * 3 ; src += 3 , dst += 4)
	{
		if(m_pImageData[src] == r && m_pImageData[src + 1] == g && m_pImageData[src + 2] == b)
		{
			pImage[dst + 3] = 0;
		}
		else
		{
			pImage[dst + 3] = 255;
		}
		pImage[dst] = m_pImageData[src];
		pImage[dst +1] = m_pImageData[src +1];
		pImage[dst + 2] = m_pImageData[src +2];
	}
	if(m_pImageData)
	{
		delete [] m_pImageData;
		m_pImageData = NULL;
	}
	m_pImageData = pImage;
}


/**\brief
 * 载入tga文件
 */
void CImage::LoadTga(char *filename)
{
	FILE *fp;
	fp = fopen(filename , "rb");
	if(fp == NULL)
	{
		::MessageBox(NULL,"tga isn't exist!","ERROR",MB_OK);
		return ;
	}
	tgaHeader tgaFileHeader;
	fread(&tgaFileHeader , 1 , sizeof(tgaHeader) , fp);
	m_iWidth = tgaFileHeader.wWidth;
	m_iHeight = tgaFileHeader.wDepth;
	m_iColor = tgaFileHeader.bBits;
	if(tgaFileHeader.bImageType == 2)
	{
		if(tgaFileHeader.bBits == 24 || tgaFileHeader.bBits == 32)
		{
				m_pImageData = LoadUncompessedTga(filename);
				return;
		}
		else
			return ;
	}
	if(tgaFileHeader.bImageType == 10)
	{
		if(tgaFileHeader.bBits == 24 || tgaFileHeader.bBits == 32)
		{
				m_pImageData = LoadCompessedTga(filename);
				return;
		}
		else
			return ;
	}
}

unsigned char* CImage::LoadCompessedTga(char *filename)
{
	FILE *fp;
	BYTE bChar;
	int iCount;
	int iPos = 0;
	unsigned char *p;
	unsigned char r , g , b , a;
	tgaHeader tga;
	fp = fopen(filename , "rb");
	fread(&tga , 1 , sizeof(tgaHeader) , fp);
	p = new unsigned char[m_iWidth * m_iHeight * 3];
	while(iPos < m_iWidth * m_iHeight * 3)
	{
		fread(&bChar , 1 , 1 , fp);
		if(bChar & 0x80)
		{
			iCount = (bChar & 0x7f) + 1;
			fread(&b , 1 , 1 ,fp);
			fread(&g , 1 , 1 , fp);
			fread(&r , 1 , 1 , fp);
			if(m_iColor == 32)
				fread(&a , 1 , 1 , fp);
			while(iCount > 0)
			{
				*(p + iPos) = r;
				*(p + iPos + 1) = g;
				*(p + iPos + 2) = b;
				iPos += 3;
				iCount--;
			}
		}
		else
		{
			iCount = bChar + 1;
			while(iCount > 0)
			{
				fread(&b , 1 , 1 ,fp);
				fread(&g , 1 , 1 , fp);
				fread(&r , 1 , 1 , fp);
				if(m_iColor == 32)
					fread(&a , 1 , 1 , fp);
				*(p + iPos) = r;
				*(p + iPos + 1) = g;
				*(p + iPos + 2) = b;
				iPos += 3;
				iCount--;
			}
		}
	}
	return p;
}

unsigned char* CImage::LoadUncompessedTga(char *filename)
{
	FILE *fp;
	tgaHeader tga;
	int iPos;
	unsigned char m;
	unsigned char *p,*q;
	p = new unsigned char[m_iWidth * m_iHeight * 3];
	fp = fopen(filename , "rb");
	fread(&tga , 1 , sizeof(tgaHeader) , fp);
	if(m_iColor == 24)
	{
		if(fread(p,1,m_iWidth*m_iHeight*3,fp)!=m_iWidth*m_iHeight*3)
		{
			::MessageBox(NULL,"read file1 error!","error",MB_OK);
			return NULL;
		}
		iPos = 0;
		
		while(iPos < m_iWidth * m_iHeight*3)
		{
			m=*(p + iPos);
			*(p + iPos) = *(p + iPos +2);
			*(p + iPos +2) = m;
			iPos += 3;
		}
		return p;
	}
	else
	{
		q = new unsigned char[m_iWidth * m_iHeight *4];
		if(fread(q , 1 , m_iWidth * m_iHeight *4 , fp)!=m_iWidth * m_iHeight *4)
		{
			::MessageBox(NULL , "read file2 error!" , "error" , MB_OK);
			return NULL;
		}
		iPos = 0;
		int iPos2=0;
		while(iPos < m_iWidth * m_iHeight*3)
		{
			*(p + iPos) = *(q + iPos2 +2);
			*(p + iPos +1) = *(q + iPos2 +1);
			*(p + iPos +2) = *(q + iPos2);
			iPos += 3;
			iPos2 += 4;
		}
		delete [] q;
		return p;
	}
}


/**\brief
 * 载入带alpha值的tga文件
 */
void CImage::LoadTgaWithAlpha(char *filename, DWCOLOR keycolor)
{
	LoadTga(filename);
	if(m_pImageData == NULL)
		return;
	unsigned char r , g , b;
	r = GetRValue(keycolor);
	g = GetGValue(keycolor);
	b = GetBValue(keycolor);
	unsigned char *pImage;
	pImage = new unsigned char[m_iWidth * m_iHeight * 4];
	int src , dst;
	for(src =0,dst=0;src<m_iWidth*m_iHeight*3;src+=3,dst+=4)
	{
		if(m_pImageData[src] == r && m_pImageData[src + 1] == g && m_pImageData[src + 2] == b)
		{
			pImage[dst + 3] = 0;
		}
		else
		{
			pImage[dst + 3] = 255;
		}
		pImage[dst] = m_pImageData[src];
		pImage[dst +1] = m_pImageData[src +1];
		pImage[dst + 2] = m_pImageData[src +2];
	}
	if(m_pImageData)
	{
		delete [] m_pImageData;
		m_pImageData = NULL;
	}
	m_pImageData = pImage;
}

