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

#ifndef ANIMATION_H_
#define ANIMATION_H_

/**
 * Handles the complete animation with 11 frames. 
 */
template<class C, int N>
class Animation {
	typedef List<RBF<C, N> *> RBF_LIST;

public:
	RBF_LIST rbfList;
	
	Animation(int steps, RBFBasicFunction phi, double suavization) {
		for (int i=0; i<steps; i++) {
			rbfList.append(new RBF<C, N>(phi, suavization));
		}
	}
	
	virtual ~Animation() {
		rbfList.deleteAll();
	}
	
	void clear() {
		rbfList.deleteAll();
	}
	
	int numberOfPoints() {
		return rbfList.firstNode()->val()->size();
	}
	
	void prepareAllRBFs() {
		RBF<C,N> * rbf = rbfList.first();
		for (int i=0; i<rbfList.length(); i++) {
			rbf->prepare();
			rbf = rbfList.next();
		}
		rbfList.first();
	}
	
	void positionInRBF(int index) {
		rbfList.first();
		for (int i=0; i<index; i++) {
			rbfList.next();
		}
	}
	
	RBF<C, N> * getRBF(int index) {
		rbfList.first();
		for (int i=0; i<index; i++) {
			rbfList.next();
		}
		return rbfList.val();
	}
	
	void firstRBF() {
		rbfList.first();
	}
	
	void nextRBF() {
		rbfList.next();
	}
	
	void addPointInRBF(Vector<C, N> & from, Vector<C, N> & to) {
		//std::cout << from.print() << " - " << to.print() << std::endl;
		
		rbfList.val()->addPoint(from, to);
	}
};
#endif /*ANIMATION_H_*/
