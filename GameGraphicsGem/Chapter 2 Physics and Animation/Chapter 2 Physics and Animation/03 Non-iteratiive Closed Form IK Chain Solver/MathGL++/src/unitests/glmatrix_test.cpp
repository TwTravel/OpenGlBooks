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

#define GLMATRIX_IOSTREAM
#include "GLMatrix.h"

using namespace std;
using namespace mathglpp;

int main()
{
   GLMatrix4f mat;
   cout<<"Creating an uninitialised matrix: "<<endl<<mat<<endl;
   mat.loadIdentity();
   cout<<"Making it into the identity matrix: "<<endl<<mat<<endl;
   cout<<"The determinant is: "<<mat.det()<<endl;
   mat[0] *= 3;
   cout<<"Scaling in x: "<<endl<<mat<<endl;
   cout<<"The determinant is: "<<mat.det()<<endl;
   mat[13] += 5;
   cout<<"Translating in y: "<<endl<<mat<<endl;
   cout<<"The determinant is: "<<mat.det()<<endl;

   GLMatrix4f adj = mat.adjoint();

   cout<<"The adjoint is: "<<endl<<adj<<endl;
   adj/=mat.det();

   cout<<"Thus the inverse is: "<<endl<<adj<<endl;


   cout<<"Test the inverse facility"<<endl<<"Input a matrix in row-major format"<<endl;
   cin>>adj;
   GLMatrix4f temp = adj.inverse();

   cout<<"The matrix input was: "<<endl<<adj<<endl;
   cout<<"The inverse is: "<<endl<<temp<<endl;
   cout<<"The determinant is: "<<adj.det()<<endl;





   return 0;
}
