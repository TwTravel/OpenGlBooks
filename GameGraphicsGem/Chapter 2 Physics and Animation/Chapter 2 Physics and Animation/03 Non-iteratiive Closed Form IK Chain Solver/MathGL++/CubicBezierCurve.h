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
#ifndef GLCUBICBEZIERCURVE_H
#define GLCUBICBEZIERCURVE_H


#include "GLMatrix.h"

namespace mathglpp
{

/**
A specialisation for the Berstein Polynomials to ease the use of cubic interpolation

@author Jacques Gasselin
*/

template <typename T>
class CubicBezierCurve4
{
public:
    enum { D = 4 };
    typedef T value_type;
    typedef GLMatrix<value_type> matrix_type;

    //!Create and premultiply a basis matrix for later interpolation, cpoints must not be NULL!!!!
    CubicBezierCurve4(const value_type* cpoints)
    :basis(cpoints)
    {
        //!Column major so it seems like it is transposed
        static value_type coeffs[D*D] =
            {  1, -0,  0, -0,
              -3,  3, -0,  0,
               3, -6,  3, -0,
              -1,  3, -3,  1 };

        //!Pre multiply
        basis *= GLMatrix<value_type>(coeffs);
    }

    //!Create and premultiply a basis matrix for later interpolation
    CubicBezierCurve4(const value_type* c0, const value_type* c1, const value_type* c2, const value_type* c3)
    :basis(c0,c1,c2,c3)
    {
        //!Column major so it seems like it is transposed
        static value_type coeffs[16] =
            {  1, -0,  0, -0,
              -3,  3, -0,  0,
               3, -6,  3, -0,
              -1,  3, -3,  1 };


        //!Pre multiply
        basis *= GLMatrix<value_type>(coeffs);
    }

    CubicBezierCurve4(const CubicBezierCurve4& bc)
    :basis(bc.basis){}

    //!Destroy the basis matrix
    ~CubicBezierCurve4(){}

    //!interface for vertex generation, interp : [0,1]
    inline void interpolate(GLclampf interp, value_type* vertex) const
    {
        vertex[0] = 1;
        vertex[1] = value_type(interp);
        vertex[2] = value_type(interp * interp);
        vertex[3] = value_type(interp * interp * interp);
        basis.vdot4(vertex);
    }

private:
    GLMatrix<value_type> basis;
};


//!Corresponds to glVertex4b ...etc
typedef CubicBezierCurve4<GLbyte> CubicBezierCurve4b;
//!Corresponds to glVertex4s ...etc
typedef CubicBezierCurve4<GLshort> CubicBezierCurve4s;
//!Corresponds to glVertex4i ...etc
typedef CubicBezierCurve4<GLint> CubicBezierCurve4i;
//!Corresponds to glVertex4f ...etc
typedef CubicBezierCurve4<GLfloat> CubicBezierCurve4f;
//!Corresponds to glVertex4d ...etc
typedef CubicBezierCurve4<GLdouble> CubicBezierCurve4d;

};

#endif
