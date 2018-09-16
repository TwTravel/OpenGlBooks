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
#ifndef GL_VECTOR_H
#define GL_VECTOR_H

#include "GLFunctions.h"

#ifdef GLVECTOR_IOSTREAM
    #include <iostream>
#endif
#include <valarray>

namespace mathglpp
{

//!a 2D vector class for OpenGL
template <typename T>
//!a 2D vector class for OpenGL
class GLVector2
{
public:
    enum { D = 2 };

    //!Create an uninitialised vector
    GLVector2()
    { }

    //!Create an initialised vector
    GLVector2(const T& v)
    {   x = y = v; }

    //!Create an initialised vector from values
    GLVector2(const T& v1, const T& v2)
    {   x = v1; y = v2; }

    //!Copy a vector
    GLVector2(const GLVector2& gv)
    {   memmove(val,gv.val,D*sizeof(T)); }

    //!Create a vector from an array
    GLVector2(const T* f)
    {   memmove(val,f,D*sizeof(T)); }

    //!Default destructor
    ~GLVector2(){}

    //! element by element initialiser
    inline void set(const T& v1, const T& v2)
    {   x = v1; y = v2; }

    //! element by element accessor
    inline T& operator[] (int ind)
    {   return val[ind]; }

    //! implicit casting to a pointer
    inline operator T* (void)
    {   return val; }

    //! implicit casting to a const pointer
    inline operator const T* (void) const
    {   return val; }

    //!GL interface, glScale()
    inline void glScale() const;
    //!GL interface, glRotate()
    inline void glRotate(const T& ang) const;
    //!GL interface, glTranslate()
    inline void glTranslate() const;
    //!GL interface, glVertex()
    inline void glVertex() const { mathglpp::glVertexv<D,T>(val); }
    //!GL interface, glNormal()
    inline void glNormal() const;

    //!copy values to a array
    inline void copyTo(T* vec) const
    {   memmove(vec,val,2*sizeof(T)); }

    //!Get the sum of this and a vector
    inline const GLVector2 operator + (const GLVector2& gv) const
    {
        return GLVector2(x+gv.x,y+gv.y);
    }

    //!Get the difference of this and a vector
    inline const GLVector2 operator - (const GLVector2& gv) const
    {
        return GLVector2(x-gv.x,y-gv.y);
    }

    //!Get the element-by-element product of this and a vector
    inline const GLVector2 operator * (const GLVector2& gv) const
    {
        return GLVector2(x*gv.x,y*gv.y);
    }

    //!Get the element-by-element quota of this and a vector
    inline const GLVector2 operator / (const GLVector2& gv) const
    {
        return GLVector2(x/gv.x,y/gv.y);
    }

    //!Get the element-by-element product of this and a scalar
    inline const GLVector2 operator * (const T& v) const
    {
        return GLVector2(x*v,y*v);
    }

    //!Get the element-by-element quota of this and a scalar
    inline const GLVector2 operator / (const T& v) const
    {
        return GLVector2(x/v,y/v);
    }

    //!Add a vector to this
    GLVector2& operator += (const GLVector2& gv)
    {
        x += gv.x;
        y += gv.y;
        return *this;
    }

    //!Subtract a vector from this
    GLVector2& operator -= (const GLVector2& gv)
    {
        x -= gv.x;
        y -= gv.y;
        return *this;
    }

    //!Multiply this by a scalar
    GLVector2& operator *= (const T& f)
    {
        x *= f;
        y *= f;
        return *this;
    }

    //!Divide this by a scalar
    GLVector2& operator /= (const T& f)
    {
        x /= f;
        y /= f;
        return *this;
    }

    //!copy this from a vector
    GLVector2& operator = (const GLVector2& v)
    {   memmove(val, v.val, D*sizeof(T)); return *this; }

    //!Equality check. NB. May not be constant time, depending on memcmp.
    inline bool operator == (const GLVector2& v)
    {   return memcmp(val, v.val, D*sizeof(T))==0;  }

    //!negate this
    inline const GLVector2 operator - () const
    {
        return GLVector2(-x,-y);
    }

