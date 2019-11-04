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

#ifndef VECTOR_H_
#define VECTOR_H_

/**
 * Simple Linear Algebra Vector class
 */ 
template <class C, int N>
class Vector {
	
public:
	C data[N];

	Vector() {
		clear();
	}
	virtual ~Vector() {}
	
	Vector(C x, C y) {
		data[0] = x;
		data[1] = y;
	}
	
	Vector(C x, C y, C z) {
		data[0] = x;
		data[1] = y;
		data[2] = z;
	}	
		
	inline void clear() {
		for (int i=0; i<N; i++) {
			data[i] = 0;
		}
	}		
		
	void copy(const Vector<C,N> & to) {
		for (int i=0; i<N; i++) {
			data[i] = to.data[i];
		}
	}			
	
    inline C operator [] (int index) const {
    	 return data[index];
    }   	

	inline void set(int index, C value) {
		data[index] = value;
	}    

    inline Vector<C,N> operator - (Vector<C,N> another) const {
    	 Vector<C,N> ret;
    	 
    	 for (int i=0; i<N; i++) {
			ret.data[i] = data[i] - another.data[i];
		 }
    	 
    	 return ret;
    }
        
    Vector<C,N> operator -= (const Vector<C,N> &another) {
    	 Vector<C,N> ret;
    	 
    	 for (int i=0; i<N; i++) {
			ret.data[i] = data[i] - another.data[i];
		 }
    	 
    	 return ret;
	}    
   
    Vector<C,N> operator + (void) const {
    	return * this;
    }
    
    Vector<C,N> operator - (void) const {
    	 Vector<C,N> ret;
    	 
    	 for (int i=0; i<N; i++) {
			ret.data[i] = -data[i];
		 }
    	 
    	 return ret;
    }   
   
    inline Vector<C,N> operator + (const Vector<C,N> & another) const {
    	 Vector<C,N> ret;
    	 
    	 for (int i=0; i<N; i++) {
			ret.data[i] = data[i] + another.data[i];
		 }
    	 
    	 return ret;
    }    
    
    inline Vector<C,N> operator * (double d) const {
    	 Vector<C,N> ret;
    	 
    	 for (int i=0; i<N; i++) {
			ret.data[i] = data[i] * d;
		 }
    	 
    	 return ret;
    }  
    
    inline Vector<C,N> operator * (Vector<C,N> d) const {
    	 Vector<C,N> ret;
    	 
    	 for (int i=0; i<N; i++) {
			ret.data[i] = data[i] * d.data[i];
		 }
    	 
    	 return ret;
    }     
    
    inline Vector<C,N> operator / (double d) const {
    	 Vector<C,N> ret;
    	 
    	 for (int i=0; i<N; i++) {
			ret.data[i] = data[i] / d;
		 }
    	 
    	 return ret;
    }   
    
   	bool sameDirection(const Vector<C,N> & other) const {
   		for (int i=0; i<N; i++) {
			if (fabs(data[i] - other.data[i]) > 0.1) return false;
		}
		return true;
   	}     
    
    double distance(const Vector<C,N> & to) {
		float dist1D;
		float distND = 0;
		for (int i=0; i<N; i++) {
			dist1D = fabs(to[i] - data[i]);
			distND += dist1D * dist1D;  
		}
		
		return sqrt(distND);
	}
	
    double squareRoot() {
		float distND = 0;
		for (int i=0; i<N; i++) {
			distND += data[i] * data[i];
		}
		
		return distND;
	}	
	
    double length() const {
		float distND = 0;
		for (int i=0; i<N; i++) {
			distND += data[i] * data[i];
		}
		
		return sqrt(distND);
	}		
	

	bool equals(double a, double b) {
		return fabs(a - b) < 0.001;
	}	
	
	void normalize() {
		double dist = length();
		for (int i=0; i<N; i++) {
			data[i] /= dist;
		}
	}
	
	bool isZero() {
		for (int i=0; i<N; i++) {
			if (!equals(data[i], 0.0f)) return false;
		}
		return true;
	}
	
	inline bool operator == (const Vector<C,N> & other) {
		for (int i=0; i<N; i++) {
			if (!equals(data[i], other[i])) return false;
		}
		return true;
	}
	
	double dot( const Vector<C,N> & other ) {
		double total = 0;
		for (int i=0; i<N; i++) {
			total += data[i] * other.data[i];
		}
		return total;
	}
	
	double dotProduct( const Vector<C,N> & other ) {
		return dot(other);
	}	
	
	float angleInDegree( const Vector<C,N> &v2 ) {
	    return acos(dot(v2)/(length() * v2.length())) * 180/ M_PI;
	}
	
	float angleInRadians( const Vector<C,N> &v2 ) {
	    return acos(dot(v2)/(length() * v2.length()));
	}
	
	// SOMENTE PARA 3D!!! 
	Vector<C,N> crossProduct3D(const Vector<C, N> &v2) {
	    Vector<C,N> vCrossProduct;
	
	    vCrossProduct.data[0] =  data[1] * v2.data[2] - data[2] * v2.data[1];
	    vCrossProduct.data[1] = -data[0] * v2.data[2] + data[2] * v2.data[0];
	    vCrossProduct.data[2] =  data[0] * v2.data[1] - data[1] * v2.data[0];
	
	    return vCrossProduct;
	}
	
	Vector<C,N> crossProduct2D(const Vector<C, N> &v2) {
	    Vector<C,N> vCrossProduct;
	
	    vCrossProduct.data[0] =  data[0] * v2.data[1];
	    vCrossProduct.data[1] = -data[1] * v2.data[0];
	
	    return vCrossProduct;
	}	
	
	Vector<C,N> crossProduct(const Vector<C, N> &v2) {
		if (N == 2) {
			return crossProduct2D(v2);
		} else if (N==3) {
			return crossProduct3D(v2);
		} else {
			std::cerr << "Cross Product of " << N << " is undefined " << std::endl;
			exit(1);
		}
	}
		
	
	double sumAll() {
		double ret = 0;
	    for (int i=0; i<N; i++) {
	    	ret += data[i];
	    }
	    return ret;
	}
	
	// SOMENTE PARA 3D!!! 
	Vector<C,N> addToLength(double toAdd) {
	    C l = length();
	    
	    double percent = (l+toAdd)/l;
	    
	    Vector<C,N> ret;
	    for (int i=0; i<N; i++) {
	    	ret.data[i] = data[i] * percent;
	    }
	    return ret;
	}
	
    std::string print() {
    	std::stringstream stream;
        bool first = true;
		stream << "[ ";
          
        for (int i=0; i<N; i++) {
        	if (!first) {
        		stream << ", ";
            }
            
            stream << data[i]; 
            first = false;                  
        }
        stream << "] ";
        
        return stream.str();
    }   	
	
};

typedef Vector<int, 2> Vector2i;
typedef Vector<int, 3> Vector3i;
typedef Vector<float, 2> Vector2f;
typedef Vector<float, 3> Vector3f;

#endif /*VECTOR_H_*/
