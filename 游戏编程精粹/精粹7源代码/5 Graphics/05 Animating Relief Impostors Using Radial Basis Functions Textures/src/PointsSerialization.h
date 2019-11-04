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

#ifndef POINTSSERIALIZATION_H_
#define POINTSSERIALIZATION_H_

class PointsSerialization {
	
	char * fileName;
	KeyFrames * frames;
	
public:
	PointsSerialization(char * _fileName, KeyFrames * _frames) {
		frames = _frames;
		fileName = _fileName;
	}
	virtual ~PointsSerialization() {}
	
	void save() {
		std::ofstream file(fileName);
		
		file << frames->imageList.size() << " " <<  (* frames->imageList.begin())->points.size() << std::endl;
		
		std::vector<Point>::iterator j;
		int cont = 0;
		
		for (KeyFrames::IMAGE_LIST_ITERATOR i = frames->imageList.begin(); i<frames->imageList.end(); i++) {			
			file << cont << std::endl;
			
			for (j = (*i)->points.begin(); j <  (*i)->points.end(); j++) {
				file << j->x << " " << j->y << std::endl;
			}
		    
		    cont++;
		}
	}
	
	void load() {
		frames->clear();
		
		std::ifstream file(fileName);
		
		int imageCount;
		int pointCount;
		int image;
		float x;
		float y;
		
		file >> imageCount;
		file >> pointCount; 
				
		for (int i = 0; i<imageCount; i++) {
			int cont = 0; 

			file >> image;
			
			for (int j = 0; j<pointCount; j++) {
				file >> x;
				file >> y;
				if (image == 0)
					frames->addPoint(x, y);
				else
					frames->updatePoint(image, j, x, y);
			}
		    
		    cont ++ ;
		}
	}
};

#endif /*POINTSSERIALIZATION_H_*/
