
#ifndef GL_FRAME_H
#define GL_FRAME_H

#include <windows.h>								

///键盘缓冲区
typedef struct {									
	BOOL keyDown [256];							
} Keys;											

//应用程序结构
typedef struct {								
	HINSTANCE		hInstance;						
	const char*		className;						
} Application;									

//窗口的初始化属性结构 
typedef struct {									
	Application*		application;				
	char*				title;	                    //标题					
	int					width;						//宽度
	int					height;						//高度
	int					bitsPerPixel;				//bpp
	BOOL				isFullScreen;				//是否全屏
} GL_WindowInit;									

///窗口结构
typedef struct {									
	Keys*				keys;						// 键盘结构
	HWND				hWnd;						// 窗口句柄
	HDC					hDC;						// 设备上下文
	HGLRC				hRC;						// 渲染上下文
	GL_WindowInit		init;						// 初始化结构体
	BOOL				isVisible;					// 是否可见
	DWORD				lastTickCount;				// 时间
} GL_Window;										

static int ScreenW=800;											  
static int ScreenH=600;											    // Window Structure


void TerminateApplication (GL_Window* window);		// 结束程序

void ToggleFullscreen (GL_Window* window);			// 切换模式

BOOL Initialize (GL_Window* window, Keys* keys);	// 初始化

void Deinitialize (void);							// 卸载工作

void Update (DWORD milliseconds);					// 更新操作

void Draw (void);									// 渲染操作

#endif												