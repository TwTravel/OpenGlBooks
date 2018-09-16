/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Viewer.cpp - Test application to view a 3D model
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include "CommonStuff.h"
#include "Renderer.h"
#include "WinMain.h"
#include "AABB3.h"
#include "MathUtil.h"
#include "Model.h"
#include "Input.h"

void	init() {
	int	i;

	// Create the main application window

	createAppWindow("Model viewer");

	// Find the most appropriate video mode to use

	//int	modeIndex = -1;
	//for (i = 0 ; i < gRenderer.getVideoModeCount() ; ++i) {
	//	VideoMode mode = gRenderer.getVideoMode(i);
	//	if (
	//		(mode.xRes == 800) &&
	//		(mode.yRes == 600) &&
	//		((mode.bitsPerPixel == 32) || (mode.bitsPerPixel == 24))
	//	) {
	//		modeIndex = i;
	//		break;
	//	}
	//}
	//if (modeIndex < 0) {
	//	ABORT("Can't set video mode");
	//}
	//VideoMode mode = gRenderer.getVideoMode(modeIndex)

	VideoMode mode;
	mode.xRes = 800;
	mode.yRes = 600;
	mode.bitsPerPixel = 24;
	mode.refreshHz = kRefreshRateDefault;

	// Set the mode

	gRenderer.init(mode);
}

void	shutdown() {
	gRenderer.shutdown();
	destroyAppWindow();
}

void	renderCube() {

	AABB3 box;
	box.min.x = box.min.y = box.min.z = -5.0f;
	box.max = -box.min;

	RenderVertex vl[8];
	for (int i = 0 ; i < 8 ; ++i) {
		vl[i].p = box.corner(i);
		//vl[i].argb = MAKE_ARGB(255, (i & 1) ? 255 : 0, (i & 2) ? 255 : 0, (i & 4) ? 255 : 0);
		vl[i].n = vl[i].p;
		vl[i].n.normalize();
		vl[i].u = (i & 1) ? 1.0f : 0.0f;
		vl[i].v = (i & 2) ? 1.0f : 0.0f;
	}

	RenderTri pl[6*2] = {
		{ 0,4,6 },
		{ 0,6,2 },
		{ 1,3,7 },
		{ 1,7,5 },
		{ 0,1,5 },
		{ 0,5,4 },
		{ 2,6,7 },
		{ 2,7,3 },
		{ 0,2,3 },
		{ 0,3,1 },
		{ 4,5,7 },
		{ 4,7,6 },
	};

	gRenderer.renderTriMesh(vl, 8, pl, 12);
}


void	mainProgram() {

	// Setup program

	init();

	// Set the window

	gRenderer.setFullScreenWindow();

	// Set the camera a little bit south and above
	// the origin, looking slightly down and to the north

	EulerAngles cameraOrient;
	cameraOrient.heading = 0.0f;
	cameraOrient.pitch = degToRad(30.0f);
	cameraOrient.bank = 0.0f;
	gRenderer.setCamera(Vector3(0.0f, 20.0f, -40.0f), cameraOrient);
	gRenderer.setZoom(fovToZoom(degToRad(60.0f)));

	// Load model

	Model model;
	model.importS3d("ar_couch.s3d");
	model.cache();

	// Spin a cube

	EulerAngles orient = kEulerAnglesIdentity;
	while (!gQuitFlag) {

		// Get ready to draw

		gRenderer.beginScene();
		gRenderer.clear();

		// Render a cube

		gRenderer.setLightEnable(true);
		gRenderer.instance(kZeroVector, orient);
		//renderCube();
		model.render();
		gRenderer.instancePop();

		// Show it

		gRenderer.endScene();
		gRenderer.flipPages();

		// Rotate cube's heading

		orient.heading += .01f;

		// Check for ESC to exit the app

		if (gKeyboard.debounce(kKeyEsc)) {
			break;
		}

	}

	// Shutdown

	shutdown();
}
