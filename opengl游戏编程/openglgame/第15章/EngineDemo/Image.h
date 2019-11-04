//========================================================
/**
*  @file      Image.h
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

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "general.h"

#pragma pack(1)


/**\brief
 * tga文件的文件头结构
 */
typedef struct _tgaHeader
{
	//! 通常为0，如果不为0，则在header后面紧跟着bLength个字节
	BYTE bLength;
	//! 0表示有调色板，1表示没有
	BYTE bPalType;
	//! 表示图象类型
	//! 0 表示未包含图象数据
	//! 1 表示没有压缩的调色板图象
	//! 2 表示没有压缩的全彩图象
	//! 3 表示没有压缩的灰度图象
	//! 9 表示RLE压缩的调色板图象
	//! 10 表示RLE压缩的全彩图象
	//! 11 表示RLE压缩的灰度图象
	BYTE bImageType;
	//! 调色板的起始索引
	WORD wPalFirstMax;
	//! 调色板数据长度
	WORD wPalLength;
	//! 调色板每一位颜色所占的位数
	BYTE wPalBits;
	//! 图象相对于屏幕左下角的X坐标
	WORD wLeft;
	//! 图象相对于屏幕左下角的Y坐标
	WORD wBottom;
	//! 图象的宽度
	WORD wWidth;
	//! 图象的高度
	WORD wDepth;
	//! 每个像素所需的位数
	BYTE bBits;

	struct descriptor
	{
		BYTE AlphaBits:4;
		//! 1表示图象数据左右相反存储
		BYTE HorMirror:1;
		//! 1表示图象数据上下颠倒存储
		BYTE VerMirror:1;
		BYTE Reserved:2;
	}Descriptor;

}tgaHeader;

#pragma pack()


/**\brief
 * 纹理文件载入类
 */
class CImage  
{
public:
	CImage(void);
	~CImage(void);


	/**\brief
	 * 载入tga文件
	 */
	void LoadTga(char *filename);


	/**\brief
	 * 载入bmp文件
	 */
	void LoadBitmap(char*filename);


	/**\brief
	 * 载入带alpha值的tga文件
	 */
	void LoadTgaWithAlpha(char*filename , DWCOLOR keycolor);


	/**\brief
	 * 载入带alpha值的bmp文件
	 */
	void LoadBitmapWithAlpha(char*filename,DWCOLOR keycolor);


	/**\brief
	 * 取得数据区的指针
	 */
	unsigned char* GetImageData(void){return m_pImageData;}


	/**\brief
	 * 取得图像的高度
	 */
	int GetHeight(void){return m_iHeight;}


	/**\brief
	 * 取得图像的宽度
	 */
	int GetWidth(void){return m_iWidth;}



private:
	//! 图像的宽
	int m_iWidth;

	//! 图像的高
	int m_iHeight;
	
	//! 颜色
	int m_iColor;

	//! 数据区指针
	unsigned char *m_pImageData;


	unsigned char* LoadUncompessedTga(char*filename);

	unsigned char* LoadCompessedTga(char *filename);
};

#endif