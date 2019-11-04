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
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_INTERVAL_DEFAULT ;

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

BOOL d3dTexture::Create( LPCTSTR file )
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
		D3DX_DEFAULT , 0 , &in , NULL , &m_Texture );
	if( m_Texture == NULL )
		return false ;
	//取得資料
	m_Width = in.Width ;
	m_Height = in.Height ;

	return true ;
//	m_Texture->Get
}
/*
    0   1
	3   2
*/
void d3dTexture::BltFast(int x, int y , float a )
{
	BltFast( x , y , x + m_Width , y + m_Height , a );
}

void d3dTexture::BltFast(int l , int t , int r , int b , float a )
{
	D3DTLVERTEX v[4] ;
	//定點的結構
	memset( v , 0 , sizeof( v ));
	//點位置
	//不旋轉
	if( a == 0.0f )
	{
		v[0].x = v[3].x = (float)(l) ;
		v[1].x = v[2].x = (float)(r);
		v[0].y = v[1].y = (float)(t);
		v[2].y = v[3].y = (float)(b);
	}else //旋轉
	{
		float ox , oy ;
		float in ;
		float s , c ;
		//角度轉徑度
		in = a * 3.1415926f / 180.0f ;
		//原點
		ox = (float)( r + l )/ 2 ;
		oy = (float)( b + t )/ 2 ;
		//相對位置
		l -= (int)ox ;
		t -= (int)oy ;
		r -= (int)ox ;
		b -= (int)oy ;
		//算新位置
		s = sinf( in );
		c = cosf( in );

		v[0].x =  c * l + s * t + ox ;
		v[0].y = -s * l + c * t + oy ;

		v[1].x =  c * r + s * t + ox ;
		v[1].y = -s * r + c * t + oy ;

		v[2].x =  c * r + s * b + ox ;
		v[2].y = -s * r + c * b + oy ;

		v[3].x =  c * l + s * b + ox ;
		v[3].y = -s * l + c * b + oy ;
	}
	//Z
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 
	v[0].z = v[1].z = v[2].z = v[3].z = 0.5f ;
	//顏色
	v[0].diffuse = v[1].diffuse = v[2].diffuse = v[3].diffuse = -1 ;
	//材質
	v[1].tu = v[2].tu = 1.0f ;
	v[2].tv = v[3].tv = 1.0f ;
	//設繪圖模式

	d3d_Device->SetTexture( 0 , m_Texture );
	d3d_Device->SetFVF( D3DFVF_TLVERTEX  );
	if( d3d_Device->DrawPrimitiveUP( D3DPT_TRIANGLEFAN , 2 , (LPVOID)v , sizeof( D3DTLVERTEX )) != D3D_OK )
		int a = 0 ;


}
/*////////////////////////////////////////////////




  X模型




/*////////////////////////////////////////////////

void d3dXFileData::Init()
{
	m_Num		= 0 ;
	m_Texture	= NULL ;
	m_Material  = NULL ;
	m_Mesh		= NULL ;
}

void d3dXFileData::Release()
{
	if( m_Mesh ) 
		m_Mesh->Release();

	if( m_Texture )
		delete [] m_Texture ;

	if( m_Material )
		delete [] m_Material ;
	Init();
}


BOOL d3dXFileData::Create( LPCTSTR file )
{
	DWORD i ;
	LPD3DXBUFFER buffer ;
	D3DXMATERIAL *matl ;
    LPD3DXMESH   pTempMesh;
	//讀檔
	if( D3DXLoadMeshFromX( file , D3DXMESH_SYSTEMMEM , 
		d3d_Device , NULL , 
		&buffer , NULL , &m_Num , &m_Mesh ) != D3D_OK )
		return false ;
	//配材質記憶體
	m_Texture = new d3dTexture[ m_Num ];
	m_Material = new D3DMATERIAL9[m_Num] ;
	//取內容
	matl = (D3DXMATERIAL*)buffer->GetBufferPointer();
	for( i = 0 ; i < m_Num ; i++ )
	{
		m_Material[i] = matl[i].MatD3D ;
		m_Material[i].Ambient = m_Material[i].Diffuse ;
		m_Texture[i].Create( matl[i].pTextureFilename );

	}
	//法向量
    if ( !(m_Mesh->GetFVF() & D3DFVF_NORMAL) )
    {
        if( m_Mesh->CloneMeshFVF( (m_Mesh->GetOptions() & D3DXMESH_32BIT )|D3DXMESH_MANAGED, m_Mesh->GetFVF() | D3DFVF_NORMAL, 
                                            d3d_Device, &pTempMesh ) != D3D_OK )
											return 0 ;
        D3DXComputeNormals( pTempMesh, NULL );

        m_Mesh->Release();
        m_Mesh = pTempMesh;
    }

	//結束
	buffer->Release();
	return true ;
}

void d3dXFileData::Draw()
{
	DWORD i ;

	for( i = 0 ; i < m_Num ; i++ )
	{
		d3d_Device->SetMaterial( &m_Material[i] );
		d3d_Device->SetTexture( 0 , m_Texture[i] );

		m_Mesh->DrawSubset( i );
	}
}