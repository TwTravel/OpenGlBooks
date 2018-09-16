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
#ifndef GLCUBICHERMITECURVE_H
#define GLCUBICHERMITECURVE_H

#include "GLMatrix.h"
/**
A specialisation for the Berstein Polynomials to ease the use of cubic interpolation

@author Jacques Gasselin
*/

namespace mathglpp
{

template <typename T>
class GLCubicHermiteCurve4
{
public:
    enum { D = 4 };
    typedef T value_type;
    typedef GLVector4<value_type> vector_type;
    typedef GLMatrix<value_type> matrix_type;

    //!Create and premultiply a basis matrix for later interpolation, cpoints must not be NULL!!!!
    GLCubicHermiteCurve4(const value_type* cpoints)
    :basis(cpoints)
    {
        //!Column major so it seems like it is transposed
        static value_type coeffs[16] =
            {  1,  0,  0,  0,
               0,  1,  0,  0,
              -3, -2,  3, -1,
               2,  1, -2,  1 };

        //!Pre multiply
        basis *= matrix_type(coeffs);
    }

    //!Create and premultiply a basis matrix for later interpolation, cpoints must not be NULL!!!!
    GLCubicHermiteCurve4(const vector_type* cpoints)
    {
        //!Column major so it seems like it is transposed
        static value_type coeffs[16] =
            {  1,  0,  0,  0,
               0,  1,  0,  0,
              -3, -2,  3, -1,
               2,  1, -2,  1 };

        cpoints[0].copyTo(&basis[0]);
        cpoints[1].copyTo(&basis[4]);
        cpoints[2].copyTo(&basis[8]);
        cpoints[3].copyTo(&basis[12]);

        //!Pre multiply
        basis *= matrix_type(coeffs);
    }

    //!Create and premultiply a basis matrix for later interpolation, cpoints must not be NULL!!!!
    GLCubicHermiteCurve4(const vector_type& c0, const vector_type& c1, const vector_type& c2, const vector_type& c3)
    {
        //!Column major so it seems like it is transposed
        static value_type coeffs[16] =
            {  1,  0,  0,  0,
               0,  1,  0,  0,
              -3, -2,  3, -1,
               2,  1, -2,  1 };

        c0.copyTo(&basis[0]);
        c1.copyTo(&basis[4]);
        c2.copyTo(&basis[8]);
        c3.copyTo(&basis[12]);

        //!Pre multiply
        basis *= matrix_type(coeffs);
    }

    GLCubicHermiteCurve4(const GLCubicHermiteCurve4& hc)
    :basis(hc.basis) {}

    //!Destroy the basis matrix
    ~GLCubicHermiteCurve4(){}

    //!OpenGL interface for vertex generation, interp : [0,1]
    inline void glVertex(GLclampf interp) const
    {
        value_type interpVec[D] = { 1, value_type(interp), value_type(interp * interp), value_type(interp * interp * interp) };
        basis.dot4(interpVec).glVertex();
    }

    //!GLVector interface for vertex generation, interp : [0,1]
    inline vector_type interpolate(GLclampf interp) const
    {
        value_type interpVec[D] = { 1, value_type(interp), value_type(interp * interp), value_type(interp * interp * interp) };
        return basis.dot4(interpVec);
    }

    //!GLVector interface for vertex generation, interp : [0,1]
    void interpolate(GLclampf interp, value_type* vertex) const
    {
        vertex[0] = 1;
        vertex[1] = value_type(interp);
        vertex[2] = value_type(interp * interp);
        vertex[3] = value_type(interp * interp * interp);
        basis.vdot4(vertex);
    }

    inline vector_type operator()(GLclampf interp) const
    {
        value_type interpVec[D] = { 1, value_type(interp), value_type(interp * interp), value_type(interp * interp * interp) };
        return basis.dot4(interpVec);
    }

    inline void operator()(GLclampf interp, value_type* v) const
    {
        v[0] = 1;
        v[1] = value_type(interp);
        v[2] = value_type(interp * interp);
        v[3] = value_type(interp * interp * interp);
        basis.vdot4(v);
    }

private:
    matrix_type basis;
};


//!Corresponds to glVertex4b ...etc
typedef GLCubicHermiteCurve4<GLbyte> GLCubicHermiteCurve4b;
//!Corresponds to glVertex4s ...etc
typedef GLCubicHermiteCurve4<GLshort> GLCubicHermiteCurve4s;
//!Corresponds to glVertex4i ...etc
typedef GLCubicHermiteCurve4<GLint> GLCubicHermiteCurve4i;
//!Corresponds to glVertex4f ...etc
typedef GLCubicHermiteCurve4<GLfloat> GLCubicHermiteCurve4f;
//!Corresponds to glVertex4d ...etc
typedef GLCubicHermiteCurve4<GLdouble> GLCubicHermiteCurve4d;

};
#endif
