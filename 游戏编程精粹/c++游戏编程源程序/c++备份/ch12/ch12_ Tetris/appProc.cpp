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



  方塊主內容



/*///////////////////////////////////////////
static const POINT app_TetrisBox[7][4] = 
{
	/*0
		□□□□
	*/{ { 0 , 1 }, { 1 , 1 } , { 2 , 1 } , { 3 , 1 } },
	/*1
		□□
		  □□
	*/{ { 0 , 1 }, { 1 , 1 } , { 2 , 1 } , { 2 , 2 } },
	/*2
		  □□
		□□
	*/{ { 0 , 2 }, { 1 , 2 } , { 1 , 1 } , { 2 , 1 } },
	/*3
		□□□
		    □
	*/{ { 0 , 1 } ,{ 1 , 1 } , { 2 , 1 } , { 2 , 2 } },
	/*4
	        □
		□□□
	*/{ { 0 , 2 } ,{ 1 , 2 } , { 2 , 2 } , { 2 , 1 } },
	/*5
		  □□
		  □□
	*/{ { 1 , 1 }, { 2 , 1 } , { 2 , 2 } , { 1 , 2 } },
	/*6
		  □
		□□□
	*/{ { 2 , 1 }, { 1 , 2 } , { 2 , 2 } , { 3 , 2 } },

};


/*///////////////////////////////////////////

  遊戲資料結構

/*///////////////////////////////////////////
typedef struct _TETRIS_DATA
{
	BYTE  Data[10][20] ;//底資料
	POINT NowBox[4] ;//目前方塊
	POINT NowPoint ;
	int   BoxNextIndex ;//下一個方塊
	int   BoxNowIndex  ;//目前方塊

	int   TimeNextGrid ;
	int   TimeGridNow  ;

	int		sLV ;//關卡
	int		sNLV ;//下一關要消的行數
	int		sScore ;//得分
	int		sDel ;//已消行數
}TETRIS_DATA ;
TETRIS_DATA ts_DA ;

//背景
d3dTexture ts_Back ;
//方塊
d3dTexture ts_Box[7] ;
//標題
d3dTexture ts_Title ;

/*///////////////////////////////////////////

  遊戲進行中

/*///////////////////////////////////////////

Tetris app_Tetris ;

void appTetrisRun()
{
	app_Proc = &app_Tetris ;
	//定初始資料
	ts_DA.sLV = 1 ;
	ts_DA.sNLV = 6 ;
//	app_Tetris.Updata();
}

//
//
//初始化
void Tetris::Updata()
{
	int i ;
	char ch[256] ;
	//開始
	memset( &ts_DA , 0 , sizeof( ts_DA ));
	//載入背景
	ts_Back.Create( "背景.tga" );
	//酨入方塊
	for( i = 0 ; i < 7 ; i++ )
	{
		wsprintf( ch , "方塊%2.2d.bmp" , i );
		ts_Box[i].Create( ch );
	}
	//第一個方塊 
	ts_DA.BoxNextIndex = rand()%7 ;
	GetBox();
	//時間
	ts_DA.TimeNextGrid = 560 ;
	//繪製模式
	d3d_Device->SetTextureStageState( 0 , D3DTSS_ALPHAARG1 , D3DTA_TEXTURE );
	d3d_Device->SetTextureStageState( 0 , D3DTSS_ALPHAARG2 , D3DTA_DIFFUSE );
	d3d_Device->SetTextureStageState( 0 , D3DTSS_COLORARG1 , D3DTA_TEXTURE );
	d3d_Device->SetTextureStageState( 0 , D3DTSS_COLORARG2 , D3DTA_DIFFUSE );

}

//
//
//迴圈
void Tetris::Run()
{
	//落下
	BoxDown();
	//位移
	Move();
	//旋轉
	Rotation();
	//繪製
	Render();
}
//
//
//繪製
void Tetris::Render()
{
	//畫物件
	RenderBack();
	//成像
	d3d_Device->Present( NULL , NULL , NULL , NULL );
}

