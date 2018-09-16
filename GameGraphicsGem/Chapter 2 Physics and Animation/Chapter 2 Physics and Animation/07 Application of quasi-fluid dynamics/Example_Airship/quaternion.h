// ver. 0.1
#ifndef _QUATMATHVECT__H
#define _QUATMATHVECT__H

#define M_PI 3.14159265358979323846
#define M_RAD 180/M_PI

#include <math.h>
#include "mathvect.h"

struct quaternion
{
        float x,y,z,w;


        quaternion(float x1,float y1,float z1,float w1);
        quaternion();

        quaternion AsixAngle();

  quaternion operator*(quaternion q);
  quaternion operator~ ();
  quaternion operator+=(quaternion q);
  quaternion operator+ (quaternion q);
  quaternion n();
  quaternion operator* (float value);
  quaternion operator/ (float value);
  float l();

};

quaternion AsixToQuat(vect3 v,float angle);

#endif
