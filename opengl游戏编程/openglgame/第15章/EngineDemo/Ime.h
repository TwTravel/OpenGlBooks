//======================================================================
/**
*  @file      Ime.h
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

#ifndef __IME_H__
#define __IME_H__


#include <imm.h>

#pragma comment ( lib, "imm32.lib" )

//! 接受字符串的最大长度
#define STR_MAX_LENGTH 256



/** \brief
 *  封装了ime相关的处理函数的类，输入法相关。
 */
class CIme
{
public:
	CIme();
	~CIme(){Release();}


	/**\brief
	 * 关闭并禁止输入法，如ime已经打开则关闭，
	 * 此后玩家不能用热键呼出ime
	 */
	void Disable();


	/**\brief
	 * 允许输入法，此后玩家可以用热键呼出ime
	 */
	void Enable(){Disable();m_bIme = true;}


	/**\brief
	 * 设置是否接收输入字符
	 */
	void SetReceiveChar(bool _b){m_bReceive=_b;}


	/**\brief
	 * 切换到下一种输入法，必须EnableIme后才有效
	 */
	void NextIme();


	/**\brief
	 * 切换全角/半角
	 */
	void SharpIme( HWND hWnd );


	/**\brief
	 * 切换中/英文标点
	 */
	void SymbolIme( HWND hWnd );


	/**\brief
	 * 状态函数
	 */
	char* GetImeName();

	/**\brief
	 * 是否按了回车键
	 */
	BOOL GetReturnState(){return m_bReturn?TRUE:FALSE;}

	/**\brief
	 * 是否全角
	 */
	bool IfImeSharp();


	/**\brief
	 * 是否中文标点
	 */
	bool IfImeSymbol();


	char*   GetImeInputCompStr();
	char*   GetImeInputCompReadStr();
	int     GetImeInputCursor();
	char*   GetImeInputCandList();
	char*   GetImeInputResultStr();


	//@{
	//! 必须在消息中调用的函数，如果返回是true，
	//  则窗口函数应直接返回0，否则应传递给DefWindowProc

	/** \brief
	 *  切换输入法的时候收到的消息。
	 *
	 *  如果禁止ime则返回false，此时窗口函数应返回0，否则DefWindowProc会打开输入法
	 */
	virtual bool OnWM_INPUTLANGCHANGEREQUEST();
	virtual bool OnWM_INPUTLANGCHANGE( HWND hWnd);
	virtual bool OnWM_IME_SETCONTEXT(){ return true; }
	virtual bool OnWM_IME_STARTCOMPOSITION(){ return true; }
	virtual bool OnWM_IME_ENDCOMPOSITION(){ return true; }
	virtual bool OnWM_IME_NOTIFY( HWND hWnd, WPARAM wParam );
	virtual bool OnWM_IME_COMPOSITION( HWND hWnd, LPARAM lParam );
	//@} 

	void Release();

	void AddCharToResultStr(char* _Char);


	/** \brief
	*获得输入字符 把得到的输入字符放入字符串列表中
	*/
	void ReceiveChar(TCHAR _iCharCode);


	/** \brief
	* 在插入符位置插入一个字符串
	*
	* \param	_Char	插入的字符串
	* \param	_dwSize	字符串的长度
	*/
	void ReceiveChar(char* _Char, DWORD _dwSize);


	/** \brief
	* 取得插入符的位置
	*/
	int GetCaret(void){return m_iImeCaret;}


	/**\brief
	 * 取得插入符在字中的位置
	 */
	int GetWordCaret(void){return m_iWordCaret;}


	/** \brief
	* "回退" 键处理
	*/
	void BackspaceKey(void);


	/** \brief
	* "DEL" 键处理
	*/
	void DeleteKey(void);


	/** \brief
	* "向右"方向键的处理
	*/
	void RightKey(void);


	/** \brief
	* "向左"方向键的处理
	*/
	void LeftKey( void );


	/**\brief
	 * 回车键的处理
	 */
	void ReturnKey(void){m_bReturn=true;}


	/**\brief
	 * 取得当前的IME对象
	 */
	static CIme* GetSysIme(){return m_pIme;}


private:
	//! ime静态指针
	static CIme* m_pIme;

	//! ime允许标志
	bool	m_bIme;

	//! 是否从键盘接收字符<除回退键>
	bool	m_bReceive;

	//! 是否按了回车
	bool	m_bReturn;

	//! 存储转换后的串
	char	m_szCompStr[ MAX_PATH ];

	//! 存储输入的串
	char	m_szCompReadStr[ MAX_PATH ];

	//! 存储整理成字符串选字表
	char	m_szCandList[ MAX_PATH ];

	//! 存储转换后的串中的光标位置
	int		m_nCursor;

	//! 存储标准的选字表
	CANDIDATELIST *m_lpCandList;

	//! 全角标志
	bool	m_bSharp;

	//! 中文标点标志
	bool	m_bSymbol;

	/**\brief
	 *  这里面存储我们使用输入法的时候，输入的字串。
	 *  上层操作只需要得到这个字串就可以了。
	 */
	char	m_chResultStr[STR_MAX_LENGTH];


	//! 存储输入法的名字
	char	m_szName[64];

	//! 当前接受字符的字符串可以接受字符的最大程度
	int		m_iMaxLen;

	//-------------------------------------------
	//! 插入符的位置 插入符在那个字符的后边
	int		m_iImeCaret;

	//! 插入符在字中的位置
	int		m_iWordCaret;
	//--------------------------------------------

	/**\brief
	 * 将选字表整理成串
	 */
	void ConvertCandList( CANDIDATELIST *pCandList,char *pszCandList );

};




///////////////////////////////////////////////
//--------------Inline函数实现-----------------

inline void CIme::NextIme()
{
	if( m_bIme )ActivateKeyboardLayout((HKL)HKL_NEXT,0);
}

inline void CIme::SharpIme( HWND hWnd )
{
	ImmSimulateHotKey( hWnd, IME_CHOTKEY_SHAPE_TOGGLE);
}

inline void CIme::SymbolIme( HWND hWnd )
{
	ImmSimulateHotKey( hWnd, IME_CHOTKEY_SYMBOL_TOGGLE);
}


/**\brief
 * 切换输入法的时候收到的消息。
 *
 * 如果禁止ime则返回false，此时窗口函数应返回0，否则DefWindowProc会打开输入法
 */
inline bool CIme::OnWM_INPUTLANGCHANGEREQUEST()
{
	return !m_bIme;
}


inline char* CIme::GetImeName()
{
	return m_szName[0]?m_szName:NULL;
}

/**\brief
 * 是否全角
 */
inline bool CIme::IfImeSharp()
{
	return m_bSharp;
}

/**\brief
 * 是否中文标点
 */
inline bool CIme::IfImeSymbol()
{
	return m_bSymbol;
}

inline char* CIme::GetImeInputCompStr()
{
	return  m_szCompStr[0] ? m_szCompStr : NULL;
}

inline char *CIme::GetImeInputCompReadStr()
{
	return  m_szCompReadStr[0] ? m_szCompReadStr : NULL;
}

inline int CIme::GetImeInputCursor()
{
	return  m_nCursor;
}

inline char* CIme::GetImeInputCandList()
{
	return  m_szCandList[0] ? m_szCandList : NULL;
}

inline	char* CIme::GetImeInputResultStr()
{
	return  m_chResultStr;
}

inline void CIme::AddCharToResultStr(char* _Char)
{
	strcat(m_chResultStr,_Char);
}


#endif