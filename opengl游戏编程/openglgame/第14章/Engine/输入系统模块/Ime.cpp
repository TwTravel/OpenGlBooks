//======================================================================
/**
*  @file      Ime.cpp
*
*  项目描述： 构造游戏引擎
*   
*  文件描述:  输入法IME类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-24	
*  修改日期： 2007-07-24
*/																			
//======================================================================


#include "stdafx.h" 
#include "ime.h"

CIme* CIme::m_pIme=NULL;


CIme::CIme():m_lpCandList(NULL),m_iImeCaret(-1),m_iWordCaret(-1),
			m_bReceive(true),m_iMaxLen(STR_MAX_LENGTH)
{
	m_pIme=this;
	m_chResultStr[0] =0 ;
	Disable(); 	//通过Disable初始化一些数据
}


void CIme::Release()
{
	Disable();
	if( m_lpCandList )
	{
		GlobalFree( (HANDLE)m_lpCandList );
		m_lpCandList = NULL;
	}
}



bool CIme::OnWM_INPUTLANGCHANGE( HWND hWnd)
{
	//输入法改变
	HKL hKL = GetKeyboardLayout( 0 );
	if( ImmIsIME( hKL ))
	{
		HIMC hIMC = ImmGetContext( hWnd );

		DWORD dwConversion;
		ImmGetConversionStatus( hIMC, &dwConversion,NULL );
		m_bSharp = ( dwConversion & IME_CMODE_FULLSHAPE )? true : false;//取得全角标志
		m_bSymbol = ( dwConversion & IME_CMODE_SYMBOL )? true : false;//取得中文标点标志

		ImmEscape( hKL, hIMC, IME_ESC_IME_NAME, m_szName );//取得新输入法名字
		ImmReleaseContext( hWnd, hIMC );
	}
	else//英文输入
		//m_szImeName[ 0 ] = 0;
		strcpy(m_szName,"英文");
	return false;//总是返回false，因为需要窗口函数调用DefWindowProc继续处理
}


bool CIme::OnWM_IME_NOTIFY( HWND hWnd, WPARAM wParam )
{
	HIMC hIMC;
	DWORD dwSize;
	DWORD dwConversion, dwSentence;
	switch( wParam )
	{
	case IMN_SETCONVERSIONMODE://全角/半角，中/英文标点改变
		hIMC = ImmGetContext( hWnd );
		ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
		m_bSharp = ( dwConversion & IME_CMODE_FULLSHAPE )? true : false;
		m_bSymbol = ( dwConversion & IME_CMODE_SYMBOL )? true : false;
		ImmReleaseContext( hWnd, hIMC );
		break;

		//case IMN_OPENCANDIDATE://进入选字状态
	case IMN_CHANGECANDIDATE://选字表翻页
		hIMC = ImmGetContext( hWnd );
		if( m_lpCandList )
		{
			GlobalFree( (HANDLE)m_lpCandList );
			m_lpCandList = NULL;
		}//释放以前的选字表
		if( dwSize = ImmGetCandidateList( hIMC, 0, NULL, 0 ))
		{
			m_lpCandList = (LPCANDIDATELIST)GlobalAlloc( GPTR, dwSize );
			if( m_lpCandList )
				ImmGetCandidateList( hIMC, 0, m_lpCandList, dwSize );
		}//得到新的选字表
		ImmReleaseContext( hWnd, hIMC );
		if( m_lpCandList )
			ConvertCandList( m_lpCandList, m_szCandList );//选字表整理成串
		break;

	case IMN_CLOSECANDIDATE://关闭选字表
		if( m_lpCandList )
		{
			GlobalFree( (HANDLE)m_lpCandList );
			m_lpCandList = NULL;
		}//释放
		m_szCandList[ 0 ] = 0;
		break;
	}

	return true;//总是返回true，防止ime窗口打开
}


