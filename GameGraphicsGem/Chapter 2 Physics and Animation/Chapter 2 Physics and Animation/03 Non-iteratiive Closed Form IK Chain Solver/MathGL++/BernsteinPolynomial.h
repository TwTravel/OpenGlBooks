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
#ifndef GLBERNSTEINPOLYNOMIAL_H
#define GLBERNSTEINPOLYNOMIAL_H

#include "GLFunctions.h"

#include <vector>

namespace mathglpp
{

/**
Bernstein Polynomials are the basis for Bezier curves.
They are called evaluators in the OpenGL API.
@author Jacques Gasselin
*/
class GLBernsteinPolynomial
{
   static std::vector< std::vector<GLuint> > pascalsTriangle;

   public:
   /**Create a bernstein polynomial of order at least n */
   GLBernsteinPolynomial(GLuint n);

   /**Default destructor */
   ~GLBernsteinPolynomial();

   /**Use to ensure that the polynomial is capable of using the desired order*/
   void setOrder(GLuint);

   /**Get interpolation product for control point c, where c = [0,...,order] & u = [0,1]*/
   const GLfloat get(const GLuint order, const GLuint c, const GLfloat u);

   /**Get interpolation product vector for all control points c, where u = [0,1]
     *This is normally used for post multiplying with a control point matrix*/
   const void getVector(const GLuint order, const GLfloat u, GLfloat* vec);

   protected:
   /**Extends Pascal's triangle as needed*/
   void extendPascal(GLuint);

   /**This is exactly the same as "c choose n" in statistics*/
   inline const GLuint getCoeff(const GLuint order, const GLuint choice)
   {  return pascalsTriangle[order][choice]; }
};

};
#endif
