// ver. 1.9.1
#ifndef _MATHVECT__H
#define _MATHVECT__H

#define M_PI 3.14159265358979323846

#include <math.h>


struct vect3
{
        float x,y,z;

        vect3(float x1,float y1,float z1);

        vect3(float a);
        vect3(float *faVect3);
        vect3();

        vect3 operator*(vect3 v); // vector  CROS product
        float operator^(vect3 v); // vector DOT product

        vect3 operator/ (float value);  // divide by scalar
        vect3 operator* (float value);  // multiple by scalar
        vect3 operator+=(vect3 v);  // add vector
        vect3 operator+(vect3 v);  // add vector
        vect3 operator-=(vect3 v);  // sub vector
        vect3 operator-(vect3 v); // sub vector

        vect3 operator- ();  // inversely vector

        // rotate vector by angle [DEG]  around Normal vector (x,y,z)
        vect3 Rotate(float alfa,float vnX,float vnY,float vnZ);

        float l(); // vector lenght

        vect3 n(); // normal vector

        void pointer(float vv[]); // take vector values from float table
};

#endif
