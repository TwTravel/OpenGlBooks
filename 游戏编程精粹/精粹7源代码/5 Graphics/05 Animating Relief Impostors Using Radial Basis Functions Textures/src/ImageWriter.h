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

#ifndef IMAGEWRITER_H_
#define IMAGEWRITER_H_

class ImageWriter
{
public:
	ImageWriter() {}
	virtual ~ImageWriter() {}
	
	bool write(const char * _filename, char * _data, int _size, int bbp,  ILenum rgb) {
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

#ifdef _WIN32    	
    	bool b = (bool) ilSaveImage((const wchar_t *)_filename);
#else    	
    	bool b = (bool) ilSaveImage(_filename);
#endif	    
    		
    	return b;
	}
	
	bool write(const char * _filename, float * _data, int _size, int bbp,  ILenum format) {
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
    	ilTexImage(_size, _size, 1, bbp, format, IL_FLOAT, _data);
    	
    	ilEnable(IL_FILE_OVERWRITE);
    	
#ifdef _WIN32    	
    	bool b = (bool) ilSaveImage((const wchar_t *)_filename);
#else    	
    	bool b = (bool) ilSaveImage(_filename);
#endif	    
    	
    		
    	return b;
	}
	
	bool write(char * _filename, Image<float> * img) {
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
    	ilTexImage(img->width, img->height, 1, img->components*sizeof(float), img->format, IL_FLOAT, img->pixels);
    	
    	ilEnable(IL_FILE_OVERWRITE);
    	
#ifdef _WIN32    	
    	bool b = (bool) ilSaveImage((const wchar_t *)_filename);
#else    	
    	bool b = (bool) ilSaveImage(_filename);
#endif	    
    	
    		
    	return b;
	}	
};

#endif /*IMAGEWRITER_H_*/
