/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// CommonStuff.h - Miscelaneous declarations that don't belong elsewhere.
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __COMMONSTUFF_H_INCLUDED__
#define __COMMONSTUFF_H_INCLUDED__

// Return true if we're under the debugger

bool	areWeBeingDebugged();

// Declare a function to terminate the program with a printf-like
// formatted error message

void	reallyAbort(const char *fmt, ...);

// Normally, we will call this function using the ABORT macro, which also
// reports the source file and line number.  This nasty looking macro
// is a handy little trick to allow our macro appear to to take a variable
// number of argmuents

extern	const char	*abortSourceFile;
extern	int		abortSourceLine;

#define ABORT (abortSourceFile = __FILE__, abortSourceLine = __LINE__, reallyAbort)

// Standard min and max functions

template <class Type>
inline const Type &min(const Type &a, const Type &b) {
	return (a < b) ? a : b;
}

template <class Type>
inline const Type &max(const Type &a, const Type &b) {
	return (a > b) ? a : b;
}

// Standard template swap routine

template <class Type>
inline void swap(Type &a, Type &b) {
	Type tmp(a);
	a = b;
	b = tmp;
}

// Global "quit" flag.  The platform-specific layer will turn on this
// flag when it receives a request from the operating system to
// terminate the app.  The application should respond to the request,
// and if it decides *not* to quit, turn the flag off.

extern bool gQuitFlag;

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __COMMONSTUFF_H_INCLUDED__

