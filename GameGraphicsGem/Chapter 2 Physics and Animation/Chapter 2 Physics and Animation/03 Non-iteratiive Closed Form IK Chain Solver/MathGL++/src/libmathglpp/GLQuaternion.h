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

#ifndef GLQUATERNION_H

#define GLQUATERNION_H



#include "GLVector.h"

#include "GLMatrix.h"



namespace mathglpp

{

/**

A quaternion is a 3D plane in imaginary space, or some say a 4D vector. It behaves mathematically much like a plane in 3D, where a rotation is a reflection about two planes, one with the normal at half the angle and another at the angle. It is useful for rotations in 3D space because multiplications of quaternions are commutative, ie. it doesn't matter in which order you do them.



@author Jacques Gasselin

*/

template <class T>

class GLQuaternion

{

public:

    //!Create a unit quaternion

    GLQuaternion()

    :w(1),v(0,0,0)

    {}



    //!Create a scaled directed quaternion

    GLQuaternion(T f, const GLVector3<T>& gv)

    :w(f),v(gv)

    {}



    //!Create a directed quaternion

    GLQuaternion(const GLVector3<T>& gv)

    :w(0),v(gv)

    {}



    //!Create from array, must be at least 4 elements long.

    GLQuaternion(const T* gq)

    :w(gq[0]),v(gq+1)

    {}



    //!Copy a quaternion.

    GLQuaternion(const GLQuaternion& gq)

    :w(gq.w),v(gq.v)

    {}





    /**Create a rotation quaternion, *MOST PEOPLE WILL WANT TO USE THIS CONSTRUCTOR ONLY*

      *NB. Do not halve angle, this is done within the constructor. *TO CONFORM WITH THE OpenGL

      *INTERFACE THE ANGLES MUST BE GIVEN IN DEGREES*/

    GLQuaternion(GLVector3<T> gv, T degrees)

    {   set(gv.unit(), degrees); }



    //!Default destructor.

    ~GLQuaternion(){}



    //!Get real or plane constant.

    inline T getW()

    {   return w; }



    //!Get imaginary or plane normal.

    inline const GLVector3<T>& getV()

    {   return v; }



    //!set from a scaled vector.

    inline void set(T _w, const GLVector3<T>& gv)

    {   w = _w; v = gv; }



    //!Set from immediates.

    inline void set(T _w, T x, T y, T z)

    {   w = _w; v.set(x,y,z); }



    //!Set from array, must be at least 4 elements long.

    inline void set(const T* gq)

    {   set(gq[0],gq[1],gq[2],gq[3]); }



    //!Set from angle and normal, NB. Do not halve angle it is done internally.

    //!TO CONFORM WITH THE OpenGL INTERFACE THE ANGLES MUST BE GIVEN IN DEGREES

    inline void set(GLVector3<T> vec, T degrees)

    {   set(cos(degrees*M_PI/360.0), vec * sin(degrees*M_PI/360.0) ); }



    //!Quaternion addition.

    GLQuaternion operator + (GLQuaternion gq)

    {   GLQuaternion temp(*this); temp += gq; return temp; }



    //!Quaternion subtraction.

    GLQuaternion operator - (GLQuaternion gq)

    {   GLQuaternion temp(*this); temp -= gq; return temp; }



    //!Quaternion self referenced addition.

    GLQuaternion& operator += (GLQuaternion gq)

    {   w += gq.w; v += gq.v; return *this; }



    //!Quaternion self referenced subtraction.

    GLQuaternion& operator -= (GLQuaternion gq)

    {   w -= gq.w; v -= gq.v; return *this; }



    //!Quaternion multiplication.

    GLQuaternion operator * (GLQuaternion gq)

    {   GLQuaternion temp(*this); temp *=  gq; return temp; }



    //!Quaternion division, NB. Do not divide by 0, there are no safety catches for that.

    GLQuaternion operator / (T f)

