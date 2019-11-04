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

#ifndef IMAGELOADER_H_
#define IMAGELOADER_H_


/**
 * Loads an image using Devil Library
 * Can load RGB (JPG) or RGBA (TGA) images 
 */
class ImageLoader {
	
public:
	bool valid;

	ImageLoader() {
		valid = true;
		/* Before calling ilInit() version should be checked. */
  		if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)  {
	    	/* wrong DevIL version */
    		valid = false;
  		}
  		
  		ilInit();
	}
	
	Image<char> * loadRBG(const char * filename) {
		if (!valid) return NULL;
		
		 /* ILuint is a 32bit unsigned integer. Variable texid will be used to store image name. */
	  	ILuint texid; 
	  	ilGenImages(1, &texid); /* Generation of one image name */
	  	ilBindImage(texid); /* Binding of image name */
	  	
	  	/* ILboolean is type similar to GLboolean and can equal GL_FALSE (0) or GL_TRUE (1)
	    it can have different value (because it's just typedef of unsigned char), but this sould be
	    avoided.
	    Variable success will be used to determine if some function returned success or failure. */  	
	  	ILboolean success;

#ifdef _WIN32
	  	success = ilLoadImage((wchar_t*) filename); /* Loading of image "image.jpg" */
#else
		success = ilLoadImage(filename); /* Loading of image "image.jpg" */
#endif	  	
	  	
	  	if (!success) {
	  		std::cout << "Image not found: " << filename << std::endl;
	  		return NULL;
	  	}
			
		/* Convert every colour component into unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */  		
	    if (!ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE)) {
		   	return NULL;
	    }
	    	
		Image<char> * ret = new Image<char>(ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
		   				      ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT),  
		   				      (char *) ilGetData());      				      
		      				      
		ilDeleteImages(1, &texid); /* Because we have already copied image data into texture data we can release memory used by image. */
		   
		return ret;  				          	
	}
	
	Image<char> * loadRBGA(const char * filename) {
		if (!valid) return NULL;
		
		 /* ILuint is a 32bit unsigned integer. Variable texid will be used to store image name. */
	  	ILuint texid; 
	  	ilGenImages(1, &texid); /* Generation of one image name */
	  	ilBindImage(texid); /* Binding of image name */
	  	
	  	/* ILboolean is type similar to GLboolean and can equal GL_FALSE (0) or GL_TRUE (1)
	    it can have different value (because it's just typedef of unsigned char), but this sould be
	    avoided.
	    Variable success will be used to determine if some function returned success or failure. */  	
	  	ILboolean success;
	  	
#ifdef _WIN32	  	
	  	success = ilLoadImage((wchar_t*) filename); /* Loading of image "image.jpg" */
#else
		success = ilLoadImage(filename); /* Loading of image "image.jpg" */
#endif	  	
	  	
	  	if (!success) {
	  		std::cout << "Image not found: " << filename << std::endl;
	  		return NULL;
	  	}
			
		/* Convert every colour component into unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */  		
	    if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
		   	return NULL;
	    }
	    	
		Image<char> * ret = new Image<char>(ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
		   				      ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT),  
		   				      (char *) ilGetData());      				      
		      				      
		ilDeleteImages(1, &texid); /* Because we have already copied image data into texture data we can release memory used by image. */
		   
		return ret;  				          	
	}	
};

#endif /*IMAGELOADER_H_*/