void Tetris::RenderBack()
{
	d3dTexture d3dRt ;
	//清空
	d3dClear();
	//開始繪製
	d3d_Device->BeginScene();
		d3d_Device->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
		d3d_Device->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );
		d3d_Device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		d3d_Device->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_SELECTARG2 );
		//畫背景
		ts_Back.BltFast( 0 , 0 );
		//畫底框
		d3dRt.BltFast( 220 , 40 , 420 , 440 , 0x8FFFFFFF );
		//文字框
		d3dRt.BltFast( 100 , 100 , 200 , 200 , 0X8F000000 );
		//下個方塊框
		d3dRt.BltFast( 440 , 60 , 560 , 180 ,  0x4F000000 );
		//畫方塊
		DrawNowBox();

	d3d_Device->EndScene();

	//繪製文字
	DrawText();

}
//畫文字
void Tetris::DrawText()
{
	int len ;
	char ch[256] ;

	//文字
	d3dHdc hdc ;
	SetBkMode( hdc , 1 );
	SetTextColor( hdc , RGB( 255 , 255 , 255 ));
	//目前分數
	len = wsprintf( ch , "分數：%d" , ts_DA.sScore );
	TextOut( hdc , 110 , 105 , ch , len );
	//已消行數
	len = wsprintf( ch , "行數：%d" , ts_DA.sDel );
	TextOut( hdc , 110 , 125 , ch , len );
	//關卡
	len = wsprintf( ch , "關卡：%d" , ts_DA.sLV );
	TextOut( hdc , 110 , 145 , ch , len );
}

//畫方塊
void Tetris::DrawBox(int gx, int gy , int index )
{
	int x , y ;

	if(( index >= 0 )&&( index < 7 ))
	{
		x = gx * 20 + 220 ;
		y = gy * 20 +  40 ;

		ts_Box[index].BltFast( x , y  );
	}
}

void Tetris::GetBox()
{
	int i ;
	//定控制基準點初始位置
	ts_DA.NowPoint.x = 3 ;
	ts_DA.NowPoint.y = -3 ;
	//定四個方塊位置
	ts_DA.BoxNowIndex = ts_DA.BoxNextIndex ;
	for( i = 0 ; i < 4 ; i++ )
	{
		ts_DA.NowBox[i].x = app_TetrisBox[ ts_DA.BoxNowIndex ][i].x ;
		ts_DA.NowBox[i].y = app_TetrisBox[ ts_DA.BoxNowIndex ][i].y ;
	}
	//下一個方塊
	ts_DA.BoxNextIndex = rand()%7;
	//時間初始化
	ts_DA.TimeGridNow = ts_DA.TimeNextGrid ;
}

void Tetris::DrawNowBox()
{
	int i , j ;
	int gx , gy ;
	BYTE index ;
	//畫底框方塊
	for( i = 0 ; i < 10 ; i++ )
	for( j = 0 ; j < 20 ; j++ )
		if( index = ts_DA.Data[i][j] )
			DrawBox( i , j , index - 1 );
	//畫目前方塊
	for( i = 0 ; i < 4 ; i++ )
	{
		gx = ts_DA.NowBox[i].x + ts_DA.NowPoint.x ;
		gy = ts_DA.NowBox[i].y + ts_DA.NowPoint.y ;
		if( IsInRect( gx , gy ))
			DrawBox(  gx , gy , ts_DA.BoxNowIndex );
	}
	//畫下一個方塊
	for( i = 0 ; i < 4 ; i++ )
		DrawBox(  app_TetrisBox[ts_DA.BoxNextIndex][i].x + 12 , app_TetrisBox[ts_DA.BoxNextIndex][i].y + 2 , ts_DA.BoxNextIndex );

}

