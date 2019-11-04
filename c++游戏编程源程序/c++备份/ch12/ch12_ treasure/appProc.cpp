// appProc.cpp: implementation of the appProc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "canvasr.h"
#include "appProc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
appProc *app_Proc = NULL ;


void appProc::Run()
{

}

void appProc::Render()
{

}

/*///////////////////////////////////////////

  遊戲用圖片動畫

/*///////////////////////////////////////////
d3dCartoon app_Back ;//背景
d3dCartoon app_Role[4] ;//娃娃
d3dCartoon app_Prop[3] ;//掉落娃娃
gameObj * app_Sort ;//排序用串列基底

d3dTexture app_Shadow ;//影子

d3dTexture app_Title ;//標題
gameRole app_GamePlay ;//玩家資訊
gameRole app_GameProp[32] ;//掉落物件

typedef struct _GAME_DATA
{
	//移動資訊
	int mWidth , mHeight ;
	int mScreenX , mScreenY ;
	//下一個掉落物品時間
	int rTimeDown ;
	//得分
	int		sTimeGame ;//遊戲時間
	int     sTimeGet  ;//物品取得時間
	int		sScore ;//得分
	int		sNum ;//取得數量
	//power
	int		sTimePower ;//力量時間
	int		sPowerNum ;//數量
}GAME_DATA ;
GAME_DATA gm_Da ;
/*///////////////////////////////////////////

  基底物件宣告

/*///////////////////////////////////////////

/*///////////////////////////////////////////

  排序

/*///////////////////////////////////////////
void gameObj::AddSort()
{
	gameObj *sort ;
	gameObj *exit ;
	m_Exit = NULL ;
	m_Next = NULL ;
	//若基底沒有物件，就直接加入
	sort = app_Sort ;
	if( !sort )
	{
		app_Sort = this ;
		return ;
	}
	//Y若為最小，加入第一個
	if( m_y <= sort->m_y )
	{
		m_Exit = sort ;
		sort->m_Next = this ;
		app_Sort = this ;
		return ;
	}
	//開始依序判斷Z
	while( sort )
	{
		if( m_y <= sort->m_y )
		{
			//加入下一個
			if( sort->m_Next )
				sort->m_Next->m_Exit = this ;
			m_Next = sort->m_Next ;
			//加入上一個
			sort->m_Next = this ;
			m_Exit = sort ;
			return ;
		}
		exit = sort ;
		sort = sort->m_Exit ;
	}
	//Y為最大值，加入到最後一個
	exit->m_Exit = this ;
	m_Next = exit ;
}

void gameObj::SortDraw()
{

}

/*///////////////////////////////////////////

  排序用物件

/*///////////////////////////////////////////
//初始化
void appSortInit()
{
	app_Sort = NULL ;
}
//繪製
void appSortDraw()
{
	gameObj * sort ;
	sort = app_Sort ;
	while( sort )
	{
		sort->SortDraw();
		sort = sort->m_Exit ;
	}
}
/*///////////////////////////////////////////

  娃娃資料

/*///////////////////////////////////////////

//繪製
void gameRole::Draw()
{
	int x , y ;

	//計算下一格
	m_GridTime -= di_DDD._time ;
	while( m_GridTime < 0 )
	{
		m_GridTime += m_GridNextTime ;
		m_Index ++ ;
		if( m_Index >= m_Cartoon->getNum())
			m_Index = 0 ;
	}
	//畫影子
	x = (int)m_x - gm_Da.mScreenX ;
	y = (int)m_y - gm_Da.mScreenY ;
	d3d_Device->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_ZERO );
	d3d_Device->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCCOLOR );
//	d3d_Device->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_MODULATE );
	if( m_Alpha )
		app_Shadow.BltFast( x - 32 , y - 32 , x + 32 , y + 24 , D3DCOLOR_ARGB( m_Alpha , m_Alpha , m_Alpha , m_Alpha ));
	else
		app_Shadow.BltFast( x - 32 , y - 32 , x + 32 , y + 24 , -1 );
	//加入排序串列
	x = x - m_Cartoon->m_Ox ;
	y = y - m_Cartoon->m_Oy - (int)m_top ;
	if(( x >= 640 )||( y >= 480 ))
		return ;
	if((( x + m_Cartoon->getWidth() ) < 0 )||(( y + m_Cartoon->getHeight()) < 0 ))
		return ;
	AddSort();
}

