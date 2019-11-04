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

#ifndef HERMITE_H_
#define HERMITE_H_

/**
 * Builds and hermite curve between each pair of points for each frame. 
 */
class Hermite {
	
	KeyFrames * frames;
	int steps;
	Animation<double, 2> * animation;
	
public:
	Hermite(KeyFrames * _frames, int _steps, Animation<double, 2> * _animation) {
		frames = _frames;
		steps = _steps;
		animation = _animation;
	}
	
	virtual ~Hermite(){}
	
	/** 
	 * Hermite basis functions
	 * 
	 * 
	 * Hermite Algorithm 

		moveto (P1);                            // move pen to startpoint
		for (int t=0; t < steps; t++) {
  			float s = (float)t / (float)steps;    // scale s to go from 0 to 1
  			float h1 =  2s^3 - 3s^2 + 1;          // calculate basis function 1
  			float h2 = -2s^3 + 3s^2;              // calculate basis function 2
  			float h3 =   s^3 - 2*s^2 + s;         // calculate basis function 3
  			float h4 =   s^3 -  s^2;              // calculate basis function 4
  			vector p = h1*P1 +                    // multiply and sum all funtions
             		   h2*P2 +                    // together to build the interpolated
                       h3*T1 +                    // point along the curve.
                       h4*T2;
  			lineto (p)                            // draw to calculated point on the curve
		}

	 */
	
	double h1(float time) {
		return 2*time*time*time - 3*time*time + 1;
	}
	
	double h2(float time) {
		return -2*time*time*time + 3*time*time;
	}
	
	double h3(float time) {
		return time*time*time - 2*time*time + time;
	}
	
	double h4(float time) {
		return time*time*time - time*time;
	}
	