    //!Get the dot product of this and a vector
    inline const T dot(const GLVector2& gv) const
    {   return x*gv.x + y*gv.y; }

    //!Get the length of this
    inline T length() const
    {   return sqrt(lengthSqr()); }

    //!Get the length squared, less computation than length()
    inline T lengthSqr() const
    {   return x * x + y * y; }

    //!Get the the unit vector of this
    const GLVector2 unit() const
    {
        return (*this)/length();
    }

    //!Normalize this, makes this a unit vector, US spelling
    GLVector2& normalize()
    {
        (*this)/=length();
        return *this;
    }

    //!Get the projection of this and a vector
    inline const T projection(const GLVector2& in) const
    {   return dot(in); }

    //!Get the orthogonal projection of this and a vector
    inline GLVector2 orthogonalProjection(const GLVector2& in) const
    {   return in - vectorProjection(in); }

    //!Get the vector projection of this and a vector
    GLVector2 vectorProjection(const GLVector2& in) const
    {   return (*this) * dot(in); }

    #ifdef GLVECTOR_IOSTREAM
    //!iostream interface for input from char stream, enable by defining GLVECTOR_IOSTREAM
    friend std::basic_istream<char, std::char_traits<char> >& operator >>
      ( std::basic_istream<char, std::char_traits<char> >& in, GLVector2& vec)
    { in>>vec.val[0]>>vec.val[1]; return in;};

    //!iostream interface for output to char stream, enable by defining GLVECTOR_IOSTREAM
    friend std::basic_ostream<char, std::char_traits<char> >& operator <<
      ( std::basic_ostream<char, std::char_traits<char> >& out, GLVector2& vec)
    { out<<vec.val[0]<<" "<<vec.val[1]; return out;};
    #endif

    //!Value array
    //!Anonymous union and struct by sebastien bloc (sebastien.bloc@free.fr)
    union
    {
        T val[D];
        struct
        {
            T x,y;
        };
    };
};

//!Corresponds to glVertex2b ...etc
typedef GLVector2<GLbyte> GLVector2b;
//!Corresponds to glVertex2s ...etc
typedef GLVector2<GLshort> GLVector2s;
//!Corresponds to glVertex2i ...etc
typedef GLVector2<GLint> GLVector2i;
//!Corresponds to glVertex2f ...etc
typedef GLVector2<GLfloat> GLVector2f;
//!Corresponds to glVertex2d ...etc
typedef GLVector2<GLdouble> GLVector2d;

//!a 2D vector class for OpenGL
template <typename T>
//!a 2D vector class for OpenGL
class GLIVector2
{
public:
    enum { D = 2 };

    //!Address another vector
    GLIVector2(const GLIVector2& gv)
    :val(gv.val)
    { }

    //!Wrap a vector from an array
    GLIVector2(T* f)
    :val(f)
    { }

    //!Default destructor
    ~GLIVector2(){}

    //! element by element initialiser
    inline void set(const T& v1, const T& v2)
    {   val[0] = v1; val[1] = v2; }

    //! element by element accessor
    inline T& operator[] (int ind)
    {   return val[ind]; }

    //! implicit casting to a pointer
    inline operator T* (void)
    {   return val; }

    //! implicit casting to a const pointer
    inline operator const T* (void) const
    {   return val; }

    //!GL interface, glScale()
    inline void glScale() const;
    //!GL interface, glRotate()
    inline void glRotate(const T& ang) const;
    //!GL interface, glTranslate()
    inline void glTranslate() const;
    //!GL interface, glVertex()
    inline void glVertex() const { mathglpp::glVertexv<D,T>(val); }
    //!GL interface, glNormal()
    inline void glNormal() const;

    //!Get the sum of this and a vector
    inline const GLVector2<T> operator + (const GLVector2<T>& gv) const
    {
        return GLVector2<T>(val[0]+gv.x,val[1]+gv.y);
    }
    
    //!Get the sum of this and a vector
    inline const GLVector2<T> operator + (const GLIVector2& gv) const
    {
        return GLVector2<T>(val[0]+gv.val[0],val[1]+gv.val[1]);
    }

    //!Get the difference of this and a vector
    inline const GLVector2<T> operator - (const GLVector2<T>& gv) const
    {
        return GLVector2<T>(val[0]-gv.x,val[1]-gv.y);
    }

