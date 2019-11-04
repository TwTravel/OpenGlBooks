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

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define 	M_PI   3.14159265358979323846

#include <ctime>
#include <windows.h>

#endif

#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <algorithm>

#include <vector>
#include <map>
#include <stdlib.h>
#include <math.h>

#ifdef _WIN32
#include <GL/glew.h>
//#include <GL/wglew.h>
#else
#include <GL/gl.h>
#endif

#include <GL/glut.h>

#include <IL/il.h>
#include <IL/ilu.h>

#include <Cg/cg.h>
#include <Cg/cgGL.h>

extern "C" {
	#include <f2c.h>
	#include <clapack.h>
}


#include "Util.h"

#include "Vector.h"
#include "Quaternion.h"
#include "Rotationable.h"

#include "Node.h"
#include "List.h"

#include "Image.h"
#include "ImageKeyFrame.h"
#include "ImageLoader.h"
#include "ImageWriter.h"

#include "KeyFrames.h"
#include "KeyFramesViewer.h"

#include "AnimationShader.h"
#include "AnimationReliefMapShader.h"

#include "RBF.h"
#include "RBFTextureWriter.h"
#include "Animation.h"
#include "Hermite.h"

#include "Moving.h"
#include "MovingCube.h"
#include "MovingReliefPlane.h"

#include "SimpleGLWriter.h"
#include "PointsSerialization.h"
#include "GeneralViewer.h"
#include "Viewer.h"


int main(int argc, char *argv[]) { 
	initViewer(argc, argv);
	
	ImageLoader * loader = new ImageLoader();
 	
 	Image<char> * base;
 	Image<char> * normal;
 	Image<char> * layer12;
 	Image<char> * layer34;
 	Image<char> * img1;
 	Image<char> * img2;
 	Image<char> * img3; 	
 	
 	if (argc <= 2) {
 		std::cout << "Inform what file you wanna use on the demo \n and if it is normal map or relief map:" << std::endl;
 		std::cout << "" << std::endl;
 		std::cout << "\t imagedit <name> <type> " << std::endl;
		std::cout << "" << std::endl;
 		std::cout << "Eg: " <<std::endl;
 		std::cout << "\t imagedit dog reliefmap" << std::endl;
 		std::cout << "\t imagedit horse reliefmap" << std::endl;
 		std::cout << "\t imagedit pendulum normalmap" << std::endl;
 		std::cout << "" << std::endl;
 		std::cout << "The pattern of names for reliefmap textures is: " << std::endl;
 		std::cout << "\t <name>.jpg for the color texture " << std::endl;
 		std::cout << "\t <name>.tga for the quad texture " << std::endl;
 		std::cout << "\t <name>_n1.jpg for the first 2 layers texture " << std::endl;
 		std::cout << "\t <name>_n2.jpg for the last 2 layers texture " << std::endl;
 		std::cout << "" << std::endl;
 		std::cout << "The pattern of names for normalmap textures is: " << std::endl;
 		std::cout << "\t <name>.tag for the color texture " << std::endl;
 		std::cout << "\t <name>_n.tga for the normal texture " << std::endl;
		std::cout << "" << std::endl;
		std::cout << "Program by Vitor Pamplona, Luciana Nedel and Manuel Menezes " << std::endl;
		std::cout << "e-mail: vitor@vitorpamplona.com " << std::endl;
		std::cout << "" << std::endl; 		
 		exit(0);
 	}
 	
 	std::string filename = argv[1];
 	std::string type = argv[2];

	if (type == "reliefmap") {
		base = loader->loadRBG( (filename + ".jpg").c_str() );
		normal = loader->loadRBGA((filename + ".tga").c_str());
	
		layer12 = loader->loadRBGA((filename + "_n1.tga").c_str());
		layer34 = loader->loadRBGA((filename + "_n2.tga").c_str());

		img1 = base; 
		img2 = base;
		img3 = base;

 		if (img1 == NULL ||  img2 == NULL ||  img3 == NULL 	||  base == NULL
 		||  normal == NULL ||  layer12 == NULL ||  layer34 == NULL) 
 			exit(0);

		base->invert();
	} else if (type == "normalmap") {
		base = loader->loadRBGA((filename + ".tga").c_str());
 		normal = loader->loadRBGA((filename + "_n.tga").c_str());
 
 		layer12 = NULL;
 		layer34 = NULL;
 
 		img1 = base;
 		img2 = base;
 		img3 = base;

 		 if (img1 == NULL ||  img2 == NULL ||  img3 == NULL	||  base == NULL ||  normal == NULL) 
 			exit(0);
 	} else {
 	 	std::cout << "Invalid Type" << std::endl;
 	 	exit(0);
	}
 	
 	((Viewer *)glutViewer)->loadKeyImage(img1);
 	((Viewer *)glutViewer)->loadKeyImage(img2);
 	((Viewer *)glutViewer)->loadKeyImage(img3);
 	
 	((Viewer *)glutViewer)->setBaseImage(base, normal, layer12, layer34);  
 		
  	((Viewer *)glutViewer)->takeControl(); 	
}
