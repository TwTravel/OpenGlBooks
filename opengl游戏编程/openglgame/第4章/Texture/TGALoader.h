//========================================================
/**
*  @file      TGALoader.h
*
*  项目描述： TGA文件载入类
*  文件描述:  纹理映射  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-12-06	
*  修改日期： 2006-12-11
*
*/     
//========================================================


#ifndef __TGALOADER_H__
#define __TGALOADER_H__

#include "stdafx.h"

/** TGA文件载入类 */
class CTGALoader
{
   public:
	  
      CTGALoader();                             /**< 构造函数 */
      ~CTGALoader();

      bool LoadTGA(const char *file);          /**< 载入TGA文件 */
      void FreeImage();                        /**< 释放内存 */ 
	  bool Load(const char* fileName); /**< 载入TGA文件为纹理 */

      unsigned int ID;                        /**< 生成纹理的ID号 */
      int imageWidth;                         /**< 图像宽度 */
      int imageHeight;                        /**< 图像高度 */
      unsigned char *image;                   /**< 指向图像数据的指针 */
      unsigned int type;                      /**< 图象类型GL_RGB 或GL_RGBA */
};


#endif