    {   GLQuaternion temp(*this); temp /= f; return temp; }



    //!Quaternion self referenced multiplication.

    GLQuaternion& operator *= (GLQuaternion gq)

    {   set( w * gq.w - v.dot(gq.v) , (gq.v * w) + (v * gq.w) + v.getCross(gq.v) ); return *this; }



    //!Quaternion self referenced division.

    GLQuaternion& operator /= (T f)

    {   set( w / f , v / f ); return *this; }



    //!Quaternion dot product.

    T dot(GLQuaternion gq)

    {   return ( w*gq.w + v.dot(gq.v) ); }



    //!Quaternion cross product.

    GLQuaternion getCross(GLQuaternion gq)

    {   return GLQuaternion( w * gq.w - v.dot(gq.v) , (gq.v * w) + (v * gq.w) + v.getCross(gq.v) ); }



    //!Quaternion complex conjugate.

    GLQuaternion getConjugate()

    {   GLQuaternion temp(*this); temp.conjugate(); return temp; }



    //!Quaternion self referenced complex conjugate.

    GLQuaternion& conjugate()

    {   v = -v; return *this; }



    //!Quaternion inverse (more like a normalised conjugate).

    GLQuaternion getInverse()

    {   GLQuaternion temp(*this); temp.inverse(); return temp; }



    //!Quaternion self referenced inverse (more like a normalised conjugate).

    GLQuaternion& inverse()

    {   conjugate(); *this /= norm(); return *this; }



    //!Real part.

    T selection()

    {   return w; }



    //!Quaternion norm (similar to vector length).

    T norm()

    {   return sqrt( w*w + v.lengthSqr() ); }



    //!Quaternion unit

    GLQuaternion unit() const

    {   return this->operator /(norm()); }



    //!Quaternion self referenced unit

    inline GLQuaternion& normalise(){ return normalize(); }

   

    GLQuaternion& normalize()

    {   return this->operator /=(norm()); }



    /**GLVector interface method for ease of use. THIS IS PROBABLY THE ONLY METHOD YOU WOULD NORMALLY USE.

      *NB. Don't forget to normalise the quaternion unless you want axial translation as well as rotation.*/

    GLVector3<T> rotateVector(const GLVector3<T>& gv)

    {   GLQuaternion temp = getCross(GLQuaternion(gv)); temp *= getConjugate(); return temp.v; }



    /**GLMatrix interface method for obtaining an equivalent rotation matrix. You would se this for

      *interfacing with the OpenGL matrix stacks.*/

    GLMatrix<T> matrix(void)

    {

        GLMatrix<T> temp;



        T xx=v.x*v.x;  T xy=v.x*v.y;  T xz=v.x*v.z;  T xw=v.x*w;

        T yy=v.y*v.y;  T yz=v.y*v.z;  T yw=v.y*w;

        T zz=v.z*v.z;  T zw=v.z*w;



        temp[0] = 1 - 2*(zz + yy);

        temp[1] = 2*(xy + zw);

        temp[2] = 2*(xz - yw);

        temp[3] = 0;



        temp[4] = 2*(xy - zw);

        temp[5] = 1 - 2*(zz + xx);

        temp[6] = 2*(yz + xw);

        temp[7] = 0;



        temp[8] = 2*(yw + xz);

        temp[9] = 2*(yz - xw);

        temp[10] = 1 - 2*(yy + xx);

        temp[11] = 0;



        temp[12] = 0;

        temp[13] = 0;

        temp[14] = 0;

        temp[15] = 1;



        return temp;

    }



private:

    //!The real plane constant.

    T w;

    //!The 3D imaginary vector.

    GLVector3<T> v;

};



/**OpenGL style interface, GLfloat*/

typedef GLQuaternion<GLfloat> GLQuaternionf;

/**OpenGL style interface, GLdouble*/

typedef GLQuaternion<GLdouble> GLQuaterniond;



};



#endif

