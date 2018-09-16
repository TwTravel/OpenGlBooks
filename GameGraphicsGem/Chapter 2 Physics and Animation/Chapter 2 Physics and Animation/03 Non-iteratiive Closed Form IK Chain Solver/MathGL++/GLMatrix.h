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
#ifndef GLMATRIX_H
#define GLMATRIX_H
#include "GLVector.h"
#include "LinearPolynomial.h"
#ifdef GLMATRIX_IOSTREAM
//#include <iostream>
#endif
namespace mathglpp
{

template <typename T>

inline void swap(T& a, T& b) { T temp = a; a = b; b = temp; }
//!column major matrix class for OpenGL
template <typename T>
class GLMatrix
{
public:

    //!Create an uninitialised matrix
    GLMatrix()
    { }

    //!Create an initialised matrix
    GLMatrix(const T val)
    { m0=m1=m2=m3=m4=m5=m6=m7=m8=m9=m10=m11=m12=m13=m14=m15=val; }
    
    //!Create a matrix from an array*/
    GLMatrix(const T* const val)
    { memmove(m,val,16*sizeof(T)); }
    
    //!Create a matrix from an array*/
    GLMatrix(const T* const v0, const T* const v1, const T* const v2, const T* const v3)
    { 
        memmove(m,v0,4*sizeof(T)); memmove(m+4,v1,4*sizeof(T));
        memmove(m+8,v2,4*sizeof(T)); memmove(m+12,v3,4*sizeof(T));
    }
   
    //!Copy a matrix
    GLMatrix(const GLMatrix& mat)
    { memmove(m,mat.m,16*sizeof(T)); }

    //!Default destructor
    ~GLMatrix()
    {}
   
    //!Get the matrix determinant
    inline const T det() const
    { return m0 * cofactorm0() - m1 * cofactorm1() + m2 * cofactorm2() - m3 * cofactorm3(); }

    //!Get the adjoint matrix
    GLMatrix adjoint() const
    {
        GLMatrix ret;
        ret[0] = cofactorm0();
        ret[1] = -cofactorm4();
        ret[2] = cofactorm8();
        ret[3] = -cofactorm12();
        ret[4] = -cofactorm1();
        ret[5] = cofactorm5();
        ret[6] = -cofactorm9();
        ret[7] = cofactorm13();
        ret[8] = cofactorm2();
        ret[9] = -cofactorm6();
        ret[10] = cofactorm10();
        ret[11] = -cofactorm14();
        ret[12] = -cofactorm3();
        ret[13] = cofactorm7();
        ret[14] = -cofactorm11();
        ret[15] = cofactorm15();
        return ret;
    }

    //!Adjoint method inverse, constant time inversion implementation
    GLMatrix inverse() const
    {
        GLMatrix ret(adjoint());
        const T determinant = m0 * ret[0] + m1 * ret[4] + m2 * ret[8] + m3 * ret[12];
        assert(determinant!=0 && "Singular matrix has no inverse");
        ret /= determinant;
        return ret;
    }

    //!Return a vector of the eigen values of the matrix
    GLVector4<T> eigenValues()
    {
        LinearPolynomial<T> lpoly(5);
        lpoly[0] = det();
        lpoly[1] = -_a*_f*_k -_a*_f*_p -_a*_k*_p -_f*_k*_p
                    -(-_a -_f)*_l*_o
                    -(-_a -_p)*_g*_j
                    -(-_a -_k)*_h*_n
                    -(-_k -_p)*_b*_e
                    -(-_f -_p)*_c*_i
                    -(-_f -_k)*_d*_n
                    -_g*_l*_n -_h*_g*_o +_b*_g*_i -_b*_h*_m  
                    -_c*_e*_j -_c*_l*_n +_d*_i*_o;
        lpoly[2] = _a*(_f +_k +_p) +_f*(_k +_p) +_k*_p -_l*_o -_g*_j -_h*_n +_b*_e -_c*_i +_d*_n;
        lpoly[3] = -_a -_f -_k -_p;
        lpoly[4] = 1;
        
        //!Solve
        std::vector<T> roots = lpoly.findRoots();
        
        return GLVector4<T>(roots[0], roots[1], roots[2], roots[3]);
    }

    //!Assign this matrix from another one
    inline GLMatrix& operator= (const GLMatrix& mat)
    { memmove(m,mat.m,16*sizeof(T)); return *this; }

    //!Equality check. NB. May not be constant time, depending on memcmp.
    inline bool operator== (const GLMatrix& mat)
    { return memcmp(m,mat.m,16*sizeof(T))==0;  }

    //!Direct access to the matrix elements, just remember they are in column major format!!
    inline T& operator[] (const int ind)
    { assert(ind > -1 && ind < 16); return m[ind]; }

    //!Implicit cast vector access as suggested by maquinn
    inline operator const T*(void) const { return m; }

    //!Implicit cast vector access as suggested by maquinn
    inline operator T*(void) { return m; }

    //!Multiply this matrix by a scalar
    inline GLMatrix& operator*= (const T& val)
    { for(register unsigned i = 0; i < 16; ++i) m[i] *= val; return *this; }

    //!Divide this matrix by a scalar
    inline GLMatrix& operator/= (const T& val)
    { for(register unsigned i = 0; i < 16; ++i) m[i] /= val; return *this; }

    //!Add a matrix to this matrix
    inline GLMatrix& operator+= (const GLMatrix& mat)
    { for(register unsigned i = 0; i < 16; ++i) m[i] += mat.m[i]; return *this; }
    //!Add a matrix to a copy of this matrix
    inline GLMatrix operator+ (const GLMatrix& mat)
    { GLMatrix ret(*this); ret += mat; return ret; }

    //!Subtract a matrix from this matrix
    inline GLMatrix& operator-= (const GLMatrix& mat)
    { for(register unsigned i = 0; i < 16; ++i) m[i] -= mat.m[i]; return *this; }