//設定方向及下一格時間
void gameRole::SetCartoon( d3dCartoon * cartoon , int NextTime , int News )
{
	//判斷是否為同方向
	if( News != -1 )
	if( m_News == News )return ;
	//更新方向資料
	m_Cartoon = cartoon ;
	m_GridTime = m_GridNextTime = NextTime ;
	m_News = News ;
}
//排序串列繪製
void gameRole::SortDraw()
{
	int dx , dy ;
	d3dTexture *texture ;
	//判定是否有無物件
	if( !m_Cartoon )return ;
	//計算繪製位置
	dx = (int)  m_x          - m_Cartoon->m_Ox - gm_Da.mScreenX  ;
	dy = (int)( m_y - m_top )- m_Cartoon->m_Oy - gm_Da.mScreenY ;
	//取得並繪製材質
	d3d_Device->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
	d3d_Device->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );
	d3d_Device->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_MODULATE );
	if( texture = m_Cartoon->getTexture( m_Index ))
		if( m_Alpha )
			texture->BltFast( dx , dy , dx + texture->getWinth() , dy + texture->getHeight() , D3DCOLOR_ARGB( m_Alpha , 255 , 255 , 255 ));
		else
			texture->BltFast( dx , dy );
}

/*///////////////////////////////////////////

  遊戲資料載入

/*///////////////////////////////////////////
BOOL appLoadGameData()
{
	int i ;

	//載入背景
	app_Back.Create2D( "m%2.2d_%2.2d.bmp" , 0 );
	//載入影子
	app_Shadow.Create( "影子02.bmp" , 0 );
	//載入掉落物品
	app_Prop[0].Create1D( "松鼠%2.2d.bmp" , 0 , D3DCOLOR_ARGB( 255 , 0 , 255 , 0 ));
	app_Prop[1].Create1D( "惡魔%2.2d.bmp" , 0 , D3DCOLOR_ARGB( 255 , 0 , 255 , 0 ));
	app_Prop[2].Create1D( "魔鹿%2.2d.bmp" , 0 , D3DCOLOR_ARGB( 255 , 0 , 255 , 0 ));
	//設定掉落物品基準點
	for( i = 0 ; i < 3 ; i++ )
	{ 
		app_Prop[i].m_Ox = 70 ;
		app_Prop[i].m_Oy = 100 ;
	}
	//載入娃娃
	app_Role[0].Create1D( "巴冷_01_%2.2d.bmp" , 0 , D3DCOLOR_ARGB( 255 , 0 , 255 , 0 ));
	app_Role[1].Create1D( "巴冷_03_%2.2d.bmp" , 0 , D3DCOLOR_ARGB( 255 , 0 , 255 , 0 ) );
	app_Role[2].Create1D( "巴冷_05_%2.2d.bmp" , 0 , D3DCOLOR_ARGB( 255 , 0 , 255 , 0 ) );
	app_Role[3].Create1D( "巴冷_07_%2.2d.bmp" , 0 , D3DCOLOR_ARGB( 255 , 0 , 255 , 0 ) );
	//設定娃娃基準點
	for( i = 0 ; i < 4 ; i++ )
	{ 
		app_Role[i].m_Ox = 70 ;
		app_Role[i].m_Oy = 100 ;
	}
	//初始第一個娃娃
	app_GamePlay.SetCartoon( app_Role , 100 , 1 );
	app_GamePlay.m_x = 320.0f ;
	app_GamePlay.m_y = 240.0f ;
	//其他設定		
	d3d_Device->SetSamplerState( 0 , D3DSAMP_MAGFILTER , D3DTEXF_POINT );
	d3d_Device->SetSamplerState( 0 , D3DSAMP_MINFILTER , D3DTEXF_POINT );
	d3d_Device->SetRenderState( D3DRS_ZENABLE , false );
	return true ;
}
/*///////////////////////////////////////////



  尋寶主內容



/*///////////////////////////////////////////
appTreasure app_Treasure ;
void appTreasureRun()
{
	app_Proc = &app_Treasure ;
}


void appTreasure::Run()
{
	//加入物品
	AddObj();
	//娃娃移動
	Move();
	//物品取得
	GetProp();
	//繪製
	Render();
	//成像
	d3d_Device->Present( NULL , NULL , NULL , NULL );
}

void appTreasure::Render()
{
	d3dTexture d3dRt ;
	//清空
	d3dClear();
	//排序串列清空
	appSortInit();
	//繪製
	d3d_Device->BeginScene();
		d3d_Device->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
		d3d_Device->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );
		d3d_Device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
		d3d_Device->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_SELECTARG2 );
		//畫背景
		app_Back.Draw2D( gm_Da.mScreenX , gm_Da.mScreenY , 640 , 480 );
		//畫資訊框
		d3dRt.BltFast( 15 , 15 , 200 , 125 , 0x8F000000 );
		//畫娃娃影子，並加入排序串列
		d3d_Device->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_SELECTARG1 );
		app_GamePlay.Draw();
		//畫掉落物品
		DrawProp();
		//畫排序串列
		appSortDraw();
	d3d_Device->EndScene();
	//打資訊文字
	DrawText();

}

