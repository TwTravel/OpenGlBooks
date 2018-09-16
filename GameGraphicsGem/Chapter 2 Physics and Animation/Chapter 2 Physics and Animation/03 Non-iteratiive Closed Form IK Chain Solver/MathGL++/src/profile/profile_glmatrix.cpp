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

#include <iostream>
#include <time.h>
#include "GLMatrix.h"

using namespace std;
using namespace mathglpp;

template <typename T>
void profile_scalar_operator_mul(const int niter)
{
    GLMatrix<T> matrix;

    for(int i = 0; i < niter; ++i)
    {
        matrix *= (niter-i);
    }
}

template <typename T>
void profile_matrix_operator_mul(const int niter)
{
    GLMatrix<T> matrix;
    GLMatrix<T> matrix1;
    matrix.loadIdentity();
    matrix1.loadIdentity();

    for(int i = 0; i < niter; ++i)
    {
        matrix *= matrix1;
    }
}

template <typename T>
void profile_inverse(const int niter)
{
    GLMatrix<T> matrix;
    matrix.loadIdentity();

    for(int i = 0; i < niter; ++i)
    {
        matrix.inverse();
    }
}

int main(int argc, char* argv[])
{
    cout<<"Profile the GLMatrix class:"<<endl;
    cout<<"Which type?"<<endl;
    cout<<"1: GLMatrix<float>"<<endl;
    cout<<"2: GLMatrix<double>"<<endl;
    int choice;
    cout<<"Enter choice: ";
    cin>>choice;

    if(choice == 1)
    {
        bool quit = false;
        int niter = 10000;
        clock_t begin, end;

        while(1)
        {
            cout<<"Profile menu for GLMatrix<float>"<<endl;
            cout<<"0: Quit"<<endl;
            cout<<"1: Change number of profiling iterations"<<endl;
            cout<<"2: profile scalar operator*="<<endl;
            cout<<"3: profile matrix operator*="<<endl;
            cout<<"4: profile inverse"<<endl;
            cout<<"Enter choice: ";
            cin>>choice;

            if(choice == 0)
            {
                return 0; 
            }
            else
            if(choice == 1)
            {
                cout<<"Enter number of iterations: ";
                cin>>niter;
            }
            else
            if(choice == 2)
            {
                begin = clock();
                profile_scalar_operator_mul<float>(niter);
                end = clock();
            }
            else
            if(choice == 3)
            {
                begin = clock();
                profile_matrix_operator_mul<float>(niter);
                end = clock();
            }
            else
            if(choice == 4)
            {
                begin = clock();
                profile_inverse<float>(niter);
                end = clock();
            }
            
            float nsecs = float(end-begin)/float(CLOCKS_PER_SEC);
            cout<<"Cumulative time taken: "<<nsecs<<endl;
            cout<<"Time per operation: "<<(nsecs/float(niter))<<endl;
        };
    }
    else
    if(choice == 2)
    {

        
    }

    return 0;
}
