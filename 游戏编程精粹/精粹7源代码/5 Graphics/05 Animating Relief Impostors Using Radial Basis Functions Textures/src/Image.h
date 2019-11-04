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

#ifndef IMAGE_H_
#define IMAGE_H_

/**
 * Class to handle an Image.
 * Loader and Writer are in other classes.
 */
template<class T>
class Image  {
	
public:
	GLuint name;
	GLint components;
    GLsizei width;
    GLsizei height;
    GLenum format;
    
    T * pixels;	

	Image() {
          components = 0;
          width = 0;
          height = 0;
          format = 0;
          pixels = NULL;	
    }
        
    virtual ~Image() {
    	if (pixels != NULL)
    		delete[] pixels;
    }    
        
	Image(GLint _components,
          GLsizei _width,
          GLsizei _height,
          GLenum _format,
          const T * _pixels) {
          
          components = _components;
          width = _width;
          height = _height;
          format = _format;
          
          pixels = new T[width*height*components*sizeof(T)];
          memcpy(pixels, _pixels, width*height*components*sizeof(T));
    }
	
	void invert() {
        T * temp = new T[width*height*components*sizeof(T)];
        memcpy(temp, pixels, width*height*components*sizeof(T));
        
        for (int x=0; x<width; x++ ) {
			for (int y=0; y < height; y++ ) {
				for (int comp=0; comp < components; comp++ ) {
					pixels[x*components + ((height - (y+1))*width*components) + comp] = temp[x*components + y*width*components + comp];
				}
			}
        }
        
        delete [] temp;
	}
};

#endif /*IMAGE_H_*/

