
#include <windows.h>

#include "di.h"
/*/////////////////////////////////////////////////////////////////////


  基本物件


/*/////////////////////////////////////////////////////////////////////

LPDIRECTINPUT7			di_DI;        //宣告 DirectInput 物件指標
LPDIRECTINPUTDEVICE7	di_Key ; //宣告輸入裝置物件指標

DWORD					di_TimeBuffer ;

BYTE					di_KeyData[256] ; //各別單放

DI_DEVICE_DATA di_DDD ;
/*/////////////////////////////////////////////////////////////////////

  建立

/*/////////////////////////////////////////////////////////////////////
BOOL diCreate( HINSTANCE hInst , HWND hWnd )
{
	di_DI = NULL ;
	di_Key = NULL ;
	memset( &di_DDD , 0 , sizeof( di_DDD ));

	//建立 DirectInput 物件
	if( DirectInputCreateEx(hInst, DIRECTINPUT_VERSION, 
        IID_IDirectInput7, (void**)&di_DI, NULL) != DI_OK )
		return false ;
	//建立輸入裝置物件
	if( di_DI->CreateDeviceEx(GUID_SysKeyboard, IID_IDirectInputDevice7,
        (void**)&di_Key, NULL) != DI_OK)
		return false ;
	//設定資料格式
	if( di_Key->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
		return false ;
	//設定程式協調層級
	if( di_Key->SetCooperativeLevel(hWnd, 
                   DISCL_BACKGROUND | DISCL_NONEXCLUSIVE) != DI_OK)
		return false ;
	//取用輸入裝置
	if( di_Key->Acquire() != DI_OK)
		return false ;
	//取目前時間
	di_TimeBuffer = timeGetTime();
	//結束
	return true ;
}

/*/////////////////////////////////////////////////////////////////////

  移除

/*/////////////////////////////////////////////////////////////////////
void diRelease()
{
	if( di_Key )di_Key->Release();
	if( di_DI )di_DI->Release();
}

/*/////////////////////////////////////////////////////////////////////

  取用裝置

/*/////////////////////////////////////////////////////////////////////
void diRun()
{
	int i ;
	int news ;
	DWORD tNow ;
	const BYTE key[4] = { DIK_Z , DIK_X , DIK_C , DIK_V , };
	//時間
	do
	{
		tNow = timeGetTime();
		di_DDD._time = tNow - di_TimeBuffer ;
	}while( di_DDD._time < 15 );
	if( di_DDD._time > 40 )di_DDD._time = 40 ;
	di_TimeBuffer = tNow ;
	//取得鍵盤狀態
	if( di_Key )
    if( di_Key->GetDeviceState( 256 ,(LPVOID)&di_KeyData )== DI_OK )
	{
	//方向鍵
		if( di_KeyData[ DIK_DOWN] & 0x80 )
			news = 1 ;//下
		else if( di_KeyData[ DIK_UP ] & 0x80 )
			news = 5 ;//上
		else if( di_KeyData[ DIK_LEFT ] & 0x80 )
			news = 7 ;//左
		else if( di_KeyData[ DIK_RIGHT ] & 0x80 )
			news = 3 ;//右
		else news = 0 ;
	//方向移動中
		if( news == di_DDD.News4ing )
		{
			if( di_DDD.News4ing )
				di_DDD.News4 = 0 ;
			else 
			{
				di_DDD.News4 = news ;
				di_DDD.News4ing = news ; 
			}
		}else
		{
			di_DDD.News4 = news ;
			di_DDD.News4ing = news ;
		}
	//按鍵
		for( i = 0 ; i < 4 ; i++ )
		{
			if( di_KeyData[ key[i]] & 0x80 )
			{
				if( di_DDD.Bning[i] )
					di_DDD.Bn[i] = false ;
				else
				{
					di_DDD.Bn[i] = true ;
					di_DDD.Bning[i] = true ;
				}
			}else
			{
				di_DDD.Bn[i] = false ;
				di_DDD.Bning[i] = false ;
			}
		}
	}
}