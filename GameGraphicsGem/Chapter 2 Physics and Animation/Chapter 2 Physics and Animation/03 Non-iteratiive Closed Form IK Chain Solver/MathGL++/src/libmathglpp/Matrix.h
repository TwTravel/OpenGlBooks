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
#ifndef MATRIX_H
#define MATRIX_H

#ifdef MATRIX_IOSTREAM
    #include <iostream>
#endif

#include <cassert>
#include <valarray>

namespace mathglpp{


//!Row-major 2D matrix class, useful for image manipulation and vertex framing 
template <typename T>
class Matrix2
{

public:
    //!Use size_t to conform with STL
    typedef size_t dim_type;
    typedef T value_type;
    typedef std::valarray<value_type> vector_type;
    typedef std::slice_array<value_type> sliced_vector_type;

    //!Create a square Matrix
    Matrix2(const dim_type& m, const dim_type& n)
    :rows(m),cols(n),vec(m*n)
    { }

    //!Copy constructor
    Matrix2(const Matrix2& mat)
    :rows(mat.rows),cols(mat.cols),vec(mat.vec)
    { }

    //!Default destructor
    ~Matrix2()
    { }

    //!Default assignment
    Matrix2& operator =(const Matrix2& rhs)
    {
        if(&rhs != this)
        { rows = rhs.rows; cols = rhs.cols; vec = rhs.vec; }
        return *this;
    }

    inline Matrix2& identity() 
    {   vec = 0; vec[std::slice(0,std::min(cols,rows),cols+1)] = 1; return *this; }
    
    
    inline Matrix2 filter(T (*filter_func)(const Matrix2&, const dim_type, const dim_type))
    {
        Matrix2 temp(*this);
        
        for(dim_type i = 0; i < rows; ++i)
            for(dim_type j = 0; j < cols; ++j)
                temp(i,j) = filter_func(*this,i,j);
        
        return temp;
    }
    
    //!Fortran style element access
    inline value_type& operator()(const dim_type i, const dim_type j) { return vec[i*cols+j]; }
    //!Fortran style element access
    inline const value_type& operator()(const dim_type i, const dim_type j) const { return vec[i*cols+j]; }
    //!C/C++ style element access
    inline value_type& operator[](const dim_type ind) { return vec[ind]; }
    //!C/C++ style element access
    inline const value_type& operator[](const dim_type ind) const { return vec[ind]; }
    //!C/C++ style element slice access
    inline const sliced_vector_type& operator[](const std::slice& sl) { return vec[sl]; }

    inline const sliced_vector_type& row(dim_type rn, dim_type cstart = 0)
    { return vec[row_slice(rn,cstart)]; }
    inline const sliced_vector_type& row(dim_type rn, dim_type cstart, dim_type cend) 
    { return vec[row_slice(rn,cstart,cend)]; }
    inline const sliced_vector_type& col(dim_type cn, dim_type rstart = 0)
    { return vec[col_slice(cn,rstart)]; }
    inline const sliced_vector_type& col(dim_type cn, dim_type rstart, dim_type rend)
    { return vec[col_slice(cn,rstart,rend)]; }

    //!Row slice
    inline const std::slice row_slice(dim_type rn, dim_type cstart = 0) const 
    { return std::slice(cstart+rn*cols,cols-cstart,1); }
    //!Row slice, bounded
    inline const std::slice row_slice(dim_type rn, dim_type cstart, dim_type cend) const 
    { return std::slice(cstart+rn*cols,cend-cstart,1); }

    //!Column slice
    inline const std::slice col_slice(dim_type cn, dim_type rstart = 0) const 
    { return std::slice(cn+rstart*cols,rows-rstart,cols); }
    //!Column slice, bounded
    inline const std::slice col_slice(dim_type cn, dim_type rstart, dim_type rend) const 
    { return std::slice(cn+rstart*cols,rend-rstart,cols); }

    //!Get the dimensions
    inline const dim_type& getM() const { return rows; }
    inline const dim_type& getN() const { return cols; }
    inline const dim_type& size() const { return vec.size(); }

    //!Return the vector
    inline operator vector_type& () { return vec; }
    inline operator const vector_type& () const { return vec; }
    inline operator const value_type*() const  { return &vec[0]; }
 
    #ifdef MATRIX_IOSTREAM
    //!iostream interface for input from char stream, enable by defining MATRIX_IOSTREAM
    friend std::basic_istream<char, std::char_traits<char> >& operator >>
      ( std::basic_istream<char, std::char_traits<char> >& in, Matrix2& mat)
    {
        for(int i = 0; i < mat.rows; ++i)
            for(int j = 0; j < mat.cols; ++j)
                in>>mat.vec[i*mat.cols+j];
        return in;
    }

    //!iostream interface for output to char stream, enable by defining MATRIX_IOSTREAM
    friend std::basic_ostream<char, std::char_traits<char> >& operator <<
      ( std::basic_ostream<char, std::char_traits<char> >& out, Matrix2& mat)
    {
        out<<"[ ";
        for(int i = 0; i < mat.rows; ++i)
        {
            for(int j = 0; j < mat.cols; ++j)
                out<<mat.vec[i*mat.cols+j]<<" ";
            if(i < mat.rows-1)
                out<<std::endl<<"  ";
        }
        out<<"]"<<std::endl;
        return out;
    }
    #endif

protected:
    //!dimensions 
    dim_type rows,cols;
    //!The actual row major vector of values
    vector_type vec;
};


}
#endif
