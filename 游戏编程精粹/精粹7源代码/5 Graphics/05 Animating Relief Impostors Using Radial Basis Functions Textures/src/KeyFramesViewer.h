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

#ifndef KEYFRAMESVIEWER_H_
#define KEYFRAMESVIEWER_H_

/**
 * A Viewer and interaction component to KeyFrames.
 * 
 * Draw a sequence of images in the top of a Window
 */
class KeyFramesViewer {	
	int img;		
	Vector2f rotationPoint;
	Vector2f fromInImage;
	std::vector<int> selectedPoints;
	
	bool mouseLeft;
	
public:
	KeyFrames frames;

	KeyFramesViewer() {
	}
	virtual ~KeyFramesViewer() {}
	
	ImageKeyFrame<char> * addImage(Image<char> * img) {
		ImageKeyFrame<char> * imgKeyFrame = new ImageKeyFrame<char>(img);
		frames.addImage(imgKeyFrame);
		return imgKeyFrame;
	}
	
	int getImageFromPosition(int x) {
		return x / 200;
	}

	 // Relative to image coordinates
	float fromScreenToImageX(int img, int x) {
		// REmove bordas de tela
		x = x - (img*200+10);
		// reescala 
		return x/190.0f;	
	}

	 // Relative to image coordinates
	int fromImageToScreenX(int img, float x) {
		x = x*190.0f;
		return (int) x + (img*200+10);
	}	
	
	 // Relative to image coordinates
	float fromScreenToImageY(int img, int y) {
		// remove bordas
		y = y - 10;
		// escala
		return y/190.0f;
	}
	
	 // Relative to image coordinates
	int fromImageToScreenY(int img, float y) {
		y = y*190.0f;
		return (int)y + 10;
	}	
	
	int getPointAround(int img, int x, int y) {
		std::vector<Point>::iterator i;
		int j=0;
		for (i = frames.imageList[img]->points.begin(); i< frames.imageList[img]->points.end(); i++) {
			if (abs(fromImageToScreenX(img, i->x) - x) < 5 
			&&  abs(fromImageToScreenY(img, i->y) - y) < 5) {
				return j;
			}
			j++;
		}
		
		return -1;
	}		
	
	void setRotationPoint(float x, float y) {
		rotationPoint = Vector2f(x,y);
	}	
	
	bool isSelected(int point) {
		std::vector<int>::iterator  i;
		for (i=selectedPoints.begin();i!=selectedPoints.end(); i++) {
			if (point == *i) return true;
		}
		return false;
	}
	
	void addSelectedPoint(int point) {
		if (!isSelected(point))
			selectedPoints.push_back(point);
	}
	
	void clearPoints() {
		selectedPoints.clear();
	}
	
	bool mouseRightPressed(int x, int y, bool shift, bool ctrl, bool alt) {
		mouseLeft = false;

		img = getImageFromPosition(x);
		fromInImage.set(0, fromScreenToImageX(img, x));
   		fromInImage.set(1, fromScreenToImageY(img, y));

		if (ctrl) {
			setRotationPoint(fromInImage[0], fromInImage[1]);
			return true;
		}
		return false;
	}
	
	// Returns if it was consumed
	bool mousePressed(int x, int y, bool shift, bool ctrl, bool alt) {
		mouseLeft = true;
		
		// Put or move a point in the keyframes
		img = getImageFromPosition(x);

   		fromInImage.set(0, fromScreenToImageX(img, x));
   		fromInImage.set(1, fromScreenToImageY(img, y));

		if ((unsigned int)img >= frames.imageList.size()) {
			img = -1;
			return false;
		}

		if (ctrl) {
			return true;
		}

   		int moving = getPointAround(img, x, y);
   		
   		if (!shift) {
			clearPoints();
		}
   		
		if (moving >= 0 ) {
			addSelectedPoint(moving);
			return true;
		} 
		
		// new point
   		if (moving < 0 && x >10 && x <= 200 && y > 10 && y <= 200) {
   			frames.addPoint(fromInImage[0],fromInImage[1]);
   			
			moving = getPointAround(img, x, y);
			addSelectedPoint(moving);
			
			return true;
   		}    		
   		return false;	
	}
	
	// Returns if it was consumed
	bool mouseReleased(int x, int y) {
		mouseLeft = false;
		img = -1;
		return false;
	}
	
