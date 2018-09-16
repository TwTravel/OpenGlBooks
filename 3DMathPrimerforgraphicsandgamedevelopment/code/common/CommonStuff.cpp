/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// CommonStuff.cpp - Miscelaneous code that doesn't belong elsewhere.
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "CommonStuff.h"

#ifdef WIN32
	#include <windows.h>
#endif

const char	*abortSourceFile = "(unknown)";
int		abortSourceLine;

/////////////////////////////////////////////////////////////////////////////
//
// global code
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// areWeBeingDebugged()
//
// Return true if we are running under the debugger

bool	areWeBeingDebugged() {
	#ifdef WIN32

		// This routine is in kernel32.  Acquire access to the
		// DLL

		HINSTANCE h = LoadLibrary("kernel32.dll");
		if (h == NULL) {
			return false;
		}

		// Fetch the code

		typedef BOOL (*PFunc)(VOID);

		PFunc func = (PFunc)GetProcAddress(h, "IsDebuggerPresent");
		if (func == NULL) {
			return false;
		}

		// Actually call the routine

		bool result = (func() != FALSE);

		// Don't hang on to any resources we don't need

		FreeLibrary(h);

		// Return the result

		return result;
	#else

		// For now, let's assume we are being debugged
		// in a debug build, and otherwise we're not being
		// debugged

		#ifdef _DEBUG
			return true;
		#else
			return false;
		#endif

	#endif
}

//---------------------------------------------------------------------------
// reallyAbort
//
// Fatal error.  Usually called through the ABORT macro

void	reallyAbort(const char *fmt, ...) {

	// Format the error message into our buffer

	char	errMsg[1024];
	va_list ap;
	va_start(ap, fmt);
	vsprintf(errMsg, fmt, ap);
	va_end(ap);

	// Tack on the source file and line number

	sprintf(strchr(errMsg, '\0'), "\n%s line %d", abortSourceFile, abortSourceLine);

	// Windows?  Dump message box

	#ifdef WIN32

		// Check if we're under the debugger

		if (areWeBeingDebugged()) {

			// Dump error message to the debug console

			OutputDebugString("FATAL ERROR: ");
			OutputDebugString(errMsg);
			OutputDebugString("\n");

			// Break the program, so we can check out what was going on.

			_asm {

				// HELLO! If you hit this breakpoint, then look at the debug
				// console, and go up the call stack to see what went wrong!

				int 3;
			}

			// You can try to keep running, if you want...

		} else {

			// Just dump a message box and terminate the app

			MessageBox(NULL, errMsg, "FATAL ERROR", MB_OK | MB_ICONERROR);
			ExitProcess(1);
		}
	#else

		// Just dump it to printf and use exit.  On most OSs,
		// this is basically useless for debugging, so you'd
		// want to do better, especially under the debugger

		printf("FATAL ERROR: %s\n", errMsg);
		exit(1);

	#endif
}