	void execute() {
		// Para cada imagem
		Vector<double, 2> P_1;
		Vector<double, 2> P0;
		Vector<double, 2> P1;
		Vector<double, 2> P2;
		Vector<double, 2> T0;
		Vector<double, 2> T1;
		Vector<double, 2> PFinal;
		
		KeyFrames::IMAGE_LIST_ITERATOR img = frames->imageList.begin();
		int imgCount = 0;
		for (KeyFrames::IMAGE_LIST_ITERATOR next = frames->imageList.begin()+1; next < frames->imageList.end(); next++) {
			// Para cada par de pontos nas duas imagens
			std::vector<Point>::iterator p_1;
			
			if (img == frames->imageList.begin()) {
				 p_1 = (*img)->points.begin();
			} else {
				 p_1 = (*(img-1))->points.begin();
			} 
			
			std::vector<Point>::iterator p0 = (*img)->points.begin();
			std::vector<Point>::iterator p1 = (*next)->points.begin();
			
			std::vector<Point>::iterator p2;
			
			if (next == frames->imageList.end()-1) {
				 p2 = (*next)->points.begin();
			} else {
				 p2 = (*(next+1))->points.begin();
			} 
			
			// Sempre ser�o os pontos do primeiro keyframe. 
			std::vector<Point>::iterator pTo = (*frames->imageList.begin())->points.begin();
			for (;p0 < (*img)->points.end();) {

				PFinal = Vector<double, 2>(pTo->x, 1-pTo->y);

				P_1.data[0] = p0->x;
				P_1.data[1] = 1-p0->y;

				P0.data[0] = p0->x;
				P0.data[1] = 1-p0->y;
				
				P1.data[0] = p1->x;
				P1.data[1] = 1-p1->y;
				
				P2.data[0] = p2->x;
				P2.data[1] = 1-p2->y;				
				
				T0 = (P1 - P_1) * 0.5;
				T1 = (P2 - P0) * 0.5;
				
				animation->positionInRBF(imgCount*steps);
				
				// Para cada est�gio entre os pontos. 
				for (int t=0; t < steps; t++) {
	  				float s = (float)t / (float)steps;    // scale s to go from 0 to 1
	  				Vector<double,2> p = P0 * h1(s) +  // multiply and sum all funtions
	             		   			     P1 * h2(s) +  // together to build the interpolated
	                       			     T0 * h3(s) +  // point along the curve.
			                             T1 * h4(s);
			                             
			        animation->addPointInRBF(p, PFinal);
			        animation->nextRBF();
				}
				
				p0++;
				p1++;
				p_1++;
				p2++;
				pTo++;
			}
			
			// Extra Points
			
			if ((*frames->imageList.begin())->extraPoints.size() > 0) {
			
				if (img == frames->imageList.begin()) {
					 p_1 = (*img)->extraPoints.begin();
				} else {
					 p_1 = (*(img-1))->extraPoints.begin();
				} 
				
				p0 = (*img)->extraPoints.begin();
				p1 = (*next)->extraPoints.begin();
				
				if (next == frames->imageList.end()-1) {
					 p2 = (*next)->extraPoints.begin();
				} else {
					 p2 = (*(next+1))->extraPoints.begin();
				} 
				
				// Sempre ser�o os pontos do primeiro keyframe. 
				pTo = (*frames->imageList.begin())->extraPoints.begin();
				for (;p0 < (*img)->extraPoints.end();) {
	
					PFinal = Vector<double, 2>(pTo->x, 1-pTo->y);
	
					P_1.data[0] = p0->x;
					P_1.data[1] = 1-p0->y;
	
					P0.data[0] = p0->x;
					P0.data[1] = 1-p0->y;
					
					P1.data[0] = p1->x;
					P1.data[1] = 1-p1->y;
					
					P2.data[0] = p2->x;
					P2.data[1] = 1-p2->y;				
					
					T0 = (P1 - P_1) * 0.5;
					T1 = (P2 - P0) * 0.5;
					
					animation->positionInRBF(imgCount*steps);
					
					// Para cada est�gio entre os pontos. 
					for (int t=0; t < steps; t++) {
		  				float s = (float)t / (float)steps;    // scale s to go from 0 to 1
		  				Vector<double,2> p = P0 * h1(s) +  // multiply and sum all funtions
		             		   			     P1 * h2(s) +  // together to build the interpolated
		                       			     T0 * h3(s) +  // point along the curve.
				                             T1 * h4(s);	
				                             
				        animation->addPointInRBF(p, PFinal);
				        animation->nextRBF();
					}
					
					p0++;
					p1++;
					p_1++;
					p2++;
					pTo++;
				}			
			}
				
			img ++;
			imgCount++;
		}
				
		// Add last RBF
		animation->positionInRBF((int) ((frames->imageList.size()-1) * steps) );
		
		std::vector<Point>::iterator pTo = (*frames->imageList.begin())->points.begin();
		std::vector<Point>::iterator pFrom = (*(frames->imageList.end()-1))->points.begin();
				
		for (; pTo < (*frames->imageList.begin())->points.end(); ) {
			P0.data[0] = pFrom->x;
			P0.data[1] = 1-pFrom->y;
			
			P1.data[0] = pTo->x;
			P1.data[1] = 1-pTo->y;
			animation->addPointInRBF(P0, P1);
			
			pTo++;
			pFrom++;
		}
		
		// Add last RBF
		animation->positionInRBF((int)((frames->imageList.size()-1) * steps) );
		
		pTo = (*frames->imageList.begin())->extraPoints.begin();
		pFrom = (*(frames->imageList.end()-1))->extraPoints.begin();
				
		for (; pTo < (*frames->imageList.begin())->extraPoints.end(); ) {
			P0.data[0] = pFrom->x;
			P0.data[1] = 1-pFrom->y;
			
			P1.data[0] = pTo->x;
			P1.data[1] = 1-pTo->y;
			animation->addPointInRBF(P0, P1);
			
			pTo++;
			pFrom++;
		}		
		
		animation->prepareAllRBFs();
		
	}
};

#endif /*HERMITE_H_*/
