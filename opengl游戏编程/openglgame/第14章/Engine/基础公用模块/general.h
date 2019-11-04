//========================================================
/**
*  @file      general.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  常用结构和全局函数  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/     
//========================================================


#ifndef _GENERAL_H_
#define _GENERAL_H_

#include "stdafx.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

#pragma warning(disable: 4267)

/** 一些常量 */
#define F3D_DEPTH	800.f
#define SYS_HWND	(SWindowInfo::GetSysWinInfo()->m_hWnd)
#define SYS_WIDTH	(SWindowInfo::GetSysWinInfo()->m_iWidth)
#define SYS_HEIGHT	(SWindowInfo::GetSysWinInfo()->m_iHeight)

#define SYS_KEYBOARD (CInput::GetSysInput()->GetKeyboard())
#define SYS_MOUSE	(CInput::GetSysInput()->GetMouse())


/** 按钮的状态 */
#define BUTTON_NORMAL	0
#define BUTTON_ACTIVE	1
#define BUTTON_DISABLE	2
#define BUTTON_DOWNL	3
#define BUTTON_DOWNR	4
#define BUTTON_CLICK	5



/** 按钮上字的类型 */
#define BTN_TEXT_LEFT	0
#define BTN_TEXT_CENTER	1
#define BTN_TEXT_RIGHT	2


/**  游戏进度 */
#define GAME_INIT		0
#define GAME_TITLE		1
#define GAME_CG			2
#define GAME_MENU		3
#define GAME_SETTING	4
#define GAME_MAIN		5
#define GAME_END		6


/** 常用宏 */
#define _ERROR(info)	MessageBox(NULL,info,"error",MB_OK)

#define _DELETE(X)		try{ if((X)!=NULL){delete (X); (X) = NULL; } } catch(...) { _ERROR("error delete!"); }
#define _DELETE_ARRAY(X)try{ if((X)!=NULL){delete[] (X); (X) = NULL; } } catch(...) { _ERROR("error delete[]!"); }
#define _FREE(X)		try{ if((X)!=NULL){free(X);(X)=NULL; } } catch(...) { _ERROR("error free()!"); }
#define _RELEASE(A)		try{ if((A)!=NULL){(A)->Release(); (A) = NULL; } } catch(...) { _ERROR("error ->Release!"); }


/** 定义颜色的类型 */
typedef DWORD DWCOLOR;


/** 随机数 */
#define _RANDOM(nMax) (rand()*nMax/RAND_MAX)


/** 将INI文件中的int型数转化为float型 */
#define PERSENT(iNum) ((float)(iNum)*0.01f)



/**\brief
 * 在v1和v2之间随机返回一个数
 */
template<class T>
inline static T RangedRandom(T v1,T v2)
{
	return v1 + (v2-v1)*((T)rand())/((T)RAND_MAX);
}



/**\brief
 * 获取文件长度
 */
static int GetFileLength(char *name)
{
	int fh, nbytes;

	fh = _open( name, _O_RDONLY );//只读模式
	if( fh== -1 ){return -1;}
	nbytes=_filelength(fh);
	_close(fh);

	return nbytes;
}



/**\brief
 * 判断文件是否存在
 */
static BOOL CheckFile(char *name)
{
	//查找文件
	WIN32_FIND_DATA fdt;
	HANDLE hFile = FindFirstFile(name,&fdt);
	if( hFile ==INVALID_HANDLE_VALUE )
		return FALSE;
	else 
	{
		FindClose(hFile);
		return TRUE;
	}
}



/**\brief
 * 由文件名获取文件路径
 */
static char* GetFilePath(char *filename)
{
	char *Ret;
	int Len=strlen(filename);
	Ret=(char *)malloc(Len+1);
	strcpy(Ret, filename);
	for(int i=Len-1; i>=0; i--)
	{
		if( Ret[i] == '\\' )//从后向前找'\'
			break;
		Ret[i]=0;			//设置成结束点
	}

	return Ret;
}



/**\brief
 * 从文件名含路径分离出文件名不含路径
 */
static char* GetFileName(char* filename)
{
	char * szTemp = strrchr(filename,'\\'); //找最后一个'\'
	++szTemp;								//跳过'\'
	char *Ret = (char*)malloc(strlen(szTemp)+1);
	strcpy(Ret,szTemp);

	return Ret;
}



/**\brief
 * 显示调试信息
 */
static void ShowMessage(char *msg,...)
{
	va_list va;
	char str[256];

	va_start(va,msg);
	vsprintf(str,msg,va);
	va_end(va);

	MessageBox(NULL,str,"Message",MB_OK);
}



/**\brief
 * char to wchar
 */
static WCHAR* WChar(char *str)
{
	WORD *wchr;
	//将后两个参数设置成NULL和0
	//得到需要的缓冲区长度
	int wsize=MultiByteToWideChar(CP_ACP, NULL, str, strlen(str), NULL, 0);

	wchr=new WORD[wsize+1];		//多出一个放0
	MultiByteToWideChar(CP_ACP, NULL, str, strlen(str), wchr, wsize);
	wchr[wsize]=0;

	return wchr;
}




/**\brief
 * wchar to char
 */
static char* Char(WCHAR *str)
{
	char *chr;
	chr=new char[wcslen(str)+1];
	memset(chr, 0, wcslen(str)+1);

	for(unsigned int i=0;i<wcslen(str);++i)
	{
		chr[i]=(unsigned char)str[i];
	}

	return chr;
}


/**\brief
 * 开始2D渲染
 */
static void BeginOrtho(int _width,int _height)
{
	//glMatrixMode(GL_MODELVIEW);
	//glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0,_width,0,_height,-1,F3D_DEPTH);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


/**\brief
 * 结束2D渲染
 */
static void EndOrtho()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	//glMatrixMode(GL_MODELVIEW);
	//glPopMatrix();
}


#endif