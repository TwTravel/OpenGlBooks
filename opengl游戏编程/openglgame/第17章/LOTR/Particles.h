#ifndef _PARTICLES_H
#define _PARTICLES_H

#include "linear.h"

/** Á£×ÓÀà */
class particle
{
	public:
		vector pos;
		vector dir;
		vector color;
		float alpha;
		float time;
		float size;
		float roll;
		float r; //radial pos
		float theta;
		float fall;

		particle()
		{
			fall=0;  
			roll=0;
			size=1;
			time=0;
			alpha=1;
			color.set(1,1,1);
		}

		void Move(float timet) //in ms
		{
			pos=pos+dir*timet;
			time-=timet;
		}

		void Display(void)
		{
			glColor4f(color.x,color.y,color.z,alpha*time);
			float s=size/2;
			glTranslatef(pos.x,pos.y,pos.z);
			glRotatef(roll*time,0,0,1);
			glBegin(GL_QUADS);
				glTexCoord2f(1,0);
				glVertex3f(-s,-s,0);
				glTexCoord2f(1,1);
				glVertex3f(-s,s,0);
				glTexCoord2f(0,1);
				glVertex3f(s,s,0);
				glTexCoord2f(0,0);
				glVertex3f(s,-s,0);
			glEnd();
		
		}
};

#endif