    //!Subtract a matrix from a copy of this matrix
    inline GLMatrix operator- (const GLMatrix& mat)
    { GLMatrix ret(*this); ret -= mat; return ret; }
    
    //!Get the matrix dot product, most commonly used form of matrix multiplication
    inline GLMatrix operator* (const GLMatrix& mat)
    {
        GLMatrix ret;
        ret.m[0]    = m[0]*mat.m0  + m[0+4]*mat.m1  + m[0+8]*mat.m2  + m[0+12]*mat.m3;
        ret.m[1]    = m[1]*mat.m0  + m[1+4]*mat.m1  + m[1+8]*mat.m2  + m[1+12]*mat.m3;
        ret.m[2]    = m[2]*mat.m0  + m[2+4]*mat.m1  + m[2+8]*mat.m2  + m[2+12]*mat.m3;
        ret.m[3]    = m[3]*mat.m0  + m[3+4]*mat.m1  + m[3+8]*mat.m2  + m[3+12]*mat.m3;
        ret.m[0+4]  = m[0]*mat.m4  + m[0+4]*mat.m5  + m[0+8]*mat.m6  + m[0+12]*mat.m7;
        ret.m[1+4]  = m[1]*mat.m4  + m[1+4]*mat.m5  + m[1+8]*mat.m6  + m[1+12]*mat.m7;
        ret.m[2+4]  = m[2]*mat.m4  + m[2+4]*mat.m5  + m[2+8]*mat.m6  + m[2+12]*mat.m7;
        ret.m[3+4]  = m[3]*mat.m4  + m[3+4]*mat.m5  + m[3+8]*mat.m6  + m[3+12]*mat.m7;
        ret.m[0+8]  = m[0]*mat.m8  + m[0+4]*mat.m9  + m[0+8]*mat.m10 + m[0+12]*mat.m11;
        ret.m[1+8]  = m[1]*mat.m8  + m[1+4]*mat.m9  + m[1+8]*mat.m10 + m[1+12]*mat.m11;
        ret.m[2+8]  = m[2]*mat.m8  + m[2+4]*mat.m9  + m[2+8]*mat.m10 + m[2+12]*mat.m11;
        ret.m[3+8]  = m[3]*mat.m8  + m[3+4]*mat.m9  + m[3+8]*mat.m10 + m[3+12]*mat.m11;
        ret.m[0+12] = m[0]*mat.m12 + m[0+4]*mat.m13 + m[0+8]*mat.m14 + m[0+12]*mat.m15;
        ret.m[1+12] = m[1]*mat.m12 + m[1+4]*mat.m13 + m[1+8]*mat.m14 + m[1+12]*mat.m15;
        ret.m[2+12] = m[2]*mat.m12 + m[2+4]*mat.m13 + m[2+8]*mat.m14 + m[2+12]*mat.m15;
        ret.m[3+12] = m[3]*mat.m12 + m[3+4]*mat.m13 + m[3+8]*mat.m14 + m[3+12]*mat.m15;
        return ret;
    }

    //!Apply the matrix dot product to this matrix
    inline GLMatrix& operator*= (const GLMatrix& mat)
    {
        GLMatrix temp(*this);
        m[0]    = temp.m[0]*mat.m0  + temp.m[0+4]*mat.m1  + temp.m[0+8]*mat.m2  + temp.m[0+12]*mat.m3;
        m[1]    = temp.m[1]*mat.m0  + temp.m[1+4]*mat.m1  + temp.m[1+8]*mat.m2  + temp.m[1+12]*mat.m3;
        m[2]    = temp.m[2]*mat.m0  + temp.m[2+4]*mat.m1  + temp.m[2+8]*mat.m2  + temp.m[2+12]*mat.m3;
        m[3]    = temp.m[3]*mat.m0  + temp.m[3+4]*mat.m1  + temp.m[3+8]*mat.m2  + temp.m[3+12]*mat.m3;
        m[0+4]  = temp.m[0]*mat.m4  + temp.m[0+4]*mat.m5  + temp.m[0+8]*mat.m6  + temp.m[0+12]*mat.m7;
        m[1+4]  = temp.m[1]*mat.m4  + temp.m[1+4]*mat.m5  + temp.m[1+8]*mat.m6  + temp.m[1+12]*mat.m7;
        m[2+4]  = temp.m[2]*mat.m4  + temp.m[2+4]*mat.m5  + temp.m[2+8]*mat.m6  + temp.m[2+12]*mat.m7;
        m[3+4]  = temp.m[3]*mat.m4  + temp.m[3+4]*mat.m5  + temp.m[3+8]*mat.m6  + temp.m[3+12]*mat.m7;
        m[0+8]  = temp.m[0]*mat.m8  + temp.m[0+4]*mat.m9  + temp.m[0+8]*mat.m10 + temp.m[0+12]*mat.m11;
        m[1+8]  = temp.m[1]*mat.m8  + temp.m[1+4]*mat.m9  + temp.m[1+8]*mat.m10 + temp.m[1+12]*mat.m11;
        m[2+8]  = temp.m[2]*mat.m8  + temp.m[2+4]*mat.m9  + temp.m[2+8]*mat.m10 + temp.m[2+12]*mat.m11;
        m[3+8]  = temp.m[3]*mat.m8  + temp.m[3+4]*mat.m9  + temp.m[3+8]*mat.m10 + temp.m[3+12]*mat.m11;
        m[0+12] = temp.m[0]*mat.m12 + temp.m[0+4]*mat.m13 + temp.m[0+8]*mat.m14 + temp.m[0+12]*mat.m15;
        m[1+12] = temp.m[1]*mat.m12 + temp.m[1+4]*mat.m13 + temp.m[1+8]*mat.m14 + temp.m[1+12]*mat.m15;
        m[2+12] = temp.m[2]*mat.m12 + temp.m[2+4]*mat.m13 + temp.m[2+8]*mat.m14 + temp.m[2+12]*mat.m15;
        m[3+12] = temp.m[3]*mat.m12 + temp.m[3+4]*mat.m13 + temp.m[3+8]*mat.m14 + temp.m[3+12]*mat.m15;
        return *this;
    }
    
