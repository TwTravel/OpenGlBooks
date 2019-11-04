//-----------------------------------------------------------------------------
// Name: GABlade.h
// Original code by Vitor Fernando Pamplona (vitor@babaxp.org)
//
// Copyright (c) 2006, Vitor Fernando Pamplona
// Copying, use and development for non-commercial purposes permitted.
// All rights for commercial use reserved; for more information       
// contact Vitor Fernando Pamplona (vitor@babaxp.org).                    
//                                                                     
// This software is unsupported.                                      
//-----------------------------------------------------------------------------
// Testcase is the base class to all unit tests
//-----------------------------------------------------------------------------
#ifndef _TESTCASE_H_
#define _TESTCASE_H_

//#include <iostream>
#include <math.h>

double EPSLON = 1.0E-7;

bool equals(float a, float b) {
	return fabs(a - b) < EPSLON;
}

bool equals(double a, double b) {
	return fabs(a - b) < EPSLON;
}

using namespace std;

class Testcase {
public :
	int ok;
	int failed;
	float totalTime;	

	Testcase(void) {
		ok = 0;
		failed = 0;
	}

	virtual ~Testcase(void) {
		ok = 0;
		failed = 0;
	}

	virtual void execute(void) {
		cout << "Executing base class" << endl;
	}
	
	void assertEquals(char *message, float a, float b) {
		if (!equals(a, b)) {
			cout << "\t" << message << ". Expected " << a << " when found " << b << endl;
			failed ++;
		} else {
			ok++;
		}
	}	
	
	void assertEquals(string message, float a, float b) {
		if (!equals(a, b)) {
			cout << "\t" << message << ". Expected " << a << " when found " << b << endl;
			failed ++;
		} else {
			ok++;
		}
	}		
	
	void assertTrue(char * message, bool b) {
		if (!b) {
			cout << "\t" << message << ". Expected true when found false\n";
			failed ++;
		} else {
			ok++;
		}
	}
	
	void assertTrue(string message, bool b) {
		if (!b) {
			cout << "\t" << message << ". Expected true when found false\n";
			failed ++;
		} else {
			ok++;
		}
	}	
	
	void assertEquals(char * message, string s1, string s2) {
		if (!(s1 == s2)) {
			cout << "\t" << message << ". Expected " << s1 << " when found " << s2 << endl;
			failed ++;
		} else {
			ok++;
		}
	}	
	
	void initClock() {
		totalTime = clock();
	}
	
	void printClockAndReset(char * msg) {
		totalTime = (double)(clock()-totalTime)/CLOCKS_PER_SEC;
		cout << "Total Stress Time to " << msg << " Test: " << totalTime << endl;
   		totalTime = clock();	
	}
	
	void assertEquals(string message, string s1, string s2) {
		if (!(s1 == s2)) {
			cout << "\t" << message << ". Expected " << s1 << " when found " << s2 << endl;
			failed ++;
		} else {
			ok++;
		}
	}		
	
	
	void print() {
		if (failed == 0) {
			cout << "\tTest OK with " << ok <<" accepts"  << endl;
		} else {
			cout << "\tTest FAILED with " << failed << "  erros and " << ok << " accepts" << endl;
		}
	}
};

#endif /* _TESTCASE_*/
