#include <windows.h>

#include <gl\gl.h>
#include <gl\glu.h>

#include "render.h"
#include "mesh.h"

#include "mesh_data_airship.h"

#include "stdio.h"


float fControlParameter[3]={0,0,0};  // float parameters to model control


Mesh *Airship;


float fVertexes[VERTEXES][3];
int iTriangles[FACES][3];

vect3 vTriangles_Surf[FACES];
vect3 vTriangles_Center[FACES];
vect3 vTriangles_Normal[FACES];

vect3 vVertexPos[VERTEXES];

vect3 vMediumSurfN;
vect3 vMediumSurfPos;


Mesh::Mesh()
{
    for (int i=0;i<VERTEXES;i++)
    {
        fVertexes_data[i][1]-=0.75f; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        fVertexes_data[i][2]+=0.05f;

        vVertexPos[i]=vect3(fVertexes_data[i]);
    }

    for (int i=0;i<FACES;i++)
    {
        iTriangles[i][0]=iTriangles_data[i][0];
        iTriangles[i][1]=iTriangles_data[i][1];
        iTriangles[i][2]=iTriangles_data[i][2];

        vTriangles_Surf[i]=-((vVertexPos[iTriangles[i][1]]-vVertexPos[iTriangles[i][0]])*
                        (vVertexPos[iTriangles[i][2]]-vVertexPos[iTriangles[i][0]]))/2.0;  // surface vector calculation

        vTriangles_Normal[i]=vTriangles_Surf[i].n(); // triangle normal vector
        vTriangles_Center[i]=(vVertexPos[iTriangles[i][0]]+vVertexPos[iTriangles[i][1]]+vVertexPos[iTriangles[i][2]])/3.0;  // center of triangle
    }
}



void Mesh::Render(char cMode)
{

vect3 vTemp;

glPushMatrix();  // convertion to global coordinate system
glTranslatef(vPosition.x,vPosition.y,vPosition.z);
glRotatef(float(qRotation.w*M_RAD),qRotation.x,qRotation.y,qRotation.z);


    glColor4f(0,0,0,0.25); // shadow color when cMode=0

    if (cMode)
    {
        glColor3f(1,1,1); // rendering center of mass
        glPointSize(5);
        glBegin(GL_POINTS);
        glVertex3f(0,0,0);
        glEnd();
        glPointSize(1);

        glColor3f(1,0,0);   // rendering velocity vector
        glBegin(GL_LINES);
        glVertex3f(0,0,0);
        vTemp=vVelLocal/2.0;
        glVertex3fv(&vTemp.x);
        glEnd();

        vTemp=vTemp/2.0;
        glPushMatrix();  // print velocity lenght
        glTranslatef(vTemp.x,vTemp.y,vTemp.z);
        glRasterPos2f(0,0);
        glPrint("%0.2fm/s",vVelLocal.l());
        glPopMatrix();

        glEnable(GL_POLYGON_OFFSET_FILL);

        glColor4f(0.5,1.0,0.5,1);
    }


    glBegin(GL_TRIANGLES); // if cMode=1     then rendering shape     else rendering shadow
    for (int i =0;i<FACES;i++)
    {
         glVertex3fv(&vVertexPos[iTriangles[i][0]].x);
         glVertex3fv(&vVertexPos[iTriangles[i][1]].x);
         glVertex3fv(&vVertexPos[iTriangles[i][2]].x);
    }
    glEnd();


    if (cMode)  // rendering mesh edges
    {
         glColor3f(0.05f,0.85f,0.05f);
         glBegin(GL_LINES);
         for (int i =0;i<FACES;i++)
         {
             glVertex3fv(&vVertexPos[iTriangles[i][0]].x); glVertex3fv(&vVertexPos[iTriangles[i][1]].x);
             glVertex3fv(&vVertexPos[iTriangles[i][1]].x); glVertex3fv(&vVertexPos[iTriangles[i][2]].x);
             glVertex3fv(&vVertexPos[iTriangles[i][2]].x); glVertex3fv(&vVertexPos[iTriangles[i][0]].x);
         }
        glEnd();

        glDisable(GL_POLYGON_OFFSET_FILL);
    }

glPopMatrix();
}


