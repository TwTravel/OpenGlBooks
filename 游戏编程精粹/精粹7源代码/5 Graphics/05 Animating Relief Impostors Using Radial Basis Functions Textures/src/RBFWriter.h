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

#ifndef RBFWRITER_H_
#define RBFWRITER_H_

template <class C, int N>
class RBFWriter {
public:
	char * filename;
	int size;
	List<RBF<C, N> * > * rbfs;

	char * data;
	char * basedData;

	float * shaderData;
	
	
	virtual ~RBFWriter() {
		delete[] shaderData;
		delete[] data;
	}

	RBFWriter(char * _filename, int _size, List<RBF<C, N> * > * _rbfs, char * _data) {
		filename = _filename;
		rbfs = _rbfs;
		data = NULL;
		shaderData = NULL;
		size = _size;
		basedData = (char *) _data;
		
		//write("basedData.png", basedData, size, 3, IL_RGB);
	}
	
	void createBuffer() {
		if (data != NULL) {
			delete[] data;
		}
		
		data = new char[size*size*3];
		memset (data,'\0',size*size*3);
	}
	
	// lin: number of the line
	// col: number of the column
	// rgb: number of the color component 
	//      0 - RED 
	//      1 - GREEN 
	//      2 - BLUE
	inline int getPositionOnBuffer(int x, int y, int rgb) {
		return y*size*3 + x*3+rgb;
	} 
	
	int getBufferSize(int width, int heigth) {
		int tam = width > heigth ? width : heigth;
    	int temp = 2;
    	while (temp < tam) {
    		temp = temp * 2;
    	}
    	return temp;
	}
	
	Image<float> * execute(List<Image<char> *> * images) {
		RBF<C, N> * aux = rbfs->first();

		int mx = getBufferSize(aux->size(), rbfs->length());
		
		shaderData = new float[mx*mx*4];
		memset (shaderData, '\0',mx*mx*4*4);

		Vector<C,N> pos;
		Vector<C,N> ret;
		double xEval, yEval;

		for (int time=0; time<rbfs->length(); time++ ) {
			createBuffer();
			
			for (int x=0; x<size; x++ ) {
				for (int y=0; y < size; y++ ) {
					pos.data[0] = ((C)x)/size;
					pos.data[1] = ((C)y)/size;
					aux->eval(pos, &ret);

					xEval = ret.data[0];
					yEval = ret.data[1];
					
					if (xEval > 1) xEval = 1;
					if (yEval > 1) yEval = 1;
					if (xEval < 0) xEval = 0;
					if (yEval < 0) yEval = 0;
					
					//std::cout << (int)(xEval*size) << " - " <<  (int)(yEval*size) << std::endl;
					
					data[getPositionOnBuffer(x,size-y,0)] = basedData[getPositionOnBuffer((int)(xEval*size), size-(int)(yEval*size), 0)]; //Red
					data[getPositionOnBuffer(x,size-y,1)] = basedData[getPositionOnBuffer((int)(xEval*size), size-(int)(yEval*size), 1)]; //Green
					data[getPositionOnBuffer(x,size-y,2)] = basedData[getPositionOnBuffer((int)(xEval*size), size-(int)(yEval*size), 2)]; //Blue
				}
			}
			std::cout << time << std::endl;
			std::stringstream st;
			st << filename << time << ".png";
			write(const_cast<char *> (st.str().c_str()), data,  size, 3, IL_RGB);
						
			if (images != NULL)	{				
				Image<char> * img = new Image<char>(3, size, size,  IL_RGB, data);
				images->append(img);
			}
			
			aux->resetPoint();
			// Saving Shader Texture
			for (int i=0; i< aux->size(); i++) {
				// Matrix time X points
				//(mx-(time+1))
				aux->setFloat4(i, shaderData, i*4 + time*mx*4);
			
			} 

			aux = rbfs->next();
		}
		
		write("shaderTex.tga", shaderData, mx, 4, IL_RGBA);
		
		Image<float> * shader = new Image<float>(4, mx, mx,  IL_RGBA, shaderData);
		
		return shader;
	}
		
	bool write(char * _filename, char * _data, int _size, int bbp,  ILenum rgb) {
		ILuint imageName; // The image name to return.
    	ilGenImages(1, &imageName); // Grab a new image name.

    	ilBindImage(imageName);
    	
/*
		Width:  Specifies the new image width. This cannot be 0.
		Height: Specifies the new image height. This cannot be 0.
		Depth:  Specifies the new image depth. Anything greater than 1 will make the image 3d. This cannot be 0.
		Bpp:    Specifies the new bytes per pixel (not bits per pixel). Common values are 3 and 4.
		Format: Specifies the data format this image has. For a list of values this can be, see the See Also section.
		Type:   Specifies the data format this image has. For a list of values this can be, see the See Also section.
		Data:   Specifies data that should be copied to the new image. If this parameter is NULL, no data is copied, 
						and the new image data consists of undefined values.
    */ 
    	ilTexImage(_size, _size, 1, bbp, rgb , IL_UNSIGNED_BYTE, _data);
    	
    	ilEnable(IL_FILE_OVERWRITE);
    	
    	bool b =  ilSaveImage(_filename);
    	
    		
    	return b;
	}
	
	bool write(char * _filename, float * _data, int _size, int bbp,  ILenum rgb) {
		ILuint imageName; // The image name to return.
    	ilGenImages(1, &imageName); // Grab a new image name.

    	ilBindImage(imageName);
    	
/*
		Width:  Specifies the new image width. This cannot be 0.
		Height: Specifies the new image height. This cannot be 0.
		Depth:  Specifies the new image depth. Anything greater than 1 will make the image 3d. This cannot be 0.
		Bpp:    Specifies the new bytes per pixel (not bits per pixel). Common values are 3 and 4.
		Format: Specifies the data format this image has. For a list of values this can be, see the See Also section.
		Type:   Specifies the data format this image has. For a list of values this can be, see the See Also section.
		Data:   Specifies data that should be copied to the new image. If this parameter is NULL, no data is copied, 
						and the new image data consists of undefined values.
    */ 
    	ilTexImage(_size, _size, 1, bbp, rgb ,  IL_FLOAT, _data);
    	
    	ilEnable(IL_FILE_OVERWRITE);
    	
    	bool b =  ilSaveImage(_filename);
    	
    		
    	return b;
	}	
		
};

#endif /*KEYFRAMEWRITER_H_*/