void appTreasure::DrawText()
{
	d3dHdc hdc ;
	int j ;
	char ch[256] ;
	SetBkMode( hdc , 1 );
	SetTextColor( hdc , RGB( 255 , 255 , 255 ));
	//剩餘時間
	j = wsprintf( ch , "時間：%2.2d:%3.3d" , gm_Da.sTimeGame / 1000 , gm_Da.sTimeGame % 1000 );
	TextOut( hdc , 20 , 20 , ch , j );
	//目前分數
	j = wsprintf( ch , "分數：%d" , gm_Da.sScore );
	TextOut( hdc , 20 , 40 , ch , j );
	//取得物品
	j = wsprintf( ch , "數量：%d" , gm_Da.sNum );
	TextOut( hdc , 20 , 60 , ch , j );
	//力量數
	strcpy( ch , "力量" );
	for( j = 0 ; j < gm_Da.sPowerNum ; j++ )
		strcat( ch , "☆" );
	TextOut( hdc , 20 , 80 , ch , strlen( ch ) );
	//連索
	if( gm_Da.sTimeGet > 0 )
	{
		j = wsprintf( ch , "連索：%2.2d:%3.3d" , gm_Da.sTimeGet / 1000 , gm_Da.sTimeGet % 1000 );
		TextOut( hdc , 20 , 100 , ch , j );
	}
}

void appTreasure::Move()
{
	float speed ;

	//遊戲時間經過
	gm_Da.sTimeGame -= di_DDD._time ;
	gm_Da.sTimeGet  -= di_DDD._time ;
	gm_Da.sTimePower -= di_DDD._time ;
	//判定遊戲是否結束
	if( gm_Da.sTimeGame <= 0 )
	{
		appTreasureGameoverRun();
		return ;
	}
	//使用加速
	if( di_DDD.Bn[0] )
	if( gm_Da.sTimePower <= 0 )
	if( gm_Da.sPowerNum > 0 )
	{
		gm_Da.sPowerNum -- ;
		gm_Da.sTimePower = 5000 ;
	}
	//娃娃移動速度
	if( gm_Da.sTimePower > 0 )
		speed = ((float)di_DDD._time )* 0.40f ;
	else
		speed = ((float)di_DDD._time )* 0.20f ;
	//娃娃移動方向
	switch( di_DDD.News4ing )
	{
	case 1 ://下
		app_GamePlay.m_y += speed ; 
		app_GamePlay.SetCartoon( &app_Role[0] , 100 , 1 );
		break ;
	case 5 ://上
		app_GamePlay.m_y -= speed ; 
		app_GamePlay.SetCartoon( &app_Role[2] , 100 , 5 );
		break ;
	case 3 ://右
		app_GamePlay.m_x += speed ; 
		app_GamePlay.SetCartoon( &app_Role[1] , 100 , 3 );
		break ;
	case 7 ://左
		app_GamePlay.m_x -= speed ; 
		app_GamePlay.SetCartoon( &app_Role[3] , 100 , 7 );
		break ;
	}
	//移動修正
	if( app_GamePlay.m_x < 0 )
		app_GamePlay.m_x = 0 ;
	else if( app_GamePlay.m_x > app_Back.getWidth() )
		 app_GamePlay.m_x = (float)app_Back.getWidth() ;
	if( app_GamePlay.m_y < 0 )
		app_GamePlay.m_y = 0 ;
	else if( app_GamePlay.m_y > app_Back.getHeight() )
		 app_GamePlay.m_y = (float)app_Back.getHeight() ;
	//捲軸修正
	gm_Da.mScreenX = (int)app_GamePlay.m_x - 320 ;
	gm_Da.mScreenY = (int)app_GamePlay.m_y - 240 ;
	if( gm_Da.mScreenX < 0 )
		gm_Da.mScreenX = 0 ;
	else if( gm_Da.mScreenX > gm_Da.mWidth )
		 gm_Da.mScreenX = gm_Da.mWidth ;
	if( gm_Da.mScreenY < 0 )
		gm_Da.mScreenY = 0 ;
	else if( gm_Da.mScreenY > gm_Da.mHeight )
		 gm_Da.mScreenY = gm_Da.mHeight ;

}

void appTreasure::AddObj()
{
	int i ;
	//掉落計時器
	gm_Da.rTimeDown -= di_DDD._time ;
	if( gm_Da.rTimeDown > 0 )return ;
	gm_Da.rTimeDown += 1000 ;
	//將娃娃加入場景
	for( i = 0 ; i < 32 ; i++ )
		if( !app_GameProp[i].m_Cartoon )
		{
			app_GameProp[i].SetCartoon( &app_Prop[rand()%3] , 100 , -1 );
			app_GameProp[i].m_x = (float)( rand()% ( app_Back.getWidth()  - 200 ) + 100 );
			app_GameProp[i].m_y = (float)( rand()% ( app_Back.getHeight() - 200 ) + 100 );
			app_GameProp[i].m_top = 480.0f ;
			app_GameProp[i].m_Alpha = 0 ;
			return ;
		}
}

