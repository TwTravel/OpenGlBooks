/*////////////////////////////////////////////////






  Direct Graphice





/*////////////////////////////////////////////////
#include "windows.h"

#include "myd3d.h"

/*//////////////////////////////////////////////

  系統

/*//////////////////////////////////////////////

LPDIRECT3D9			d3d_3D ;
LPDIRECT3DDEVICE9	d3d_Device ;
/*//////////////////////////////////////////////

  檔案是否存在

/*//////////////////////////////////////////////
BOOL mylibFileIs( LPCTSTR name )
{
	HFILE f ;
	f = _lopen( name , OF_READ );
	if( f == -1 )
		return false ;
	_lclose( f );
	return true ;
}
/*//////////////////////////////////////////////

  初始化

/*//////////////////////////////////////////////
void d3dInit()
{
	d3d_3D = NULL ;
	d3d_Device = NULL ;
}

/*//////////////////////////////////////////////

  移除

/*//////////////////////////////////////////////
void d3dRelease()
{
	if( d3d_3D ) d3d_3D->Release();
	if( d3d_Device ) d3d_Device->Release();
}

/*//////////////////////////////////////////////

  建立
  1.調整螢幕始用區大小
  2.建立D3d 及 Device 
  3.繪圖頁清空

/*//////////////////////////////////////////////
BOOL d3dCreate( HWND hWnd , int width , int height , BOOL IsWindow )
{
	//調整螢幕始用區大小
	if( !d3dSetDisplaySize( hWnd , width , height ))
		return false ;
	//建立D3D 裝置
	if( !d3dDeviceCreate( hWnd , IsWindow ))
		return false ;
	//繪圖頁清空
	d3dClear();
	//結束
	return true ;
}


/*//////////////////////////////////////////////

  調整螢幕大小

/*//////////////////////////////////////////////
BOOL d3dSetDisplaySize( HWND hWnd , int width , int height )
{
	//調整螢幕大小
	RECT rect ;
	HDC hdc ;
	//取得始用區位置
	hdc = GetDC( hWnd );
	rect.left = (GetDeviceCaps( hdc , HORZRES ) - width )/2;
	rect.top = (GetDeviceCaps( hdc , VERTRES ) - height )/2;
	ReleaseDC( hWnd , hdc );
	rect.right = rect.left + width ;
	rect.bottom = rect.top + height ;
	//調整始用區
	AdjustWindowRectEx( &rect , GetWindowLong( hWnd , GWL_STYLE ) , (GetMenu( hWnd)!=NULL) , 
		GetWindowLong( hWnd , GWL_EXSTYLE) );
	//移動始用區
	MoveWindow( hWnd , rect.left , rect.top , rect.right - rect.left , rect.bottom - rect.top , true );
	return true ;
}
/*//////////////////////////////////////////////

  建立D3d 及 Device 

/*//////////////////////////////////////////////
BOOL d3dDeviceCreate( HWND hWnd , BOOL IsWindow  )
{
	//建立d3d主要裝置
	d3d_3D =  Direct3DCreate9( D3D_SDK_VERSION );
	if( !d3d_3D )
		return false ;
	//取得目前桌面顯示模式
	D3DDISPLAYMODE d3ddm ;
	if( d3d_3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT , &d3ddm ) != D3D_OK )
		return false ;
	//建立繪圖裝置
	D3DPRESENT_PARAMETERS d3dpp ;
	memset( &d3dpp , 0 , sizeof( d3dpp ));
	d3dpp.Windowed = IsWindow ;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD ;
	d3dpp.BackBufferFormat = d3ddm.Format ;
	d3dpp.hDeviceWindow = hWnd ;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER ;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.EnableAutoDepthStencil = TRUE;

	if( d3d_3D->CreateDevice( D3DADAPTER_DEFAULT , 
		D3DDEVTYPE_HAL , hWnd , D3DCREATE_SOFTWARE_VERTEXPROCESSING , 
		&d3dpp , &d3d_Device ) != D3D_OK )
		return false ;
	return true ;
}
/*//////////////////////////////////////////////

  繪圖頁清空

/*//////////////////////////////////////////////
void d3dClear( UINT color )
{
	d3d_Device->Clear( 0 , 0 , D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER , color , 1.0f , 0 );
}
/*//////////////////////////////////////////////





  Direct Graphice Hdc Object
  class d3dHdc 





/*//////////////////////////////////////////////
d3dHdc::d3dHdc()
{
	m_hdc = 0 ;
	m_Surface = 0 ;
	//取得裝置
	if( !d3d_Device )
		return ;
	if( d3d_Device->GetBackBuffer( 0 , 0 , D3DBACKBUFFER_TYPE_MONO , &m_Surface ) != D3D_OK )
		return ;

	m_Surface->GetDC( &m_hdc );

}

