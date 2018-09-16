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
#include "GLBernsteinPolynomial.h"

namespace mathglpp
{

std::vector<std::vector<GLuint> > GLBernsteinPolynomial::pascalsTriangle;

GLBernsteinPolynomial::GLBernsteinPolynomial(GLuint ord)
{
   if(ord >= pascalsTriangle.size() )
      extendPascal(ord);
}

GLBernsteinPolynomial::~GLBernsteinPolynomial()
{
}

void GLBernsteinPolynomial::setOrder(GLuint ord)
{
   if(ord >= pascalsTriangle.size())
       extendPascal(ord);
}

void GLBernsteinPolynomial::extendPascal(GLuint level)
{
   if(pascalsTriangle.size() < 2)
   {
      std::vector<GLuint> row;
      row.push_back(1);

      pascalsTriangle.push_back(row);
      row.push_back(1);
      pascalsTriangle.push_back(row);
   }

   if(level >= pascalsTriangle.size() )
   {
      std::vector<GLuint> row;
      row.push_back(1);

      for(register GLuint i = 1; i < pascalsTriangle.size(); ++i)
         row.push_back( pascalsTriangle[pascalsTriangle.size()-1][i-1] +
                        pascalsTriangle[pascalsTriangle.size()-1][i] );

      row.push_back(1);
      pascalsTriangle.push_back(row);

      extendPascal(level);
   }
}

const GLfloat GLBernsteinPolynomial::get(const GLuint order, const GLuint choice, const GLfloat u)
{
   assert(choice <= order);

   if(choice == 0)
   {
      if( u <= 0.0000001 )
         return 1;
      else
      if( u >= 0.9999999 )
         return 0;
      else
		  return pow(GLfloat(1-u), GLint(order));
   }
   else
   if(choice == order)
   {
      if( u <= 0.0000001 )
         return 0;
      else
      if( u >= 0.9999999 )
         return 1;
      else
         return pow((GLfloat)u, (GLint)choice);
   }
   else
   if( u <= 0.0000001 )
      return 0;
   else
   if( u >= 0.9999999 )
      return 0;
   else
      return pascalsTriangle[order][choice]
           * pow((GLfloat)u, (GLint)choice)
       * pow((GLfloat)(1-u), (GLint)(order - choice));
}

const void GLBernsteinPolynomial::getVector(const GLuint order, const GLfloat u, GLfloat* vec)
{
   assert(vec);
   for(register GLuint i = 0; i < order; ++i)
       vec[i] = get(order,i,u);
}

};
