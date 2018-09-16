
#ifndef _PHYSIC__H
#define _PHYSIC__H


#include "mathvect.h"
#include "quaternion.h"

struct Physic
{
float fM; //mass [kg];
float fI; //!!!!! SIMPLIFICATION imersion tensor [kg*m*2]


 vect3 vVelLocal;   // linear velocity in local (model) coordinate system [m/s]
 vect3 vAVelLocal; // angler velocity in local (model) coordinate system [rad/s]



quaternion  qOrientation;  // orientation i quaternion [rad]
vect3      vPosition; // center of mass position [m]

vect3      vA_Velocity; // angle velocity [rad/s]
vect3      vVelocity; // linear velocity [m/s]

vect3      vA_Acceleration; // angle acceleration [r/s^2]
vect3      vAcceleration;   // linear acceleration [m/s^2]

vect3      vTorque;     // total torque [N*m]
vect3      vForce;      // total force [N]

vect3      vG; // graviti vector
quaternion  qRotation; // orientation represented by normal rotation vector and angle

      Physic();

      void Sim(float);  // calculation next step simulation (delta_t)
};

#endif