d3dHdc::~d3dHdc()
{
	Release();
}

void d3dHdc::Release()
{
	if( m_Surface )
	{
		if( m_hdc )
			m_Surface->ReleaseDC( m_hdc );
	//	m_Surface->Release();

		m_hdc = NULL ;
		m_Surface = NULL ;
	}

}

/*////////////////////////////////////////////////




  3D 材質




/*////////////////////////////////////////////////

d3dTexture::d3dTexture()
{
	m_Texture = NULL ;
}

d3dTexture::~d3dTexture()
{
	Release();
}

void d3dTexture::Release()
{
	if( m_Texture )
		m_Texture->Release();
	m_Texture = NULL ;
}

BOOL d3dTexture::Create( LPCTSTR file , DWORD ColorKey )
{
	D3DXIMAGE_INFO in ;
	memset( &in , 0 , sizeof( in ));
	//初始化
	Release();
	//載入
	D3DXCreateTextureFromFileEx( d3d_Device ,
		file , D3DX_DEFAULT , D3DX_DEFAULT , 
		0 , 0 , D3DFMT_UNKNOWN , D3DPOOL_MANAGED ,
		D3DX_DEFAULT , 
		D3DX_DEFAULT , ColorKey , &in , NULL , &m_Texture );
	if( m_Texture == NULL )
		return false ;
	//取得資料
	m_Width = in.Width ;
	m_Height = in.Height ;

	return true ;
}

/*
    0   1
	3   2
*/
void d3dTexture::BltFast(int x, int y)
{
	BltFast( x , y , x + m_Width , y + m_Height , -1 );
}

void d3dTexture::BltFast(int l , int t , int r , int b , DWORD diffuse )
{
	D3DTLVERTEX v[4] ;
	//頂點的結構
	memset( v , 0 , sizeof( v ));
	v[0].x = v[3].x = (float)(l) ;
	v[1].x = v[2].x = (float)(r);
	v[0].y = v[1].y = (float)(t);
	v[2].y = v[3].y = (float)(b);

	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 
	v[0].z = v[1].z = v[2].z = v[3].z = 0.5f ;

	//顏色值
	v[0].diffuse = v[1].diffuse = v[2].diffuse = v[3].diffuse = diffuse ;

	v[1].tu = v[2].tu = 1.0f ;
	v[2].tv = v[3].tv = 1.0f ;
	//設繪圖模式
	d3d_Device->SetTexture( 0 , m_Texture );
	d3d_Device->SetFVF( D3DFVF_TLVERTEX  );
	d3d_Device->DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , (LPVOID)v , sizeof( D3DTLVERTEX ));


}

/*////////////////////////////////////////////////




  材質串



/*////////////////////////////////////////////////
d3dCartoon::d3dCartoon()
{
	Init();
}

d3dCartoon::~d3dCartoon()
{
	Release();
}

void d3dCartoon::Init()
{
	m_Texture = NULL ;
	m_Num = 0 ;
	m_Ox = m_Oy = 0 ;
	m_Gx = m_Gy = 0 ;
	m_Width = m_Height = 0 ;
}

void d3dCartoon::Release()
{
	if( m_Texture ) delete [] m_Texture ;
	Init();
}