void Mesh::CalcPressure()
{

float fHeightTriangle; // height to medium boundary (altitude 0m)
float fStatPressure;  // static pressure value
float fDynaPressure;  // dynamic pressure value

vect3 vTriVelo;  // center of triangle velocity
vect3 vTriVeloN; // normal of center of triangle velocity
vect3 vTriForce; // force on triangle
vect3 vTriForceDyn; // dynamic force on triangle
float fTriVeloLen; // enter of triangle velocity langth


vect3 vVertexVelo[VERTEXES]; // vertexes velocity
float fVertexHeight[VERTEXES]; // vertexes height

vect3 vTorqueSum=vect3(0,0,0);
vect3 vForceSum=vect3(0,0,0);


 vMediumSurfN=vect3(0,1,0);

 vMediumSurfPos=-vMediumSurfN*(vPosition^vMediumSurfN); // boundary surface conversion to local coordinates system

      vMediumSurfN=vMediumSurfN.Rotate(float(-qRotation.w*M_RAD),qRotation.x,qRotation.y,qRotation.z);
      vMediumSurfPos=vMediumSurfPos.Rotate(float(-qRotation.w*M_RAD),qRotation.x,qRotation.y,qRotation.z);


//SET ELEVATOR POINTS POSITION
    vVertexPos[286]=vect3(fVertexes_data[286])+vect3(0.f,0.16f,0.f)*fControlParameter[1];
    vVertexPos[390]=vect3(fVertexes_data[390])+vect3(0.f,0.16f,0.f)*fControlParameter[1];
    vVertexPos[391]=vect3(fVertexes_data[391])+vect3(0.f,0.16f,0.f)*fControlParameter[1];
    vVertexPos[278]=vect3(fVertexes_data[278])+vect3(0.f,0.16f,0.f)*fControlParameter[1];
    vVertexPos[383]=vect3(fVertexes_data[383])+vect3(0.f,0.16f,0.f)*fControlParameter[1];
    vVertexPos[384]=vect3(fVertexes_data[384])+vect3(0.f,0.16f,0.f)*fControlParameter[1];

//SET RUDER POINTS POSITION
    vVertexPos[289]=vect3(fVertexes_data[289])+vect3(0.16f,0.f,0.f)*fControlParameter[2];
    vVertexPos[372]=vect3(fVertexes_data[372])+vect3(0.16f,0.f,0.f)*fControlParameter[2];
    vVertexPos[373]=vect3(fVertexes_data[373])+vect3(0.16f,0.f,0.f)*fControlParameter[2];


for (int i=0;i<30;i++)   // recalculation only morphed triangles ( where one or more points change position )
{
        int iTempTri=iTriangle_Morph[i];
        vTriangles_Surf[iTempTri]=-((vVertexPos[iTriangles[iTempTri][1]]-vVertexPos[iTriangles[iTempTri][0]])*
                                    (vVertexPos[iTriangles[iTempTri][2]]-vVertexPos[iTriangles[iTempTri][0]]))/2.0; // new surface vector

        vTriangles_Normal[iTempTri]=vTriangles_Surf[iTempTri].n();
        vTriangles_Center[iTempTri]=(vVertexPos[iTriangles[iTempTri][0]]+vVertexPos[iTriangles[iTempTri][1]]+vVertexPos[iTriangles[iTempTri][2]])/3.0; // new center of triangle
}


for (int i=0;i<VERTEXES;i++)
{ // calculate vertex height over medium boundary and vertex velocity
    fVertexHeight[i]=(vVertexPos[i]-vMediumSurfPos)^vMediumSurfN;
    vVertexVelo[i]=-(vVelLocal-vAVelLocal*vVertexPos[i]);

}

char cOverMedSurf; // how many triangle vertexes are over the medium boudary

for (int i=0;i<FACES;i++)
{

    cOverMedSurf=0;

    if (fVertexHeight[iTriangles_data[i][0]] >0) cOverMedSurf++;
    if (fVertexHeight[iTriangles_data[i][1]] >0) cOverMedSurf++;
    if (fVertexHeight[iTriangles_data[i][2]] >0) cOverMedSurf++;


//all triangle vertexes UNDERWATER
    if (cOverMedSurf==0)
    {
        fHeightTriangle=(fVertexHeight[iTriangles[i][0]]+fVertexHeight[iTriangles[i][1]]+fVertexHeight[iTriangles[i][2]])/3.0f; // calculate center of triangle under the medium boundary
        vTriVelo=(vVertexVelo[iTriangles[i][0]]+vVertexVelo[iTriangles[i][1]]+vVertexVelo[iTriangles[i][2]])/3.0f; // calculate center of triangle velocity
        fTriVeloLen=vTriVelo.l();
        vTriVeloN=vTriVelo/fTriVeloLen;

        fStatPressure=RO_WATER * 10.0f * fHeightTriangle;  // ro * g * h
        fDynaPressure=RO_WATER * (vTriangles_Normal[i]^vTriVeloN) * fTriVeloLen*fTriVeloLen/2.0f; // ro * cp * V^2 / 2.0


        vTriForceDyn=vTriangles_Surf[i]*fDynaPressure; // Force from dynamic pressure
        vTriForceDyn=vTriForceDyn*2.0f - vTriVeloN*(vTriVeloN^vTriForceDyn)*1.5f; //dynamic pressure correction

        vTriForce=vTriangles_Surf[i]*fStatPressure + vTriForceDyn;  // add together static and dynamic pressure force vectors

        vForceSum+=vTriForce; // sum all forces to one force on rigid body
        vTorqueSum+=vTriForce*vTriangles_Center[i]; //sum all torques to one torque on rigid body
    }

// all triangle vertexes in AIR
    if(cOverMedSurf==3)
    {
        fHeightTriangle=(fVertexHeight[iTriangles_data[i][0]]+fVertexHeight[iTriangles_data[i][1]]+fVertexHeight[iTriangles_data[i][2]])/3.0f;
        vTriVelo=(vVertexVelo[iTriangles[i][0]]+vVertexVelo[iTriangles[i][1]]+vVertexVelo[iTriangles[i][2]])/3.0f;
        fTriVeloLen=vTriVelo.l();
        vTriVeloN=vTriVelo/fTriVeloLen;

        fStatPressure=RO_AIR * 10.0f * fHeightTriangle;  // ro * g * h
        fDynaPressure=RO_AIR * (vTriangles_Normal[i]^vTriVeloN) * fTriVeloLen*fTriVeloLen/2.0f; // ro * cp * V^2 / 2.0


        vTriForceDyn=vTriangles_Surf[i]*fDynaPressure; // Force from dynamic pressure
        vTriForceDyn=vTriForceDyn*2.0f - vTriVeloN*(vTriVeloN^vTriForceDyn)*1.5f; //dynamic pressure correction

        vTriForce=vTriangles_Surf[i]*fStatPressure + vTriForceDyn; // add together static and dynamic pressure force vectors

        vForceSum+=vTriForce;// sum all forces to one force on rigid body
        vTorqueSum+=vTriForce*vTriangles_Center[i]; //sum all torques to one torque on rigid body
    }
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Triangle cuted by medium boundary ( triangle partly in air and water
    if ((cOverMedSurf==1)||(cOverMedSurf==2))
    {
        vect3 vTriA,vTriB,vTriC; // triangle points ( point A,B,C)
        vect3 vTriAB,vTriAC; // points on edges AB and AC ( edges points on medium boudary
        vect3 vAB,vAC; // AB and AC edge
        vect3 vTempTriCenter; // new triangle center
        vect3 vTempTriSurface; // new triangle surface
        char cTriPoints[3];
        int iTriPointSelect; // select single (alone) point on one side medium boundary


        if (fVertexHeight[iTriangles[i][0]]>0) cTriPoints[0]=1; else cTriPoints[0]=0;
        if (fVertexHeight[iTriangles[i][1]]>0) cTriPoints[1]=1; else cTriPoints[1]=0;
        if (fVertexHeight[iTriangles[i][2]]>0) cTriPoints[2]=1; else cTriPoints[2]=0;

        if (cTriPoints[0]==cTriPoints[1]) iTriPointSelect=2; // with point is alone on one side of boundary
        else
        {
            if (cTriPoints[0]==cTriPoints[2]) iTriPointSelect=1;
            else iTriPointSelect=0;
        }

        vTriA=vVertexPos[iTriangles[i][(iTriPointSelect  )%3]]; // create triangle ABC
        vTriB=vVertexPos[iTriangles[i][(iTriPointSelect+1)%3]];
        vTriC=vVertexPos[iTriangles[i][(iTriPointSelect+2)%3]];

        vAB=(vTriB-vTriA);
        vAC=(vTriC-vTriA);

        vTriAB=vTriA+vAB*((vMediumSurfPos-vTriA)^vMediumSurfN)/
                                (vAB^vMediumSurfN); // boundary clipping point on edge AB

        vTriAC=vTriA+vAC*((vMediumSurfPos-vTriA)^vMediumSurfN)/
                                (vAC^vMediumSurfN); // boundary clipping point on edge AC

        //          TRIANGLE             A - AB - AC
        vTempTriCenter=(vTriA+vTriAB+vTriAC)/3.0; // center of new triangle
        vTempTriSurface=-((vTriAB-vTriA)*(vTriAC-vTriA))/2.0; // surface of new triangle

        fHeightTriangle=(vTempTriCenter-vMediumSurfPos)^vMediumSurfN; // height center of new triangle
        vTriVelo=-(vVelLocal-vAVelLocal*vTempTriCenter); // velocity of new triangle
        fTriVeloLen=vTriVelo.l();
        vTriVeloN=vTriVelo/fTriVeloLen;

        if (cTriPoints[iTriPointSelect]==1) // new triangle is in air or in water
        {
                fStatPressure=RO_AIR * 10.0f * fHeightTriangle;  // ro * g * h
                fDynaPressure=RO_AIR * (vTriangles_Normal[i]^vTriVeloN) * fTriVeloLen*fTriVeloLen/2.0f; // ro * cp * V^2 / 2.0
        }                              // new triangle have the same normal like base triangle
        else
        {
                fStatPressure=RO_WATER * 10.0f * fHeightTriangle;  // ro * g * h
                fDynaPressure=RO_WATER * (vTriangles_Normal[i]^vTriVeloN) * fTriVeloLen*fTriVeloLen/2.0f; // ro * cp * V^2 / 2.0
        }


        vTriForceDyn=vTriangles_Surf[i]*fDynaPressure; // Force from dynamic pressure
        vTriForceDyn=vTriForceDyn*2.0f - vTriVeloN*(vTriVeloN^vTriForceDyn)*1.5; //dynamic pressure correction

        vTriForce=vTempTriSurface*fStatPressure + vTriForceDyn; // add together static and dynamic pressure force vectors
        vForceSum+=vTriForce;
        vTorqueSum+=vTriForce*vTempTriCenter; //

        //          TRIANGLE             B - AC - AB
        vTempTriCenter=(vTriB+vTriAC+vTriAB)/3.0f;  // center of new triangle
        vTempTriSurface=-((vTriAC-vTriB)*(vTriAB-vTriB))/2.0f; // surface of new triangle

        fHeightTriangle=(vTempTriCenter-vMediumSurfPos)^vMediumSurfN;  // height center of new triangle
        vTriVelo=-(vVelLocal-vAVelLocal*vTempTriCenter);  // velocity of new triangle
        fTriVeloLen=vTriVelo.l();
        vTriVeloN=vTriVelo/fTriVeloLen;

        if (cTriPoints[iTriPointSelect]==1) // if vTriA is in one medium vTriB and vTriC is in oposite side (oposite Medium)
        {
                fStatPressure=RO_WATER * 10.0f * fHeightTriangle;  // ro * g * h
                fDynaPressure=RO_WATER * (vTriangles_Normal[i]^vTriVeloN) * fTriVeloLen*fTriVeloLen/2.0f; // ro * cp * V^2 / 2.0
        }
        else
        {
                fStatPressure=RO_AIR * 10.0f * fHeightTriangle;  // ro * g * h
                fDynaPressure=RO_AIR * (vTriangles_Normal[i]^vTriVeloN) * fTriVeloLen*fTriVeloLen/2.0f; // ro * cp * V^2 / 2.0
        }


        vTriForceDyn=vTempTriSurface*fDynaPressure;
        vTriForceDyn=vTriForceDyn*2.0f - vTriVeloN*(vTriVeloN^vTriForceDyn)*1.5; //dynamic pressure correction

        vTriForce=vTempTriSurface*fStatPressure + vTriForceDyn; // add together static and dynamic pressure force vectors
        vForceSum+=vTriForce;
        vTorqueSum+=vTriForce*vTempTriCenter; //

        //          TRIANGLE             C - AC - B
        vTempTriCenter=(vTriC+vTriAC+vTriB)/3.0f;
        vTempTriSurface=-((vTriAC-vTriC)*(vTriB-vTriC))/2.0f;

        fHeightTriangle=(vTempTriCenter-vMediumSurfPos)^vMediumSurfN;
        vTriVelo=-(vVelLocal-vAVelLocal*vTempTriCenter); // velocity of new triangle
        fTriVeloLen=vTriVelo.l();
        vTriVeloN=vTriVelo/fTriVeloLen;

        if (cTriPoints[iTriPointSelect]==1) // if vTriA is in one medium vTriB and vTriC is in oposite side (oposite Medium)
        {
                fStatPressure=RO_WATER * 10.0f * fHeightTriangle;  // ro * g * h
                fDynaPressure=RO_WATER * (vTriangles_Normal[i]^vTriVeloN) * fTriVeloLen*fTriVeloLen/2.0f; // ro * cp * V^2 / 2.0
        }
        else
        {
                fStatPressure=RO_AIR * 10.0f * fHeightTriangle;  // ro * g * h
                fDynaPressure=RO_AIR * (vTriangles_Normal[i]^vTriVeloN) * fTriVeloLen*fTriVeloLen/2.0f; // ro * cp * V^2 / 2.0
        }


        vTriForceDyn=vTempTriSurface*fDynaPressure;
        vTriForceDyn=vTriForceDyn*2.0f - vTriVeloN*(vTriVeloN^vTriForceDyn)*1.5; //dynamic pressure correction

        vTriForce=vTempTriSurface*fStatPressure + vTriForceDyn; // Force on triangle surface
        vForceSum+=vTriForce;
        vTorqueSum+=vTriForce*vTempTriCenter; //

    } // end-> if ((cOverMedSurf==1)||(cOverMedSurf==2))


} // end-> for (int i=0;i<FACES;i++)


    // Add Engine Force;
    vForceSum+=vect3(0,0,-fControlParameter[0]*9);     // 11N

    // back to global coordinate system
    vTorque=vTorqueSum.Rotate(float(qRotation.w*M_RAD),qRotation.x,qRotation.y,qRotation.z);
    vForce=vForceSum.Rotate(float(qRotation.w*M_RAD),qRotation.x,qRotation.y,qRotation.z);
}
