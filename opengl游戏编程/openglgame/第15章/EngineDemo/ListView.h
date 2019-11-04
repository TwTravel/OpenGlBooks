//========================================================
/**
*  @file      ListView.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  列表框类
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================

#ifndef _LIST_VIEW_H_
#define _LIST_VIEW_H_

#include "window.h"
#include "button.h"
#include "ScrollBar.h"
#include <vector>
using namespace std;

//! 每个列表项的高度
#define ITEM_HEIGHT 20


/**\brief
 * ListView列表框类
 */
class ListView : public CWindow
{
public:
	ListView(void);
	~ListView(void){}


	/**\brief
	 * 取得现在的总项数
	 */
	unsigned int GetItemNum(){return m_vItems.size();}


	/**\brief
	 * 取得当前选中项的序号
	 */
	unsigned int GetCurrItem(){return m_nCurrItem;}


	/**\brief
	 * 取得列表项上的字符串
	 */
	char* GetItemText(unsigned int _nWhere);


	/**\brief
	 * 取得边框宽度
	 */
	int GetBorderWidth(){return m_iBorderWidth;}


	/**\brief
	 * 设置边框宽度
	 */
	void SetBorderWidth(int _width){m_iBorderWidth=_width;}


	/**\brief
	 * 取得列表项的颜色
	 */
	SColor GetItemColor(){return m_sItemColor;}


	/**\brief
	 * 设置列表框的颜色
	 */
	void SetItemColor(SColor _color){m_sItemColor=_color;}


	/**\brief
	 * 设置字体的颜色，在Insert之前调用此函数
	 */
	void SetTextColor(SColor _color){m_sTextColor=_color;}


	/**\brief
	 * 插入一个列表项
	 *
	 *\param	_iWhere	插入的位置，为-1时插到表尾
	 *\param	_str	插入项上的内容
	 *\return	BOOL	插入是否成功
	 */
	BOOL InsertItem(unsigned int _iWhere,char* _str);


	/**\brief 
	 * 删除一个列表项，默认参数-1时删除表尾元素
	 */
	BOOL DeleteItem(unsigned int _iWhere=-1);


	/**\brief
	 * 取得滚动条的指针
	 */
	CScrollBar* GetScrollBar(){return &m_cScroll;}


private:
	//! 列表项链表
	vector<CButton> m_vItems;

	//! 右边的滚动条
	CScrollBar m_cScroll;

	//! 边框宽度
	int m_iBorderWidth;

	//! 当前选中项
	unsigned int m_nCurrItem;

	//! 当前选中项上的字符串
	char* m_chCurrText;

	//! 列表项的颜色
	SColor m_sItemColor;

	//! 字体的颜色
	SColor m_sTextColor;

	/**\brief
	 * 绘制列表
	 */
	virtual void OnDraw(void);


	//--------------------------
	//下面的参数只是渲染时要用的中间变量

	//! 不用滚动时最多能放下的项数
	unsigned int m_nMaxItemNum;

	//! 列表项的宽
	int m_iItemWidth;

	//! 列表项的x
	int m_iItemX;

	//! 第一个列表项的y
	int m_iFirstY;


};



#endif