bool CIme::OnWM_IME_COMPOSITION( HWND hWnd, LPARAM lParam )
{//输入改变
	HIMC hIMC;
	DWORD dwSize;
	hIMC = ImmGetContext( hWnd );
	//取得szCompStr
	if( (DWORD)lParam & GCS_COMPSTR )
	{
		dwSize = ImmGetCompositionString( hIMC, GCS_COMPSTR, (void*)m_szCompStr, sizeof( m_szCompStr ));
		m_szCompStr[ dwSize ] = 0;
	}
	//取得szCompReadStr
	if( (DWORD)lParam & GCS_COMPREADSTR )
	{
		dwSize = ImmGetCompositionString( hIMC, GCS_COMPREADSTR, (void*)m_szCompReadStr, sizeof( m_szCompReadStr ));
		m_szCompReadStr[ dwSize ] = 0;
	}
	//取得nImeCursor
	if( lParam & GCS_CURSORPOS )
	{
		m_nCursor = 0xffff & ImmGetCompositionString( hIMC, GCS_CURSORPOS, NULL, 0 );
	}

	if( lParam & GCS_RESULTSTR )
	{
		char str[ MAX_PATH ];
		dwSize = ImmGetCompositionString( hIMC, GCS_RESULTSTR, (void*)str, sizeof( str ));//取得汉字输入串
		str[ dwSize ] = 0;
		ReceiveChar( str, dwSize );
	}

	ImmReleaseContext( hWnd, hIMC );
	return true;//总是返回true，防止ime窗口打开
}


void  CIme::Disable()
{
	//while( ImmIsIME( GetKeyboardLayout( 0 )))
	//	ActivateKeyboardLayout(( HKL )HKL_NEXT, 0 );//如果ime打开通过循环切换到下一个关闭
	m_bIme = false;
	m_szName[ 0 ] = 0;

	m_szCompReadStr[ 0 ] = 0;
	m_szCandList[ 0 ] = 0;
	m_nCursor = 0;

	m_chResultStr[0] = 0;
	m_iImeCaret=-1;
	m_iWordCaret=-1;
	m_bReceive=true;
	m_bReturn=false;
}


void CIme::ConvertCandList( CANDIDATELIST *pCandList, char *pszCandList )
{//转换CandidateList到一个串，\t分隔每一项
	unsigned int i;
	if( pCandList->dwCount < pCandList->dwSelection )
	{
		pszCandList[ 0 ] = 0;
		return;
	}

	//待选字序号超出总数，微软拼音第二次到选字表最后一页后再按PageDown会出现这种情况，并且会退出选字状态，开始一个新的输入
	//但微软拼音自己的ime窗口可以解决这个问题，估计微软拼音实现了更多的接口，所以使用了这种不太标准的数据
	//我现在无法解决这个问题，而且实际使用中也很少遇到这种事，而且其它标准输入法不会引起这种bug
	//非标准输入法估计实现的接口比较少，所以应该也不会引起这种bug
	for( i = 0; ( i < pCandList->dwCount - pCandList->dwSelection )&&( i < pCandList->dwPageSize ); ++i )
	{
		*pszCandList++ = ( i % 10 != 9 )? i % 10 + '1' : '0';//每项对应的数字键
		*pszCandList++ = '.';//用'.'分隔
		strcpy( pszCandList, (char*)pCandList
			+ pCandList->dwOffset[ pCandList->dwSelection + i ] );//每项实际的内容
		pszCandList += strlen( pszCandList );
		*pszCandList++ = '\t';//项之间以'\t'分隔
	}
	*( pszCandList - 1 )= 0;//串尾，并覆盖最后一个'\t'
}


void CIme::ReceiveChar(TCHAR _iCharCode)
{
	if(!m_bIme)return;
	if(!m_bReceive&&_iCharCode!='\b')return;

	if( !_iCharCode )return;

	switch(_iCharCode) 
	{
	case '\b':
		BackspaceKey();
		break;
	case '\r':break;
	case '\t':break;
	case VK_ESCAPE:break;
	default:
		{
			int t_iLen = (int)strlen(m_chResultStr);

			if(t_iLen >= m_iMaxLen-1)
			{
				t_iLen = m_iMaxLen-1;
			}
			if( m_iImeCaret < m_iMaxLen-1 
				&& (int)strlen(m_chResultStr) < m_iMaxLen )
			{
				++m_iImeCaret;
				++m_iWordCaret;//////////////

				//将插入符后面的每个字符向后移动一位
				for(int i = t_iLen; i>=m_iImeCaret; i--)
				{
					m_chResultStr[i+1] = m_chResultStr[i];
				}
				m_chResultStr[m_iImeCaret] = (char)_iCharCode;
			}
			break;
		}
	}
}