    //!Apply the matrix dot product to this matrix
    //!unrolling by sebastien bloc
    inline GLMatrix& mult3by3(const GLMatrix& mat)
    {
        GLMatrix temp(*this);
        m0 = temp.m0*mat.m0+temp.m4*mat.m1+temp.m8*mat.m2;
        m4 = temp.m0*mat.m4+temp.m4*mat.m5+temp.m8*mat.m6;
        m8 = temp.m0*mat.m8+temp.m4*mat.m9+temp.m8*mat.m10;
        m1 = temp.m1*mat.m0+temp.m5*mat.m1+temp.m9*mat.m2;
        m5 = temp.m1*mat.m4+temp.m5*mat.m5+temp.m9*mat.m6;
        m9 = temp.m1*mat.m8+temp.m5*mat.m9+temp.m9*mat.m10;
        m2 = temp.m2*mat.m0+temp.m6*mat.m1+temp.m10*mat.m2;
        m6 = temp.m2*mat.m4+temp.m6*mat.m5+temp.m10*mat.m6;
        m10 = temp.m2*mat.m8+temp.m6*mat.m9+temp.m10*mat.m10;
        m3 = temp.m3*mat.m0+temp.m7*mat.m1+temp.m11*mat.m2;
        m7 = temp.m3*mat.m4+temp.m7*mat.m5+temp.m11*mat.m6;
        m11 = temp.m3*mat.m8+temp.m7*mat.m9+temp.m11*mat.m10;
        return *this;
    }

    //!Get the matrix vector dot product, used to transform vertecies
    inline GLVector4<T> operator* (const GLVector4<T>& vec)
    {
        GLVector4<T> ret;
        ret.val[0] = vec.x * m[0] + vec.y * m[0+4] + vec.z * m[0+8] + vec.w * m[0+12];
        ret.val[1] = vec.x * m[1] + vec.y * m[1+4] + vec.z * m[1+8] + vec.w * m[1+12];
        ret.val[2] = vec.x * m[2] + vec.y * m[2+4] + vec.z * m[2+8] + vec.w * m[2+12];
        ret.val[3] = vec.x * m[3] + vec.y * m[3+4] + vec.z * m[3+8] + vec.w * m[3+12];
        return ret;
    }

    //!Get the matrix vector dot product with w = 1, use for transforming non 4D vectors
    inline GLVector3<T> operator* (const GLVector3<T>& vec)
    {
        GLVector3<T> ret;
        //scale translate and rotate disregarding w scaling
        ret[0] = vec[0] * m[0] + vec[1] * m[0+4] + vec[2] * m[0+8] + m[0+12];
        ret[1] = vec[0] * m[1] + vec[1] * m[1+4] + vec[2] * m[1+8] + m[1+12];
        ret[2] = vec[0] * m[2] + vec[1] * m[2+4] + vec[2] * m[2+8] + m[2+12];
        //do w scaling
        register T resip = 1 / (vec[0]*m[3] + vec[1]*m[3+4] + vec[2]*m[3+8] + m[3+12]);
        ret[0] *= resip;
        ret[1] *= resip;
        ret[2] *= resip;
        return ret;
    }

    //!Get the matrix vector dot product with w = 1, use for transforming non 4D vectors
    inline GLVector2<T> operator* (const GLVector2<T>& vec)
    {
        GLVector2<T> ret;
        ret[0] = vec[0]*m[0] + vec[1]*m[0+4] + m[0+12];
        ret[1] = vec[0]*m[1] + vec[1]*m[1+4] + m[1+12];
        //scale translate and rotate disregarding w scaling
        //do w scaling
        register T resip = 1/(vec[0]*m[3] + vec[1]*m[3+4] + m[3+12]);
        ret[0] *= resip;
        ret[1] *= resip;
        return ret;
    }

    //!Get the matrix vector dot4 product, used to transform vertecies
    inline GLVector4<T> operator* (const T* const v_arr)
    {
        GLVector4<T> ret;
        ret.val[0] = v_arr[0]*m[0] + v_arr[1]*m[0+4] + v_arr[2]*m[0+8] + v_arr[3]*m[0+12];
        ret.val[1] = v_arr[0]*m[1] + v_arr[1]*m[1+4] + v_arr[2]*m[1+8] + v_arr[3]*m[1+12];
        ret.val[2] = v_arr[0]*m[2] + v_arr[1]*m[2+4] + v_arr[2]*m[2+8] + v_arr[3]*m[2+12];
        ret.val[3] = v_arr[0]*m[3] + v_arr[1]*m[3+4] + v_arr[2]*m[3+8] + v_arr[3]*m[3+12];
        return ret;
    }

    //!Get the matrix vector dot2 product, used to transform non-4D vertecies
    inline GLVector2<T> dot2 (const T* const v_arr) const
    {
        GLVector2<T> ret;
        ret[0] = v_arr[0]*m[0] + v_arr[1]*m[0+4] + m[0+12];
        ret[1] = v_arr[0]*m[1] + v_arr[1]*m[1+4] + m[1+12];
        //scale translate and rotate disregarding w scaling
        //do w scaling
        register T resip = 1/(v_arr[0]*m[3] + v_arr[1]*m[3+4] + m[3+12]);
        ret[0] *= resip;
        ret[1] *= resip;
        return ret;
    }

    //!Get the matrix vector dot2 product, used to transform non-4D vertecies
    inline GLVector2<T> dot2 (const T x, const T y, const T w = 1) const
    {
        GLVector2<T> ret;
        ret[0] = x*m[0] + y*m[0+4] + w*m[0+12];
        ret[0] = x*m[0] + y*m[1+4] + w*m[1+12];
        //!W scale
        T resip = 1/(x*m[3] + y*m[3+4] + w*m[3+12]);
        ret[0] *= resip;
        ret[1] *= resip;
        return ret;
    }

