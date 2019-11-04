/*////////////////////////////////////////////////






  Direct Graphice





/*////////////////////////////////////////////////

#if !defined(__MYD3D_H___)
#define __MYD3D_H___


#include "d3d9.h"

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d9.lib")


/*//////////////////////////////////////////////

  系統

/*//////////////////////////////////////////////
extern LPDIRECT3D9			d3d_3D ;
extern LPDIRECT3DDEVICE9	d3d_Device ;
//初始化
void d3dInit();
//移除
void d3dRelease();
//建立
BOOL d3dCreate( HWND hWnd , int width , int height , BOOL IsWindow  );
//調整螢幕始用區大小
BOOL d3dSetDisplaySize( HWND hWnd , int width , int height );
//建立D3d 及 Device 
BOOL d3dDeviceCreate( HWND hWnd , BOOL IsWindow );
//繪圖頁清空
void d3dClear( UINT color = 0 );

#endif