    //!Get the difference of this and a vector
    inline const GLVector2<T> operator - (const GLIVector2& gv) const
    {
        return GLVector2<T>(val[0]-gv.val[0],val[1]-gv.val[1]);
    }
    
    //!Get the element-by-element product of this and a vector
    inline const GLVector2<T> operator * (const GLVector2<T>& gv) const
    {
        return GLVector2<T>(val[0]*gv.x,val[1]*gv.y);
    }

    //!Get the element-by-element product of this and a vector
    inline const GLVector2<T> operator * (const GLIVector2& gv) const
    {
        return GLVector2<T>(val[0]*gv.x,val[1]*gv.y);
    }

    //!Get the element-by-element quota of this and a vector
    inline const GLVector2<T> operator / (const GLVector2<T>& gv) const
    {
        return GLVector2<T>(val[0]/gv.x,val[1]/gv.y);
    }

    //!Get the element-by-element quota of this and a vector
    inline const GLVector2<T> operator / (const GLIVector2& gv) const
    {
        return GLVector2<T>(val[0]/gv.val[0],val[1]/gv.val[1]);
    }

    //!Get the element-by-element product of this and a scalar
    inline const GLVector2<T> operator * (const T& v) const
    {
        return GLVector2<T>(val[0]*v,val[1]*v);
    }

    //!Get the element-by-element quota of this and a scalar
    inline const GLVector2<T> operator / (const T& v) const
    {
        return GLVector2<T>(val[0]/v,val[1]/v);
    }

    //!Add a vector to this
    GLIVector2& operator += (const GLVector2<T>& gv)
    {
        val[0] += gv.x;
        val[1] += gv.y;
        return *this;
    }

    //!Add a vector to this
    GLIVector2& operator += (const GLIVector2& gv)
    {
        val[0] += gv.val[0];
        val[1] += gv.val[0];
        return *this;
    }

    //!Subtract a vector from this
    GLIVector2& operator -= (const GLVector2<T>& gv)
    {
        val[0] -= gv.x;
        val[1] -= gv.y;
        return *this;
    }

    //!Subtract a vector from this
    GLIVector2& operator -= (const GLIVector2<T>& gv)
    {
        val[0] -= gv.val[0];
        val[1] -= gv.val[1];
        return *this;
    }

    //!Multiply this by a scalar
    GLIVector2& operator *= (const T& f)
    {
        val[0] *= f;
        val[1] *= f;
        return *this;
    }

    //!Divide this by a scalar
    GLIVector2& operator /= (const T& f)
    {
        val[0] /= f;
        val[1] /= f;
        return *this;
    }

    //!copy this from a vector
    GLIVector2& operator = ( const GLVector2<T>& v )
    {   memmove(val, v.val, D*sizeof(T)); return *this; }

    //!copy this from a vector
    GLIVector2& operator = ( const GLIVector2& v )
    {   memmove(val, v.val, D*sizeof(T)); return *this; }

    //!negate this
    inline const GLVector2<T> operator - () const
    {
        return GLVector2<T>(-val[0],-val[1]);
    }

    //!Get the dot product of this and a vector
    inline const T dot(const GLVector2<T>& gv) const
    {   return val[0]*gv.x + val[1]*gv.y; }

    //!Get the dot product of this and a vector
    inline const T dot(const GLIVector2& gv) const
    {   return val[0]*gv.val[0] + val[1]*gv.val[1]; }
    
    //!Get the length of this
    inline T length() const
    {   return sqrt(lengthSqr()); }

    //!Get the length squared, less computation than length()
    inline T lengthSqr() const
    {   return val[0]*val[0] + val[1]*val[1]; }

    //!Get the the unit vector of this
    const GLVector2<T> unit() const
    {
        return (*this)/length();
    }

    //!Normalize this, makes this a unit vector, US spelling
    GLIVector2& normalize()
    {
        (*this)/=length();
        return *this;
    }

    //!Get the projection of this and a vector
    inline const T projection(const GLVector2<T>& in) const
    {   return dot(in); }