    //!Get the matrix vector dot2 product, used to transform non-4D vertecies
    inline void vdot2 (T* const v_arr, const T w = 1) const
    {
        const T x = v_arr[0];
        const T y = v_arr[1];
        v_arr[0] = x * m[0] + y * m[0+4] + w * m[0+12];
        v_arr[1] = x * m[1] + y * m[1+4] + w * m[1+12];
        //!W scale
        T resip = 1 / (x * m[3] + y * m[3+4] + w * m[3+12]);
        v_arr[0] *= resip;
        v_arr[1] *= resip;
    }
    
    //!Get the matrix vector dot3 product, used to transform non-4D vertecies
    inline GLVector3<T> dot3 (const T* const v_arr) const
    {
        GLVector3<T> ret;
        //scale translate and rotate disregarding w scaling
        ret[0] = v_arr[0]*m[0] + v_arr[1]*m[0+4] + v_arr[2]*m[0+8] + m[0+12];
        ret[1] = v_arr[0]*m[1] + v_arr[1]*m[1+4] + v_arr[2]*m[1+8] + m[1+12];
        ret[2] = v_arr[0]*m[2] + v_arr[1]*m[2+4] + v_arr[2]*m[2+8] + m[2+12];
        //do w scaling
        register T resip = 1/(v_arr[0]*m[3] + v_arr[1]*m[3+4] + v_arr[2]*m[3+8] + m[3+12]);
        ret[0] *= resip;
        ret[1] *= resip;
        ret[2] *= resip;
        return ret;
    }

    //!Get the matrix vector dot3 product, used to transform non-4D vertecies
    inline GLVector3<T> dot3 (const T x, const T y, const T z, const T w = 1) const
    {
        GLVector3<T> ret;
        ret[0] = x * m[0] + y * m[0 + 4] + z * m[0 + 8] + w * m[0 + 12];
        ret[1] = x * m[1] + y * m[1 + 4] + z * m[1 + 8] + w * m[1 + 12];
        ret[1] = x * m[2] + y * m[2 + 4] + z * m[2 + 8] + w * m[2 + 12];
        //!W scale
        const T resip = 1 / (x * m[3] + y * m[3 + 4] + z * m[3 + 8] + w * m[3 + 12]);
        ret[0] *= resip;
        ret[1] *= resip;
        ret[2] *= resip;
        return ret;
    }

    //!Get the matrix vector dot3 product, used to transform non-4D vertecies
    inline void vdot3 (T* const v_arr, const T w = 1) const
    {
        const T x = v_arr[0];
        const T y = v_arr[1];
        const T z = v_arr[2];

        v_arr[0] = x * m[0] + y * m[0 + 4] + z * m[0 + 8] + w * m[0 + 12];
        v_arr[1] = x * m[1] + y * m[1 + 4] + z * m[1 + 8] + w * m[1 + 12];
        v_arr[2] = x * m[2] + y * m[2 + 4] + z * m[2 + 8] + w * m[2 + 12];
        //!W scale
        const T resip = 1 / (x * m[3] + y * m[3 + 4] + z * m[3 + 8] + w * m[3 + 12]);
        v_arr[0] *= resip;
        v_arr[1] *= resip;
        v_arr[2] *= resip;
    }

    //!Get the matrix vector dot4 product, used to transform 4D vertecies
    inline GLVector4<T> dot4 (const T* const v_arr) const
    {
        GLVector4<T> ret;
        const T x = v_arr[0];
        const T y = v_arr[1];
        const T z = v_arr[2];
        const T w = v_arr[3];

        //scale translate and rotate disregarding w scaling
        ret[0] = x * m[0] + y * m[0 + 4] + z * m[0 + 8] + w * m[0 + 12];
        ret[1] = x * m[1] + y * m[1 + 4] + z * m[1 + 8] + w * m[1 + 12];
        ret[2] = x * m[2] + y * m[2 + 4] + z * m[2 + 8] + w * m[2 + 12];
        ret[3] = x * m[3] + y * m[3 + 4] + z * m[3 + 8] + w * m[3 + 12];
        return ret;
    }

    //!Get the matrix vector dot4 product, used to transform 4D vertecies
    inline void vdot4 (T* const v_arr) const
    {
        const T x = v_arr[0];
        const T y = v_arr[1];
        const T z = v_arr[2];
        const T w = v_arr[3];
        v_arr[0] = x * m[0] + y * m[0 + 4] + z * m[0 + 8] + w * m[0 + 12];
        v_arr[1] = x * m[1] + y * m[1 + 4] + z * m[1 + 8] + w * m[1 + 12];
        v_arr[2] = x * m[2] + y * m[2 + 4] + z * m[2 + 8] + w * m[2 + 12];
        v_arr[3] = x * m[3] + y * m[3 + 4] + z * m[3 + 8] + w * m[3 + 12];
    }

    //!Get the matrix vector dot4 product, used to transform 4D vertecies
    inline GLVector4<T> dot4 (const T x, const T y, const T z, const T w)
    {
        GLVector4<T> ret;
        ret.val[0] = x * m[0] + y * m[0+4] + z * m[0+8] + w * m[0+12];
        ret.val[1] = x * m[1] + y * m[1+4] + z * m[1+8] + w * m[1+12];
        ret.val[2] = x * m[2] + y * m[2+4] + z * m[2+8] + w * m[2+12];
        ret.val[3] = x * m[3] + y * m[3+4] + z * m[3+8] + w * m[3+12];
        return ret;
    }

    //!Transform the vertex and send it to OpenGL
    inline void glVertex3v(const T* const v_arr)
    {
        (this->dot3(v_arr)).glVertex();
    }

    //!Transform the vertex and send it to OpenGL
    inline void glVertex3(const T x, const T y, const T z)
    {
        (this->dot3(x,y,z)).glVertex();
    }

    //!Transform the vertex and send it to OpenGL
    inline void glVertex4v(const T* const v_arr)
    {
        (this->operator *(v_arr)).glVertex();
    }

