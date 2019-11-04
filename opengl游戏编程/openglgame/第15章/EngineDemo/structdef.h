//========================================================
/**
*  @file      structdef.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  常用结构定义  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/     
//========================================================


#ifndef __STRUCT_DEF_H__
#define __STRUCT_DEF_H__


/**\brief
 * 窗口信息结构
 */
struct SWindowInfo
{
	//! 是否全屏显示
	static BOOL			m_bFullScreen;

	//! 窗口宽
	static int			m_iWidth;

	//! 窗口高
	static int			m_iHeight;
	
	//! 颜色位数
	static int			m_iBits;

	//! 程序的事例句柄
	static HINSTANCE	m_hInstance;

	//! 窗口句柄
	static HWND			m_hWnd;

	//! 窗口名
	static char*		m_strWinName;

	SWindowInfo()		{ m_pInfo = this; }
	~SWindowInfo()		{ _DELETE(m_strWinName);}

	/**\brief
	 * 获得当前窗口信息的指针
	 */
	static SWindowInfo* GetSysWinInfo(){return m_pInfo;}

	/**\brief
	 * 从INI文件载入窗口信息
	 */
	static BOOL LoadIni(char* _filename,char* _index);

private:
	//! 窗口信息指针
	static SWindowInfo* m_pInfo;
};



/**\brief
 * 2D空间中的点
 */
struct SPoint2
{
	union
	{
		struct{int x,y;};
		int point2[2];
	};

	SPoint2()									:x(0),y(0){}
	SPoint2(int _x,int _y)						:x(_x),y(_y){}
	const SPoint2 &operator=(const SPoint2 &p)	{x=p.x;y=p.y;return *this;}
};


/**\brief
 * 3D空间中的点
 */
struct SPoint3
{
	union
	{
		struct {float x,y,z;};
		float point3[3];
	};

	SPoint3()									:x(0.0f),y(0.0f),z(0.0f){}
	SPoint3(float _x,float _y,float _z)			:x(_x),y(_y),z(_z){}
	const SPoint3 &operator=(const SPoint3 &p)	{x=p.x;y=p.y;z=p.z;return *this;}
};



/**\brief
 * 颜色结构
 */
struct SColor
{
	union
	{
		struct {float r,g,b,a;};
		float color4[4];
	};

	SColor()									:r(0.0f),g(0.0f),b(0.0f),a(0.0f){}
	SColor(float _r,float _g,float _b,float _a)	:r(_r),g(_g),b(_b),a(_a){}
	const SColor &operator=(const SColor &c)	{r=c.r;g=c.g;b=c.b;a=c.a;return *this;}
	const SColor operator!(){SColor c;c.r=1.f-r;c.g=1.f-g;c.b=1.f-b;return c;}
};



/**\brief
 * 矩形结构
 */
struct SRect
{
	union
	{
		struct {int x,y,width,height;};
		int rect[4];
	};

	SRect():x(0),y(0),width(100),height(100){}
	SRect(int _x,int _y,int _w,int _h)		:x(_x),y(_y),width(_w),height(_h){};
	const SRect &operator=(const SRect &r)	{x=r.x;y=r.y;width=r.width;height=r.height;return *this;}
	BOOL PointIn(SPoint2 p){if(p.x>=x&&p.y>=y&&p.x<=x+width&&p.y<=y+height)return TRUE;else return FALSE;}
};



#endif