    //!Get the projection of this and a vector
    inline const T projection(const GLIVector2& in) const
    {   return dot(in); }

    //!Get the orthogonal projection of this and a vector
    inline GLVector2<T> orthogonalProjection(const GLVector2<T>& in) const
    {   return in - vectorProjection(in); }

    //!Get the orthogonal projection of this and a vector
    inline GLVector2<T> orthogonalProjection(const GLIVector2& in) const
    {   return in - vectorProjection(in); }
    
    //!Get the vector projection of this and a vector
    GLVector2<T> vectorProjection(const GLVector2<T>& in) const
    {   return (*this) * dot(in); }
    
    //!Get the vector projection of this and a vector
    GLVector2<T> vectorProjection(const GLIVector2& in) const
    {   return (*this) * dot(in); }

    #ifdef GLVECTOR_IOSTREAM
    //!iostream interface for input from char stream, enable by defining GLVECTOR_IOSTREAM
    friend std::basic_istream<char, std::char_traits<char> >& operator >>
      ( std::basic_istream<char, std::char_traits<char> >& in, GLIVector2& vec)
    { in>>vec.val[0]>>vec.val[1]; return in;};

    //!iostream interface for output to char stream, enable by defining GLVECTOR_IOSTREAM
    friend std::basic_ostream<char, std::char_traits<char> >& operator <<
      ( std::basic_ostream<char, std::char_traits<char> >& out, GLIVector2& vec)
    { out<<vec.val[0]<<" "<<vec.val[1]; return out;};
    #endif

    T* val;
};

//!Corresponds to glVertex2b ...etc
typedef GLIVector2<GLbyte> GLIVector2b;
//!Corresponds to glVertex2s ...etc
typedef GLIVector2<GLshort> GLIVector2s;
//!Corresponds to glVertex2i ...etc
typedef GLIVector2<GLint> GLIVector2i;
//!Corresponds to glVertex2f ...etc
typedef GLIVector2<GLfloat> GLIVector2f;
//!Corresponds to glVertex2d ...etc
typedef GLIVector2<GLdouble> GLIVector2d;

//!a 3D vector class for OpenGL
template <typename T>
//!a 3D vector class for OpenGL
class GLVector3
{
public:
    enum { D = 3 };
    typedef T value_type;

    //!Create an uninitialised vector
    GLVector3() { }

    //!Create an initialised vector
    GLVector3(const T& v)
    :x(v), y(v), z(v) { }

    //!Create an initialise vector
    GLVector3(const T& a, const T& b, const T& c)
    :x(a),y(b),z(c){}

    //!Copy a vector
    GLVector3(const GLVector3& gv)
    {   memmove(val,gv.val,D*sizeof(T)); }

    //!Create a vector from an array
    GLVector3(const T* f)
    {   memmove(val,f,D*sizeof(T)); }

    //!Default destructor
    ~GLVector3(){}

    //! element by element initialiser
    inline void set(const T& v1, const T& v2, const T& v3)
    {   x = v1; y = v2; z = v3; }

    //! element by element accessor
    inline T& operator[] (int ind)
    {   return val[ind]; }

    //! implicit casting to a pointer
    inline operator T* (void)
    {   return val; }

    //! implicit casting to a const pointer
    inline operator const T* (void) const
    {   return val; }

    //!GL interface, glScale()
    void glScale() const;
    //!GL interface, glRotate()
    void glRotate(const T& ang) const;
    //!GL interface, glTranslate()
    void glTranslate() const;
    //!GL interface, glVertex()
    inline void glVertex() const { mathglpp::glVertexv<D,T>(val); }
    //!GL interface, glNormal()
    void glNormal() const;

    //!copy values to a array
    inline void copyTo(T* vec) const
    {   memmove(vec,val,D*sizeof(T)); }

    //!Get the sum of this and a vector
    inline const GLVector3 operator + (const GLVector3& gv) const
    {
        return GLVector3(x+gv.x,y+gv.y,z+gv.z);
    }

    //!Get the difference of this and a vector
    inline const GLVector3 operator - (const GLVector3& gv) const
    {
        return GLVector3(x-gv.x,y-gv.y,z-gv.z);
    }