    //!Transform the vertex and send it to OpenGL
    inline void glVertex4(const T x, const T y, const T z, const T w)
    {
        (this->dot4(x,y,z,w)).glVertex();
    }

    //!Transform a run of vertecies and send them to OpenGL
    void glVertex3v(const int num, const T* const v_arr);

    //!Transform a run of vertecies and send them to OpenGL
    void glVertex4v(const int num, const T* const v_arr);

    //!GL interface, glMultMatrix
    inline void glMultMatrix(void) { mathglpp::glMultMatrix<T>(m); };
    //!GL interface, glLoadMatrix
    inline void glLoadMatrix(void) { mathglpp::glLoadMatrix<T>(m); };
    //!GL interface, glGet, gets a matrix from OpenGL
    inline void glGet(const GLenum pname) { mathglpp::glGet<T>(pname,m); };

    //!Transpose the matrix
    inline GLMatrix& transpose(void)
    {
        swap(m1,m4); swap(m2,m8); swap(m3,m12);
        swap(m6,m9); swap(m7,m13); swap(m11,m14);

        return *this;
    }

    //!Return the transpose
    inline GLMatrix getTranspose(void)
    {
        return (GLMatrix(this).transpose());
    }

    //!Special glMatricies
    //!Identity matrix
    static GLMatrix identity(void)
    {
        GLMatrix ret;

        ret.m0 = 1;   ret.m4 = 0;   ret.m8  = 0;  ret.m12 = 0;
        ret.m1 = 0;   ret.m5 = 1;   ret.m9  = 0;  ret.m13 = 0;
        ret.m2 = 0;   ret.m6 = 0;   ret.m10 = 1;  ret.m14 = 0;
        ret.m3 = 0;   ret.m7 = 0;   ret.m11 = 0;  ret.m15 = 1;

        return ret;
    }

    //!Make this an identity matrix
    inline GLMatrix& loadIdentity(void)
    {
        m0 = 1;   m4 = 0;   m8  = 0;  m12 = 0;
        m1 = 0;   m5 = 1;   m9  = 0;  m13 = 0;
        m2 = 0;   m6 = 0;   m10 = 1;  m14 = 0;
        m3 = 0;   m7 = 0;   m11 = 0;  m15 = 1;

        return *this;
    }

    //!Zero matrix
    static GLMatrix zero(void)
    {
        return GLMatrix(T(0));
    }

    //!Make this an identity matrix
    inline GLMatrix& loadZero(void)
    {
        m0 = 0;   m4 = 0;   m8  = 0;  m12 = 0;
        m1 = 0;   m5 = 0;   m9  = 0;  m13 = 0;
        m2 = 0;   m6 = 0;   m10 = 0;  m14 = 0;
        m3 = 0;   m7 = 0;   m11 = 0;  m15 = 0;

        return *this;
    }

    //!OpenGL rotation matrix
    static GLMatrix glRotate(const T angle, T x, T y, T z);

    //!Make this an OpenGL rotation matrix
    inline GLMatrix& loadRotate(const T angle, T x, T y, T z)
    {
        register T magSqr = x*x + y*y + z*z;
        if(magSqr != 1.0)
        {
            const T mag = sqrt(magSqr);
            x /= mag;
            y /= mag;
            z /= mag;
        }
        const T c = T(cos(angle * M_PI / 180));
        const T s = T(sin(angle * M_PI / 180));
        m0 = x*x*(1-c)+c;
        m1 = y*x*(1-c)+z*s;
        m2 = z*x*(1-c)-y*s;
        m3 = 0;

        m4 = x*y*(1-c)-z*s;
        m5 = y*y*(1-c)+c;
        m6 = z*y*(1-c)+x*s;
        m7 = 0;

        m8 = x*z*(1-c)+y*s;
        m9 = y*z*(1-c)-x*s;
        m10 = z*z*(1-c)+c;
        m11 = 0;

        m12 = 0;
        m13 = 0;
        m14 = 0;
        m15 = 1;

        return *this;
    }

    //!Load rotate[X,Y,Z,XYZ] specialisations by sebastien bloc
    //!Make this an OpenGL rotation matrix: same as loadRotate(angle,1,0,0)
    inline GLMatrix& loadRotateX(const T angle)
    {
        const T c = T(cos(angle * M_PI / 180));
        const T s = T(sin(angle * M_PI / 180));
        m0 = 1;
        m1 = 0;
        m2 = 0;
        m3 = 0;

        m4 = 0;
        m5 = c;
        m6 = s;
        m7 = 0;

        m8 = 0;
        m9 = -s;
        m10 = c;
        m11 = 0;

        m12 = 0;
        m13 = 0;
        m14 = 0;
        m15 = 1;

        return *this;
    }

    //!Make this an OpenGL rotation matri0: same as loadRotate(angle,0,1,0)
    inline GLMatrix& loadRotateY(const T angle)
    {
        const T c = T(cos(angle * M_PI / 180));
        const T s = T(sin(angle * M_PI / 180));
        m0 = c;
        m1 = 0;
        m2 = -s;
        m3 = 0;

        m4 = 0;
        m5 = 1;
        m6 = 0;
        m7 = 0;

        m8 = s;
        m9 = 0;
        m10 = c;
        m11 = 0;

        m12 = 0;
        m13 = 0;
        m14 = 0;
        m15 = 1;

        return *this;
    }

    //!Make this an OpenGL rotation matrix: same as loadRotateZ(angle,0,0,1)
    inline GLMatrix& loadRotateZ(const T angle)
    {
        const T c = T(cos(angle * M_PI / 180));
        const T s = T(sin(angle * M_PI / 180));
        m0 = c;
        m1 = s;
        m2 = 0;
        m3 = 0;

        m4 = -s;
        m5 = c;
        m6 = 0;
        m7 = 0;

        m8 = 0;
        m9 = 0;
        m10 = 1;
        m11 = 0;

        m12 = 0;
        m13 = 0;
        m14 = 0;
        m15 = 1;

        return *this;
    }