void Tetris::BoxDown()
{
	int i ;
	int gx , gy ;
	//落下時間
	if( di_DDD.News4ing == 1 )
		ts_DA.TimeGridNow -= ( di_DDD._time * 5 );
	else
		ts_DA.TimeGridNow -= ( di_DDD._time  );
	//落下成功
	while( ts_DA.TimeGridNow < 0 )
	{
		ts_DA.TimeGridNow += ts_DA.TimeNextGrid ;
		if( IsDown( ts_DA.NowPoint.x , ts_DA.NowPoint.y + 1 ))
			ts_DA.NowPoint.y ++ ;
		else
		{
			//停下來的方塊若未進入格子內，代表遊戲結束
			for( i = 0 ; i < 4 ; i++ )
				if( !IsInRect( ts_DA.NowBox[i].x + ts_DA.NowPoint.x , ts_DA.NowBox[i].y + ts_DA.NowPoint.y ))
				{
					appTetrisGameOverRun();
					return ;
				}

			//資料到後緩衝區
			for( i = 0 ; i < 4 ; i++ )
			{
				gx = ts_DA.NowBox[i].x + ts_DA.NowPoint.x ;
				gy = ts_DA.NowBox[i].y + ts_DA.NowPoint.y ;
				ts_DA.Data[gx][gy] = ts_DA.BoxNowIndex + 1 ;
			}
			//判斷是否可消除
			ListDelIs();
			//取下一個方塊
			GetBox();
			//結束
			return ;
		}
	}
}

BOOL Tetris::IsDown( int nx , int ny )
{
	int i ;
	int gx , gy ;
	//
	for( i = 0 ; i < 4 ; i++ )
	{
		gx = ts_DA.NowBox[i].x + nx ;
		gy = ts_DA.NowBox[i].y + ny ;
		//是否到底
		if( gy >= 20 )
			return false ;
		//是否在格子內
		if( gy >= 0 )
		if( !IsInRect( gx , gy ))
			return false ;
		//下面是否有方塊
		else if( ts_DA.Data[gx][gy] )
			return false ;
	}
	//
	return true ;

}

BOOL Tetris::IsInRect(int gx, int gy)
{
	if(( gx >= 0 )&&( gx < 10 ))
		if(( gy >= 0 )&&( gy < 20 ))
			return true ;
	return false ;

}
//移動
void Tetris::Move()
{
	if( di_DDD.News4 == 3 )//→
	{
		if( IsDown( ts_DA.NowPoint.x + 1 , ts_DA.NowPoint.y ))
			ts_DA.NowPoint.x ++ ;
	}else if( di_DDD.News4 == 7 )//←
	{
		if( IsDown( ts_DA.NowPoint.x - 1 , ts_DA.NowPoint.y ))
			ts_DA.NowPoint.x -- ;
	}

}

//旋轉
void Tetris::Rotation()
{
	//轉換矩陣
	static const POINT ro[4][4] = 
	{
		{{ 3 , 0 } , { 2 , 0 } , { 1 , 0 } , { 0 , 0 }},
		{{ 3 , 1 } , { 2 , 1 } , { 1 , 1 } , { 0 , 1 }},
		{{ 3 , 2 } , { 2 , 2 } , { 1 , 2 } , { 0 , 2 }},
		{{ 3 , 3 } , { 2 , 3 } , { 1 , 3 } , { 0 , 3 }},
	};

	int i ;
	int gx , gy ;
	POINT po[4] ;
	//如果沒按下第一鍵，代表要不旋轉
	if( !di_DDD.Bn[0] )
		return ;
	//算轉換後位置
	for( i = 0 ; i < 4 ; i++ )
	{
		po[i].x = ro[ ts_DA.NowBox[i].x ][ ts_DA.NowBox[i].y].x ;
		po[i].y = ro[ ts_DA.NowBox[i].x ][ ts_DA.NowBox[i].y].y ;
		//判定是否在矩型內，如果不在矩型內不轉
		gx = po[i].x + ts_DA.NowPoint.x ;
		gy = po[i].y + ts_DA.NowPoint.y ;
		if( !IsInRect( gx , gy ))
			return ;
		//判定是否有方塊，如果有方塊不轉
		else if( ts_DA.Data[gx][gy] )
			return ;
	}
	//旋轉成功，設新的資料
	memcpy( ts_DA.NowBox , po , sizeof( po ));

}