    //!Get the element-by-element product of this and a vector
    inline const GLVector3 operator * (const GLVector3& gv) const
    {
        return GLVector3(x*gv.x,y*gv.y,z*gv.z);
    }

    //!Get the element-by-element quota of this and a vector
    inline const GLVector3 operator / (const GLVector3& gv) const
    {
        return GLVector3(x/gv.x,y/gv.y,z/gv.z);
    }

    //!Get the element-by-element product of this and a scalar
    inline const GLVector3 operator * (const T& v) const
    {
        return GLVector3(x*v,y*v,z*v);
    }


    //!Get the element-by-element quota of this and a scalar
    inline const GLVector3 operator / (const T& v) const
    {
        return GLVector3(x/v,y/v,z/v);
    }

    //!Add a vector to this
    GLVector3& operator += (const GLVector3& gv)
    {
        x += gv.x;
        y += gv.y;
        z += gv.z;
        return *this;
    }

    //!Subtract a vector from this
    GLVector3& operator -= (const GLVector3& gv)
    {
        x -= gv.x;
        y -= gv.y;
        z -= gv.z;
        return *this;
    }

    //!Multiply this by a scalar
    GLVector3& operator *= (const T& f)
    {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    //!Divide this by a scalar
    GLVector3& operator /= (const T& f)
    {
        x /= f;
        y /= f;
        z /= f;
        return *this;
    }

    //!copy this from a vector
    GLVector3& operator = (const GLVector3& gv)
    {   memmove(val, gv.val, D*sizeof(T)); return *this; }
    
	//!Equality check. NB. May not be constant time, depending on memcmp.
    inline bool operator == (const GLVector3& gv)
    {   return memcmp(val, gv.val, D*sizeof(T))==0;  }
    
	//!negate this
    inline const GLVector3 operator - () const
    {
        return GLVector3(-x,-y,-z);
    }

    //!Get the dot product of this and a vector
    inline const T dot(const GLVector3& gv) const
    {   return x*gv.x + y*gv.y + z*gv.z;  }

    //!Get the length of this
    inline T length() const
    {   return sqrt(lengthSqr()); }

    //!Get the length squared, less computation than length()
    inline T lengthSqr() const
    {   return x*x + y*y + z*z;  }

    //!Get the cross-product of this and a vector
    const GLVector3 getCross(const GLVector3& gv) const
    {
        return GLVector3(y*gv.z-z*gv.y,z*gv.x-x*gv.z,x*gv.y-y*gv.x);
    }

    //!Apply the cross-product of this and a vector
    GLVector3& cross(const GLVector3& gv)
    {
        T temp[] = { x, y, z };

        x = temp[1] * gv.z - temp[2] * gv.y;
        y = temp[2] * gv.x - temp[0] * gv.z;
        z = temp[0] * gv.y - temp[1] * gv.x;

        return *this;
    }

    //!Get the the unit vector of this
    const GLVector3 unit() const
    {
        return (*this)/length();
    }

    //!Normalize this, makes this a unit vector, US spelling
    GLVector3& normalize()
    {
        (*this)/=length();
        return *this;
    }

    //!Get the projection of this and a vector
    inline const T projection(const GLVector3& in) const
    {   return dot(in); }

    //!Get the orthogonal projection of this and a vector
    inline GLVector3 orthogonalProjection(const GLVector3& in) const
    {   return in - (*this) * dot(in); }

    //!Get the vector projection of this and a vector
    GLVector3 vectorProjection(const GLVector3& in) const
    {   return (*this) * dot(in); }

    #ifdef GLVECTOR_IOSTREAM
    //!iostream interface for input from char stream, enable by defining GLVECTOR_IOSTREAM
    friend std::basic_istream<char, std::char_traits<char> >& operator >>
      ( std::basic_istream<char, std::char_traits<char> >& in, GLVector3& vec)
    { in>>vec.val[0]>>vec.val[1]>>vec.val[2]; return in;};

    //!iostream interface for output to char stream, enable by defining GLVECTOR_IOSTREAM
    friend std::basic_ostream<char, std::char_traits<char> >& operator <<
      ( std::basic_ostream<char, std::char_traits<char> >& out, GLVector3& vec)
    { out<<vec.val[0]<<" "<<vec.val[1]<<" "<<vec.val[2]; return out;};
    #endif

    //!Value array
    //!Anonymous union and struct by sebastien bloc (sebastien.bloc@free.fr)
    union
    {
        T val[D];
        struct
        {
            T x,y,z;
        };
    };
};


//!Corresponds to glVertex3b ...etc
typedef GLVector3<GLbyte> GLVector3b;
//!Corresponds to glVertex3s ...etc
typedef GLVector3<GLshort> GLVector3s;
//!Corresponds to glVertex3i ...etc
typedef GLVector3<GLint> GLVector3i;
//!Corresponds to glVertex3f ...etc
typedef GLVector3<GLfloat> GLVector3f;
//!Corresponds to glVertex3d ...etc
typedef GLVector3<GLdouble> GLVector3d;


//!a 3D vector array class for OpenGL
template <typename T>
//!a 3D vector array class for OpenGL
class GLVectorArray3
{
public:
    enum { D = 3 };
    typedef T value_type;
    typedef std::valarray<value_type> vector_type;
    
