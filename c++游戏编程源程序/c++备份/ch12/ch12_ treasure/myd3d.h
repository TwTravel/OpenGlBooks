/*////////////////////////////////////////////////






  Direct Graphice





/*////////////////////////////////////////////////

#if !defined(__MYD3D_H___)
#define __MYD3D_H___


#include "d3d9.h"
#include "d3dx9tex.h"

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

/*//////////////////////////////////////////////




  常用頂點資料




/*//////////////////////////////////////////////

/*//////////////////////////////////////////////

  已上色，已轉換

/*//////////////////////////////////////////////
const DWORD D3DFVF_TLVERTEX = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE |  D3DFVF_SPECULAR | D3DFVF_TEX1 );
typedef struct _D3DTLVERTEX 
{
	float x , y , z , rhw ;
	D3DCOLOR  diffuse , specular;
	float tu, tv;
}D3DTLVERTEX ;
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

/*//////////////////////////////////////////////





  Direct Graphice Hdc Object
  class d3dHdc 





/*//////////////////////////////////////////////

class d3dHdc 
{
private :
	HDC m_hdc ;
	LPDIRECT3DSURFACE9 m_Surface ;

public :
	void Release();
	inline operator HDC(){ return m_hdc ;};
public :
	d3dHdc();
	~d3dHdc();
};

/*////////////////////////////////////////////////




  3D 材質




/*////////////////////////////////////////////////
class d3dTexture
{
private :
	int					m_Width ;
	int					m_Height ;
	LPDIRECT3DTEXTURE9	m_Texture ;
public :
	void BltFast( int x , int y );
	void BltFast( int l , int t , int r , int b , DWORD diffuse );
public :
	BOOL Create( LPCTSTR file , DWORD ColorKey );
	void Release();
	inline operator LPDIRECT3DTEXTURE9(){ return m_Texture ;};
	inline int getWinth(){ return m_Width ; }
	inline int getHeight(){ return m_Height ;}
public :
	d3dTexture();
	~d3dTexture();
};

/*////////////////////////////////////////////////




  材質串



/*////////////////////////////////////////////////
class d3dCartoon
{
private :
	int m_Num ;//總數量
	int m_Gx , m_Gy ;//2D分頁
	int m_Width , m_Height ;
	d3dTexture * m_Texture ;
private :
	void Create( int num );
public :
	int m_Ox , m_Oy ;//基準點
public ://建立材質
	int Create1D( LPCTSTR strFormat , int from , DWORD ColorKey );
	int Create2D( LPCTSTR strFormat , DWORD ColorKey );
public ://取得材質
	d3dTexture* getTexture( int index );
	d3dTexture* getTexture( int gx , int gy );
public ://取得圖片資料
	inline int getNum(){ return m_Num ; }
	inline int getWidth(){ return m_Width ;}
	inline int getHeight(){ return m_Height ;}
public ://地圖繪製
	void Draw2D( int sx , int sy , int sw , int sh );

public :
	d3dCartoon();
	~d3dCartoon();
	void Init();
	void Release();

};
#endif

