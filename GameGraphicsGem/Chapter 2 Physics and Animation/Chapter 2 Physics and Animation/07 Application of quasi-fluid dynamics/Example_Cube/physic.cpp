#include "physic.h"


Physic::Physic()
{
	fM=6000;
	fI=6000;

	vPosition=vect3(0,2,0);
	qOrientation=quaternion();
	qRotation=qOrientation.AsixAngle();

	vVelocity=vect3(10,0,0);
	vA_Velocity=vect3(0,0,0);
	vAcceleration=vect3(0,0,0);
	vA_Acceleration=vect3(0,0,0);
	vG=vect3(0,-10,0);
	vTorque=vForce=vect3(0,0,0);

}


// S I M U L A T I O N
void Physic::Sim(float fdt)
{

quaternion qTemp;
vect3 vA_Delta;


 vAcceleration=vForce/fM+vG;//-vVelocity*100.0/fM;

float fTestStability=vAcceleration.l()-vVelocity.l()/fdt;

if (fTestStability>(15.0/fdt)) //HOLDING for STABILITY
 {
 	vForce=vect3(0,0,0);
 	vTorque=vect3(0,0,0);

 	vVelocity=vect3(0,0,0);
 	vA_Velocity=vect3(0,0,0);

 	vAcceleration=vect3(0,0,0);
 }


 vA_Acceleration=vTorque/fI;

 vVelocity+=vAcceleration*fdt;
 vA_Velocity+=vA_Acceleration*fdt;

 vPosition+=vVelocity*fdt;
 vA_Delta=vA_Velocity*fdt;

 qTemp=AsixToQuat(vA_Delta,vA_Delta.l());

 qOrientation=qOrientation*qTemp; // poprawka robocza

 qOrientation=qOrientation.n();

 qRotation=qOrientation.AsixAngle();

 // calculate local AngleVelocity and Velocity
 vAVelLocal=vA_Velocity.Rotate(float(-qRotation.w*M_RAD),qRotation.x,qRotation.y,qRotation.z);
 vVelLocal=vVelocity.Rotate(float(-qRotation.w*M_RAD),qRotation.x,qRotation.y,qRotation.z);

}