    //!Create an uninitialised vector
    GLVectorArray3():vec() { }

    //!Create an initialised vector
    GLVectorArray3(const T& v)
    :vec(v) { }

    //!Copy a vector
    GLVectorArray3(const GLVectorArray3& gv)
    :vec(gv.vec)
    { }

    //!Copy a vector
    GLVectorArray3(const vector_type& v)
    :vec(v)
    { }

    //!Create a vector from an array
    GLVectorArray3(const T* f, int nVec)
    :vec(f,nVec*D)
    { }

    //!Default destructor
    ~GLVectorArray3(){}

    //! element by element accessor
    inline T& operator[] (int ind)
    {   return vec[ind]; }

    //! implicit casting to a pointer
    inline operator T* (void)
    {   return &vec[0]; }

    //! implicit casting to a const pointer
    inline operator const T* (void) const
    {   return &vec[0]; }

    //!copy values to a array
    inline void copyTo(T* v) const
    {   memmove(v,&vec[0],D*sizeof(T)); }

    //!Get the sum of this and a vector
    inline const GLVectorArray3 operator + (const GLVectorArray3& gv) const
    {
        return GLVectorArray3(*this)+=gv;
    }

    //!Get the difference of this and a vector
    inline const GLVectorArray3 operator - (const GLVectorArray3& gv) const
    {
        return GLVectorArray3(*this)-=gv;
    }

    //!Get the element-by-element product of this and a vector
    inline const GLVectorArray3 operator * (const GLVectorArray3& gv) const
    {
        return GLVectorArray3(*this)*=gv;
    }

    //!Get the element-by-element quota of this and a vector
    inline const GLVectorArray3 operator / (const GLVectorArray3& gv) const
    {
        return GLVectorArray3(*this)/=gv;
    }

    //!Get the element-by-element product of this and a scalar
    inline const GLVectorArray3 operator * (const T& v) const
    {
        return GLVectorArray3(*this)*=v;
    }

    //!Get the element-by-element quota of this and a scalar
    inline const GLVectorArray3 operator / (const T& v) const
    {
        return GLVectorArray3(*this)/=v;
    }

    //!Add a vector to this
    GLVectorArray3& operator += (const GLVectorArray3& gv)
    {
        vec += gv.vec;
        return *this;
    }

    //!Subtract a vector from this
    GLVectorArray3& operator -= (const GLVectorArray3& gv)
    {
        vec -= gv.vec;
        return *this;
    }

    //!Multiply this by a scalar
    GLVectorArray3& operator *= (const T& f)
    {
        vec *= f;
        return *this;
    }

    //!Divide this by a scalar
    GLVectorArray3& operator /= (const T& f)
    {
        vec /= f;
        return *this;
    }

    //!copy this from a vector
    GLVectorArray3& operator = (const GLVectorArray3& gv)
    {
        vec = gv.vec;
        return *this;
    }

    //!negate this
    inline const GLVectorArray3 operator - () const
    {
        return GLVectorArray3(-vec);
    }