/*////////////////////////////////////////////////

  取得材質

/*////////////////////////////////////////////////
d3dTexture* d3dCartoon::getTexture( int index )
{
	if(( index < m_Num )&&( m_Num >= 0 ))
		return &m_Texture[index] ;
	return NULL ;
}

d3dTexture* d3dCartoon::getTexture( int gx , int gy )
{
	if(( gx < m_Gx )&&( gy < m_Gy ))
	if(( gx >= 0 )&&( gy >= 0 ))
		return &m_Texture[ gy * m_Gy + gx ] ;
	return NULL ;
}

/*////////////////////////////////////////////////

  建立材質

/*////////////////////////////////////////////////
void d3dCartoon::Create( int num )
{
	Release();

	m_Texture = new d3dTexture[num] ;
	m_Num = num ;
}

int d3dCartoon::Create1D( LPCTSTR strFormat , int from , DWORD ColorKey )
{
	int i , num ;
	char ch[256] ;

	//判斷共有多少動畫格
	num = from ;
	while( true )
	{
		wsprintf( ch , strFormat , num );
		if( !mylibFileIs( ch ))
			break ;
		num ++ ;
	}
	if( num == from )return 0 ;
	//建立材質
	Create( num );
	//載入材質
	for( i = from ; i < num ; i++ )
	{
		wsprintf( ch , strFormat , i );
		m_Texture[i].Create( ch , ColorKey );
	}
	//記錄單格長寬
	m_Width = m_Texture[0].getWinth();
	m_Height = m_Texture[0].getHeight();
	//結束
	return m_Num ;
}

int d3dCartoon::Create2D( LPCTSTR strFormat , DWORD ColorKey )
{
	int i , j , num ;
	int gx , gy ;
	char ch[256] ;
	//取得橫向張數
	gx = 0 ;
	while( 1 )
	{
		wsprintf( ch , strFormat , gx , 0 );
		if( !mylibFileIs( ch ))
			break ;
		gx ++ ;
	}
	//取得直向張數
	gy = 0 ;
	while( 1 )
	{
		wsprintf( ch , strFormat , 0 , gy );
		if( !mylibFileIs( ch ))
			break ;
		gy ++ ;
	}
	//計算總張數
	num = gx * gy ;
	if( num == 0 )return 0 ;
	//建立材質
	Create( num );
	//載入材質
	for( j = 0 ; j < gy ; j++ )
	for( i = 0 ; i < gx ; i++ )
	{
		wsprintf( ch , strFormat , i , j );
		m_Texture[ j * gx + i ].Create( ch , ColorKey );
	}
	//取得總長寬
	for( i = 0 ; i < gx ; i++ )
		m_Width += m_Texture[i].getWinth() ;
	for( j = 0 ; j < gy ; j++ )
		m_Height += m_Texture[j*gx].getHeight();
	//結束，並記錄資料
	m_Gx = gx ; 
	m_Gy = gy ;
	return m_Num ;
}

/*////////////////////////////////////////////////

  地圖繪製

/*////////////////////////////////////////////////
void d3dCartoon::Draw2D( int sx , int sy , int sw , int sh )
{
	int i , j  ;
	int x , y ;
	int w , h ;
	d3dTexture *texture ;
	//判斷是否已載入
	if( m_Texture == NULL )
		return ;
	//取得材質第一張大小
	w = m_Texture[0].getWinth();
	h = m_Texture[0].getHeight();
	//計算左上角圖格
	for( j = 0 , y = -sy ; j < m_Gy ; j++ , y += h )
	for( i = 0 , x = -sx ; i < m_Gx ; i++ , x += w )
	{
		//判定是否在螢幕內
		if(( x < sw )&&( y < sh ))
		if((( x + w )>0 )&&(( y + h )> 0 ))
		{
			//取得材質
			texture = getTexture( i , j );
			if( !texture )break ;
			//繪製
			texture->BltFast( x , y );
		}
	}
}
