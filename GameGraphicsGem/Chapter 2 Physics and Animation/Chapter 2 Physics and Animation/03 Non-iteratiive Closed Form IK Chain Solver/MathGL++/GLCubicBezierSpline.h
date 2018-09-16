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
#ifndef MATHGLPPGLCUBICBEZIERSPLINE4_H
#define MATHGLPPGLCUBICBEZIERSPLINE4_H

#include "GLCubicBezierCurve.h"
#include <vector>
#include <cmath>

namespace mathglpp {

/**
@author Jacques Gasselin
*/
template <typename T>
class CubicBezierSpline4
{
public:

    enum { D = 4 };
    typedef T value_type;
    typedef GLVector4<value_type> vector_type;
    typedef CubicBezierCurve4<value_type> curve_type;

    //!Create and premultiply a basis matrix for later interpolation, cpoints must not be NULL!!!!
    CubicBezierSpline4(GLint ncurves, const value_type* cpoints)
    :number_of_curves(ncurves)
    {
        value_type* c0 = cpoints;

        //!Assume end points line up
        for(GLint i = 0; i < number_of_curves; ++i)
        {
            //!Copy the points into a bezier curve interpolator
            curves.push_back(curve_type(c0));
            //!Point to the start of the next curve
            c0+=12;
        }
    }

    CubicBezierSpline4(GLint ncurves, const vector_type* cpoints)
    :number_of_curves(ncurves)
    {
        const vector_type* c0 = cpoints;

        //!Assume end points line up
        for(GLint i = 0; i < number_of_curves; ++i)
        {
            //!Copy the points into a bezier curve interpolator
            curves.push_back(curve_type(*c0, *(c0+1), *(c0+2), *(c0+3)));
            //!Point to the start of the next curve
            c0+=3;
        }
    }

    ~CubicBezierSpline4() { curves.clear(); }

    //!GLVector interface for vertex generation, interp : [0,1]
    inline vector_type interpolate(GLclampf interp) const
    {
        GLclampf c; GLclampf u = modff(interp*number_of_curves, &c);
        if(c == number_of_curves){ c = number_of_curves-1; u = 1.0; }
        return curves[int(c)].interpolate(u);
    }

    //!GLVector interface for vertex generation, interp : [0,1]
    void interpolate(GLclampf interp, value_type* vertex) const
    {
        GLclampf c; GLclampf u = modff(interp*number_of_curves, &c);
        if(c == number_of_curves){ c = number_of_curves-1; u = 1.0; }
        return curves[int(c)].interpolate(u,vertex);
    }

private:
    GLint number_of_curves;
    std::vector<curve_type > curves;
};

//!Corresponds to glVertex4b ...etc
typedef CubicBezierSpline4<GLbyte> CubicBezierSpline4b;
//!Corresponds to glVertex4s ...etc
typedef CubicBezierSpline4<GLshort> CubicBezierSpline4s;
//!Corresponds to glVertex4i ...etc
typedef CubicBezierSpline4<GLint> CubicBezierSpline4i;
//!Corresponds to glVertex4f ...etc
typedef CubicBezierSpline4<GLfloat> CubicBezierSpline4f;
//!Corresponds to glVertex4d ...etc
typedef CubicBezierSpline4<GLdouble> CubicBezierSpline4d;

};

#endif
