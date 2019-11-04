//========================================================
/**
*  @file      TGALoader.cpp
*
*  项目描述： 阴影体
*  文件描述:  TGA载入类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     BrightXu
*  电子邮件:  huoxini@hotmail.com
*  创建日期： 2006-12-06	
*  修改日期： 2006-12-11
*
*/     
//========================================================

#include "TGALoader.h"

/** 构造函数 */
CTGALoader::CTGALoader()
{
  /** 设置为默认值 */
  image = 0;
  type = 0;
  ID = -1;
  imageWidth = 0;
  imageHeight = 0;
}

/** 析构函数 */
CTGALoader::~CTGALoader()
{
   FreeImage();           /**< 释放内存 */
}

/** 载入TGA文件 */
bool CTGALoader::LoadTGA(const char* file)
{
   FILE *pfile;
   unsigned char tempColor;              /**< 用于交换颜色分量 */
   unsigned char bitCount;               /**< 每象素的bit位数 */
   int colorMode;                        /**< 颜色模式 */
   long tgaSize;                         /**< TGA文件大小 */
   unsigned char unCompressHeader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0}; /**< 未压缩TGA文件头 */
   unsigned char tgaHeader[12];          /**< 文件头 */
   unsigned char header[6];              /**< 文件头前6个字节 */

   /** 检查文件名是否为空 */
   if(!file) 
	   return false;
   
   /** 打开文件 */
   pfile = fopen(file, "rb");
   if(!pfile) 
	   return false;

   /** 读取文件头前12个字节 */
   fread(tgaHeader, 1, sizeof(tgaHeader), pfile);

   /** 比较文件是否为未压缩文件 */
   if(memcmp(unCompressHeader, tgaHeader, sizeof(unCompressHeader)) != 0)
	   {
		   MessageBox(NULL,"文件类型错误!","错误",MB_OK);
		   fclose(pfile);
		   return false;
	   }

   /** 读取6个字节 */
   fread(header, 1, sizeof(header), pfile);

   /** 计算图像的宽度和高度 */
   imageWidth = header[1] * 256 + header[0];
   imageHeight = header[3] * 256 + header[2];

   /** 获取每象素的bit位数 */
   bitCount = header[4];

   /**　计算颜色模式和图像大小 */
   colorMode = bitCount / 8;
   tgaSize = imageWidth * imageHeight * colorMode;

   /** 分配内存 */
   image = new unsigned char[sizeof(unsigned char) * tgaSize];

   /** 读取数据 */
   fread(image, sizeof(unsigned char), tgaSize, pfile);

   /** 将BGA格式转化为RGA格式 */
   for(long index = 0; index < tgaSize; index += colorMode)
	   {
		   tempColor = image[index];
		   image[index] = image[index + 2];
		   image[index + 2] = tempColor;
	   }

   /** 关闭文件 */
   fclose(pfile);

   /** 设置图象类型 */
   if(colorMode == 3) 
	   type = GL_RGB;
   else 
	   type = GL_RGBA;

   return true;
}

bool CTGALoader::Load(const char* fileName)
{
	if(!LoadTGA(fileName))
	{
		MessageBox(NULL,"载入TGA文件失败!","错误",MB_OK);
		exit(0);
	}
	/** 生成纹理对象名称 */
	glGenTextures(1, &ID);
   
    /** 创建纹理对象 */
    glBindTexture(GL_TEXTURE_2D, ID);
	
	/** 控制滤波 */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
   
	/** 创建纹理 */
   	gluBuild2DMipmaps(GL_TEXTURE_2D, type, imageWidth,
	                  imageHeight, type, GL_UNSIGNED_BYTE,
	                  image);
   return true;
}

void CTGALoader::FreeImage()
{
   /** 释放内存 */
   if(image)
      {
         delete[] image;
         image = 0;
      }
}