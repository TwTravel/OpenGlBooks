/*
    Imagedit - Demo for Animating Relief Impostors using Radial Basis Functions Textures
    Copyright (C) 2007 Vitor Fernando Pamplona (vitor@vitorpamplona.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef RBFTEXTUREWRITER_H_
#define RBFTEXTUREWRITER_H_


/** 
 * 	Transforms an RBF class in a texture. 
 */
template <class C, int N>
class RBFTextureWriter {
public:
	List<RBF<C, N> * > * rbfs;

	float * shaderData;

	virtual ~RBFTextureWriter() {
		delete[] shaderData;
	}

	RBFTextureWriter(List<RBF<C, N> * > * _rbfs) {
		rbfs = _rbfs;
		shaderData = NULL;
	}
		
	int getBufferSize(int width, int heigth) {
		int tam = width > heigth ? width : heigth;
    	int temp = 2;
    	while (temp < tam) {
    		temp = temp * 2;
    	}
    	return temp;
	}
	
	Image<float> * execute() {
		RBF<C, N> * aux = rbfs->first();

		int mx = getBufferSize(aux->size(), rbfs->length());
		
		shaderData = new float[mx*mx*4];
		memset (shaderData, '\0',mx*mx*4*4);

		for (int time=0; time<rbfs->length(); time++ ) {
			
			aux->resetPoint();
			// Saving Shader Texture
			for (int i=0; i< aux->size(); i++) {
				// Matrix time X points
				//(mx-(time+1))
				aux->setFloat4(i, shaderData, i*4 + time*mx*4);
			
			} 

			aux = rbfs->next();
		}
		
		Image<float> * shader = new Image<float>(4, mx, mx,  IL_RGBA, shaderData);

		//ImageWriter().write("RBFTexture.tga", shaderData, mx, 16, IL_RGBA);
		
		return shader;
	}	
		
};


#endif /*RBFTEXTUREWRITER_H_*/

