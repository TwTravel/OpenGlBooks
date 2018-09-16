
#ifndef _MESH__H
#define _MESH__H

#include "physic.h"

#define RO_AIR  1.25f
#define RO_WATER  1000.f


extern float fControlParameter[3]; // float parameters to model control


struct Mesh : Physic
{

void CalcPressure(void);
void Render(char);

     Mesh();

};


extern Mesh *Airship;

#endif