void CIme::ReceiveChar(char* _Char, DWORD _dwSize)
{
	if(!m_bIme)return;
	if(!m_bReceive&&_Char[0]!='\b')return;

	int t_iLen;
	int t_iLen2 = (int)strlen(_Char);

	while ( (int)strlen(m_chResultStr)+t_iLen2 > m_iMaxLen )
	{
		t_iLen2 -= 2;
	}
	
	m_iWordCaret+=(t_iLen2%2==1)?((t_iLen2+1)/2):(t_iLen2/2);/////////////

	for(int i=0; i<t_iLen2; ++i )
	{
		t_iLen = (int)strlen(m_chResultStr);

		if( t_iLen >= m_iMaxLen-1 )
		{
			t_iLen = m_iMaxLen-1;
		}
		if( m_iImeCaret < m_iMaxLen-1 )
		{
			++m_iImeCaret;
			//将插入符后面的每个字符向后移动一位
			for(int j = t_iLen; j>=m_iImeCaret; j--)
			{
				m_chResultStr[j+1] = m_chResultStr[j];
			}
			if( m_iImeCaret < m_iMaxLen )
				m_chResultStr[m_iImeCaret] = _Char[i];
		}
	}

}


/** \brief
* "向右"方向键的处理
*/
void CIme::RightKey( void )
{
	int t_iLen = (int)strlen(m_chResultStr);

	if( m_iImeCaret < t_iLen-2 )
	{
		// 判断是否是汉字
		if( (BYTE)m_chResultStr[m_iImeCaret+1] > 160
			&& (BYTE)m_chResultStr[m_iImeCaret+2] > 160 )
			m_iImeCaret +=2;
		else
            ++m_iImeCaret;
		++m_iWordCaret;/////////////
	}
	else if( m_iImeCaret < t_iLen-1 )
	{
		++m_iImeCaret;
		++m_iWordCaret;/////////////
	}
}

/** \brief
* "向左"方向键的处理
*/
void CIme::LeftKey( void )
{
	//判断汉字
	if( m_iImeCaret-1 > -1 )
	{
		if( (BYTE)m_chResultStr[m_iImeCaret-1] > 160
			&& (BYTE)m_chResultStr[m_iImeCaret] > 160 )
		{
			m_iImeCaret -=2;
		}
		else
			m_iImeCaret--;
		m_iWordCaret--;////////////
	}
	else if( m_iImeCaret > -1 )
	{
		m_iImeCaret--;
		m_iWordCaret--;////////////
	}
}

/** \brief
* "回退" 键处理
*/
void CIme::BackspaceKey(void)
{
	int t_iLen = (int)strlen(m_chResultStr);
	//判断汉字
	if( m_iImeCaret-1 > -1 )
	{
		//删除汉字
		if( (BYTE)m_chResultStr[m_iImeCaret-1] > 160
			&& (BYTE)m_chResultStr[m_iImeCaret] > 160 )
		{
			for(int i = m_iImeCaret; i<t_iLen; ++i)
			{
				m_chResultStr[i] = m_chResultStr[i+1];
			}
			m_iImeCaret--;
			for(int i = m_iImeCaret; i<t_iLen; ++i)
			{
				m_chResultStr[i] = m_chResultStr[i+1];
			}
			m_iImeCaret--;
		}
		else//删除一个字母
		{
			for(int i = m_iImeCaret; i<t_iLen; ++i)
			{
				m_chResultStr[i] = m_chResultStr[i+1];
			}
			m_iImeCaret--;
		}
		m_iWordCaret--;//////////////
	}
	else if( m_iImeCaret > -1 )
	{
		for(int i = m_iImeCaret; i<t_iLen; ++i)
		{
			m_chResultStr[i] = m_chResultStr[i+1];
		}
		m_iImeCaret--;
		m_iWordCaret--;////////////
	}
	if(m_iWordCaret<-1)m_iWordCaret=-1;///////////
}


/** \brief
* "DEL" 键处理
*/
void CIme::DeleteKey(void)
{
	int t_iLen = (int)strlen(m_chResultStr);

	if( m_iImeCaret < t_iLen-2 )
	{
		// 判断是否是汉字
		if( (BYTE)m_chResultStr[m_iImeCaret+1] > 160
			&& (BYTE)m_chResultStr[m_iImeCaret+2] > 160 )
		{
			//将插入符后面的每个字符向前移动二位
			for(int i = m_iImeCaret+1; i<t_iLen-1; ++i)
			{
				m_chResultStr[i] = m_chResultStr[i+2];
			}
		}
		else
		{
			//将插入符后面的每个字符向前移动一位
			for(int i = m_iImeCaret+1; i<t_iLen; ++i)
			{
				m_chResultStr[i] = m_chResultStr[i+1];
			}
		}
	}
	else if( m_iImeCaret < t_iLen-1 )
	{
		//将插入符后面的每个字符向前移动一位
		for(int i = m_iImeCaret+1; i<t_iLen; ++i)
		{
			m_chResultStr[i] = m_chResultStr[i+1];
		}
	}

	if(m_iWordCaret<-1)m_iWordCaret=-1;////////////
}


