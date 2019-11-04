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

#ifndef UTIL_H_
#define UTIL_H_

float totalTime;

double degToRad(double degree) {
	return degree *(M_PI / 180);
}

double radToDeg(double radians) {
	return radians * (180/M_PI);
}

char * convert(std::string teste) {
	return const_cast<char *>(teste.c_str());
}

#ifdef _WIN32
typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
#endif

void setVSync(int interval=1) {
#ifdef _WIN32
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

    if( wglSwapIntervalEXT )
      wglSwapIntervalEXT(interval);
#endif	
}

void glError() { 
    GLenum err = glGetError(); 
    while (err != GL_NO_ERROR) {
       fprintf(stderr, "glError: %s caught at %s:%u\n", (char *)gluErrorString(err), __FILE__, __LINE__); 
       err = glGetError(); 
    } 
}

void cgError(CGcontext cg_context) {
	CGerror LastError = cgGetError();
	if(LastError) {
		printf("%s\n\n", cgGetErrorString(LastError));
		printf("%s\n", cgGetLastListing(cg_context));
		printf("Cg error, exiting...\n");
		exit(0);
	}
}

void initTime() {
	totalTime = clock();
}

void printTime(char * msg) {
	totalTime = (double)(clock()-totalTime)/CLOCKS_PER_SEC;
	std::cout << msg << " Total Time: " << totalTime << std::endl;
}

#endif /*UTIL_H_*/
