#ifndef RBFTEST_H_
#define RBFTEST_H_

int NUM_TESTS= 100;

class RBFTest :  public Testcase {
public:

	void testInicial() {
		RBF<double, 2> rbf;
		
		Vector<double, 2> p1(-1, 5);
		Vector<double, 2> p2( 1, 5);
		Vector<double, 2> p3(-1, 0);
		Vector<double, 2> p4( 1, 0);

		Vector<double, 2> p1Test( 0, 0);

		Vector<double, 2> r1( 0, 5);
		Vector<double, 2> r2( 2, 5);
		Vector<double, 2> r3( 0, 0);
		Vector<double, 2> r4( 2, 0);
		
		Vector<double, 2> r1Test( 0.984891, 0.0180186);
		
		rbf.addPoint( p1, r1);
		rbf.addPoint( p2, r2);
		rbf.addPoint( p3, r3);
		rbf.addPoint( p4, r4);
		
		assertTrue("Test p1", rbf.eval(p1) == r1);
		assertTrue("Test p2", rbf.eval(p2) == r2);
		assertTrue("Test p3", rbf.eval(p3) == r3);
		assertTrue("Test p4", rbf.eval(p4) == r4);
		assertTrue("Test p1Test", rbf.eval(p1Test) == r1Test);
	}
	
	double random() {
		return rand() % 300;
	}
	
	void testRandomized() {
		srand((unsigned)time(0)); 
		
		RBF<double, 2> rbf;

		List<Vector<double, 2> *> points;
		List<Vector<double, 2> *> results;

		Vector<double, 2> * p;
		Vector<double, 2> * q;		

		for (int i =0; i<NUM_TESTS; i++) {
			p = new Vector<double,2>(  random() ,  random());
			q = new Vector<double,2>(  random() ,  random());
			
			rbf.addPoint(*p, *q);
			
			points.append(p);
			results.append(q);
		}
		
		rbf.prepare();
		
		p = points.first();
		q = results.first();
		
		for (int i =0; i<NUM_TESTS; i++) {

			std::stringstream msg;  
			
			msg << "Test p " << p->print() << " and q " << q->print() << " with result " << rbf.eval(*p).print();
				
			assertTrue(msg.str(), rbf.eval(*p) == *q);
			
			p = points.next();
			q = results.next();			
		}
		
		points.clear();
		results.clear();
	}
	
	void execute(void) {
        cout << "Executing RBFTest" << endl;
        
        testInicial();
        testRandomized();
    }
};

#endif /*VORONOITEST_H_*/

