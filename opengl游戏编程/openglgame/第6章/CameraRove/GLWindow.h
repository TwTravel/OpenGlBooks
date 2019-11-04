//========================================================
/**
*  @file      GLWindows.h
*
*  项目描述： 摄像机漫游
*  文件描述:  程序窗口类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-09-13	
*  修改日期： 2006-10-10
*
*/     
//========================================================
#ifndef	__GLWINDOW_H__
#define	__GLWINDOW_H__

#include  "stdafx.h"                                            /**< 包含stdafx.h头文件 */

/** windows窗口类 */
class GLWindow															
{
public:
	
	/** 构造函数 */
	GLWindow();
	
	/** 创建windows窗口 */
	bool	Create(const char * window_title, const char * class_name, bool fullscreen,HINSTANCE h_instance, LPVOID lpParam);
	
	/** 删除OpenGL窗口 */
	void	Destroy();
	
	/** 改变窗口的显示设置 */
	bool	ChangeScreenSetting();
	
	/** 当窗口大小改变时，通知OpenGL调整大小 */
	void	ReshapeGL();
	
	/** Swap Buffers (Double Buffering) */
	void	SwapBuffers() { ::SwapBuffers(m_hDC); }


	
	/** 设置窗口左上角的位置 */
	void	SetPosX(int x);
	void	SetPosX(unsigned short x) { SetPosX((signed short)x); }		
	void	SetPosY(int y);
	void	SetPosY(unsigned short y) { SetPosY((signed short)y); }		
	
	/** 返回窗口的大小 */
	int		GetWidth();
	int		GetHeight();
	
	/** 设置窗口的大小 */
	void	SetWidth(int width);
	void	SetHeight(int height);
	
	/** 返回窗口左上角的位置 */
	int		GetPosX();
	int		GetPosY();
	
	/** 设置窗口的颜色位深 */
	void	SetHiColor()	{ m_BitsPerPixel = 16; }
	void	SetTrueColor()	{ m_BitsPerPixel = 32; }

	/** 重载运算符，可以让GL_Window m_Window声明后的m_Window作为窗口句柄使用 */
	operator HWND() { return m_hWnd; }

private:
	HWND	m_hWnd;														/**< 窗口句柄 */
	HDC		m_hDC;														/**< 设备描述表 */
	HGLRC	m_hRC;														/**< OpenGL渲染描述表 */

	int		m_WindowPosX;												/**< 窗口的左上角的X位置 */
	int		m_WindowPosY;												/**< 窗口的左上角的Y位置 */
	int		m_WindowWidth;												/**< 窗口的宽度 */
	int		m_WindowHeight;												/**< 窗口的高度 */
	int		m_ScreenWidth;												/**< 全屏的宽度 */
	int		m_ScreenHeight;												/**< 全屏的高度 */
	int		m_BitsPerPixel;												/**< 颜色位深 */
	bool	m_IsFullScreen;												/**< 是否全屏 */
};

#endif	// __GLWINDOW_H__