/***************************************************************************
 *   Copyright (C) 2005 by Jacques Gasselin de Richebourg                  *
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
#include "GLFunctions.h"
#include "fixedfloat26_6.h"

namespace mathglpp
{

template <>
void glVertexv<2,GLdouble>(const GLdouble* arr) { ::glVertex2dv(arr); }
template <>
void glVertexv<2,GLfloat>(const GLfloat* arr) { ::glVertex2fv(arr); }
template <>
void glVertexv<2,GLint>(const GLint* arr) { ::glVertex2iv(arr); }
template <>
void glVertexv<2,GLshort>(const GLshort* arr) { ::glVertex2sv(arr); }
template <>
void glVertexv<2,GLbyte>(const GLbyte* arr) { ::glVertex2s(arr[0], arr[1]); }
template <>
void glVertexv<2,FixedFloat26_6>(const FixedFloat26_6* arr) { ::glVertex2f(arr[0].toFloat(), arr[1].toFloat()); }

template <>
void glVertexv<3,GLdouble>(const GLdouble* arr) { ::glVertex3dv(arr); }
template <>
void glVertexv<3,GLfloat>(const GLfloat* arr) { ::glVertex3fv(arr); }
template <>
void glVertexv<3,GLint>(const GLint* arr) { ::glVertex3iv(arr); }
template <>
void glVertexv<3,GLshort>(const GLshort* arr) { ::glVertex3sv(arr); }
template <>
void glVertexv<3,GLbyte>(const GLbyte* arr) { ::glVertex3s(arr[0], arr[1], arr[2]); }
template <>
void glVertexv<3,FixedFloat26_6>(const FixedFloat26_6* arr) { ::glVertex3f(arr[0].toFloat(), arr[1].toFloat(), arr[2].toFloat()); }

template <>
void glVertexv<4,GLdouble>(const GLdouble* arr) { ::glVertex4dv(arr); }
template <>
void glVertexv<4,GLfloat>(const GLfloat* arr) { ::glVertex4fv(arr); }
template <>
void glVertexv<4,GLint>(const GLint* arr) { ::glVertex4iv(arr); }
template <>
void glVertexv<4,GLshort>(const GLshort* arr) { ::glVertex4sv(arr); }
template <>
void glVertexv<4,GLbyte>(const GLbyte* arr) { ::glVertex4s(arr[0], arr[1], arr[2], arr[3]); }
template <>
void glVertexv<4,FixedFloat26_6>(const FixedFloat26_6* arr) { ::glVertex4f(arr[0].toFloat(), arr[1].toFloat(), arr[2].toFloat(), arr[3].toFloat()); }

template <>
void glLoadMatrix<GLdouble>(const GLdouble* m) { ::glLoadMatrixd(m); }
template <>
void glLoadMatrix<GLfloat>(const GLfloat* m) { ::glLoadMatrixf(m); }

template <>
void glMultMatrix<GLdouble>(const GLdouble* m) { ::glMultMatrixd(m); }
template <>
void glMultMatrix<GLfloat>(const GLfloat* m) { ::glMultMatrixf(m); }

template <>
void glGet<GLboolean>(GLenum pname, GLboolean* params) { ::glGetBooleanv(pname, params); }
template <>
void glGet<GLdouble>(GLenum pname, GLdouble* params) { ::glGetDoublev(pname, params); }
template <>
void glGet<GLfloat>(GLenum pname, GLfloat* params) { ::glGetFloatv(pname, params); }
template <>
void glGet<GLint>(GLenum pname, GLint* params) { ::glGetIntegerv(pname, params); }

template <>
void glMap1<GLdouble>(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble* points)
{   ::glMap1d(target, u1, u2, stride, order, points); }
template <>
void glMap1<GLfloat>(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat* points)
{   ::glMap1f(target, u1, u2, stride, order, points); }

template <>
void glMap2<GLdouble>(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble* points)
{   ::glMap2d(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points); }
template <>
void glMap2<GLfloat>(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat* points)
{   ::glMap2f(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points); }

template <>
void glMapGrid1<GLdouble>(GLint un, GLdouble u1, GLdouble u2) { ::glMapGrid1d(un, u1, u2); }
template <>
void glMapGrid1<GLfloat>(GLint un, GLfloat u1, GLfloat u2) { ::glMapGrid1f(un, u1, u2); }


template <>
void glMapGrid2<GLdouble>(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2) { ::glMapGrid2d(un, u1, u2, vn, v1, v2); }
template <>
void glMapGrid2<GLfloat>(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2) { ::glMapGrid2f(un, u1, u2, vn, v1, v2); }


template <>
void glEvalCoord1<GLdouble>(GLdouble u) { ::glEvalCoord1d(u); }
template <>
void glEvalCoord1<GLfloat>(GLfloat u) { ::glEvalCoord1f(u); }

template <>
void glEvalCoord2<GLdouble>(GLdouble u, GLdouble v) { ::glEvalCoord2d(u, v); }
template <>
void glEvalCoord2<GLfloat>(GLfloat u, GLfloat v) { ::glEvalCoord2f(u, v); }


};