void appTreasure::DrawProp()
{
	int i ;
	float speed = ((float)di_DDD._time )* 0.30f ;
	for( i = 0 ; i < 32 ; i++ )
		if( app_GameProp[i].m_Cartoon )
		{
			if( app_GameProp[i].m_top > speed )
				app_GameProp[i].m_top -= speed ;
			else app_GameProp[i].m_top = 0.0f ;
			app_GameProp[i].Draw();
		}
}

void appTreasure::GetProp()
{
	int i ;
	int x , y ;
	int Score ;
	float len ;

	for( i = 0 ; i < 32 ; i++ )
		if( app_GameProp[i].m_Cartoon )
		if( app_GameProp[i].m_top <= 0.0f )
		if( app_GameProp[i].m_Alpha )
		{
			app_GameProp[i].m_Alpha -= di_DDD._time ;
			if( app_GameProp[i].m_Alpha <= 0 )
				app_GameProp[i].m_Cartoon = NULL ;
		}else
		{
			//取兩者矩離
			x = abs((int)( app_GamePlay.m_x - app_GameProp[i].m_x ));
			y = abs((int)( app_GamePlay.m_y - app_GameProp[i].m_y ));
			len = sqrtf((float)( x * x + y * y ));
			if( len < 32.0f )
			{
				//透明值索引
				app_GameProp[i].m_Alpha = 255 ;
				//算分數
				if( gm_Da.sTimeGet > 0 )
					Score = gm_Da.sTimeGet / 500 + 1 ;
				else Score = 1 ;
				gm_Da.sScore += ( Score * Score * 100 ) ;
				gm_Da.sNum ++ ;
				gm_Da.sTimeGet = 2000 ;

			}

		}

}

/*///////////////////////////////////////////

  準備開始

/*///////////////////////////////////////////
appTreasureStart app_TreasureStart ;
void appTreasureStartRun()
{
	app_Proc = &app_TreasureStart ;
	app_Title.Create( "接娃娃.tga" , 0 );
}

void appTreasureStart::Run()
{
	int i ;
	//開始執行
	if( di_DDD.Bn[0] )
	{
		//初始第一個娃娃
		app_GamePlay.SetCartoon( app_Role , 100 , 1 );
		app_GamePlay.m_x = 320.0f ;
		app_GamePlay.m_y = 240.0f ;
		//遊戲資料初始化
		memset( &gm_Da , 0 , sizeof( gm_Da ));
		gm_Da.sTimeGame = 60000 ;//遊戲時間
		gm_Da.sPowerNum = 3 ;//力量
		//娃娃可移動範圍
		gm_Da.mWidth = app_Back.getWidth() - 640 ;
		gm_Da.mHeight = app_Back.getHeight()-480 ;
		//將目前物品清空
		for( i = 0 ; i < 32 ; i++ )
			app_GameProp[i].m_Cartoon = NULL ;
		//遊戲開始
		appTreasureRun();
		return ;
	}
	//繪製
	Render();
}


void appTreasureStart::Render()
{
	d3dTexture text ;
	//繪製
	app_Treasure.Render();
	//畫標題
	d3d_Device->BeginScene();
		//打暗底圖
		text.BltFast( 0 , 0 , 640 , 480 , 0x80000000 );
		//畫標題
		app_Title.BltFast( 0 , 0 );
	d3d_Device->EndScene();
	//成像
	d3d_Device->Present( NULL , NULL , NULL , NULL );
}

/*///////////////////////////////////////////

  遊戲結束

/*///////////////////////////////////////////
appTreasureGameover app_TreasureGameover ;
void appTreasureGameoverRun()
{
	app_Proc = &app_TreasureGameover ;
	//載入標題
	app_Title.Create( "gameover.tga" , 0 );
	//時間規零
	gm_Da.sTimeGet = gm_Da.sTimeGame = 0 ;
}

void appTreasureGameover::Run()
{
	//開始執行
	if( di_DDD.Bn[0] )
	{
		//回遊戲主畫面
		appTreasureStartRun();
		return ;
	}
	//繪製
	Render();
}

void appTreasureGameover::Render()
{
	d3dTexture text ;
	//繪製底圖
	app_Treasure.Render();
	//畫其他物品
	d3d_Device->BeginScene();
		//打暗底色
		text.BltFast( 0 , 0 , 640 , 480 , 0x80000000 );
		//畫標題
		app_Title.BltFast( 0 , 0 );
	d3d_Device->EndScene();
	//成像
	d3d_Device->Present( NULL , NULL , NULL , NULL );
}