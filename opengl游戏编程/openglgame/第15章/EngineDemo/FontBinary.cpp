//========================================================
/**
*  @file      FontBinary.cpp
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

#include "stdafx.h"
#include ".\fontbinary.h"
#include <cstdio>
#include <io.h>
#include <string>

unsigned char* FontBinary::wordLib_ = 0;
unsigned int	FontBinary::libSize_ = 0;
FontBinary::AutoPtr FontBinary::aptr_(0);

const FontBinary fontbinary_s_c("sys\\HZK16");

using namespace std;


FontBinary::FontBinary(void)
{
}


FontBinary::FontBinary(char* fileName)
{
	if (!wordLib_)
	{
		FILE* fp=fopen(fileName,"rb");
		if (fp)
		{
			libSize_ = filelength(fp->_file);
			wordLib_ = new unsigned char[libSize_];
			fread(wordLib_,libSize_,sizeof(unsigned char),fp);
			fclose(fp);
			aptr_.ptr_=0;
		}
	}
}


FontBinary::~FontBinary(void)
{
}


CharacterBinary
FontBinary::Format(const unsigned char* code)
{
	unsigned int pos=0;
	if (code && *(unsigned char*)code)
		while (* (unsigned char*)code)
			if (character[pos].Format( code ))
				++pos;

	CharacterBinary binary;
	unsigned int i=0;
	unsigned int j=0;
	for (;i < pos;++i)
	{
		binary.high_ = character[i].GetWidth();
		binary.width_+= binary.high_;
		j += (binary.high_>>3)*character[i].GetHigh();
	}

	binary.high_=16;
	binary.size_ = j;
	binary.binary_ = new unsigned char[j];
	unsigned char* data = binary.binary_;

	for (j=0;j < 16;++j)
		for (i=0;i < pos;++i)
			character[i].GetLine( data, j);

	return binary;
}



FontBinary::Character::Character(const unsigned char*& inCharacter)
:high_(0),width_(0)
{
	Format(inCharacter);
}


unsigned int
FontBinary::Character::Format(const unsigned char *& inCharacter)
{
	unsigned int retNum=0;
	if (inCharacter)
		if ( *(unsigned char*)inCharacter > 0x80)
			retNum=FormatChinese(inCharacter);
		else
			retNum=FormatEnglish(inCharacter);
	return retNum;
}


unsigned int
FontBinary::Character::FormatChinese(const unsigned char*& inCharacher)
{
	high_=width_=16;
	unsigned int offset = (94*(*(unsigned char*)inCharacher-0xa0-1)
		+ ( *( (unsigned char*)inCharacher + 1)-0xa0-1))*32;
    memcpy(binary_ , wordLib_ + offset,32);
	inCharacher += 2;
	int	bin= (int)binary_;
	__asm
	{
		mov ecx,bin
		mov edx,ecx
		add edx,28

again_loop:
		mov		eax,[ecx]
		xchg	eax,[edx]
		xchg	eax,[ecx]
		add		ecx,4
		sub		edx,4
		cmp		edx,ecx
		ja		again_loop

		//;开始双字的高低字互换
		push ebx
		mov ecx,0x8
		mov ebx,bin
chg_again:
		mov eax,[ebx]
		mov dx,ax
		shr eax,16
		shl edx,16
		or	eax,edx
		mov [ebx],eax
		add ebx,4
		dec ecx
		jnz chg_again
		pop ebx

	}
	return 0x20;
}


unsigned int
FontBinary::Character::FormatEnglish(const unsigned char*& uc)
{
	high_=16;
	width_=16;
	unsigned int offset=(*uc + 0x9b)<<5;

    memcpy(binary_ , wordLib_ + offset,32);

//begin_change:
	++uc;
	int	bin= (int)binary_;

	__asm
	{
		mov ecx,bin
		mov edx,ecx
		add edx,28

again_loop:
		mov		eax,[ecx]
		xchg	eax,[edx]
		xchg	eax,[ecx]
		add		ecx,4
		sub		edx,4
		cmp		edx,ecx
		ja		again_loop

		//;开始双字的高低字互换
		push ebx
		mov ecx,0x8
		mov ebx,bin
chg_again:
		mov eax,[ebx]
		mov dx,ax
		shr eax,16
		shl edx,16
		or	eax,edx
		mov [ebx],eax
		add ebx,4
		dec ecx
		jnz chg_again
		pop ebx

	}

	return 32;
}


unsigned int
FontBinary::Character::GetLine(unsigned char*& pout,unsigned int line)const
{
	unsigned int retNum=0;
	if (line < high_)
	{
		unsigned int byte = (width_>>3);
		size_t offset = line* byte;
		memcpy(pout,binary_+offset,byte);
		pout+= byte;
		retNum = byte;
	}
	return retNum;
}




CharacterBinary::CharacterBinary(void)
:high_(0),width_(0),binary_(0),size_(0)
{
}


CharacterBinary::CharacterBinary(const char* bn)
:high_(0),width_(0),size_(0)
{
	FontBinary fn;
	*this = fn.Format( ( unsigned char*)bn);
}


CharacterBinary::CharacterBinary(const unsigned char* bn)
:high_(0),width_(0),binary_(0),size_(0)
{
	FontBinary fn;
	*this=fn.Format(bn);
}


CharacterBinary::CharacterBinary(const CharacterBinary& cb)
:high_(cb.high_),width_(cb.width_),size_(0)
{
	size_t size = ((high_*width_)>>3);
	size_=size;
	binary_ = new unsigned char [size];
	memcpy(binary_,cb.binary_,size);
}


CharacterBinary::~CharacterBinary(void)
{
	delete []binary_;
}


void
CharacterBinary::Format(const char* cb)
{
	FontBinary fn;
	*this = fn.Format( (unsigned char*) cb);
}


void
CharacterBinary::Format(const unsigned char* cb)
{
	FontBinary fn;
	*this = fn.Format( (unsigned char*) cb);
}


CharacterBinary&
CharacterBinary::operator = (const CharacterBinary& cb)
{
	if ( &cb != this)
	{
		high_=cb.high_;
		width_=cb.width_;
		size_t size=((high_*width_)>>3);
		if (size_ < size)
		{
			size_ = size;
			delete []binary_;
			binary_ = new unsigned char[ size ];
		}

		memcpy(binary_,cb.binary_,size);
	}
	return *this;
}

