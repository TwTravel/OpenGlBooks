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

#ifndef RBF_H_
#define RBF_H_

// Types of evaluation.
typedef enum{
	RBFGaussian,
	RBFMultiquadric
} RBFBasicFunction;

/**
 * Defines an Radial Basis Function that represents an implicit function of an object or mapping 
 */
// Data type and dimension.
template <class C, int N>
class RBF {

private: 
	// DEFINITIONS
	RBFBasicFunction phi;
	float suavization;

	struct RBFPoint {
		Vector<C,N> from; 
		Vector<C,N> to;
	};
	
	List<RBFPoint *> listPoints;
	bool prepared;
	
	// Ax = b 
	// A == Distance Matrix
	// x == Lambda
	// b == Result Matrix
	
	
	// Lamda
   	/*       P1  P2  P3
   	 * x: [  0   2   4
   	 * y:    5   5   5]
  	 */    	
	C * lambda;         // RBF coefficients.
	 
	
	/** Evaluates the basis function by a type: Gaussian or Multiquadric 
	 *  r - distance 
	 *  h - suavization between functions
	 */  
	float basisFunction(RBFBasicFunction type, float r, float h){
		switch(type){
			case RBFGaussian:     return exp(-h*r*r);
			case RBFMultiquadric: return sqrt(r*r+h*h);
		}
		return 0;
	}
		
	// build the distance matrix 
	void solveLinearEquations() {
		int n = listPoints.length();
		
		double * distanceMatrix = new C[n * n];
		double * resultMatrix = new C[n * N];
		
		Node<RBFPoint *> * aux1 = listPoints.firstNode();
		Node<RBFPoint *> * aux2 = listPoints.firstNode();
    
    	// calculates for each pair of points the distance applyed to the basis function.
    	for (int i = 0; i < n; ++i) {
    		aux2 = listPoints.firstNode();
    		
			/*        P1  P2
    		 * P1: [  0   2 
    		 * P2:    2   0 ]
    		 */    		
    		
    		for (int j = 0; j < n; ++j) {
        		distanceMatrix[ i * n + j ] = 
        				basisFunction(phi, 
        							  aux1->val()->from.distance(aux2->val()->from), 
        							  suavization);
        	
        		aux2 = aux2->next();	
    		}
    		
    		/*       P1  P2  P3
    		 * x: [  0   2   4
    		 * y:    5   5   5]
    		 */
			for (int j = 0; j < N; ++j) {			    		    		
    			resultMatrix[j * n + i] = aux1->val()->to[j];
			}
    		
    		aux1 = aux1->next();    		
    	}
    	
		if (lambda != NULL) {	
			delete[] lambda;
			lambda = NULL;	
		}
			
		lambda = new C[n*N];

		// Solve the linear equation.

#ifdef _WIN32
		integer info;
		integer nWin;
		integer nhrs;
		integer *indx = new integer[n];

		nhrs = 2;
		nWin = n;
		//int dgetrf_(integer *m, integer *n, doublereal *a, integer * lda, integer *ipiv, integer *info);
		dgetrf_(&nWin,&nWin,distanceMatrix, &nWin, indx,&info);

		//int dgetrs_(char *trans, 
		//			  integer *n, integer *nrhs, doublereal *a, integer *lda, 
		//			  integer *ipiv, doublereal *b, integer * ldb, integer *info);
		dgetrs_("No Transpose",&nWin, &nhrs, distanceMatrix, &nWin,indx,resultMatrix,&nWin,&info);
#else
		int *indx = new int[n];

		//int clapack_dgetrf(const enum CBLAS_ORDER Order, const int M, const int N, double *A, const int lda, int *ipiv);
		clapack_dgetrf(CblasRowMajor, n, n, distanceMatrix, n, indx);
		
		//int clapack_dgetrs(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE Trans,
        //                   const int N, const int NRHS, const double *A, const int lda,
        //                   const int *ipiv, double *B, const int ldb);
		clapack_dgetrs(CblasRowMajor, CblasNoTrans, n , N, distanceMatrix, n, indx, resultMatrix, n);
#endif

		for (int j = 0; j < n*2; ++j) {
			lambda[j] = resultMatrix[j];
		}		
		
		delete[] distanceMatrix;
		delete[] resultMatrix;
		delete[] indx;
		
		distanceMatrix= NULL;
		resultMatrix = NULL;
		indx = NULL;
	}		
		
public:

	RBF() {
		phi = RBFMultiquadric;
		suavization = 10;
		lambda = NULL;
		prepared = false;
	}

	RBF(RBFBasicFunction _phi, float _suavization) {
		phi = _phi;
		suavization = _suavization;
		lambda = NULL;
		prepared = false;
	}
	
	void addPoint(Vector<C, N> & from, Vector<C, N> & to) {
		RBFPoint * c = new RBFPoint();
		
		c->from = from;
		c->to = to;
		
		listPoints.append(c);
		
		prepared = false;
	} 

	int size() {
		return listPoints.length();
	}

	void prepare() {
		if (listPoints.length() == 0) return;
		
		solveLinearEquations();
		
		prepared = true;
	}
	
	bool isBuild() {
		return listPoints.length() ==0;
	}
	
	// Solves the RBF.
	void eval(Vector<C, N> & _x, Vector<C, N> * ret) {
		if (!prepared) prepare();
		
		int n = listPoints.length();

		Node<RBFPoint *> * aux = listPoints.firstNode();

    	// calculates for each pair of points the distance applyed to the basis function.
    	C distance;
    	ret->clear();

		// Lamda
    	/*       P1  P2  P3
    	 * x: [  0   2   4
    	 * y:    5   5   5]
    	 */    	
    	ret->clear();
    	
    	for (int i = 0; i < n; ++i) {
    		distance = aux->val()->from.distance(_x);
    		
    		// Multiquadric
       		distance = sqrt(distance*distance + suavization*suavization);
        
        	for (int j = 0; j < N; ++j) {	
        		ret->data[j] += distance * lambda[j * n + i];
        		
        		if (fabs(ret->data[j]) < 0.001)
        		   ret->data[j] = 0; 
        	}
        		
    		aux = aux->next();    		
    	}
	} 

	void resetPoint() {
		listPoints.first();
	}


	void setFloat4(int point, float * data, int startPoint) {
		data[startPoint]   = (float)listPoints.val()->from.data[0]; // X
		data[startPoint+1] = (float)listPoints.val()->from.data[1]; // Y
		data[startPoint+2] = (float)lambda[point];
		data[startPoint+3] = (float)lambda[listPoints.length()+point];
		listPoints.next();
		/*
		std::cout << " X: " << data[startPoint] <<
				     " Y: " << data[startPoint+1] <<
				     " LX: " << data[startPoint+2] <<
				     " LY: " << data[startPoint+3] << std::endl;
				     */ 
	}

	virtual ~RBF() {
		if (lambda != NULL)
			delete[] (lambda);
		
		listPoints.deleteAll();
	}
};

#endif /*RBF_H_*/
