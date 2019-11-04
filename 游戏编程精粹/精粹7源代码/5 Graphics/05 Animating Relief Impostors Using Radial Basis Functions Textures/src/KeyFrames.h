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

#ifndef KEYFRAMES_H_
#define KEYFRAMES_H_

/**
 * Holds a list of all KeyFrames seen in the top of application. 
 */
class KeyFrames {
	
public:
	typedef std::vector<ImageKeyFrame<char> *> IMAGE_LIST;
	typedef IMAGE_LIST::iterator IMAGE_LIST_ITERATOR;

	IMAGE_LIST imageList;

	KeyFrames() {
		
	}
	
	virtual ~KeyFrames() {
		clear(); 
	}
	
	void addImage(ImageKeyFrame<char> * _image) {
		imageList.push_back(_image);
	}
	

	void addPoint(float x, float y) {
		int cont=0;
		for (IMAGE_LIST_ITERATOR i = imageList.begin(); i<imageList.end(); i++) {
			(*i)->addPoint(x, y);
		    cont++;
		}	   
	}


	// X and Y between 0 and 1
	void updatePoint(int img, int point, float x, float y) {
		imageList[img]->updatePoint(point, x, y);
	}
	
	void updatePointAddingDelta(int img, int point, float x, float y) {
		imageList[img]->updatePointAddingDelta(point, x, y);
	}
	
	void updatePointRotating(int img, int point, float x, float y, float angle) {
		imageList[img]->updatePointRotating(point, x, y, angle);
	}	

	void clear() {
		for (IMAGE_LIST_ITERATOR i = imageList.begin(); i<imageList.end(); i++) {
			(*i)->deleteUserPoints();
		}	   
	}	
	
	bool hasPoints() {
		return (*imageList.begin())->points.size() > 0;
	}
	
};

#endif /*KEYFRAMES_H_*/