    //!Apply an OpenGL rotation matrix to this
    inline GLMatrix& applyRotate(const T angle, const T x, const T y, const T z)
    {
        static GLMatrix temp;
        temp.loadRotate(angle, x, y, z);
        return mult3by3(temp);
    }

    //!Apply rotate[X,Y,Z,XYZ] specialisations by sebastien bloc
    //!Apply an OpenGL rotation matrix to this
    inline GLMatrix& applyRotateX(const T angle)
    {
        static GLMatrix temp;
        temp.loadRotateX(angle);
        return mult3by3(temp);
    }

    //!Apply an OpenGL rotation matrix to this
    inline GLMatrix& applyRotateY(const T angle)
    {
        static GLMatrix temp;
        temp.loadRotateY(angle);
        return mult3by3(temp);
    }

    //!Apply an OpenGL rotation matrix to this
    inline GLMatrix& applyRotateZ(const T angle)
    {
        static GLMatrix temp;
        temp.loadRotateZ(angle);
        return mult3by3(temp);
    }

    //!Apply an OpenGL rotation matrix to this
    GLMatrix& applyRotateXYZ(const T x, const T y, const T z)
    {
        static GLMatrix temp;
        temp.loadRotateX(x);
        mult3by3(temp);
        temp.loadRotateY(y);
        mult3by3(temp);
        temp.loadRotateZ(z);
        return mult3by3(temp);
    }

    //!OpenGL scale matrix
    static GLMatrix glScale(const T x, const T y, const T z)
    {
        GLMatrix ret;
        ret.m0 = x;     ret.m4 = 0;     ret.m8 = 0;     ret.m12 = 0;
        ret.m1 = 0;     ret.m5 = y;     ret.m9 = 0;     ret.m13 = 0;
        ret.m2 = 0;     ret.m6 = 0;     ret.m10 = z;    ret.m14 = 0;
        ret.m3 = 0;     ret.m7 = 0;     ret.m11 = 0;    ret.m15 = 1;
        return ret;
    }

    //!Make this an OpenGL scale matrix
    inline GLMatrix& loadScale(const T x, const T y, const T z)
    {
        m0 = x;   m4 = 0;   m8  = 0;  m12 = 0;
        m1 = 0;   m5 = y;   m9  = 0;  m13 = 0;
        m2 = 0;   m6 = 0;   m10 = z;  m14 = 0;
        m3 = 0;   m7 = 0;   m11 = 0;  m15 = 1;
        return *this;
    }

    //!Apply an OpenGL scale matrix to this
    inline GLMatrix& applyScale(const T x, const T y)
    {
        /*improved version*/
        //assuming z = 1
        m0 *= x;    m1 *= x;    m2 *= x;    m3 *= x;
        m4 *= y;    m5 *= y;    m6 *= y;    m7 *= y;
        return *this;
    }

    //!Apply an OpenGL scale matrix to this
    inline GLMatrix& applyScale(const T x, const T y, const T z)
    {
        /*improved version*/
        m0 *= x;    m1 *= x;    m2 *= x;    m3 *= x;
        m4 *= y;    m5 *= y;    m6 *= y;    m7 *= y;
        m8 *= z;    m9 *= z;    m10 *= z;   m11 *= z;
        return *this;
    }

    //!Apply an OpenGL scale matrix to this
    inline GLMatrix& applyScale(const GLVector2<T>& scale)
    {
        /*improved version*/
        //Assuming z = 1
        m0 *= scale.x;    m1 *= scale.x;    m2 *= scale.x;    m3 *= scale.x;
        m4 *= scale.y;    m5 *= scale.y;    m6 *= scale.y;    m7 *= scale.y;
        return *this;
    }

    //!Apply an OpenGL scale matrix to this
    inline GLMatrix& applyScale(const GLVector3<T>& scale)
    {
        /*improved version*/
        m0 *= scale.x;    m1 *= scale.x;    m2 *= scale.x;    m3 *= scale.x;
        m4 *= scale.y;    m5 *= scale.y;    m6 *= scale.y;    m7 *= scale.y;
        m8 *= scale.z;    m9 *= scale.z;    m10 *= scale.z;   m11 *= scale.z;
        return *this;
    }

    //!OpenGL translate matrix
    static GLMatrix glTranslate(const T x, const T y, const T z);

    //!Make this an OpenGL translate matrix
    inline GLMatrix& loadTranslate(const T x, const T y, const T z)
    {
        m0 = 1;   m4 = 0;   m8  = 0;  m12 = x;
        m1 = 0;   m5 = 1;   m9  = 0;  m13 = y;
        m2 = 0;   m6 = 0;   m10 = 1;  m14 = z;
        m3 = 0;   m7 = 0;   m11 = 0;  m15 = 1;
        return *this;
    }

    //!Apply an OpenGL translate matrix to this
    inline GLMatrix& applyTranslate(const T x, const T y)
    {
        /*improved version*/
        //assuming z = 0
        m12 += m0*x + m4*y;
        m13 += m1*x + m5*y;
        m14 += m2*x + m6*y;
        return *this;
    }

    //!Apply an OpenGL translate matrix to this
    inline GLMatrix& applyTranslate(const T x, const T y, const T z)
    {
        /*improved version*/
        m12 += m0*x + m4*y + m8*z;
        m13 += m1*x + m5*y + m9*z;
        m14 += m2*x + m6*y + m10*z;
        return *this;
    }

    //!Apply an OpenGL translate matrix to this
    inline GLMatrix& applyTranslate(const GLVector2<T>& trans)
    {
        /*improved version*/
        //assuming z = 0
        m12 += m0 * trans.x + m4 * trans.y;
        m13 += m1 * trans.x + m5 * trans.y;
        m14 += m2 * trans.x + m6 * trans.y;
        return *this;
    }

    //!Apply an OpenGL translate matrix to this
    inline GLMatrix& applyTranslate(const GLVector3<T>& trans)
    {
        /*improved version*/
        m12 += m0 * trans.x + m4 * trans.y + m8 * trans.z;
        m13 += m1 * trans.x + m5 * trans.y + m9 * trans.z;
        m14 += m2 * trans.x + m6 * trans.y + m10 * trans.z;
        return *this;
    }

