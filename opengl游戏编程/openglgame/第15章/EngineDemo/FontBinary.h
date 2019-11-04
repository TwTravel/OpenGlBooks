//========================================================
/**
*  @file      FontBinary.h
*
*  项目描述： 构造游戏引擎
*  文件描述:  基础的一个字符库类，对应一个字库，通过构造
*             函数中传递的文件名来确定其字库
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2007-07-26	
*  修改日期： 2007-07-26
*
*/ 
//========================================================
#pragma once

class CharacterBinary;

/**\brief
 * 基础的一个字符库类
 */
class FontBinary
{
public:
	FontBinary(char* fileName);//字库文件
	FontBinary(void);
	~FontBinary(void);

	CharacterBinary	Format(const unsigned char* code);
protected:

	class	Character
	{
	public:
		Character(void){}
		Character(const unsigned char*& inCharacter);
		~Character(void){}

		unsigned int	Format(const unsigned char*&);
		unsigned int	GetLine(unsigned char*& pout,unsigned int line)const;
		unsigned int	GetWidth(void)const{return width_;}
		unsigned int	GetHigh(void)const{return high_;}

	private:
		unsigned int	FormatChinese(const unsigned char*&);
		unsigned int	FormatEnglish(const unsigned char*&);
		void			ChangeModel(void);

		unsigned char	high_;
		unsigned char	width_;
		unsigned char	binary_[32];
	};

	struct AutoPtr
	{
		AutoPtr(void* ptr):ptr_(ptr){}
		~AutoPtr(void){delete []ptr_;}
		void* ptr_;
	};


	Character	character[128];
	static AutoPtr	aptr_;
	static unsigned char*	wordLib_;
	static unsigned int		libSize_;
};


//********************************************************
//通过上面那个字库类来格式化一个字符串
//字符个数不能超过128，不是长度，而就是个数
//*****************************************************


class CharacterBinary
{
	friend FontBinary;
public:
	CharacterBinary(void);

	//通过传入字符串来获得格式化的二进制点阵，只要用这种方式
	//构造的，不用再调用Format命令了
	CharacterBinary(const unsigned char*);
	CharacterBinary(const char*);

	CharacterBinary(const CharacterBinary&);
	~CharacterBinary(void);

	const	unsigned char* GetBinary(void)const{return binary_;}
	unsigned int	GetWidth(void)const{return width_;}
	unsigned int	GetHigh(void)const{return high_;}

	//格式化一个字串，得到一个二进制的字串点阵
	void			Format(const unsigned char*);
	void			Format(const char*);

	CharacterBinary&	operator = (const CharacterBinary&);
private:
	unsigned int	high_;
	unsigned int	width_;
	unsigned int	size_;
	unsigned char* binary_;
};