    //!Value array
    vector_type vec;
};

//!Corresponds to glVertex3b ...etc
typedef GLVectorArray3<GLbyte> GLVectorArray3b;
//!Corresponds to glVertex3s ...etc
typedef GLVectorArray3<GLshort> GLVectorArray3s;
//!Corresponds to glVertex3i ...etc
typedef GLVectorArray3<GLint> GLVectorArray3i;
//!Corresponds to glVertex3f ...etc
typedef GLVectorArray3<GLfloat> GLVectorArray3f;
//!Corresponds to glVertex3d ...etc
typedef GLVectorArray3<GLdouble> GLVectorArray3d;

//!a 3D rational vector class for OpenGL
template <typename T>
//!a 3D rational vector class for OpenGL
class GLVector4
{
public:
    //!Keep the dimensions as a defined enum
    enum { D = 4 };

    //!Create an uninitialised vector
    GLVector4()
    { }

    //!Create an initialised vector
    GLVector4(const T& v)
    {   x = y = z = w = v; }

    //!Create an initialised vector from values
    GLVector4(const T& v1, const T& v2, const T& v3, const T& v4)
    {   x = v1; y = v2; z = v3; w = v4; }

    //!Copy a vector
    GLVector4(const GLVector4& gv)
    {   memmove(val,gv.val,D*sizeof(T)); }

    //!Create a vector from an array
    GLVector4(const T* f)
    {   memmove(val,f,D*sizeof(T)); }

    //!Default destructor
    ~GLVector4(){}

    //! element by element initialiser
    inline void set(const T& v1, const T& v2, const T& v3, const T& v4)
    {   x = v1; y = v2; z = v3; w = v4;}

    //! element by element accessor
    inline T& operator[] (int ind)
    {   return val[ind]; }

    //! implicit casting to a pointer
    inline operator T* (void)
    {   return val; }

    //! implicit casting to a const pointer
    inline operator const T* (void) const
    {   return val; }

    //!GL interface, glScale()
    inline void glScale() const;
    //!GL interface, glRotate()
    inline void glRotate(const T& ang) const;
    //!GL interface, glTranslate()
    inline void glTranslate() const;
    //!GL interface, glVertex()
    inline void glVertex() const { mathglpp::glVertexv<D,T>(val); }
    //!GL interface, glNormal()
    inline void glNormal() const;

    //!copy values to a array
    inline void copyTo(T* vec) const
    {   memmove(vec,val,D*sizeof(T)); }

    //!Get the sum of this and a vector
    inline const GLVector4 operator + (const GLVector4& gv) const
    {
        return GLVector4(x+gv.x,y+gv.y,z+gv.z,w+gv.w);
    }

    //!Get the difference of this and a vector
    inline const GLVector4 operator - (const GLVector4& gv) const
    {
        return GLVector4(x-gv.x,y-gv.y,z-gv.z,w-gv.w);
    }

    //!Get the element-by-element product of this and a vector
    inline const GLVector4 operator * (const GLVector4& gv) const
    {
        return GLVector4(x*gv.x,y*gv.y,z*gv.z,w*gv.w);
    }

    //!Get the element-by-element quota of this and a vector
    inline const GLVector4 operator / (const GLVector4& gv) const
    {
        return GLVector4(x/gv.x,y/gv.y,z/gv.z,w/gv.w);
    }

    //!Get the element-by-element product of this and a scalar
    inline const GLVector4 operator * (const T& v) const
    {
        return GLVector4(x*v,y*v,z*v,w*v);
    }


    //!Get the element-by-element quota of this and a scalar
    inline const GLVector4 operator / (const T& v) const
    {
        return GLVector4(x/v,y/v,z/v,w/v);
    }

    //!Add a vector to this
    GLVector4& operator += (const GLVector4& gv)
    {
        x += gv.x;
        y += gv.y;
        z += gv.z;
        w += gv.w;
        return *this;
    }

    //!Subtract a vector from this
    GLVector4& operator -= (const GLVector4& gv)
    {
        x -= gv.x;
        y -= gv.y;
        z -= gv.z;
        w -= gv.w;
        return *this;
    }