	//!Apply this matrix to the incoming translate matrix to this
	inline GLVector3<T>& translate(const GLVector3<T>& trans)
	{
		/*improved version*/
		GLVector3<T> res;
		res.x = m12 + m0 * trans.x + m4 * trans.y + m8 * trans.z;
		res.y = m13 * trans.x + m5 * trans.y + m9 * trans.z;
		res.z = m14 * trans.x + m6 * trans.y + m10 * trans.z;
		return res;
	}

	//!Get this matricies position
	inline const GLVector3<T>& getPosition()
	{
		return getColumn(3);
		//return GLVector3<T>(m12, m13, m14);
	}

	//!Set this matricices position
	inline void setPosition(const GLVector3<T>& trans)
	{
		m12 = trans.x; m13 = trans.y; m14 = trans.z;
	}

	inline const GLVector3<T>& getColumn(unsigned short idx)
	{
		if (idx < 0 && idx >= 4)
			idx = 0; // Some sort of check here?

		return *(GLVector3<T>*)m_c_r[idx];
	}

    #define glFrustrum glFrustum
    //!OpenGL frustum matrix
    static GLMatrix glFrustum(const GLdouble left, const GLdouble right,
                              const GLdouble bottom, const GLdouble top,
                              const GLdouble zNear, const GLdouble zFar)
    {
        GLMatrix ret;
        ret.m0 = T(2 * zNear / (right - left));
        ret.m1 = 0;
        ret.m2 = 0;
        ret.m3 = 0;

        ret.m4 = 0;
        ret.m5 = T(2 * zNear / (top - bottom));
        ret.m6 = 0;
        ret.m7 = 0;

        ret.m8 = T((right + left) / (right - left));
        ret.m9 = T((top + bottom) / (top - bottom));
        ret.m10 = -T((zFar + zNear) / (zFar - zNear));
        ret.m11 = -1;

        ret.m12 = 0;
        ret.m13 = 0;
        ret.m14 = -T(2 * zFar * zNear / (zFar - zNear));
        ret.m15 = 0;

        return ret;
    }

    #define loadFrustrum loadFrustum
    //!Make this with an OpenGL frustum matrix
    inline GLMatrix& loadFrustum(const GLdouble left, const GLdouble right,
                                 const GLdouble bottom, const GLdouble top,
                                 const GLdouble zNear, const GLdouble zFar)
    {
        m0 = T(2 * zNear / (right-left));
        m1 = 0;
        m2 = 0;
        m3 = 0;

        m4 = 0;
        m5 = T(2 * zNear / (top-bottom));
        m6 = 0;
        m7 = 0;

        m8 = T((right + left) / (right - left));
        m9 = T((top + bottom) / (top - bottom));
        m10 = -T((zFar + zNear) / (zFar - zNear));
        m11 = -1;

        m12 = 0;
        m13 = 0;
        m14 = T(-2 * zFar * zNear / (zFar - zNear));
        m15 = 0;

        return *this;
    }

    //!OpenGL orthogonal matrix
    static GLMatrix glOrtho(const GLdouble left, const GLdouble right,
                            const GLdouble bottom, const GLdouble top,
                            const GLdouble zNear, const GLdouble zFar)
    {
        GLMatrix ret;
        ret.m0 = T(2 / (right - left));
        ret.m1 = 0;
        ret.m2 = 0;
        ret.m3 = 0;

        ret.m4 = 0;
        ret.m5 = T(2 / (top - bottom));
        ret.m6 = 0;
        ret.m7 = 0;

        ret.m8 = 0;
        ret.m9 = 0;
        ret.m10 = T(-2 / (zFar - zNear));
        ret.m11 = 0;

        ret.m12 = -T((right + left) / (right - left));
        ret.m13 = -T((top + bottom) / (top - bottom));
        ret.m14 = -T((zFar + zNear) / (zFar - zNear));
        ret.m15 = 1;

        return ret;
    }

    //!OpenGL orthogonal matrix
    inline GLMatrix& loadOrtho(const GLdouble left, const GLdouble right,
                               const GLdouble bottom, const GLdouble top,
                               const GLdouble zNear, const GLdouble zFar)
    {
        m0 = T(2 / (right - left));
        m1 = 0;
        m2 = 0;
        m3 = 0;

        m4 = 0;
        m5 = T(2 / (top - bottom));
        m6 = 0;
        m7 = 0;

        m8 = 0;
        m9 = 0;
        m10 = -T(2 / (zFar - zNear));
        m11 = 0;

        m12 = -T((right + left) / (right - left));
        m13 = -T((top + bottom) / (top - bottom));
        m14 = -T((zFar + zNear) / (zFar - zNear));
        m15 = 1;

        return *this;
    }

    //!OpenGL View Matrix.
    inline GLMatrix& loadView(const T fx, const T fy, const T fz,
                              const T ux, const T uy, const T uz,
                              const T sx, const T sy, const T sz)
    {
        m0 = sx;
        m1 = ux;
        m2 = -fx;
        m3 = 0;

        m4 = sy;
        m5 = uy;
        m6 = -fy;
        m7 = 0;

        m8 = sz;
        m9 = uz;
        m10 = -fz;
        m11 = 0;

        m12 = 0;
        m13 = 0;
        m14 = 0;
        m15 = 1;

        return *this;
    }
    
    //!OpenGL View Matrix.
    inline GLMatrix& loadView(const T* const f, const T* const u, const T* const s)
    {
        m0 = s[0];
        m1 = u[0];
        m2 = -f[0];
        m3 = 0;

        m4 = s[1];
        m5 = u[1];
        m6 = -f[1];
        m7 = 0;

        m8 = s[2];
        m9 = u[2];
        m10 = -f[2];
        m11 = 0;

        m12 = 0;
        m13 = 0;
        m14 = 0;
        m15 = 1;

        return *this;
    }
    
