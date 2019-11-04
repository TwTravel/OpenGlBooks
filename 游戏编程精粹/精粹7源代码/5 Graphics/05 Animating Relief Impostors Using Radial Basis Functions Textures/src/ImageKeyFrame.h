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

#ifndef IMAGE_KEY_FRAME_H_
#define IMAGE_KEY_FRAME_H_



struct Point {
	// x,y between 0 and 1
	float x;
	float y;
};

/**
 * Class to handle each key frame seen in the top of the demo.
 * Composed by and image and control points.
 */ 
template<class T>
class ImageKeyFrame : public Image<T> {
	
public:
	std::vector<Point> extraPoints;
    std::vector<Point> points;

	ImageKeyFrame() : Image<T>() {
		createQuadPoints();          
    }
        
    virtual ~ImageKeyFrame(){
    	points.clear();
    	extraPoints.clear();
    }    
        
	ImageKeyFrame(GLint _components,
          GLsizei _width,
          GLsizei _height,
          GLenum _format,
          const T * _pixels) : Image<T>(_components, _width, _height, _format, _pixels)  {
          
          createQuadPoints();
    }
    
	ImageKeyFrame(Image<T> * img) : Image<T>(img->components, img->width, img->height, img->format, img->pixels)  {
          createQuadPoints();
    }    
	
	void addPoint(float x, float y) {
		Point p;
		p.x = x;
		p.y = y;
		points.push_back(p);
	}
	
	void updatePoint(int index, float x, float y) {
		points[index].x = x;
		points[index].y = y;
	}
	
	void updatePointAddingDelta(int index, float x, float y) {
		points[index].x += x;
		points[index].y += y;
	}		
	
	void updatePointRotating(int index, float x, float y, float angle) {
		Quaternion q;
		q.setVector(points[index].x - x, points[index].y - y, 0);
		Quaternion r;
		r.setRotor(angle, 0,0,1);
		q = q.rotate(r);
		
		points[index].x = q.x + x;
		points[index].y = q.y + y;
	}

	void createQuadPoints() {
		Point p;
		
		p.x =  0;
		p.y =  0;
		extraPoints.push_back(p);

		p.x =  1;
		p.y =  0;
		extraPoints.push_back(p);
		
		p.x =  1;
		p.y =  1;
		extraPoints.push_back(p);

		p.x =  0;
		p.y =  1;
		extraPoints.push_back(p);		
	}

	void deleteUserPoints() {
		points.clear();
	}

	void deleteAllPoints() {
		points.clear();
		extraPoints.clear();
	}
};

#endif /*IMAGE_H_*/