    //!Multiply this by a scalar
    GLVector4& operator *= (const T& f)
    {
        x *= f;
        y *= f;
        z *= f;
        w *= f;
        return *this;
    }

    //!Divide this by a scalar
    GLVector4& operator /= (const T& f)
    {
        x /= f;
        y /= f;
        z /= f;
        w /= f;
        return *this;
    }

    //!copy this from a vector
    GLVector4& operator = (const GLVector4& gv)
    {   memmove(val,gv.val,D*sizeof(T)); return *this; }

    //!Equality check. NB. May not be constant time, depending on memcmp.
    inline bool operator == (const GLVector4& v)
    {   return memcmp(val, v.val, D*sizeof(T))==0;  }

    //!negate this
    inline const GLVector4 operator - () const
    {
        return GLVector4(-x,-y,-z,-w);
    }

    //!Get the dot product of this and a vector
    inline const T dot(const GLVector4& gv) const
    {   return x*gv.x + y*gv.y + z*gv.z + w*gv.w;  }

    //!Get the length of this
    inline T length() const
    {   return	(lengthSqr()); }

    //!Get the length squared, less computation than length()
    inline T lengthSqr() const
    {   return x*x + y*y + z*z + w*w;  }

    //!Get the cross-product of this and a vector
    const GLVector4 getCross(const GLVector4& gv) const
    {
        return GLVector4(y*gv.z-z*gv.y,z*gv.w-w*gv.z,w*gv.x-x*gv.w,x*gv.y-y*gv.x);
    }

    //!Apply the cross-product of this and a vector
    GLVector4& cross(const GLVector4& gv)
    {
        T temp[] = { x, y, z, w };

        x = temp[1] * gv.z - temp[2] * gv.y;
        y = temp[2] * gv.w - temp[3] * gv.z;
        z = temp[3] * gv.x - temp[0] * gv.w;
        w = temp[0] * gv.y - temp[1] * gv.x;

        return *this;
    }

    //!Get the the unit vector of this
    const GLVector4 unit() const
    {
        return (*this)/length();
    }

    //!Normalize this, makes this a unit vector, US spelling
    GLVector4& normalize()
    {
        (*this)/=length();
        return *this;
    }

    //!Get the projection of this and a vector
    inline const T projection(const GLVector4& in) const
    {   return dot(in); }

    //!Get the orthogonal projection of this and a vector
    inline GLVector4 orthogonalProjection(const GLVector4& in) const
    {   return in - vectorProjection(in); }

    //!Get the vector projection of this and a vector
    GLVector4 vectorProjection(const GLVector4& in) const
    {   return (*this) * dot(in); }

    #ifdef GLVECTOR_IOSTREAM
    //!iostream interface for input from char stream, enable by defining GLVECTOR_IOSTREAM
    friend std::basic_istream<char, std::char_traits<char> >& operator >>
      ( std::basic_istream<char, std::char_traits<char> >& in, GLVector4& vec)
    { in>>vec.val[0]>>vec.val[1]>>vec.val[2]>>vec.val[3]; return in;}

    //!iostream interface for output to char stream, enable by defining GLVECTOR_IOSTREAM
    friend std::basic_ostream<char, std::char_traits<char> >& operator <<
      ( std::basic_ostream<char, std::char_traits<char> >& out, GLVector4& vec)
    { out<<vec.val[0]<<" "<<vec.val[1]<<" "<<vec.val[2]<<" "<<vec.val[3]; return out;}
    #endif

    //!Value array
    //!Anonymous union and struct by sebastien bloc (sebastien.bloc@free.fr)
    union
    {
        T val[D];
        struct
        {
            T x,y,z,w;
        };
    };
};

//!Corresponds to glVertex4b ...etc
typedef GLVector4<GLbyte> GLVector4b;
//!Corresponds to glVertex4s ...etc
typedef GLVector4<GLshort> GLVector4s;
//!Corresponds to glVertex4i ...etc
typedef GLVector4<GLint> GLVector4i;
//!Corresponds to glVertex4f ...etc
typedef GLVector4<GLfloat> GLVector4f;
//!Corresponds to glVertex4d ...etc
typedef GLVector4<GLdouble> GLVector4d;

};

#endif