	// Returns if it was consumed
	bool mouseMotion(int x, int y) {
		if (img < 0) return false;
		if (y > 200) return false;

		if (mouseLeft) {
			float deltaX = fromScreenToImageX(img, x) - fromInImage[0];
			float deltaY = fromScreenToImageY(img, y) - fromInImage[1];
			
			for (unsigned int i=0; i<selectedPoints.size(); i++) {
				frames.updatePointAddingDelta(img, selectedPoints[i], deltaX, deltaY);
			}
		} else {
			Vector2f toInImage;
			toInImage.set(0, fromScreenToImageX(img, x));
   			toInImage.set(1, fromScreenToImageY(img, y));
			
			Vector2f from = fromInImage - rotationPoint;
			Vector2f to = toInImage - rotationPoint;

			if (from.isZero() || to.isZero()) return false;

			double angle = from.angleInRadians(to);
			
			if (to.crossProduct(from).sumAll() > 0) {
				angle *= -1;
			}			
			
			for (unsigned int i=0; i<selectedPoints.size(); i++) {
				frames.updatePointRotating(img, selectedPoints[i], rotationPoint[0], rotationPoint[1], angle);
			}			
		}
	   	
	   	fromInImage.set(0, fromScreenToImageX(img, x));
   		fromInImage.set(1, fromScreenToImageY(img, y));				
		
		return true;
	}
	
	void clear() {
		frames.clear();
	}
	
	void gridRegular() {
		frames.clear();
		
		for (int i = 0; i<=3; i++) {
			for (int j = 0; j<=3; j++) {
				frames.addPoint(i/4.0f + 0.15, j/4.0f + 0.15);
			}
		}
	}
	
	bool hasPoints() {
		return frames.hasPoints();
	}
	
	void display() {
		int cont = 0; 
		for (KeyFrames::IMAGE_LIST_ITERATOR i = frames.imageList.begin(); i<frames.imageList.end(); i++) {
			renderKeyFrame(cont, *(*i));
		    
		    cont ++ ;
		}
	}
	
	void renderKeyFrame(int key, ImageKeyFrame<char> & image) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,  image.name);
	
		/* Draw a quad with the default texture */
    	
		glBegin(GL_QUADS);
    	glTexCoord2i(0, 1);  glVertex2i(10+(key*200),   10);
    	glTexCoord2i(0, 0);  glVertex2i(10+(key*200),  200);
    	glTexCoord2i(1, 0);  glVertex2i(200+(key*200), 200);
	    glTexCoord2i(1, 1);  glVertex2i(200+(key*200),  10);
	    glEnd();
	    
	    glDisable(GL_TEXTURE_2D);
	    
		glPointSize(3.0);
		
		glColor4f(0.0f, 0.0f, 1.0f, 1.0);
		glDisable(GL_BLEND);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);      

		std::vector<Point>::iterator j;
    
    	// extrapoints
	    if (image.extraPoints.size() > 0) {
	   		glColor4d(1.0f, 0.0f, 0.0f, 1.0f);
			glPointSize(3.0);
			glBegin(GL_POINTS);
		
			glColor3f(1.0f, 1.0f, 1.0f);
			for (j = image.extraPoints.begin(); j <  image.extraPoints.end(); j++) {
				glVertex2i(fromImageToScreenX(key, j->x), fromImageToScreenY(key, j->y));
			}
	    	glEnd();
	    }	
	    
	    // normal points
		glColor4f(0.0f, 0.0f, 1.0f, 1.0);
		glPointSize(4.0);
		glBegin(GL_POINTS);
		int pointIdx = 0;
		for (j = image.points.begin(); j <  image.points.end(); j++) {
			if (isSelected(pointIdx++)) 
				glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			else
				glColor4f(0.0f, 0.0f, 1.0f, 1.0);
			
			glVertex2i(fromImageToScreenX(key, j->x), fromImageToScreenY(key, j->y));
		}

		// rotation point
		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		glVertex2i(fromImageToScreenX(key, rotationPoint[0]), fromImageToScreenY(key, rotationPoint[1]));

		glEnd();

	    glColor3d(1.0f, 1.0f, 1.0f);
	}
};

#endif /*IMAGEVIEWER_H_*/
