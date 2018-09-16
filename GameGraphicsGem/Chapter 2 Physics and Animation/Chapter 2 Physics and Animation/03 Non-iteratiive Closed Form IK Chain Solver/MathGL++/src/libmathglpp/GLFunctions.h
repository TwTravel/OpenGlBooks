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
#ifndef MATHGLPPGLFUNCTIONS_H
#define MATHGLPPGLFUNCTIONS_H


#ifdef _WIN32
   #pragma warning (disable:4069)
   #pragma warning (disable:4201)
   #include <windows.h>
   #include <GL/gl.h>
   #define M_PI   3.14159265358979323846
#else
   #ifdef HAVE_CONFIG_H
      #include "config.h"
      #ifndef OPENGL_HEADER
         #define OPENGL_HEADER <GL/gl.h>
      #endif
      #include OPENGL_HEADER
   #else
      #include <GL/gl.h>
   #endif
#endif

#include <cstring>
#include <cmath>
#include <cassert>

namespace mathglpp{

template <int D, typename T>
void glVertexv(const T* /*arr*/);// { assert(0 && "OpenGL does not support glVertexv for this type and dimension"); }

template <typename T>
void glVertex2(T /*x*/, T /*y*/);// { assert(0 && "OpenGL does not support glVertex2 for this type"); }

template <typename T>
void glVertex3(T /*x*/, T /*y*/, T /*z*/);// { assert(0 && "OpenGL does not support glVertex3 for this type"); }

template <typename T>
void glVertex4(T /*x*/, T /*y*/, T /*z*/, T /*w*/);// { assert(0 && "OpenGL does not support glVertex4 for this type"); }

template <typename T>
void glLoadMatrix(const T* /*m*/);// { assert(0 && "OpenGL does not support glLoadMatrix for this type"); }

template <typename T>
void glMultMatrix(const T* /*m*/);// { assert(0 && "OpenGL does not support glMultMatrix for this type"); }

template <typename T>
void glGet(GLenum /*pname*/, T* /*params*/);// { assert(0 && "OpenGL does not support glGet for this type"); }

template <typename T>
void glMap1(GLenum /*target*/, T /*u1*/, T /*u2*/, GLint /*stride*/, GLint /*order*/, const T* /*points*/);// { assert(0 && "OpenGL does not support glMap1 for this type"); }

template <typename T>
void glMap2(GLenum /*target*/, T /*u1*/, T /*u2*/, GLint /*ustride*/, GLint /*uorder*/, T /*v1*/, T /*v2*/, GLint /*vstride*/, GLint /*vorder*/, const T* /*points*/);// { assert(0 && "OpenGL does not support glMap1 for this type"); }

template <typename T>
void glMapGrid1(GLint /*un*/, T /*u1*/, T /*u2*/);// { assert(0 && "OpenGL does not support glMapGrid1 for this type"); }

template <typename T>
void glMapGrid2(GLint /*un*/, T /*u1*/, T /*u2*/, GLint /*vn*/, T /*v1*/, T /*v2*/);//  { assert(0 && "OpenGL does not support glMapGrid2 for this type"); }

template <typename T>
void glEvalCoord1(T /*u*/);//  { assert(0 && "OpenGL does not support glEvalCoord1 for this type"); }

template <typename T>
void glEvalCoord2(T /*u*/, T /*v*/);//  { assert(0 && "OpenGL does not support glEvalCoord2 for this type"); }

};

#endif

