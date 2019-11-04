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

#ifndef VIEWER_H_
#define VIEWER_H_

void timerFunctionGLUT(int value);
void errorCG();

/**
 * All window and interaction processing of the demo.  
 */
class Viewer : public GeneralViewer {
private: 
	char * defaultTitle;

	RBFBasicFunction phi;
	float suavization;
	
	KeyFramesViewer * keyViewer;
	Animation<double, 2> * rbfs;
	Image<float> * rbfRepresentation;
	
	Image<char> * baseImage;
	Image<char> * baseNormal;
	Image<char> * baseLayer12;
	Image<char> * baseLayer34;
		
	std::stringstream msg;
	float animationTime;
	float messageTime;
	float shininess;
	
	Moving * movingObj;
	SimpleGLWriter * glWriter;
	PointsSerialization * points; 
	
	bool showHelp;
	bool frontAndBack;
	bool play;
	int singleLayerAnimation;
	float incAnimation;
	
	Quaternion light;
	Quaternion lightRotor;

	Vector2i from;
public:	
	AnimationShader * shaderSimpleMultiLayer;
	AnimationShader * shaderFastMultiLayer;
	AnimationShader * shaderSingleLayer;
	AnimationShader * shader;

	void sendImageToGPU(Image<char> * _img) {
		glGenTextures(1, &_img->name); /* Texture name generation */
   		glBindTexture(GL_TEXTURE_2D, _img->name); /* Binding of texture name */
   		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); /* We will use linear interpolation for magnification filter */
   		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); /* We will use linear interpolation for minifying filter */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   		glTexImage2D(GL_TEXTURE_2D, 0, _img->components, _img->width, _img->height, 0,  _img->format, GL_UNSIGNED_BYTE, _img->pixels); /* Texture specification */
	} 
	
	void setBaseImage(Image<char> * _img, Image<char> * _normalmap, Image<char> * _layer12, Image<char> * _layer34) {
		baseImage = _img;
		baseNormal = _normalmap;
		baseLayer12 = _layer12;		
		baseLayer34 = _layer34;				
		
		sendImageToGPU(baseImage);
		sendImageToGPU(baseNormal);
		
		if (isReliefMap()) {
			sendImageToGPU(baseLayer12);
			sendImageToGPU(baseLayer34);
		}
	}

	void loadKeyImage(Image<char> * img) {
		sendImageToGPU(	keyViewer->addImage(img) );
	}

	void mousePressed(int x, int y, bool shift, bool ctrl, bool alt) {
		keyViewer->mousePressed(x, y, shift, ctrl, alt);
		
   		if (y > 200) {
   			// Mouse interaction   		
   			from.set(0,x);
			from.set(1,y);
   		} 

   		display();  
	} 
	
	void mouseReleased(int x, int y, bool shift, bool ctrl, bool alt) {
		keyViewer->mouseReleased(x, y);
		from.set(0,-1);
		from.set(1,-1);
		generateRBF();
	} 
	
	virtual void mouseRightPressed(int x, int y, bool shift, bool ctrl, bool alt) {
		keyViewer->mouseRightPressed(x, y, shift, ctrl, alt);
	}
	
	void mouseMotion(int x, int y) {
		if (keyViewer->mouseMotion(x,y)) {
			generateRBF();
		} else if (y > 200) {
			movingObj->mouseMove(from[0], x, from[1], y);
   			from.set(0,x);
			from.set(1,y);
		}
	}
	
	void toggleSingleMultiLayerAnimation() {
		if (!isReliefMap()) {return;}
		
		singleLayerAnimation = (singleLayerAnimation + 1) % 3;	

		if (singleLayerAnimation == 0) {
			shader = shaderSingleLayer;	
		} else if (singleLayerAnimation == 1) { 
			shader = shaderSimpleMultiLayer;	
		} else {
			shader = shaderFastMultiLayer;
		}		
	}
	
	void keyPressed(unsigned char key) {
		switch (key) {
			case 'v' :
			case 'V' : frontAndBack = !frontAndBack; break;
			
			case 'h' :
			case 'H' :  toggleShowHelp(); break;
						
			case 'a' :
			case 'A' : play=!play; break;
			
			case 'r' :
			case 'R' : generateRBF(); break;
			
			case 't' :
			case 'T' : toggleSingleMultiLayerAnimation(); break;
						
			case 'n' :
			case 'N' : keyViewer->clear(); break;
			
			case 's' :
			case 'S' : points->save(); break;
			
			case 'l' :
			case 'L' : points->load(); generateRBF(); break;
						
			case 'z' : if (suavization < 1.1) 
							suavization -=0.01;
					   else 
					   		suavization --;	 
					   break;
			case 'Z' : if (suavization < 1.1) 
							suavization +=0.01;
					   else 
					   		suavization ++;	 
					   break;

			case 'i' : if (shininess < 1.1) 
							shininess -=0.01;
					   else 
					   		shininess --;	 
					   break;
			case 'I' : if (shininess < 1.1) 
							shininess +=0.01;
					   else 
					   		shininess ++;	 
					   break;					   
			//case 'q' :		 	
			//case 'Q' : phi = RBFMultiquadric; break;
			case 'g' :
			case 'G' : keyViewer->gridRegular(); break;			
		}
		
		if (suavization < 0 || fabs(suavization) < 0.001) {
			suavization = 0.01f;
		} 
		if (shininess < 0 || fabs(shininess) < 0.001) {
			shininess = 0.01f;
		} 		
		 
	}

	void generateRBF() {
		if (!keyViewer->hasPoints()) {
			msg.str("");
			msg << "Mark control points in image 1";
			messageTime = 0.0f;
			
			return;
		}
		
		if (rbfRepresentation!=NULL) {
			unloadRBFImage();
			delete rbfRepresentation;
		}
		
		if (rbfs != NULL) 
			delete rbfs;
		
		rbfs = new Animation<double, 2>(11, phi, suavization);

		// Buils an Hermite Curve between the three main frames. 
		Hermite hermiteCurve(&keyViewer->frames, 5, rbfs);
		hermiteCurve.execute();
		
		//RBFWriter<double, 2> filewriter = RBFWriter<double, 2>("output", 256, &rbfs->rbfList, (*frames.imageList.begin())->pixels );
		//filewriter.execute();	
			
		RBFTextureWriter<double, 2> writer = RBFTextureWriter<double, 2>(&rbfs->rbfList);
		rbfRepresentation = writer.execute();			
				
		msg.str("");
		msg << "Animation built!";
		messageTime = 0.0f;		
		
		loadRBFImage();
		
		//glutPostRedisplay();
	}

	void unloadRBFImage() {
		if (rbfRepresentation != NULL) {
			glDeleteTextures(1, &rbfRepresentation->name);
		}
	}

	void loadRBFImage() {
		if (rbfRepresentation != NULL) {
			glGenTextures(1, &rbfRepresentation->name); // Texture name generation //
   			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, rbfRepresentation->name); // Binding of texture name 
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // We will use linear interpolation for magnification filter
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // We will use linear interpolation for minifying filter 
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP );
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP );
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_FLOAT_RGBA32_NV, rbfRepresentation->width, rbfRepresentation->height, 0,  GL_RGBA, GL_FLOAT, rbfRepresentation->pixels); // Texture specification
		}   		
	}
	
	
	void display() {
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	  	glClearDepth(0.0f);
	  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

		if (showHelp) {
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(0.0, width, height, 0.0, 0.0, 100.0);

			glMatrixMode(GL_MODELVIEW);	
			glLoadIdentity();	
		
		  	glColor3f(1.0f, 1.0f, 1.0f);
			
			keyViewer->display();

			renderMessages();
		}
		
		renderMotionPlane();

		updateFPS();
		
		glutSwapBuffers();
	}

	void renderMessages() {
		if (!showHelp) return;
		
		glWriter->setPositionFromBotton(10.f, 23);

		glWriter->renderString("Options ");
		glWriter->renderString("---------------------------");
		glWriter->renderString("H: Show/Hide this help ");		
		glWriter->renderString("N: Clean control points ");		
		//glWriter->renderString("R: Computes RBF coefficients ");
		glWriter->renderString("A: Start/Stop animation");
		//glWriter->renderString("Q: Multiquadric RBF equation ");
		glWriter->renderString("G: Grid Regular ");
		glWriter->renderString("z: Smoothness-- ");
		glWriter->renderString("Z: Smoothness++ ");
		glWriter->renderString("i: Shininess-- ");
		glWriter->renderString("I: Shininess++ ");
		//glWriter->renderString("M: Change Model ");		
		glWriter->renderString("V: Back Animation");
		if (isReliefMap())
			glWriter->renderString("T: Change Animation Technique");
			
		glWriter->renderString("L: Load Points.txt ");
		glWriter->renderString("S: Save Points.txt ");
		glWriter->renderString("    ");
		glWriter->renderString("System State ");
		glWriter->renderString("---------------------------");

		std::stringstream msg2;

		msg2.str("");
		msg2 << "Phi ";
		switch(phi){
			case RBFGaussian:     msg2 << "Gaussian"; break;
			case RBFMultiquadric: msg2 << "Multiquadric"; break;
		}
		
		msg2 << " with smoothness " << suavization;			
		glWriter->renderString(msg2);

		msg2.str("");
		msg2 << "Shader: " << shader->name << std::endl;			
		glWriter->renderString(msg2);

		//msg2.str("");
		//msg2 << "Luz: " << light[0] << ", " << light[1]  << ", "<< light[2] << ", " << light[3] << std::endl;
		//glWriter->renderString(msg2);

		msg2.str("");
		msg2 << "Shininess: " << shininess << std::endl;
		glWriter->renderString(msg2);

		glWriter->renderString(msg);
	}
	
	void toggleShowHelp() {
		showHelp = !showHelp;

		if (showHelp) {
			movingObj->setObjPosition(20, -20, 0);
		} else {
			movingObj->setObjPosition(0, 0, 0);
		}
	}

	void renderMotionPlane() {
		if (rbfRepresentation != NULL) {
			movingObj->prepare(20.0, width, height);
			shader->enableShader();
			
			float * lightArray = light.toArray();
			shader->setParameters(animationTime*10, rbfs->numberOfPoints(), suavization, shininess, 
								  lightArray, rbfRepresentation != NULL, rbfRepresentation->name, baseImage->name, 
								  baseNormal->name);
			delete[] lightArray;					  

			if (isReliefMap())
				((AnimationReliefMapShader*)shader)->setParameters(baseLayer12->name, baseLayer34->name);						
						
			movingObj->draw(20.0, -1);
			
	  		shader->disableShader();
	  		movingObj->unprepare();
		} else {
			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, baseImage->name);
			movingObj->prepare(20.0, width, height);
			movingObj->draw(20.0, 1);
			movingObj->unprepare();

			glDisable(GL_TEXTURE_2D);
		}
		
		glDisable(GL_BLEND);   
	}

	void renderLight(Quaternion light) {
		glBegin(GL_LINE);
		glPointSize(4);
		glColor4f(1.0, 0.0, 0.0, 1.0f);
		glVertex3f(0, 0, 0);
		glVertex3f(light.x, light.y, light.z);
		glEnd();
	}
	
	void timerFunction() {
		if (!play) {
			glutTimerFunc(100,timerFunctionGLUT,1);  
			return;
		}

		animationTime+=incAnimation;
		messageTime+=1;

		if (animationTime - 1 > 0.01 || animationTime < 0.01) {
			// Passou um segundo: 
			if (frontAndBack) {
				incAnimation = -incAnimation;
				
				if (animationTime - 1 > 0.01) {
					animationTime = 1.0f;
				} else {
					animationTime = 0.0f;
				}
				
			} else {
				animationTime = 0.0f;
				
				if (incAnimation < 0) {
					incAnimation = -incAnimation;
				}
			}
		}
			
		if (messageTime > 30) {
			msg.str("");
			msg << "Animation time: ";
			msg << animationTime;		
		}
				
		light = light.rotate(lightRotor);

		glutTimerFunc(100,timerFunctionGLUT,1);   
		//glutPostRedisplay();
	}

	bool isReliefMap() {
		return baseLayer12 != NULL;
	}	

	void initShaders() {

		if (isReliefMap()) {
			shaderSimpleMultiLayer = new AnimationReliefMapShader("Fast MultiLayer", "vp-reliefmap.cg", "fp-reliefmap-multilayer-animation-fast.cg"); 
			shaderFastMultiLayer = new AnimationReliefMapShader("Perfect MultiLayer", "vp-reliefmap.cg", "fp-reliefmap-multilayer-animation-perfect.cg"); 
			shaderSingleLayer = new AnimationReliefMapShader("SingleLayer", "vp-reliefmap.cg", "fp-reliefmap-singlelayer-animation.cg");
			
			shader = shaderSingleLayer;
			shaderSingleLayer->initShader();

			shader = shaderSimpleMultiLayer;
			shaderSimpleMultiLayer->initShader();

			shader = shaderFastMultiLayer;
			shaderFastMultiLayer->initShader();

			shader = shaderSingleLayer;
		} else {
			shader = new AnimationShader("NormalMap", "vp-normalmap.cg", "fp-normalmap.cg");
			shader->initShader();
		}
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);	
	}

	void updateFPS() {
		static int frame = 0;
		static int lastTime = 0;

		frame++;
		int currentTime = glutGet(GLUT_ELAPSED_TIME);
		if ((currentTime - lastTime) > 1000.0) {

			std::ostringstream ret;
			ret << defaultTitle << " (FPS: " << frame * 1000.0 / double(currentTime - lastTime) << ")";
			glutSetWindowTitle(ret.str().c_str());
			lastTime = currentTime;
		    frame = 0;
	    }
	}

	Viewer() {
		singleLayerAnimation = 0;
		from.set(0,-1);
		from.set(1,-1);
		animationTime = 0.0f;
		messageTime = 90.0f;
		phi = RBFMultiquadric;
		suavization = 0.5;
		rbfs = NULL;
		rbfRepresentation = NULL;
		showHelp = true;
		incAnimation = 0.1;
		frontAndBack=false;
		shininess = 30;
		play = true;
		
		light.setVector(-4.0f, -150.0f, 10.0f);
		//lightRotor.setRotor(degToRad(3), 1, 1,0);

		defaultTitle = "Normal and Relief Maps Animation";
		initGlut(defaultTitle);
		init();
		
		cgSetErrorCallback(errorCG);

		glWriter = new SimpleGLWriter(width, height);
		keyViewer = new KeyFramesViewer();
		points = new PointsSerialization("points.txt", &keyViewer->frames);

		if (isReliefMap()) 
			movingObj = new MovingReliefPlane();
		else
			movingObj = new MovingReliefPlane();

		movingObj->setCameraPosition(0, 0, 100);
		movingObj->setObjPosition(20, -20, 0);
	}

	virtual void takeControl() {
		initShaders();
		glutTimerFunc(100,timerFunctionGLUT,1);
		glutMainLoop();
	}
};

void timerFunctionGLUT(int value) {
	((Viewer *)glutViewer)->timerFunction();
}

void errorCG() {
	cgError(((Viewer *)glutViewer)->shader->cg_context);
}

void initViewer (int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutViewer = new Viewer();

	setVSync(0);
}

#endif /*VIEWER_H_*/
