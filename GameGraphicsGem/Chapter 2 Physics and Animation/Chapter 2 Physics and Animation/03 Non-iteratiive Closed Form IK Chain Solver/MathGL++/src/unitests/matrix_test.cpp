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

#define MATRIX_IOSTREAM
#include "Matrix.h"

using namespace std;
using namespace mathglpp;

float filter(const Matrix2<float>& orig, unsigned i, unsigned j)
{
    return orig(i,j)+((float)i+j)*(random()/((float)RAND_MAX+1.0));
}

int main()
{
    Matrix2<float> cube(4,4);
    cout<<cube;
    
    cube.identity();
    cout<<cube;
    
    cube.row(1) = 3;
    cout<<cube;
    
    cube.col(3) = 4;
    cout<<cube;
    
    cube = cube.filter(filter);
    cout<<cube;
    return 0;
}
