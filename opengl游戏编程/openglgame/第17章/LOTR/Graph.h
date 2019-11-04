#ifndef _GRAPH_H
#define _GRAPH_H

#include "windows.h"
#include "GL\gl.h"
#include "GL\glu.h"

float random(float center, float range, int doublesided=1);
unsigned char* GetBitmap(char bmpfile[],int &sizex,int &sizey);
unsigned char* GetJPEG(char bmpfile[],int &sizex,int &sizey);

/** 纹理类 */
class CTexture
{
public:
	GLuint* textures;
	int loaded;
	int number;

	CTexture()
	{
		loaded=0;
		number=0;
	}
    
	/** 初始化 */
	void Init(int num)
	{
		number=num;
		textures=new GLuint[number];
		glGenTextures(number,textures);

	}
	void Bind(int num)
	{
		glBindTexture(GL_TEXTURE_2D,textures[num]);
	}

	/**　载入纹理 */
	void Load(char * List,int num,int masked=0)
	{
		
		if(number!=0)
		{
			
				glBindTexture(GL_TEXTURE_2D,textures[num]);
				glPixelStorei(GL_UNPACK_ALIGNMENT,1);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

				int numpx,numpy;
				unsigned char* bmp=GetJPEG(List,numpx,numpy);
				
				if(bmp!=0)
				{
					if(!masked)
					{
						glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, numpx, numpy, 0, GL_RGB, GL_UNSIGNED_BYTE, bmp);
						delete [] bmp;
						loaded=1;
					}
					else
					{
						unsigned char* mbmp=new unsigned char[numpx*numpy*4];
						if(mbmp!=0)
						{
							for(long b=0;b<numpx*numpy;b++)
							{
								mbmp[4*b]=bmp[3*b+2];
								mbmp[4*b+1]=bmp[3*b+1];
								mbmp[4*b+2]=bmp[3*b+0];
								if(mbmp[4*b]==0&&mbmp[4*b+1]==0&&mbmp[4*b+2]==0)
									mbmp[4*b+3]=0;
								else
									mbmp[4*b+3]=255;
							}

							glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, numpx, numpy, 0, GL_RGBA, GL_UNSIGNED_BYTE, mbmp);
							delete [] bmp;
							delete [] mbmp;

							loaded=1;
						}
					}
				}
				else loaded=0;
		}
		
	}

	void UnLoad()
	{
		if(loaded)
			glDeleteTextures(number,textures);
		loaded=0;
	}

};

/**********************************************
*CSpotLights
**********************************************
*Defines a spotlight (Not used in Whiplash)
**********************************************/
class CSpotLights
{
	public:
		int light_index;
		float x,y,z;
		float r,g,b;
		float cont_a;
		float lin_a;

		CSpotLights()
		{
			light_index=0;
			x=y=z=r=g=b=cont_a=lin_a=0;

		}

		~CSpotLights()
		{

		}

		void Set(int index,float xt,float yt,float zt, float rt, float gt, float bt, float ca,float la)
		{
			light_index=index;
			x=xt;
			y=yt;
			z=zt;
			r=rt;
			g=gt;
			b=bt;
			cont_a=ca;
			lin_a=la;
		}

		void Display()
		{
			GLfloat SpotAmbient[]    = { r/10, g/10, b/10, 1.0f };
			GLfloat SpotDiffuse[]    = { r, g, b, 1.0f };
			GLfloat SpotSpecular[]    = { r, g, b, 1.0f };
			GLfloat SpotPosition[]    = { x, y, z, 1.0f };
			
			glLightf(GL_LIGHT0+light_index, GL_SPOT_CUTOFF ,180);
			glLightf(GL_LIGHT0+light_index, GL_CONSTANT_ATTENUATION  ,cont_a);
			glLightf(GL_LIGHT0+light_index, GL_LINEAR_ATTENUATION ,lin_a);
		
	 	    glLightfv(GL_LIGHT0+light_index, GL_AMBIENT, SpotAmbient);
			glLightfv(GL_LIGHT0+light_index, GL_DIFFUSE, SpotDiffuse);
			glLightfv(GL_LIGHT0+light_index, GL_SPECULAR, SpotSpecular);
			glLightfv(GL_LIGHT0+light_index, GL_POSITION, SpotPosition);
		}

		void Enable()
		{
			glEnable(GL_LIGHT0+light_index);
		}

		void Disable()
		{
			glDisable(GL_LIGHT0+light_index);
		}
};
#endif