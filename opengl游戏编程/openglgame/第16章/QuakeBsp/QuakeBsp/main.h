
#ifndef _MAIN_H
#define _MAIN_H

#pragma warning(disable: 4786)			
#pragma warning(disable: 4244)

#include <windows.h>					
#include <gl\gl.h>						
#include <gl\glu.h>						
#include <gl\glaux.h>					
#include <iostream>						
#include <fstream>						
#include <string>						
#include <vector>						
#include <mmsystem.h>					
#include "image.h"
using namespace std;					

/** 窗口属性 */
#define SCREEN_WIDTH  800				
#define SCREEN_HEIGHT 600				
#define SCREEN_DEPTH 16					

#define MAX_TEXTURES 1000				 /**< 纹理的最大树木 */

extern UINT g_Texture[MAX_TEXTURES];	/**< 纹理数组 */

typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);
typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum texture);

/** 多重纹理 */
extern PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC  glClientActiveTextureARB;

#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1


/** 三维向量结构 */
struct CVector3
{
public:
	
	CVector3() {}

	CVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	CVector3 operator+(CVector3 vVector)
	{
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	CVector3 operator-(CVector3 vVector)
	{
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}

	CVector3 operator*(float num)
	{
		return CVector3(x * num, y * num, z * num);
	}

	CVector3 operator/(float num)
	{
		return CVector3(x / num, y / num, z / num);
	}

	float x, y, z;						
};


/** 2D点类结构 */
class CVector2 
{
public:

	CVector2() {}

	CVector2(float X, float Y) 
	{ 
		x = X; y = Y;
	}

	CVector2 operator+(CVector2 vVector)
	{
		return CVector2(vVector.x + x, vVector.y + y);
	}

	CVector2 operator-(CVector2 vVector)
	{
		return CVector2(x - vVector.x, y - vVector.y);
	}
	
	CVector2 operator*(float num)
	{
		return CVector2(x * num, y * num);
	}

	CVector2 operator/(float num)
	{
		return CVector2(x / num, y / num);
	}

	float x, y;
};


/** 全局变量 */
extern bool  g_bFullScreen;									
extern HWND  g_hWnd;										
extern RECT  g_rRect;										
extern HDC   g_hDC;											
extern HGLRC g_hRC;											
extern HINSTANCE g_hInstance;								

extern double g_FrameInterval;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

/** 主循环 */
WPARAM MainLoop();

bool CreateTexture(UINT &texture, LPSTR strFileName);

void ChangeToFullScreen();

HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance);

bool bSetupPixelFormat(HDC hdc);

void SizeOpenGLScreen(int width, int height);

void InitializeOpenGL(int width, int height);

void Init(HWND hWnd);

void RenderScene();

void DeInit();
#endif 
