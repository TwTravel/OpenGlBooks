/*____________________________________________________________________
|
| File: main.cpp
|
| Description: Contains code to test the vocab library.
|
| Functions: main
|
| (C) Copyright 2007 Timothy E. Roden.  
|	
|	This code is included on the CD for the book Game Programming Gems 7.
| The author, Timothy E. Roden, grants the purchaser of the book 
| unrestricted rights to use this software for any research or commercial 
| purpose provided acknowledgement is given of its origin.
|
| Compiler: Tested with Microsoft Visual Studio 2005 (Version 8.0.50727.762)
|___________________________________________________________________*/

// Enable older CRT functions (such as strcpy) without warnings from vc8 (vc 2005 .net)
#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vc8 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <windows.h>

#include <iostream>
#include <fstream>
using namespace std;

#include "vocab.h"

void main () 
{
  bool found;
  Vocabulary vocab;
  int i, n;
  byte *phonemes, visemes[100];

  cout << "Loading vocabulary ..." << endl;
  vocab = vocab_Init ("CMU Dictionary\\cmudict_0_6.txt");
  cout << "...done" << endl;

  if (vocab) {
		// Translate the word "market" into phonemes
		cout << "Translating the word: hello" << endl;
		found = vocab_TranslateWord (vocab, "hello", &n, &phonemes);
    if (found) {
      cout << "Word found!" << endl;
			// Print out the phonemes
      cout << "(phonemes) ";
      for (i=0; i<n; i++)
        cout << (int)(phonemes[i]) << ' ';
      cout << endl;
      // Convert phonemes to visemes
      vocab_TranslatePhonemesToVisemes (phonemes, visemes, n);
			// Print out the visemes
			cout << "(visemes) ";
      for (i=0; i<n; i++)
        cout << (int)(visemes[i]) << ' ';
      cout << endl;
      free (phonemes);
    }
    else
      cout << "Word not found" << endl;
  }

  vocab_Free (vocab);
}
