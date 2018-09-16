/***************************************************************************
 *   Copyright (C) 2004 by Jacques Gasselin                                *
 *   jacquesgasselin@hotmail.com                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef MATHGLPPFIXEDFLOAT26_6_H
#define MATHGLPPFIXEDFLOAT26_6_H

namespace mathglpp {

/**
A class template for 26.6 fixed point float arithmetic


@author Jacques Gasselin
*/
class FixedFloat26_6
{
public:
    FixedFloat26_6(char c):val(int(c)<<6){}
    FixedFloat26_6(short s):val(int(s)<<6){}
    FixedFloat26_6(int i):val(i<<6){}
    FixedFloat26_6(float f):val(int(f*64.0f)){}
    FixedFloat26_6(double d):val(int(d*64.0)){}
    FixedFloat26_6(const FixedFloat26_6& ff):val(ff.val){}
    ~FixedFloat26_6(){}

    inline FixedFloat26_6 operator +(const FixedFloat26_6& ff) const
    {   FixedFloat26_6 ret(*this); ret+=ff; return ret; }

    inline FixedFloat26_6& operator +=(const FixedFloat26_6& ff)
    {   val+=ff.val; return *this; }

    inline FixedFloat26_6 operator -(const FixedFloat26_6& ff) const
    {   FixedFloat26_6 ret(*this); ret+=ff; return ret; }

    inline FixedFloat26_6& operator -=(const FixedFloat26_6& ff)
    {   val+=ff.val; return *this; }

    inline FixedFloat26_6 operator *(const FixedFloat26_6& ff) const
    {   FixedFloat26_6 ret(*this); ret*=ff; return ret; }

    inline FixedFloat26_6& operator *=(const FixedFloat26_6& ff)
    {   val = int((val/64.0)*(ff.val/64.0)*64.0); return *this; }

    inline FixedFloat26_6 operator /(const FixedFloat26_6& ff) const
    {   FixedFloat26_6 ret(*this); ret*=ff; return ret; }

    inline FixedFloat26_6& operator /=(const FixedFloat26_6& ff)
    {   val = int((val/64.0)/(ff.val/64.0)*64.0); return *this; }

    inline bool operator ==(const FixedFloat26_6& ff) const
    {   return val == ff.val; }

    inline operator int() const { return val; }

    inline char toChar() const { return char(val>>6); }
    inline short toShort() const { return short(val>>6); }
    inline int toInt() const { return int(val>>6); }
    inline float toFloat() const { return float(val/64.0f); }
    inline double toDouble() const { return double(val/64.0); }

private:
    int val;
};

typedef FixedFloat26_6 float26_6;
};

#endif
