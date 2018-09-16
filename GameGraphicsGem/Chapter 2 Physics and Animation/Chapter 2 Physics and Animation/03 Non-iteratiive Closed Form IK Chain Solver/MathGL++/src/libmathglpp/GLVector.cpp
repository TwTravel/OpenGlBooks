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

#include "GLVector.h"

#ifdef GLVECTOR_USE_SSE
typedef int v4sf __attribute__ ((mode(V4SF)));
#endif

namespace mathglpp
{

template <>
void GLVector2<GLbyte>::glScale() const
{
    ::glScalef(val[0],val[1],0);
}

template <>
void GLVector2<GLshort>::glScale() const
{
    ::glScalef(val[0],val[1],0);
}

template <>
void GLVector2<GLint>::glScale() const
{
    ::glScalef(static_cast<GLfloat>(val[0]),static_cast<GLfloat>(val[1]),0.0);
}

template <>
void GLVector2<GLfloat>::glScale() const
{
    ::glScalef(val[0],val[1],0.0);
}

template <>
void GLVector2<GLdouble>::glScale() const
{
    ::glScaled(val[0],val[1],0.0);
}

template <>
void GLVector2<GLfloat>::glRotate(const GLfloat& ang) const
{
    ::glRotatef(ang,val[0],val[1],0);
}

template <>
void GLVector2<GLdouble>::glRotate(const GLdouble& ang) const
{
    ::glRotated(ang,val[0],val[1],0);
}

template <>
void GLVector2<GLfloat>::glTranslate() const
{
    ::glTranslatef(val[0],val[1],0);
}

template <>
void GLVector2<GLdouble>::glTranslate() const
{
    ::glTranslated(val[0],val[1],0);
}

template <>
void GLVector2<GLbyte>::glVertex() const
{
    ::glVertex2s(x,y); //!Closest possible fit
}

template <>
void GLVector2<GLfloat>::glNormal() const
{
    ::glNormal3f(val[0],val[1],0);
}

template <>
void GLVector2<GLdouble>::glNormal() const
{
    ::glNormal3d(val[0],val[1],0);
}

template <>
void GLVector3<GLfloat>::glScale() const
{
    ::glScalef(val[0],val[1],val[2]);
}

template <>
void GLVector3<GLdouble>::glScale() const
{
    ::glScaled(val[0],val[1],val[2]);
}

template <>
void GLVector3<GLfloat>::glRotate(const GLfloat& ang) const
{
    ::glRotatef(ang,val[0],val[1],val[2]);
}

template <>
void GLVector3<GLdouble>::glRotate(const GLdouble& ang) const
{
    ::glRotated(ang,val[0],val[1],val[2]);
}

template <>
void GLVector3<GLfloat>::glTranslate() const
{
    ::glTranslatef(val[0],val[1],val[2]);
}

template <>
void GLVector3<GLdouble>::glTranslate() const
{
    ::glTranslated(val[0],val[1],val[2]);
}

template <>
void GLVector3<GLbyte>::glVertex() const
{
    ::glVertex3s(x,y,z); //!Closest possible fit
}

template <>
void GLVector3<GLfloat>::glNormal() const
{
    ::glNormal3fv(val);
}

template <>
void GLVector3<GLdouble>::glNormal() const
{
    ::glNormal3dv(val);
}

template <>
GLVector3<GLfloat>& GLVector3<GLfloat>::operator /= (const GLfloat& v)
{
    GLfloat r=1.0f/v;
    x*=r;
    y*=r;
    z*=r;
    return *this;
}

template <>
GLVector3<GLdouble>& GLVector3<GLdouble>::operator /= (const GLdouble& v)
{
    GLdouble r=1.0/v;
    x*=r;
    y*=r;
    z*=r;
    return *this;
}

template <>
const GLVector3<GLfloat> GLVector3<GLfloat>::operator / (const GLfloat& v) const
{
    GLfloat r=1.0f/v;
    return GLVector3<GLfloat>(x*r,y*r,z*r);
}

template <>
const GLVector3<GLdouble> GLVector3<GLdouble>::operator / (const GLdouble& v) const
{
    GLdouble r=1.0/v;
    return GLVector3<GLdouble>(x*r,y*r,z*r);
}

template <>
void GLVector4<GLfloat>::glScale() const
{
    ::glScalef(val[0],val[1],val[2]);
}

template <>
void GLVector4<GLdouble>::glScale() const
{
    ::glScaled(val[0],val[1],val[2]);
}

template <>
void GLVector4<GLfloat>::glRotate(const GLfloat& ang) const
{
    ::glRotatef(ang,val[0],val[1],val[2]);
}

template <>
void GLVector4<GLdouble>::glRotate(const GLdouble& ang) const
{
    ::glRotated(ang,val[0],val[1],val[2]);
}

template <>
void GLVector4<GLfloat>::glTranslate() const
{
    ::glTranslatef(val[0],val[1],val[2]);
}

template <>
void GLVector4<GLdouble>::glTranslate() const
{
    ::glTranslated(val[0],val[1],val[2]);
}

template <>
void GLVector4<GLbyte>::glVertex() const
{
    ::glVertex4s(x,y,z,w); //!Closest possible fit
}

template <>
void GLVector4<GLfloat>::glNormal() const
{
    ::glNormal3fv(val);
}

template <>
void GLVector4<GLdouble>::glNormal() const
{
    ::glNormal3dv(val);
}

template <>
const GLVector4<GLfloat> GLVector4<GLfloat>::operator / (const GLfloat& v) const
{
    GLfloat r=1.0f/v;
    return GLVector4<GLfloat>(x*r,y*r,z*r,w*r);
}

template <>
const GLVector4<GLdouble> GLVector4<GLdouble>::operator / (const GLdouble& v) const
{
    GLdouble r=1.0/v;
    return GLVector4<GLdouble>(x*r,y*r,z*r,w*r);
}

#ifdef GLVECTOR_USE_SSE
template <>
GLVector4<GLfloat>& GLVector4<GLfloat>::operator += (const GLVector4<GLfloat>& gv)
{
    v4sf* a = (v4sf*)val;
    v4sf* b = (v4sf*)gv.val;

    *a = __builtin_ia32_addps(*a,*b);
    return *this;
}

template <>
GLVector4<GLfloat>& GLVector4<GLfloat>::operator -= (const GLVector4<GLfloat>& gv)
{
    v4sf* a = (v4sf*)val;
    v4sf* b = (v4sf*)gv.val;

    *a = __builtin_ia32_subps(*a,*b);
    return *this;
}

#endif

};