    //!OpenGL View Matrix.
    inline GLMatrix& loadView(const GLVector3<T>& front, const GLVector3<T>& up, const GLVector3<T> side)
    {
        m0 = side.x;
        m1 = up.x;
        m2 = -front.x;
        m3 = 0;

        m4 = side.y;
        m5 = up.y;
        m6 = -front.y;
        m7 = 0;

        m8 = side.z;
        m9 = up.z;
        m10 = -front.z;
        m11 = 0;

        m12 = 0;
        m13 = 0;
        m14 = 0;
        m15 = 1;

        return *this;
    }


    #ifdef GLMATRIX_IOSTREAM
    //!iostream interface for input from char stream, enable by defining GLMATRIX_IOSTREAM
    //!Row major order is expected to conform with standard output
    friend std::basic_istream<char, std::char_traits<char> >& operator >>
      ( std::basic_istream<char, std::char_traits<char> >& in, GLMatrix& mat)
    {
        for(int j = 0; j < 4; ++j)
            for(int i = 0; i < 4; ++i)
                in>>mat[i*4+j];
        return in;
    }

    //!iostream interface for output to char stream, enable by defining GLMATRIX_IOSTREAM
    //!Row major order is expected to conform with standard input
    friend std::basic_ostream<char, std::char_traits<char> >& operator <<
      ( std::basic_ostream<char, std::char_traits<char> >& out, GLMatrix& mat)
    {
        for(int j = 0; j < 4; ++j)
        {
            for(int i = 0; i < 4; ++i)
            {
                out<<mat[i*4+j]<<" ";
            }
            out<<std::endl;
        }
        return out;
    }
    #endif


protected:

    /**Cofactor maker after the looping determinant theory I'm sure we all learnt in high-school
      *  factor1 |   ^          |
      *  ------------|----------v----
      *          | major   |
      *          |         |  minor
      *              ^          |
      *  factor2 |   |          |
      *  ------------|----------v----
      *          | major   |
      *          |         |  minor
      *              ^          |
      *  factor3 |   |          |
      *  ------------|----------v----
      *          | major   |
      *          |         |  minor
      *              ^          |
      *              |          v
      */
    #define cofactor_maker(f1,mj1,mi1, f2,mj2,mi2, f3,mj3,mi3) \
    f1*(mj1*mi1-mj2*mi3) + f2*(mj2*mi2-mj3*mi1) + f3*(mj3*mi3-mj1*mi2)

    //inline T cofactorm0()const { return m5*(m10*m15-m11*m14)+m6*(m11*m13-m8*m15)+m7*(m8*m14*m10*m13);  }
    //!Cofactor of m0
    inline const T cofactorm0() const { return cofactor_maker(m5,m10,m15, m6,m11,m13, m7,m9,m14); }
    //!Cofactor of m1
    inline const T cofactorm1() const { return cofactor_maker(m6,m11,m12, m7,m8,m14, m4,m10,m15); }
    //!Cofactor of m2
    inline const T cofactorm2() const { return cofactor_maker(m7,m8,m13, m4,m9,m15, m5,m11,m12); }
    //!Cofactor of m3
    inline const T cofactorm3() const { return cofactor_maker(m4,m9,m14, m5,m10,m12, m6,m8,m13); }

    //!Cofactor of m4
    inline const T cofactorm4() const { return cofactor_maker(m9,m14,m3, m10,m15,m1, m11,m13,m2); }
    //!Cofactor of m5
    inline const T cofactorm5() const { return cofactor_maker(m10,m15,m0, m11,m12,m2, m8,m14,m3); }
    //!Cofactor of m6
    inline const T cofactorm6() const { return cofactor_maker(m11,m12,m1, m8,m13,m3, m9,m15,m0); }
    //!Cofactor of m7
    inline const T cofactorm7() const { return cofactor_maker(m8,m13,m2, m9,m14,m0, m10,m12,m1); }

    //!Cofactor of m8
    inline const T cofactorm8() const { return cofactor_maker(m13,m2,m7, m14,m3,m5, m15,m1,m6); }
    //!Cofactor of m9
    inline const T cofactorm9() const { return cofactor_maker(m14,m13,m4, m15,m0,m6, m12,m2,m7); }
    //!Cofactor of m10
    inline const T cofactorm10() const { return cofactor_maker(m15,m0,m5, m12,m1,m7, m13,m3,m4); }
    //!Cofactor of m11
    inline const T cofactorm11() const { return cofactor_maker(m12,m1,m6, m13,m2,m4, m14,m0,m5); }

    //!Cofactor of m12
    inline const T cofactorm12() const { return cofactor_maker(m1,m6,m11, m2,m7,m9, m3,m5,m10); }
    //!Cofactor of m13
    inline const T cofactorm13() const { return cofactor_maker(m2,m7,m8, m3,m4,m10, m10,m6,m11); }
    //!Cofactor of m14
    inline const T cofactorm14() const { return cofactor_maker(m3,m4,m9, m0,m5,m11, m1,m7,m8); }
    //!Cofactor of m15
    inline const T cofactorm15() const { return cofactor_maker(m0,m5,m10, m1,m6,m8, m2,m4,m9); }

private:
    //!value array
    //!Anonymous union and struct by sebastien bloc (sebastien.bloc@free.fr)
    union
    {
        //!Straight column major vector layout
        T m[16];
        //!Column-Row matrix layout
        T m_c_r[4][4];
        struct
        {
            //!Fast individual element access
            T m0,m1,m2,m3,m4,m5,m6,m7,m8,m9,m10,m11,m12,m13,m14,m15;
        };
        struct
        {
            //!Fast individual element access
            T _a,_b,_c,_d,_e,_f,_g,_h,_i,_j,_k,_l,_m,_n,_o,_p;
        };
    };
};

typedef GLMatrix<GLfloat>  GLMatrix4f;
typedef GLMatrix<GLdouble>  GLMatrix4d;



}

#endif

