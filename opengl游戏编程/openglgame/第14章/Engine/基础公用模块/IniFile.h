//========================================================
/**
*  @file      IniFile.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  INI文件读写类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================
#ifndef _INI_FILE_H_
#define _INI_FILE_H_

//! 错误代码
#define ERROR_DATA -99999999

#include "stdafx.h"


/**\brief
 * INI文件读写类
 */
class CIniFile
{
public:
	CIniFile();
	~CIniFile();

	/**\brief
	 * 初始化打开配置文件
	 */
	CIniFile(char *);


	/**\brief
	 * 返回文件内容
	 */
	char*	GetData() {return Data;}
	
	
	/**\brief
	 * 返回文件的行数
	 */
	int		GetLines(int);

	
	/**\brief
	 * 打开配置文件
	 */
	BOOL	Open(char *);
	
	
	/**\brief
	 * 保存配置文件
	 */
	BOOL	Save(char *filename=NULL);


	/**\brief
	 * 读一个整数
	 */
	int		ReadInt(char *, char *);

	
	/**\brief
	 * 读一个字符串
	 */
	char*	ReadText(char *, char *);
	
	
	/**\brief
	 * 在指定的行读一整数
	 */
	int		ReadInt(char *, int );
	
	
	/**\brief
	 * 在指定的行读一字符串（等号后面的）
	 */
	char*	ReadText(char *, int);
	
	
	/**\brief
	 * 在指定行读一字符名称（等号前面的）
	 */
	char*	ReadData(char *, int);

	
	/**\brief
	 * 写一个整数
	 */
	BOOL	WriteInt(char *, char *, int);
	
	
	/**\brief
	 * 写一个字符串
	 */
	BOOL	WriteText(char *, char *, char *);

	
	/**\brief
	 * 返回连续的行数（从INDEX到第一个空行）
	 */
	int		GetContinueDataNum(char *);


private:
	//! 文件名
	char	FileName[MAX_PATH];
	
	//! 文件长度
	int		DataLen;
	
	//! 文件内容
	char*	Data;

	//! 索引数目（[]的数目）
	int		IndexNum;
	
	//! 索引点位置列表
	int*	IndexList;

	//! 当前指针
	int		Point;
	
	//! 当前行列
	int		Line, Word;


	/**\brief
	 * 初始化索引
	 */
	void	InitIndex();
	
	
	/**\brief
	 * 返回标题位置
	 */
	int		FindIndex(char *);
	
	
	/**\brief
	 * 返回数据位置
	 */
	int		FindData(int, char *);
	
	
	/**\brief
	 *  提行
	 */
	int		GotoNextLine(int);
	
	
	/**\brief
	 * 在指定位置读一数据名称
	 */
	char*	ReadDataName(int &);
	
	
	/**\brief
	 * 在指定位置读字符串
	 */
	char*	ReadText(int);

	
	/**\brief
	 * 加入一个索引
	 */
	BOOL	AddIndex(char *);
	
	
	/**\brief
	 * 在当前位置加入一个数据
	 */
	BOOL	AddData(int, char *, char *);
	
	
	/**\brief
	 * 在当前位置修改一个数据的值
	 */
	BOOL	ModityData(int, char *, char *);

};

#endif