//方塊消除
void Tetris::ListDelIs()
{
	//分數
	static const int Score[] = 	{ 0 ,100 , 300 , 1000 , 4000 };

	int i , j , k , num ;
	int list[20] ;
	//判斷橫的是否全部有方塊
	num = 0 ;
	memset( list , 0 , sizeof( list ));
	for( j = 19 ; j >= 0 ; j-- )
		for( i = 0 ; i < 10 ; i++ )
			if( ts_DA.Data[i][j] )
				list[j] ++ ;
	//判斷填滿行
	for( j = 0 ; j < 20 ; j++ )
		if( list[j] == 10 )
		{
			//消去該行
			for( i = 0 ; i < 10 ; i++ )
				ts_DA.Data[i][j] = 0 ;
			//消去行加一
			num ++ ;
		}

	if( num == 0 )
		return ;
	//方塊落下
	for( i = j = 19 ;( j >= 0 ); j-- )
	{
		//落下
		for( ; i >= 0 ; i-- )
			if( list[i] != 10 )
			{
				//如果兩者不同行，就上→下
				if( i != j )
					for( k = 0 ; k < 10 ; k++ )
					{
						ts_DA.Data[k][j] = ts_DA.Data[k][i] ;
						ts_DA.Data[k][i] = 0 ;
					}
				//行數上升
				i-- ;
				break ;
			}
	}
	//記錄分數
	ts_DA.sScore += Score[num] ;
	ts_DA.sDel += num ;
	ts_DA.sNLV -= num ;
	if( ts_DA.sNLV <= 0 )
	{
		ts_DA.sNLV = 6 ;
		ts_DA.sLV ++ ;
		if( ts_DA.sLV < 10 )
			ts_DA.TimeNextGrid = 60 + ( 10 - ts_DA.sLV )* 50 ;
	}
}

/*///////////////////////////////////////////



  準備開始



/*///////////////////////////////////////////
TetrisIs app_TetrisIs ;
void appTetrisIsRun()
{
	app_Tetris.Updata();
	app_Proc = &app_TetrisIs ;
	ts_Title.Create( "俄羅斯方塊.tga" );
}

void TetrisIs::Run()
{
	if( di_DDD.Bn[0] )
		appTetrisRun();

	Render();
}

void TetrisIs::Render()
{
	//畫背景
	app_Tetris.RenderBack();
	//畫文字
	d3d_Device->BeginScene();
		d3d_Device->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_SELECTARG1 );
		ts_Title.BltFast( 0 , 0 );
	d3d_Device->EndScene();
	//成像
	d3d_Device->Present( NULL , NULL , NULL , NULL );
}

/*///////////////////////////////////////////



  遊戲結束



/*///////////////////////////////////////////
TetrisGameOver app_TSGameOver ;
void appTetrisGameOverRun()
{
	app_Proc = &app_TSGameOver ;
	ts_Title.Create( "gameover.tga" );
}

void TetrisGameOver::Run()
{
	if( di_DDD.Bn[0] )
		appTetrisIsRun();
	//繪製
	Render();
}

void TetrisGameOver::Render()
{
	//畫背景
	app_Tetris.RenderBack();
	//畫文字
	d3d_Device->BeginScene();
		d3d_Device->SetTextureStageState( 0 , D3DTSS_ALPHAOP , D3DTOP_SELECTARG1 );
		ts_Title.BltFast( 0 , 0 );
	d3d_Device->EndScene();

	//成像
	d3d_Device->Present( NULL , NULL , NULL , NULL );

}
