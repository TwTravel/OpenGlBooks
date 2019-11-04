/*____________________________________________________________________
|
| File: common.h
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

#define NOT !
#define AND &&
#define OR  ||

#define DEBUG_WRITE(_str_) { printf (_str_); printf ("\n"); }

#define DEBUG_ERROR(_str_)	\
{														\
	printf(_str_);						\
	printf("\n");							\
	exit (1);									\
}
