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


#ifndef GLPLANE_H
#define GLPLANE_H


#include "GLVector.h"


/**A Simple Plane class that builds upon the vector class, GLVector. A concise and easy to use interface for plane arithmetic
  *is the design aim of this class.
  *@author Jacques Gasselin de Richebourg
  */

namespace mathglpp
{

template <typename T>
class GLPlane
{
public:
    //!Typedef it so that the class is more resilient to change
    typedef GLVector3<T> vector_type;

    //!Construct a zero plane, for convenience mainly
    GLPlane()
    :normal(T(0)),D(T(0))
    {}

    //!Construct a directed plane, with a translation so that op is on the plane
    GLPlane(const vector_type& n, const vector_type& op)
    :normal(n), D(-(n.dot(op))) {}

    //!Construct a directed plane, so that op[1,2,3] are on the plane
    GLPlane(const vector_type& op1, const vector_type& op2, const vector_type& op3)
    { normal = (op3 - op2).getCross( (op1 - op2) );  normal.normalize(); D = -(normal.dot(op1)); }

    //!Normal copy constructor
    GLPlane(const GLPlane& pl): normal(pl.normal), D(pl.D) {}

    //!Normal destructor
    ~GLPlane(){}

    //!3 point mutator, the three points for a plane
    inline void set(const vector_type& op1, const vector_type& op2, const vector_type& op3)
    { normal = (op3 - op2).getCross( (op1 - op2) );  normal.normalize(); D = -(normal.dot(op1)); }

    //!Simple const accessor, normal
    inline const vector_type& getNormal(void) const
    { return normal; }

    //!Simple mutator, normal.
    inline void setNormal(const vector_type& n)
    { normal = n; }

    //!Simple const accessor, deflection.
    inline const T& getD(void) const
    { return D; }

    //!Simple mutator, deflection.
    inline void setD(const T& d)
    { D = d; }

    //!Use this to find out how far away from the plane a point is
    inline T distanceToPoint(const vector_type& p) const
    { return (normal.dot(p) + D); }

    //!Use this to find out how far along the line the plane is intersected.
    //!Used for picking and simple raytracing mostly.
    inline T intersectionWithLine(const vector_type& dir, const vector_type& origin) const
    { return - (normal.dot(origin) + D) / normal.dot(dir); }

    //!Use this to get a plane reflected vector.
    inline vector_type reflectedVector(const vector_type& in) const
    { return in - (normal*(distanceToPoint(in))*2); }

    //!Get a plane reflection matrix from the plane equation.
    inline GLMatrix<T> reflectionMatrix()
    {
        GLMatrix<T> ret;

        ret[0] = 1 - 2*(normal.x*normal.x);
        ret[4] =   - 2*(normal.x*normal.y);
        ret[8] =   - 2*(normal.x*normal.z);
        ret[12] = -2*normal.x*D;

        ret[1] =   - 2*(normal.y*normal.x);
        ret[5] = 1 - 2*(normal.y*normal.y);
        ret[9] =   - 2*(normal.y*normal.z);
        ret[13] = -2*normal.y*D;

        ret[2] =   - 2*(normal.z*normal.x);
        ret[6] =   - 2*(normal.z*normal.y);
        ret[10] = 1  - 2*(normal.z*normal.z);
        ret[14] = -2*normal.z*D;

        ret[3] = 0;
        ret[7] = 0;
        ret[11] = 0;
        ret[15] = 1;

        return ret;
    }

    //!Get an orthogonal projection matrix. Useful for simple shadows and other casting.
    inline GLMatrix<T> directionalProjectionMatrix(const vector_type& dir)
    {
        GLMatrix<T> ret;

        ret[0] = 1+1*(dir.x*normal.x);
        ret[4] =   1*(dir.x*normal.y);
        ret[8] =   1*(dir.x*normal.z);
        ret[12] =  1*dir.x*D;

        ret[1] =   1*(dir.y*normal.x);
        ret[5] = 1+1*(dir.y*normal.y);
        ret[9] =   1*(dir.y*normal.z);
        ret[13] =  1*dir.y*D;

        ret[2] =   1*(dir.z*normal.x);
        ret[6] =   1*(dir.z*normal.y);
        ret[10] =1+1*(dir.z*normal.z);
        ret[14] =  1*dir.z*D;

        ret[3] = 0;
        ret[7] = 0;
        ret[11] = 0;
        ret[15] = 1;

        return ret;
    }

    //!Point perspective projection of vertecies with respect to a projection source, useful for shadow projections
    inline GLMatrix<T> pointProjectionMatrix(const vector_type& l)
    {
        GLMatrix<T> ret;

        T d = -l.dot(normal);
        T c = D - d;

        ret[0] = -l.x*normal.x + c;
        ret[4] = -l.x*normal.y;
        ret[8] = -l.x*normal.z;
        ret[12]= -c*l.x - d*l.x;

        ret[1] = -l.y*normal.x;
        ret[5] = -l.y*normal.y + c;
        ret[9] = -l.y*normal.z;
        ret[13]= -c*l.y - d*l.y;

        ret[2] = -l.z*normal.x;
        ret[6] = -l.z*normal.y;
        ret[10] = -l.z*normal.z + c;
        ret[14]= -c*l.z - d*l.z;

        ret[3] = -normal.x;
        ret[7] = -normal.y;
        ret[11] = -normal.z;
        ret[15]= -d;

        return ret;
    }

private:

    //!The plane values, normal
    vector_type normal;
    //!The plane values, deflection
    T D;
};


//!OpenGL style typing, GLbyte correspondance
typedef GLPlane<GLbyte> GLPlaneb;
//!OpenGL style typing, GLshort correspondance
typedef GLPlane<GLshort> GLPlanes;
//!OpenGL style typing, GLint correspondance
typedef GLPlane<GLint> GLPlanei;
//!OpenGL style typing, GLfloat correspondance
typedef GLPlane<GLfloat> GLPlanef;
//!OpenGL style typing, GLdouble correspondance
typedef GLPlane<GLdouble> GLPlaned;


};
#endif
