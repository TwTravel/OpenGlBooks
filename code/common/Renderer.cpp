/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Renderer.cpp - Very simple low-level 3D renderer interface
//
// Visit gamemath.com for the latest version of this file.
//
// --------------------------------------------------------------------------
//
// This file implements a very simple 3D renderer via Direct3D.  Our goal was
// to provide a simple, cross-platform rendering engine.  If you do not want
// to learn DirectX (and there are MANY people who work in graphics related
// firlds who have no need to know DirectX) then you don't need to look
// at the inside of this file - you can just get the benefits of a simple,
// clean rendering API without digging through mountains of DirectX
// documentation.
//
// <soapbox>
//
// Nowadays lots of people want to learn Direct3D, before they know graphics.
// As is typical of Microsoft, some people act as if the two are the same,
// like DirectX is "how things are done."  Graphics is an academic pursuit
// unto itself, and many researchers spent many years investigating
// techniques and interfaces for rendering, long before Microsoft got
// involved in DirectX.  It is more important to learn how graphics work
// from a platform- and API-independent standpoint than it is to learn how
// DirectX works, with all its quirks and shortcomings.  And DirectX changes
// every year - sometimes radically.  Most serious projects do most of their
// high-level graphics work using a graphics abstraction (such as this one)
// and leave the messy details hidden in a lower level.  Certainly any
// cross-platform project must do this, but even ones only destinated for
// one platform can make use of this insulation.  Please don't put DirectX
// code anywhere in your high level code.  In fact you may notice that all
// of the files on gamemath.com can be used to make a platform-independent
// game loop, with the "game" files (the project-specific files) never
// including a big huge mess of stuff like <windows.h> or <xtl.h>.
//
// Many other books or Internet sources provide so-called "wrappers"
// around DirectX, which are designed to make it "easy" to write DirectX
// applications.  Supposedly, you can use their code without knowing any
// DirectX.  However, let us differentiate between a "wrapper" and an
// "abstraction layer."  A wrapper is a very thin translation layer,
// usually designed to do nothing more than translate calls.  It often
// can decrease compile times, facilitate DLL linkage, or provide shorthand
// for commonly used operations.  However, a "wrapper" really doesn't
// help insulate you from the implementation details any or provide any
// platform independence.  An abstraction layer, on the other hand,
// is designed to provide insulation from implementation details and/or
// platform independance.  We have provided an abstraction layer, not a
// wrapper.  This is evidenced by the fact that the header file does
// not contain a single platform independent element and could be ported
// to any number of platforms without a line of code being changed.
//
// Many wrappers or abstraction layers provided in books and on the Internet
// are useless because they are just as complicated or have as many quirks
// (or bugs) as the "messy" internals they proport to hide you from.
// (And what's more - they are usually more poorly documented!)
// Therefore, you not only have to learn DirectX, for example, but you have
// to learn their weird API as well.  We have tried to design our interface
// layer so that it provides basic graphics functionality in the most
// straightforward manner possible, truly hiding DirectX details.  Of course,
// with any interface, there is a learning curve associated with it.  We
// hope that our design and comments make this curve as short as possible.
//
// </soapbox>
//
// So, the interface is completely platform independent, and much of the
// implentation is as well.  All that said, however, eventually you have to
// impement the interface on a particular platform.  It is possible, and
// in many ways advantageous, to make this file sit atop different
// lower level layers - i.e. one for DirectX, one for OpenGL, one for
// PlayStationII, etc.  For simplicity, we have not done this.  We have
// chosen one API (Direct3D) and made calls to it directly in this file.
// This makes it basically impossible to support multiple platforms or
// APIs easily without duplciating the entire contents of the file, but the
// purpose of this code is ease of understanding the API most people are
// most interested in learning - DirectX.
//
// We have also not concerned ourselved with optimization too much.  This
// is an "engine" designed for you to look at the guts and learn how things
// work.  Optimization would complicate the internals significantly and
// make things more difficult to understand.
//
// There is only one instance of class Renderer, a global variable
// gRenderer.  (It is a "singleton" in C++ terminology.)  Some internal
// state variables are declared here as private members.  Other internal
// state variables are declared statically in the various C++ files -
// declaring all the variables here as private would be a drain on compile
// times, since it would require declaring more structures and including
// other files that are not necessary.  C++ has the unfortunate property
// that all class internal members are visible to clients of the class, even
// if they are not accessible.  In other words, they must be processed by
// the compiler, and if they are changed, seemingly unrelated files are
// often recompiled.  Putting state variables statically, rather than in
// the private section, avoids this problem.  A C++ purist might object,
// but a person waiting on the compiler will not.
//
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>

#include "CommonStuff.h"
#include "Renderer.h"
#include "WinMain.h"
#include "MathUtil.h"
#include "Bitmap.h"

#include <d3d8.h>

/////////////////////////////////////////////////////////////////////////////
//
// local data
//
/////////////////////////////////////////////////////////////////////////////

// Direct3D interface object

static LPDIRECT3D8 		pD3D = NULL;

// Direct3D device interface

static LPDIRECT3DDEVICE8 	pD3DDevice = NULL;

// List of video modes

static int		videoModeCount;
static VideoMode	*videoModeList;

// The clip matrix.  This transforms camera space points to clip
// space, aka "canonical view volume space."  it is computed
// by computeClipMatrix().  D3D calls this matrix the "projection"
// matrix

static D3DMATRIX	clipMatrix;

// Instance stack system.  This is an OpenGL-like system to manage the
// current reference frame.  For example, by default, without instancing,
// all 3D coordinates submitted to the rendering system are assumed to be
// in world space.  Now, let's say we "instance" into an object's local
// reference frame, by specifying the position and orientation of the
// object.  Now any 3D coordinates we submit will be transformed from
// local space to world space and then into camera space.  Instancing
// can be performed multiple times, for example, to render a tire within a
// car.

struct InstanceInfo {

	// The model->world matrix

	Matrix4x3	modelToWorldMatrix;
};

const int	kMaxInstanceDepth = 8;

static int		instanceStackPtr = 0;
static InstanceInfo	instanceStack[kMaxInstanceDepth];

// The model->clip matrix.  This matrix takes a point in the current
// reference frame, and transforms it to clip space.  Note that this is a
// 4x4 matrix.  We could have our own nice 4x4 matrix class, but since they
// are actually not used very much outside of the graphcis internals, we just
// use D3D's here

static D3DMATRIX	modelToClipMatrix;

// The modelToClipMatrix is only used by the software vertex processing
// routines.  it's relatively expeisive to compute.  So we won't compute
// it every time.  We'll just keep track of if it's valid or not,
// and compute it on demand whenever it is needed.  This flag keeps
// tradck of whether the modelToClipMatrix is "dirty" and needs to be
// recomputed.

static bool	needToComputeModelToClipMatrix = true;

// Model->camera matrix.  This matrix takes a point in the current
// reference frame, and transforms it to camera space.  This
// transform does not contain the zoom or perspective projection, and so
// we are using our 4x3 matrix class.  This matrix is computed when
// the modelToClipMatrix is computed

static Matrix4x3	modelToCameraMatrix;

// The current D3D "material" which controls material properties for
// the standard local illumunation model.  We will only be using a few
// of these values.

static D3DMATERIAL8 d3dMaterial;

// Floating point projection values, for software projection

static float	windowCenterX, windowCenterY;
static float	halfWindowSizeX, halfWindowSizeY;

// Precomputed far fog distance, as a clip-space z value.

static float	farFogClipSpaceZ;

// The D3D directional light

static D3DLIGHT8 d3dDirectionalLight;

// Texture cache variables.  See the notes above the resetTextureCache()
// for more details.

struct TextureCacheEntry {

	// Symbolic name of the texture (usually a filename)

	char	name[kMaxTextureNameChars];

	// Size

	int	xSize, ySize;

	// Direct3D interface object.  We're going to let D3D manage
	// the memory

	LPDIRECT3DTEXTURE8	d3dTexture;
};

// max number of textures - you may need more than this

const int	kTextureCacheSize = 512;

// next available texture slot

static int	nextTextureSlot;

// The texture cache bookeeping info

static TextureCacheEntry	textureCacheList[kTextureCacheSize];

/////////////////////////////////////////////////////////////////////////////
//
// local utility helper functions
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// setD3DRenderState
//
// Basically just a thin wrapper around pD3DDevice->SetRenderState,
// with added pointer safety check and validation of the return code,
// for debugging.
//
// For speed, we could cache the states to prevent setting states
// redundantly.  However, most of the higher level code should already be
// doing that, and hopefully Microsoft is doing the same as well.

static void	setD3DRenderState(D3DRENDERSTATETYPE state, unsigned value) {

	// Make sure device exists

	if (pD3DDevice == NULL) {
		assert(false);
		return;
	}

	// !SPEED! We could check if the state actually changed here.
	// Or, we could assume that the good folks in Redmond will
	// do this for us.  Ahem.....

	// Set the state

	HRESULT result = pD3DDevice->SetRenderState(state, value);

	// Check for error

	assert(SUCCEEDED(result));
}

//---------------------------------------------------------------------------
// setD3DDirectionalLight
//
// Install the directional light into D3D render context

static void	setD3DDirectionalLight() {

	// Make sure device exists

	if (pD3DDevice == NULL) {
		assert(false);
		return;
	}

	// Set light to D3D at light index 0

	HRESULT result = pD3DDevice->SetLight(0, &d3dDirectionalLight);

	// Check for error

	assert(SUCCEEDED(result));
}

//---------------------------------------------------------------------------
// checkMesh
//
// Debug utility function to check that a triangle mesh is valid.
//
// Returns true if the mesh is OK to be rendered

static bool checkMesh(const void *vertexList, int vertexCount, const RenderTri *triList, int triCount) {

	// No triangles?

	if (triCount < 1) {
		return false;
	}

	// NULL pointers?

	if ((vertexList == NULL) || (triList == NULL)) {
		assert(false);
		return false;
	}

	// Check in a debug build that all the indices are valid

	#ifdef _DEBUG
		for (int i = 0 ; i < triCount ; ++i) {
			for (int j = 0 ; j < 3 ; ++j) {
				int	index = triList[i].index[j];
				if ((index < 0) || (index >= vertexCount)) {
					assert(false);
					return false;
				}
			}
		}
	#endif

	// Mesh is OK

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
// class Renderer master init stuff
//
/////////////////////////////////////////////////////////////////////////////

// Global renderer class object

Renderer	gRenderer;

//---------------------------------------------------------------------------
// Renderer::Renderer
//
// Constructor - Reset internal state variables

Renderer::Renderer() {

	// Slam some internal variables

	screenX = 0;
	screenY = 0;
	cameraPos.zero();
	cameraOrient.identity();
	zoomX = 1.0f; // 90 degree field of view
	zoomY = 0.0f; // auto-compute
	nearClipPlane = 1.0f;
	farClipPlane = 1000.0f;
	windowX1 = 0;
	windowY1 = 0;
	windowX2 = 0;
	windowY2 = 0;
	windowSizeX = 0;
	windowSizeY = 0;
	depthBufferRead = true;
	depthBufferWrite = true;
	blendEnable = true;
	sourceBlendMode = eSourceBlendModeSrcAlpha;
	destBlendMode = eDestBlendModeInvSrcAlpha;
	constantARGB = MAKE_ARGB(255,0,0,0);
	constantOpacity = 1.0f;
	fogEnable = false;
	fogColor = MAKE_RGB(255,255,255);
	fogNear = 0.0f;
	fogFar = 1000.0f;
	lightEnable = true;
	ambientLightColor = MAKE_RGB(64,64,64);
	directionalLightVector.x = .707f;
	directionalLightVector.y = -.707f;
	directionalLightVector.z = 0.0f;
	directionalLightColor = MAKE_RGB(192,192,192);
	backfaceMode = eBackfaceModeCCW;
	currentTextureHandle = 0;
	textureClamp = false;

	// And now set the camera, to force some stuff to be
	// recomputed

	setCamera(kZeroVector, kEulerAnglesIdentity);

	// Set level 0 instance (the world) reference frame

	instanceStack[0].modelToWorldMatrix.identity();
}

//---------------------------------------------------------------------------
// Renderer::getVideoModeCount
//
// Returns the number of viewo modes available, or 0 if 3D is no workey

int	Renderer::getVideoModeCount() {

	// Check if we already know

	if (videoModeCount > 0) {
		return videoModeCount;
	}

	// List has not yet been created.  Nothing should be allocated yet

	assert(pD3D == NULL);
	assert(pD3DDevice == NULL);
	
	// Create a Direct3D object

	pD3D = Direct3DCreate8(D3D_SDK_VERSION);
	if (pD3D == NULL) {
		videoModeCount = 0;
		return 0;
	}

	// Enumerate the adapter modes in two passes.  On the first pass,
	// we'll just count the number of modes.  On the second pass,
	// we'll actually fill in the mode list

	for (int pass = 0 ; pass < 2 ; ++pass) {

		// Enumerate modes

		int	modeIndex = 0;
		while (modeIndex < pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT)) {

			// Enumerate the next mode.

			D3DDISPLAYMODE mode;
			HRESULT result = pD3D->EnumAdapterModes(
				D3DADAPTER_DEFAULT,
				modeIndex,
				&mode
			);
			if (FAILED(result)) {
				break;
			}
			++modeIndex;

			// Convert D3D mode structure to our own

			VideoMode ourMode;
			ourMode.xRes = mode.Width;
			ourMode.yRes = mode.Height;
			ourMode.refreshHz = mode.RefreshRate;
			switch (mode.Format) {
				case D3DFMT_A8R8G8B8:
					ourMode.bitsPerPixel = 32;
					break;

				case D3DFMT_R8G8B8:
				case D3DFMT_X8R8G8B8:
					ourMode.bitsPerPixel = 24;
					break;

				case D3DFMT_R5G6B5:
				//case D3DFMT_X1R5G5B5:
				//case D3DFMT_A1R5G5B5:
				//case D3DFMT_A4R4G4B4:
				//case D3DFMT_X4R4G4B4:
					ourMode.bitsPerPixel = 16;
					break;

				default:

					// Unknown or unrecognized mode - skip it

					continue;
			}

			// Count it, or add it to the list

			if (pass == 0) {

				// 1st pass - just count it

				++videoModeCount;
			} else {

				// 2nd pass.  Actually add it to the list.
				// First, we check for duplicates

				bool	dup = false;
				for (int i = 0 ; i < videoModeCount ; ++i) {
					VideoMode *v = &videoModeList[i];
					if (
						(v->xRes == ourMode.xRes) &&
						(v->yRes == ourMode.yRes) &&
						(v->bitsPerPixel == ourMode.bitsPerPixel) &&
						(v->refreshHz == ourMode.refreshHz)
					) {
						dup = true;
						break;
					}
				}

				// Add it, if not a dup

				if (!dup) {
					videoModeList[videoModeCount] = ourMode;
					++videoModeCount;
				}
			}

			// Keep enumerating video modes
		}

		// Any valid modes found?

		if (videoModeCount == 0) {

			// Nope - clean up and report failure.

			pD3D->Release();
			pD3D = NULL;
			return 0;
		}

		// End of first pass?

		if (pass == 0) {

			// Yep.  Allocate list.  Notice that we are
			// possibly allocating more memory than we
			// will need, because there might be duplicates.
			// We don't care about minor differences in
			// pixel formats with the same bit depth

			videoModeList = new VideoMode[videoModeCount];

			// Reset count for next pass

			videoModeCount = 0;
		}
	}

	// Return number of modes

	return videoModeCount;
}

//---------------------------------------------------------------------------
// Renderer::getVideoMode
//
// Returns a video mode, by index

const VideoMode &Renderer::getVideoMode(int index) {

	// Make sure list is built

	if (getVideoModeCount() < 1) {
		ABORT("3D support not detected.");
	}
	assert(videoModeCount > 0);
	assert(pD3D != NULL);

	// Index should be in range

	assert(index >= 0);
	assert(index < videoModeCount);

	// Return it

	return videoModeList[index];
}

//---------------------------------------------------------------------------
// Renderer::init
//
// Startup the renderer in the specified screen mode.  This must be called
// only once, at program startup.  You MUST call this before any other
// 3D calls are made.

void	Renderer::init(const VideoMode &mode) {
	HRESULT	result;

	// Make sure Direct3D interface is created

	getVideoModeCount();

	// We should have a D3D object, but not a D3D object

	assert(pD3D != NULL);
	assert(pD3DDevice == NULL);

	// Figure out actual pixel format to use for desired bit depth

	int		modeIndex = 0;
	D3DDISPLAYMODE	d3dMode;
	for (;;) {

		// Get the mode

		result = pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT,modeIndex,&d3dMode);
		++modeIndex;

		// No more modes?  Then we couldn't find an appropriate mode

		if (FAILED(result)) {
			ABORT("Can't find valid video mode for %dx%dx%dbpp", mode.xRes, mode.yRes, mode.bitsPerPixel);
		}

		// Will this mode do for what they want?

		if (d3dMode.Width != mode.xRes) continue;
		if (d3dMode.Height != mode.yRes) continue;
		if (mode.bitsPerPixel == 16) {

			if (d3dMode.Format == D3DFMT_R5G6B5) {
				break;
			}

		} else if (mode.bitsPerPixel == 24) {

			if (d3dMode.Format == D3DFMT_R8G8B8) {
				break;
			}
			if (d3dMode.Format == D3DFMT_X8R8G8B8) {
				break;
			}

		} else if (mode.bitsPerPixel == 32) {

			if (d3dMode.Format == D3DFMT_A8R8G8B8) {
				break;
			}

		} else {

			// Huh?  You are asking for an invalid bit depth

			assert(false);
		}
	}

	// Figure out Z buffer format.  We'll start by assuming
	// a 16-bit depth buffer

	D3DFORMAT depthBufferFormat = D3DFMT_D16;

	// If in higher bit depth, we'll shoot for the depth buffer
	// with the most resolution possible

	if (mode.bitsPerPixel > 16) {

		// First, try a full 32-bit z-buffer.
		// if that doesn't work, we'll try a 24-bit
		// z-buffer

		if (SUCCEEDED(pD3D->CheckDeviceFormat(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			d3dMode.Format,
			D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE,
			D3DFMT_D32
		))) {
			depthBufferFormat = D3DFMT_D32;
		} else if (SUCCEEDED(pD3D->CheckDeviceFormat(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			d3dMode.Format,
			D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE,
			D3DFMT_D24S8
		))) {
			depthBufferFormat = D3DFMT_D24S8;
		}
	}

	// Fill in the "present" parameters

	D3DPRESENT_PARAMETERS	presentParms;
	presentParms.BackBufferWidth = mode.xRes;
	presentParms.BackBufferHeight = mode.yRes;
	presentParms.Windowed = 0;
	presentParms.BackBufferFormat = d3dMode.Format;
	presentParms.MultiSampleType = D3DMULTISAMPLE_NONE;
	presentParms.EnableAutoDepthStencil = TRUE;
	presentParms.AutoDepthStencilFormat = depthBufferFormat;
	presentParms.Flags = 0;
	presentParms.hDeviceWindow = gHwndApp;
	presentParms.BackBufferCount = 2;
	presentParms.SwapEffect = D3DSWAPEFFECT_FLIP;
	if (mode.refreshHz == kRefreshRateDefault) {
		presentParms.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	} else if (mode.refreshHz == kRefreshRateFastest) {
		presentParms.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_UNLIMITED;
	} else {
		assert(mode.refreshHz > 0);
		presentParms.FullScreen_RefreshRateInHz = mode.refreshHz;
	}
	presentParms.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	// Create hardware transform device, hopefully with vertex and
	// pixel shader support.

	result = pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		gHwndApp,
		D3DCREATE_FPU_PRESERVE | D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParms,
		&pD3DDevice
	);
	if (!SUCCEEDED(result)) {

		// Nope - try falling back on software vertex processing

		result = pD3D->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			gHwndApp,
			D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&presentParms,
			&pD3DDevice
		);
		if (!SUCCEEDED(result)) {
			ABORT("Can't set video mode to %dx%dx%dbpp", mode.xRes, mode.yRes, mode.bitsPerPixel);
		}
	}

	// Remember resolution

	screenX = mode.xRes;
	screenY = mode.yRes;

	// Set a full screen window initially

	setFullScreenWindow();

	// Clear out any garbage on the screen

	for (int i = 0 ; i < 4 ; ++i) {
		beginScene();
		clear(kClearFrameBuffer | kClearDepthBuffer);
		endScene();
		flipPages();
	}

	// Set a default render state

	setD3DRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	setD3DRenderState(D3DRS_ZWRITEENABLE, TRUE);
	setD3DRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	setD3DRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	setD3DRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	setD3DRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	setD3DRenderState(D3DRS_AMBIENT, ambientLightColor);
	setD3DRenderState(D3DRS_FOGENABLE, fogEnable);
	setD3DRenderState(D3DRS_FOGCOLOR, fogColor);
	setD3DRenderState(D3DRS_FOGSTART, *(DWORD*)(&fogNear));
	setD3DRenderState(D3DRS_FOGEND, *(DWORD*)(&fogFar));
	setD3DRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// Set texture wrapping mode

	result = pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	assert(SUCCEEDED(result));
	result = pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	assert(SUCCEEDED(result));

	// Setup the material with the properties that we want

	memset(&d3dMaterial, 0, sizeof(d3dMaterial));
	d3dMaterial.Diffuse.r = 1.0f;
	d3dMaterial.Diffuse.g = 1.0f;
	d3dMaterial.Diffuse.b = 1.0f;
	d3dMaterial.Diffuse.a = 1.0f;
	d3dMaterial.Ambient = d3dMaterial.Diffuse;
	d3dMaterial.Specular = d3dMaterial.Diffuse;
	d3dMaterial.Power = 50.0f; // arbitrary

	// Force upload of the D3D material

	constantARGB ^= 0xff000000; // force alpha portion to change
	setOpacity(constantOpacity);

	// Setup directional light

	memset(&d3dDirectionalLight, 0, sizeof(d3dDirectionalLight));
	d3dDirectionalLight.Type = D3DLIGHT_DIRECTIONAL;
	d3dDirectionalLight.Diffuse.r = GET_R(directionalLightColor) / 255.0f;
	d3dDirectionalLight.Diffuse.g = GET_G(directionalLightColor) / 255.0f;
	d3dDirectionalLight.Diffuse.b = GET_B(directionalLightColor) / 255.0f;
	d3dDirectionalLight.Diffuse.a = 0.0f;
	d3dDirectionalLight.Specular = d3dDirectionalLight.Diffuse;
	d3dDirectionalLight.Ambient.r = 0.0f;
	d3dDirectionalLight.Ambient.g = 0.0f;
	d3dDirectionalLight.Ambient.b = 0.0f;
	d3dDirectionalLight.Ambient.a = 0.0f;
	d3dDirectionalLight.Direction.x = directionalLightVector.x;
	d3dDirectionalLight.Direction.y = directionalLightVector.y;
	d3dDirectionalLight.Direction.z = directionalLightVector.z;
	d3dDirectionalLight.Falloff = 1.0f;
	d3dDirectionalLight.Theta = kPi;
	d3dDirectionalLight.Phi = kPi;
	setD3DDirectionalLight();

	// Enable it

	result = pD3DDevice->LightEnable(0, TRUE);
	assert(SUCCEEDED(result));

	// Prepare the texture cache

	resetTextureCache();
}

//---------------------------------------------------------------------------
// Renderer::shutdown
//
// Called to shutdown the renderer.  This MUST be called at program shutdown
// time, and no other renderer calls may be made after that

void	Renderer::shutdown() {

	// Free texture resources

	freeAllTextures();

	// Delete Direct3D device object

	if (pD3DDevice != NULL) {
		pD3DDevice->Release();
		pD3DDevice = NULL;
	}

	// Delete Direct3D interface

	if (pD3D != NULL) {
		pD3D->Release();
		pD3D = NULL;
	}

	// Delete list of video modes

	delete [] videoModeList;
	videoModeList = NULL;
	videoModeCount = 0;
}

//---------------------------------------------------------------------------
// Renderer::flipPages
//
// Called when scene rendering is complete, to "present" the back buffer

void	Renderer::flipPages() {

	// Make sure we have a device

	if (pD3DDevice != NULL) {

		// Toggle it

		HRESULT result = pD3DDevice->Present(NULL,NULL,NULL,NULL);

		// We could lose the surface - we'll ignore this error

		assert((result == D3DERR_DEVICELOST) || SUCCEEDED(result));
	}

	// Perform frame-time processing

	idle();
}

//---------------------------------------------------------------------------
// Renderer::beginScene
//
// Called at the beginning of the scene.  You must call this before any
// rendering primitives.

void	Renderer::beginScene() {

	// Make sure we have a device

	if (pD3DDevice == NULL) {
		assert(false);
	} else {

		// Do it

		HRESULT result = pD3DDevice->BeginScene();
		assert(SUCCEEDED(result));
	}
}

//---------------------------------------------------------------------------
// Renderer::endScene
//
// Called at the end of the scene.  You must call this after all
// primitives have been rendered.

void	Renderer::endScene() {

	// Make sure we have a device

	if (pD3DDevice == NULL) {
		assert(false);
	} else {

		// Do it

		HRESULT result = pD3DDevice->EndScene();
		assert(SUCCEEDED(result));
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// class Renderer camera and reference frame maintenance functions
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Renderer::setCamera
//
// Sets the position and orientation of the camera

void	Renderer::setCamera(const Vector3 &pos, const EulerAngles &orient) {

	// Remember position and orientation

	cameraPos = pos;
	cameraOrient = orient;

	// Recompute world -> camera matrix

	worldToCameraMatrix.setupParentToLocal(cameraPos, cameraOrient);

	// Upload this to the rendering API, if we have been initted

	if (pD3DDevice != NULL) {

		// Convert our 4x3 matrix to D3D-style 4x4 matrix

		D3DMATRIX	m;
		m._11 = worldToCameraMatrix.m11;
		m._12 = worldToCameraMatrix.m12;
		m._13 = worldToCameraMatrix.m13;
		m._14 = 0.0f;

		m._21 = worldToCameraMatrix.m21;
		m._22 = worldToCameraMatrix.m22;
		m._23 = worldToCameraMatrix.m23;
		m._24 = 0.0f;

		m._31 = worldToCameraMatrix.m31;
		m._32 = worldToCameraMatrix.m32;
		m._33 = worldToCameraMatrix.m33;
		m._34 = 0.0f;

		m._41 = worldToCameraMatrix.tx;
		m._42 = worldToCameraMatrix.ty;
		m._43 = worldToCameraMatrix.tz;
		m._44 = 1.0f;

		// Tell D3D about it

		HRESULT result = pD3DDevice->SetTransform(D3DTS_VIEW, &m);
		assert(SUCCEEDED(result));
	}

	// The model->clip matrix must be recomputed, next time we need it

	needToComputeModelToClipMatrix = true;
}

//---------------------------------------------------------------------------
// Renderer::setZoom
//
// Sets the horizontal and vertical zoom factors.  You may specify the two
// values manually, or you can specify only one value, and have the other
// value be automatically computed according to the porportions of the
// window.  Pass 0.0 to mean "auto-compute this value."  You must specify
// at least one zoom value - you can't "auto-compute" both values.

void	Renderer::setZoom(float xZoom, float yZoom) {

	// Remember zoom values

	zoomX = xZoom;
	zoomY = yZoom;

	// Re-compute the clip matrix.

	computeClipMatrix();
}

//---------------------------------------------------------------------------
// Renderer::setNearFarClippingPlanes
//
// Sets the near and far clippping plane distances.  Setting the proper
// distance is important because it effects depth buffering resolution.
// Remember that the z-buffer only has limited precision.  The near and
// far clipping planes are used to scale the depth values in order to
// maximize the resolution.  You should set the far clipping plane
// to a reasonable maximum length, say twice the farthest thing you
// expect to draw in your scene.  The near clipping plane is
// necessary to prevent divide by zero in the perspective division.
// A reasonable value might be 1 foot or 1 inch or so.

void	Renderer::setNearFarClippingPlanes(float n, float f) {

	// Sanity check

	assert(n > 0.0f);
	assert(f > n);

	// Remember values

	nearClipPlane = n;
	farClipPlane = f;

	// Re-compute the clip matrix.

	computeClipMatrix();
}

//---------------------------------------------------------------------------
// Renderer::setNearFarClippingPlanes
//
// Sets the 2D output viewport window.  Coordinates are in screen space
// (i.e. pixels.)

void	Renderer::setWindow(int x1, int y1, int xSize, int ySize) {

	// Sanity check.

	assert(xSize > 0);
	assert(ySize > 0);

	// Remember window values

	windowX1 = x1;
	windowY1 = y1;
	windowSizeX = xSize;
	windowSizeY = ySize;
	windowX2 = x1 + xSize;
	windowY2 = y1 + ySize;

	// Compute values for software projection

	halfWindowSizeX = (float)windowSizeX / 2.0f;
	halfWindowSizeY = (float)windowSizeY / 2.0f;
	windowCenterX = (float)windowX1 + halfWindowSizeX;
	windowCenterY = (float)windowY1 + halfWindowSizeY;

	// Upload to the direct3D device, if we are ready

	if (pD3DDevice != NULL) {
		D3DVIEWPORT8 viewData;
		viewData.X = x1;
		viewData.Y = y1;
		viewData.Width  = xSize;
		viewData.Height = ySize;
		viewData.MinZ = 0.0F;
		viewData.MaxZ = 1.0F;
		HRESULT result = pD3DDevice->SetViewport(&viewData);
		assert(SUCCEEDED(result));
	}

	// Since the window proportions may have changed, the clip
	// matrix may need to change.

	computeClipMatrix();
}

//---------------------------------------------------------------------------
// Renderer::setFullScreenWindow
//
// Sets the window to the full screen.  This is really just shorthand
// for a very common operation.

void	Renderer::setFullScreenWindow() {
	setWindow(0, 0, screenX, screenY);
}

//---------------------------------------------------------------------------
// Renderer::getWindow
//
// Accessor to get the window dimensions

void	Renderer::getWindow(int *x1, int *y1, int *xSize, int *ySize) {

	// Return whatever options they request

	if (x1 != NULL) *x1 = windowX1;
	if (y1 != NULL) *y1 = windowY1;
	if (xSize != NULL) *xSize = windowSizeX;
	if (ySize != NULL) *ySize = windowSizeY;
}

//---------------------------------------------------------------------------
// Renderer::instance
//
// "Instance" a reference frame, nested within the current modeling space.
// This hails from OpenGL.  See the comments at the top of this file
// regarding the nesting system and its purpose.

void	Renderer::instance(const Vector3 &pos, const EulerAngles &orient) {

	// Make sure we aren't full

	assert(instanceStackPtr < kMaxInstanceDepth-1);

	// Get shortcut to destination entry in the stack

	++instanceStackPtr;
	InstanceInfo *d = &instanceStack[instanceStackPtr];

	// Check for a very special common case - the first level.
	// We can avoid some matrix computations in this case.

	if (instanceStackPtr == 1) {

		// Compute model to world matrix directly.  We
		// assume that the parent->world matrix is identity

		d->modelToWorldMatrix.setupLocalToParent(pos, orient);
	} else {

		// We are nested and must concatenate.  First
		// we compute the local->parent matrix

		Matrix4x3 localToParent;
		localToParent.setupLocalToParent(pos, orient);

		// Concatenate with the parent->world matrix

		d->modelToWorldMatrix = localToParent * instanceStack[instanceStackPtr-1].modelToWorldMatrix;
	}

	// Update world matrix to D3D

	updateModelToWorldMatrix();
}
	
//---------------------------------------------------------------------------
// Renderer::instancePop
//
// "Pop" the instance stack one level - restoring the reference frame to the
// parent.

void	Renderer::instancePop() {

	// Make sure stack isn't empty

	assert(instanceStackPtr > 0);

	// Pop stack

	--instanceStackPtr;

	// Update world matrix to D3D

	updateModelToWorldMatrix();
}

/////////////////////////////////////////////////////////////////////////////
//
// class Renderer render context functions
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Renderer::setDepthBufferMode
//
// Enable/disable reading and/or writing of the depth buffer.
//
// When depth buffering is disabled, pixels are written regardless of
// any previous values in the depth buffer.
//
// When depth writing is disabled, the depth buffer is not updated, even
// when a color value is written to the frame buffer.
//
// Default operation is for both reading and writing to be enabled

void	Renderer::setDepthBufferMode(bool readEnabled, bool writeEnabled) {

	// Check if read mode changing

	if (depthBufferRead != readEnabled) {
		depthBufferRead = readEnabled;
		setD3DRenderState(D3DRS_ZFUNC, readEnabled ? D3DCMP_LESSEQUAL : D3DCMP_ALWAYS);
	}

	// Check if write mode changing

	if (depthBufferWrite != writeEnabled) {
		depthBufferWrite = writeEnabled;
		setD3DRenderState(D3DRS_ZWRITEENABLE, writeEnabled);
	}
}

//---------------------------------------------------------------------------
// Renderer::setBlendEnable
//
// Enable/disable alpha blending.  Why would you want to do disable alpha
// blending?
//
// First, depending on the rendering hardware, it may be faster to render
// without alpha blending, even if you know that it would not make a visual
// difference.
//
// Second, you may wish to render with textures that contain alpha, or with
// alpha in the vertices, and you wish to ignore that alpha data.

void	Renderer::setBlendEnable(bool blend) {

	// Check if state changing

	if (blendEnable != blend) {

		// Remember new state

		blendEnable = blend;

		// Set state to D3D

		setD3DRenderState(D3DRS_ALPHABLENDENABLE, blend);
	}
}

//---------------------------------------------------------------------------
// Renderer::setSourceBlendMode
// Renderer::setDestBlendMode
//
// Set the source/dest blend factors to use when alpha blending.  Remember,
// when a pixel is going to be written to the frame buffer, it's value is
// computed as:
//
//	(sf*sc)  +  (df*dc)
//
// Where:
//
//	sc	is the "source" color.  That is, the color of the pixel
//		on the polygon after it has been lit and fogged.
//	dc	is the "destination" color.  This is the color that is
//		already in the frame buffer - the color that we are "blending
//		on top of."
//	sf/df	are the source and destination blend factors, respectively.
//		These values are actually ARGB colors, not simple scalars.
//		However, they often are "greyscale values" based on the same
//		scalar used in all channels.  They can either be a constant,
//		such as 0 or 1, or they may be computed per pixel based on
//		a number of different factors.  The most frequently used
//		factor to use is the source alpha.
//
// A few examples of common source/dest combinations:
//
// 
//	eSourceBlendModeSrcAlpha, eDestBlendModeInvSrcAlpha
//
//	"Normal" alpha blending, where the alpha channel basically
//	controls the "opacity" of the source, and the final color is
//	a simple linear interpolation between the source and destination
//	colors.
//
//
//	eSourceBlendModeSrcAlpha, eDestBlendModeOne
//
//	"Additive" blending.  The source color (times alpha) is added to the
//	destination.  This is useful for special effects like fires, halos,
//	lens flares, explosions, etc.  Alpha acts as a "brightness control"
//
//
//	eSourceBlendModeZero, eDestBlendModeSrcColor
//
//	"Multiplicative" blending.  The source and destination colors are
//	multiplied together to produce the final output color.  Alpha is
//	ignored.  This is often used for multi-pass lightmapping, where
//	one pass is the diffuse texture, and another texture controls
//	lighting on a per-texel basis.
//
//
// We have not provided enums for all the possible blending modes
// supported by DirectX.  We have done this for a couple or reasons.
// First, this engine is a learning engine.  We have limited the options
// in order to simplify.  Second, many of the options are not portable.
// Some graphics cards don't support all the combinations.  (I.e. the
// PS2 cannot do multiplicative blending.)  Third, many of the options
// aren't that useful for comon purposes.  We have tried to provide
// the most useful modes, and omitted the more "advanced" or "esoteric"
// modes.

void	Renderer::setSourceBlendMode(ESourceBlendMode mode) {

	// Check if state changing

	if (sourceBlendMode != mode) {

		// Remember new state

		sourceBlendMode = mode;

		// Map our enum to D3D enum

		D3DBLEND b;
		switch (mode) {
			default:
				assert(false);
			case eSourceBlendModeSrcAlpha:
				b = D3DBLEND_SRCALPHA;
				break;
			case eSourceBlendModeOne:
				b = D3DBLEND_ONE;
				break;
			case eSourceBlendModeZero:
				b = D3DBLEND_ZERO;
				break;
		}

		// Set state to D3D

		setD3DRenderState(D3DRS_SRCBLEND, b);
	}
}

void	Renderer::setDestBlendMode(EDestBlendMode mode) {

	// Check if state changing

	if (destBlendMode != mode) {

		// Remember new state

		destBlendMode = mode;

		// Map our enum to D3D enum

		D3DBLEND b;
		switch (mode) {
			default:
				assert(false);
			case eDestBlendModeInvSrcAlpha:
				b = D3DBLEND_INVSRCALPHA;
				break;
			case eDestBlendModeOne:
				b = D3DBLEND_ONE;
				break;
			case eDestBlendModeZero:
				b = D3DBLEND_ZERO;
				break;
			case eDestBlendModeSrcColor:
				b = D3DBLEND_SRCCOLOR;
				break;
		}

		// Set state to D3D

		setD3DRenderState(D3DRS_DESTBLEND, b);
	}
}

//---------------------------------------------------------------------------
// Renderer::setRGB
//
// Sets the global constant color, which is used for 2D primitives.  We do
// not use constant color to shade 3D polygons.  You must use prelit
// polygons for that, and put the color in the vertices.
// 
// Any alpha int he input is ignored, and the global constant alpha is not
// unchanged.

void	Renderer::setRGB(unsigned int rgb) {

	// Mask off alpha value in the input color, if any alpha
	// was specified.

	rgb &= 0x00ffffff;

	// Set RGB portion, leaving alpha portion unchanged

	constantARGB = (constantARGB & 0xff000000) | rgb;
}

//---------------------------------------------------------------------------
// Renderer::setARGB
//
// Sets the global constant color and opacity simulataneously

void	Renderer::setARGB(unsigned argb) {

	// Set const alpha value using floating point function

	setOpacity(GET_A(argb) / 255.0f);

	// Slam value

	constantARGB = argb;
}

//---------------------------------------------------------------------------
// Renderer::setOpacity
//
// Sets the global constant alpha value.  This is used for 2D primitives,
// and for 3D primitives than do not specify a color in the vertex.  For any 3D
// primitives that specify a color in the vertex (such as those using
// RenderVertexL) the alpha comes directly from the vertex.

void	Renderer::setOpacity(float a) {

	// Sanity check range

	assert(a >= 0.0f);
	assert(a <= 1.0f);

	// Snap to nearest value of 255

	int	newAlpha = (int)floor(a * 255.0f + .5f);

	// Check if value not changing

	if (newAlpha == GET_A(constantARGB)) {
		return;
	}

	// Remember value in floating point and in the
	// alpha bits of the global constant color

	constantOpacity = (float)newAlpha / 255.0f;
	constantARGB = (newAlpha << 24) | (constantARGB & 0x00ffffff);

	// Set the alpha values in the D3D Material

	d3dMaterial.Diffuse.a = constantOpacity;
	d3dMaterial.Ambient.a = constantOpacity;
	d3dMaterial.Specular.a = constantOpacity;

	// Tell D3D

	if (pD3DDevice == NULL) {
		assert(false);
		return;
	}
	HRESULT result = pD3DDevice->SetMaterial(&d3dMaterial);
	assert(SUCCEEDED(result));
}

//---------------------------------------------------------------------------
// Renderer::setFogEnable
//
// Enable of disable fog.

void	Renderer::setFogEnable(bool flag) {

	// Check if state changing

	if (fogEnable != flag) {

		// Remember new state

		fogEnable = flag;

		// Set state to D3D

		setD3DRenderState(D3DRS_FOGENABLE, flag);
	}

}

//---------------------------------------------------------------------------
// Renderer::setFogColor
//
// Sets the fog color.  That is, the color that objects fade towards as they
// are fogged out.

void	Renderer::setFogColor(unsigned rgb) {

	// Mask off alpha value in the input color, if any alpha
	// was specified.

	rgb &= 0x00ffffff;

	// Check if state changing

	if (fogColor != rgb) {

		// Remember new state

		fogColor = rgb;

		// Set state to D3D

		setD3DRenderState(D3DRS_FOGCOLOR, rgb);
	}
}

//---------------------------------------------------------------------------
// Renderer::setFogDistance
//
// Sets the fog distances:
//
//	nearFog		The distance where the fog effect starts.  Objects
//			within this distance receive no fog effect.
//	farFog		The distance where objects are completely fogged
//			out.

void	Renderer::setFogDistance(float nearFog, float farFog) {

	// Check if near value changing

	if (nearFog != fogNear) {

		// Yes - remember new value

		fogNear = nearFog;

		// Update to D3D

		setD3DRenderState(D3DRS_FOGSTART, *(DWORD*)(&nearFog));
	}

	// Check if far value changing

	if (farFog != fogFar) {

		// Yes - remember new value

		fogFar = farFog;

		// Update to D3D

		setD3DRenderState(D3DRS_FOGEND, *(DWORD*)(&farFog));

		// Re-compute the clip matrix.  We pre-compute
		// the far value in clip coordinates for outcode
		// testing.  This value is computed in this function.

		computeClipMatrix();
	}
}

//---------------------------------------------------------------------------
// Renderer::setAmbientLightColor
//
// Sets the ambient light color.  When lighting is enabled, this is the
// light color that is always present, even when no other light is
// shining on the object.
//
// Usually, it is a relatively dark greyscale value, such as (64,64,64)
//
// See Section 15.4 for more on lighting.

void	Renderer::setAmbientLightColor(unsigned rgb) {

	// Mask off alpha value in the input color, if any alpha
	// was specified.

	rgb &= 0x00ffffff;

	// Check if state changing

	if (ambientLightColor != rgb) {

		// Remember new state

		ambientLightColor = rgb;

		// Set state to D3D

		setD3DRenderState(D3DRS_AMBIENT, rgb);
	}
}

//---------------------------------------------------------------------------
// Renderer::setDirectionalLightVector
//
// Controls the direction of the directional light.  The input vector
// is a unit vector in inertial space which points in the direction that
// the light is "facing."  In other words, it points away from the light,
// not towards the light.  (This is the opposite of the l vector.)  E.g.
// at high noon, you could use (0, -1, 0) for the sun.
//
// See Section 15.4 for more on lighting.

void	Renderer::setDirectionalLightVector(const Vector3 &v) {

	// Sanity check - the input vector should be normalized

	assert(fabs(vectorMag(v) - 1.0f) < .001f);

	// Check if state changing

	if (directionalLightVector != v) {

		// Remember new state

		directionalLightVector = v;

		// Set light vector in D3D light struct

		d3dDirectionalLight.Direction.x = directionalLightVector.x;
		d3dDirectionalLight.Direction.y = directionalLightVector.y;
		d3dDirectionalLight.Direction.z = directionalLightVector.z;

		// Set it to D3D

		setD3DDirectionalLight();
	}
}

//---------------------------------------------------------------------------
// Renderer::setDirectionalLightColor
//
// Controls the color of the directional light.
//
// See Section 15.4 for more on lighting.

void	Renderer::setDirectionalLightColor(unsigned rgb) {

	// Mask off alpha value in the input color, if any alpha
	// was specified.

	rgb &= 0x00ffffff;

	// Check if state changing

	if (directionalLightColor != rgb) {

		// Remember new state

		directionalLightColor = rgb;

		// Put color into diffuse and specular channels of
		// D3D light struct

		d3dDirectionalLight.Diffuse.r = GET_R(directionalLightColor) / 255.0f;
		d3dDirectionalLight.Diffuse.g = GET_G(directionalLightColor) / 255.0f;
		d3dDirectionalLight.Diffuse.b = GET_B(directionalLightColor) / 255.0f;
		d3dDirectionalLight.Specular = d3dDirectionalLight.Diffuse;

		// Set it to D3D

		setD3DDirectionalLight();
	}
}

//---------------------------------------------------------------------------
// Renderer::setLightEnable
//
// Enable/disable lighting calculations for 3D primitives that do not
// specify a color explicitly.  (i.e. primitives using RenderVertex vertices.)
// This has no effect on 2D primitives or prelit primites.
//
// When lighting is enabled, the current lighting context (ambient light,
// directional light vector, etc.) is used in conjunction with the
// vertex normal to compute a lighting value.
//
// When lighting is disabled, the primitive is drawn "unlit" - i.e. fully
// bright, as if the result of the lighting equation was a white color.
// 
// In either case, the opacity comes from the global constant opacity.

void	Renderer::setLightEnable(bool flag) {

	// Remember new state

	lightEnable = flag;

	// Note: we not apply this state directly to D3D.
	// For some reason, D3D allows you to "enable" lighting,
	// even if your vertices are already prelit and don't
	// have a normal.  Sort of stupid.  So we must manually
	// control the lighting flag on a per-primitive basis
}

//---------------------------------------------------------------------------
// Renderer::setBackfaceMode
//
// Controls culling of primitives based on clockwise/counterclockwise
// enumeration of vertices in screen space.
//
// eBackfaceModeCCW	Polygons with vertices enumerated counter-clockwise
//			are culled.  This is the default, since we normally
//			list our vertices clockwise around the polygon.
//
// eBackfaceModeCW	Polygons with vertices enumerated clockwise are
//			culled.  This can be useful if we have "mirrored"
//			an object, or the view, since the faces are flipped
//			inside out.
//
// eBackfaceModeDisable	No polygons are culled based on vertex order.

void	Renderer::setBackfaceMode(EBackfaceMode mode) {

	// Check if state changing

	if (backfaceMode != mode) {

		// Remember new state

		backfaceMode = mode;

		// Map our enum to D3D enum

		D3DCULL c;
		switch (mode) {
			default:
				assert(false);
			case eBackfaceModeCCW:
				c = D3DCULL_CCW;
				break;
			case eBackfaceModeCW:
				c = D3DCULL_CW;
				break;
			case eBackfaceModeDisable:
				c = D3DCULL_NONE;
				break;
		}

		// Set state to D3D

		setD3DRenderState(D3DRS_CULLMODE, c);
	}

}

//---------------------------------------------------------------------------
// Renderer::selectTexture
//
// Select a texture into the rendering context.  The texture is selected
// by handle

void	Renderer::selectTexture(int handle) {

	// Sanity check handle validity

	if (handle < 1 || handle >= nextTextureSlot) {
		assert(false);
		return;
	}

	// Check if state is changing

	if (handle != currentTextureHandle) {

		// Get shortcut to structure

		TextureCacheEntry *t = &textureCacheList[handle];

		// Make sure texture exists

		if (t->d3dTexture == NULL) {
			assert(false);
			return;
		}

		// Remember new state

		currentTextureHandle = handle;

		// Select it in D3D

		assert(pD3DDevice != NULL);
		HRESULT result = pD3DDevice->SetTexture(0, t->d3dTexture);
		assert(SUCCEEDED(result));
	}
}

//---------------------------------------------------------------------------
// Renderer::selectTexture
//
// Select a texture into the rendering context using a TextureReference.
// This interface can be more conveinent to use in the very common
// case where the texture is in a file - your upper level code doesn't
// have to keep track of the filename and the handle seperately - 
// they are stored together.
//
// This function can be called on a texture that is not yet cached.
// in this case, the texture is "demand cached."  However, for best
// performance, you should always cache all textures for before rendering

void	Renderer::selectTexture(TextureReference &texture) {

	// First, make sure the texture is cached in the graphics system,
	// and the handle is correct.  Notice that if the texture is
	// already cached, then this should return very quickly.

	cacheTexture(texture);
	
	// Now, the handle is correct, and we can select the texture by
	// handle.

	selectTexture(texture.handle);
}

//---------------------------------------------------------------------------
// Renderer::setTextureClamp
//
// Enable/disbale texture clamping.  When textures are clamped, UV values
// outside the 0...1 range are clamped, effectively "streaking" the outer
// pixel borders outward.  When textures are not clamp, they are "tiled,"
// using only the fractional portion of the UV coordinates and discarding
// the numbers to the left of the decimal point.

void	Renderer::setTextureClamp(bool flag) {
	HRESULT result;

	// Safety check - make sure we have a device

	if (pD3DDevice == NULL) {
		assert(false);
		return;
	}

	// Check if state changing

	if (textureClamp != flag) {

		// Remember new state

		textureClamp = flag;

		// Figure out D3D addressing mode to use

		D3DTEXTUREADDRESS m = flag ? D3DTADDRESS_CLAMP : D3DTADDRESS_WRAP;

		// Set state to D3D for both U and V

		result = pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, m);
		assert(SUCCEEDED(result));
		result = pD3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, m);
		assert(SUCCEEDED(result));
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// class Renderer rendering primitives
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Renderer::clear
//
// Clear the screen and/or frame buffer

void	Renderer::clear(int options) {

	// Make sure we have a device

	if (pD3DDevice == NULL) {
		assert(false);
		return;
	}

	// Did they request to clear anything?  I suppose you
	// could call this function without requesting to clear anything,
	// but this is most likely a bug...

	if (!(options & (kClearFrameBuffer | kClearDepthBuffer))) {
		assert(false);
		return;
	}

	// Figure out what color to use.  By default,
	// we use black

	unsigned frameBufferARGB = MAKE_ARGB(0,0,0,0);
	if (options & kClearToConstantColor) {
		frameBufferARGB = constantARGB;
		assert(!(options & kClearToFogColor));
	} else if (options & kClearToFogColor) {
		frameBufferARGB = fogColor;
	}

	// Always slam alpha to zero, if we have alpha in the frame buffer

	frameBufferARGB &= 0x00ffffff;

	// Figure out what to clear

	DWORD	clearWhat = 0;
	if (options & kClearFrameBuffer) {
		clearWhat |= D3DCLEAR_TARGET;
	}
	if (options & kClearDepthBuffer) {
		clearWhat |= D3DCLEAR_ZBUFFER;
	}

	// Clear it

	HRESULT result = pD3DDevice->Clear(
		0,
		NULL,
		clearWhat,
		frameBufferARGB,
		1.0F,
		0
	);
	assert(SUCCEEDED(result));
}

//---------------------------------------------------------------------------
// Renderer::renderTriMesh
//
// Render a triangle mesh, given vertex data in various formats.
//
// The input is a vertex list, and a triangle list.  The vertex list contains
// vertex data in various formats.  (See below.)  The triangle list is a list
// of indices into the vertex list.
//
// A few performance problems with this code:
//
// 1.	The use of DrawIndexedPrimitiveUP, where "UP" stands for "User
//	pointer."  For "ad-hoc" dynamic geometry (i.e. that which is generated
//	procedurally on the frame, this isn't a major problem.  However,
//	for "fixed" geometry that does not change from frame to frame - such
//	as models, static scenery, etc, it would be much better to use
//	vertex buffers.  But that requires exposing much more of the internals
//	to the higher level code than we would want for this "learning"
//	environment.
//
// 2.	We set the shader and lighting mode for every primitive.  Let's
//	hope DirectX is smart enough to not stall needlessly.

void	Renderer::renderTriMesh(const RenderVertex *vertexList, int vertexCount, const RenderTri *triList, int triCount) {
	HRESULT result;

	// Make sure we have something to render

	if (!checkMesh(vertexList, vertexCount, triList, triCount)) {
		return;
	}

	// Make sure we have a device

	if (pD3DDevice == NULL) {
		assert(false);
		return;
	}

	// Enable lighting, if user has enabled it

	setD3DRenderState(D3DRS_LIGHTING, lightEnable);

	// Set the vertex shader using a flexible vertex format

	result = pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
	assert(SUCCEEDED(result));

	// Render it using "user pointer" data

	result = pD3DDevice->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,
		0,
		vertexCount,
		triCount,
		triList,
		D3DFMT_INDEX16,
		vertexList,
		sizeof(vertexList[0])
	);
	assert(SUCCEEDED(result));
}

void	Renderer::renderTriMesh(const RenderVertexL *vertexList, int vertexCount, const RenderTri *triList, int triCount) {
	HRESULT result;

	// Make sure we have something to render

	if (!checkMesh(vertexList, vertexCount, triList, triCount)) {
		return;
	}

	// Make sure we have a device

	if (pD3DDevice == NULL) {
		assert(false);
		return;
	}

	// These are pre-lit vertices.  Disable D3D lighting

	setD3DRenderState(D3DRS_LIGHTING, FALSE);

	// Set the vertex shader using a flexible vertex format

	result = pD3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	assert(SUCCEEDED(result));

	// Render it using "user pointer" data

	result = pD3DDevice->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,
		0,
		vertexCount,
		triCount,
		triList,
		D3DFMT_INDEX16,
		vertexList,
		sizeof(vertexList[0])
	);
	assert(SUCCEEDED(result));
}

void	Renderer::renderTriMesh(const RenderVertexTL *vertexList, int vertexCount, const RenderTri *triList, int triCount) {
	HRESULT result;

	// Make sure we have something to render

	if (!checkMesh(vertexList, vertexCount, triList, triCount)) {
		return;
	}

	// Make sure we have a device

	if (pD3DDevice == NULL) {
		assert(false);
		return;
	}

	// These are pre-lit vertices.  Disable D3D lighting

	setD3DRenderState(D3DRS_LIGHTING, FALSE);

	// Set the vertex shader using a flexible vertex format

	result = pD3DDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	assert(SUCCEEDED(result));

	// Render it using "user pointer" data

	result = pD3DDevice->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST,
		0,
		vertexCount,
		triCount,
		triList,
		D3DFMT_INDEX16,
		vertexList,
		sizeof(vertexList[0])
	);
	assert(SUCCEEDED(result));
}

//---------------------------------------------------------------------------
// Renderer::dot
//
// Plots a 2D dot at the given integer coordinates using the current constant
// color (including alpha) and blending state.  There is no lighting, fog,
// or z-buffering.

void	Renderer::dot(int x, int y) {

	// !KLUDGE! Just draw a 1x1 box.  Hey, it works!

	boxFill(x, y, x+1, y+1);
}

//---------------------------------------------------------------------------
// Renderer::line
//
// Plots an arbitrarily-oriented line from (x1,y1) to (x2,y2), using the
// current constant color (including alpha) and blending state.  There is no
// lighting, fog, or z-buffering.

void	Renderer::line(int x1, int y1, int x2, int y2) {

	// !FIXME!

	assert(false);
}

//---------------------------------------------------------------------------
// Renderer::boxFill
//
// Plots a filled rectangle.  The x2 and y2 coordinates specify "one past"
// the last pixel to be plotted.  (Similar to the Win32 convention.)  In
// other words, the width of the rectangle is given by x2-x1.  Some people
// find this convention to be "weird" or "off by one."  However, it is
// actually much more elegant and usually avoids countless +1's and -1's that
// eventually cancel each other out.  This also more closely matches how
// it would work if floating point coordinates were used.

void	Renderer::boxFill(int x1, int y1, int x2, int y2) {

	// Clip to window

	if (x1 < windowX1) {
		x1 = windowX1;
	}
	if (y1 < windowY1) {
		y1 = windowY1;
	}
	if (x2 > windowX2) {
		x2 = windowX2;
	}
	if (y2 > windowY2) {
		y2 = windowY2;
	}

	// Check for empty rectangle

	if (x1 >= x2 || y1 >= y2) {
		return;
	}

	// !FIXME!

	assert(false);
}

void	Renderer::videoSave() {
}

void	Renderer::videoRestore() {
}

/////////////////////////////////////////////////////////////////////////////
//
// class Renderer texture cache functions
//
//---------------------------------------------------------------------------
//
// Some notes about the design of the texture cache:
//
// The texture cache is not actually a "cache" per se, since no texture
// is ever automatically ejected from the cache.  It's actually just a
// "manager" which keeps track of all the textures that are loaded.
//
// Each texture in the cache is accessible by a "handle."  This is a simple
// integer value that the upper level code knows nothing about, other than
// zero is never returned as a texture handle.
//
// In addition, textures may be given a text "name."  If a texture does
// not have a name, it is "anonymous" and is only accessible via handle.
// All named textures must have a unique name - there will never be two
// textures with the same name in the cache.  Use the findTexture()
// function to search the cache for a texture with a given name and
// retreive its handle.  Be warned that this function is slow.  Texture
// names are never case sensitive.
//
// A special texture always exists in the cache with the handle
// kWhiteTexture.  This texture is a solid white texture, very useful
// for rendering debugging objects, etc.  It effectively can be used
// to render an "untextured" object.  (For simplicity, we didn't
// support "untextured" rendering - you always must use a texture.)
//
// You can create a texture manually by calling allocTexture() and then
// setTextureImage().  This way, textures can be made procedurally in code.
// Or, you can load a texture from disk using the cacheTexture() function.
// For disk textures, the "name" of the texture is the exact same
// as the filename (including any path and extension.)
//
// You select a texture into the rendering context by handle.  You
// may re-allocate, free, or otherwise manipulate the currently selected
// texture.  However, in this case, the graphics system may deselect your
// texture and select another texture (like the white texture).
//
// The "name" and "handle" interfaces provide a simple and relatively flexible
// low-level interface to texture cache.  A much easier but less flexible
// interface is also provided using the TextureReference structure.  This
// interface is the "lazy man's" interface.  The TextureReference struct
// keeps track of the texture name and handle in a single struct.  What's
// more, you don't even have to maintain the handle yourself - the graphics
// system will do everything for you.  All you have to do is fill in the
// name, and then you can cache or select the texture easily.  In fact,
// you don't even have to cache the texture before rendering.  When you
// select the texture into the rendering context, if it isn't already
// selected, the graphics system will automatically "demand cache" it
// for you.  This is not the best for performance, but it will always
// function properly.
//
// Note that to use TextureReference efficiently, they must be kept around
// between frames.  Otherwise, the texture must be located by name every
// time it is used - which is going to be slow.  For example, don't do this:
//
//	void	renderSomething() {
//
//		// Fill in local texture structure
//
//		TextureReference tex;
//		strcpy(tex.name, "mytexture.tga");
//
//		// Select the texture.  YIKES!  This works, but will be
//		slow.
//
//		gRenderer.selectTexture(tex);
//
//		// ... render something here using that texture
//	}
//
// For best performance, keep the texture reference around persistently,
// either as a class member, global variable, or static local variable.
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Renderer::resetTextureCache
//
// Resets texture cache to default state - only containing a single,
// white texture.

void	Renderer::resetTextureCache() {

	// Free all textures

	freeAllTextures();

	// Create a small white texture

	const int	kSz = 32;
	int	whiteTextureHandle = allocTexture("white", kSz, kSz);

	// It should have gone into the reserved "white texture" slot

	assert(whiteTextureHandle == kWhiteTexture);

	// Set the data

	unsigned *whiteImage = new unsigned[kSz*kSz];
	memset(whiteImage, 0xff, kSz*kSz*4);
	setTextureImage(whiteTextureHandle, whiteImage);
	delete [] whiteImage;

	// Select it

	currentTextureHandle = 0;
	selectTexture(kWhiteTexture);
}

//---------------------------------------------------------------------------
// Renderer::findTexture
//
// Locates a texture, by name.  Returns the handle to the texture, or 0
// if not found.  The search is not case sensitive.  "Anonymous" textures
// (those with no name) cannot be located with this function.

int	Renderer::findTexture(const char *name) {

	// Can't search using an empty name.

	assert(name != NULL);
	assert(name[0] != '\0');

	// Search list linearly.  We could implement some sort
	// of hash table or binary search to make this fast,
	// but this function probably won't be called in speed
	// critical situations, so we're going to keep it simple.

	for (int i = 1 ; i < nextTextureSlot ; ++i) {
		if (stricmp(name, textureCacheList[i].name) == 0) {
			return i;
		}
	}

	// Not found

	return 0;
}

//---------------------------------------------------------------------------
// Renderer::allocTexture
//
// Creates a texture and optionally gives it a name.  If the name
// is specified, then it must be unique - any texture currently
// existing with that name is replaced, and the same handle is returned.
//
// If no name is specified, then the texture is "anonymous" and may only
// be accessed by handle.
//
// The return value is a handle that you can use to reference the texture.
// Texture handles are always > 0.
//
// Currently, this function never returns failure - if there is a problem,
// it will abort the program.

int	Renderer::allocTexture(const char *name, int xSize, int ySize) {
	TextureCacheEntry *t;

	// Can't do this without a device

	assert(pD3DDevice != NULL);

	// Verify texture size.  We require a power of two,

	const kMaxTextureSz = 4096;
	assert(xSize > 0);
	assert(xSize <= kMaxTextureSz);
	assert((xSize & (xSize-1)) == 0); // make sure it's a power of two
	assert(ySize > 0);
	assert(ySize <= kMaxTextureSz);
	assert((ySize & (ySize-1)) == 0); // make sure it's a power of two

	// Did they specify a name?

	int	slot = 0;
	if ((name != NULL) && (name[0] != '\0')) {
		slot = findTexture(name);
	}

	// Replace existing slot?

	if (slot > 0) {
		t = &textureCacheList[slot];

		// Already allocated?

		if (t->d3dTexture != NULL) {

			// In the right size?

			if ((t->xSize == xSize) && (t->ySize == ySize)) {

				// Already ready already

				return slot;
			}

			// Exists, but in the wrong size.
		}
	} else {

		// Need a new slot.  First, we'll search for an empty slot

		for (int i = 1 ; i < nextTextureSlot ; ++i) {
			if (textureCacheList[i].d3dTexture == NULL) {
				slot = i;
				break;
			}
		}

		// All slots taken?

		if (slot == 0) {

			// Need to allocate another slot at the end.  Is the list full?

			if (nextTextureSlot >= kTextureCacheSize) {
				ABORT("texture cache full.");
			}

			// Snag the next one

			slot = nextTextureSlot;

			// Grow the list

			++nextTextureSlot;
		}

		// Get shortcut pointer

		t = &textureCacheList[slot];
	}

	// Make sure and clear out anything that's already there

	freeTexture(slot);

	// Set the name and size

	if (name != NULL) {
		strcpy(t->name, name);
	}
	t->xSize = xSize;
	t->ySize = ySize;

	// Allocate D3D texture

	HRESULT result = pD3DDevice->CreateTexture(
		xSize,			// width
		ySize,			// height
		1,			// number of miplevels - we are not mip-mapping
		0,			// usage - we do not need to be able to render into textures
		D3DFMT_A8R8G8B8,	// pixel format - we will only use 32-bit
		D3DPOOL_MANAGED,	// memory pool - we'll let D3D manage the textures so we don't have to "restore."
		&t->d3dTexture		// return pointer to texture interface object here
	);

	// Check for failure.  We won't handle errors gracefully.

	if (FAILED(result)) {
		ABORT("Can't allocate %dx%d 32-bit texture", xSize, ySize);
	}

	// Return "handle" to texture, which is simply the index into the list

	return slot;
}

//---------------------------------------------------------------------------
// Renderer::freeTexture
//
// Free a texture, given the handle.

void	Renderer::freeTexture(int handle) {

	// Harmlessly ignore zero handle

	if (handle == 0) {
		return;
	}

	// Sanity check handle validity

	if ((handle < 1) || (handle >= nextTextureSlot)) {
		assert(false);
		return;
	}

	// Check if we are freeing the currently selected texture

	if (handle == currentTextureHandle) {

		// OK, they are freeing the current texture.
		// Select the white texture.  Unless this is
		// the hite texture that we are freeing - then
		// select a NULL texture

		if (handle == kWhiteTexture) {
			currentTextureHandle = 0;
		} else {
			currentTextureHandle = kWhiteTexture;
		}

		// Does the device exist?

		if (pD3DDevice != NULL) {

			// Select the new texture

			pD3DDevice->SetTexture(0, textureCacheList[currentTextureHandle].d3dTexture);
		}

	}

	// Get shortcut

	TextureCacheEntry *t = &textureCacheList[handle];

	// Release D3D thingy, if it exists

	if (t->d3dTexture != NULL) {
		t->d3dTexture->Release();
	}

	// Clear it out

	memset(t, 0, sizeof(*t));
}

//---------------------------------------------------------------------------
// Renderer::setTextureImage
//
// Set the image for a texture.  The image data is always 32-bit (to keep
// things simple).  Each pixel is an unsigned value of the MAKE_ARGB
// style (i.e. 0xAARRGGBB).  The pixel data should be accessed as 32-bit
// units, not as bytes.  The stride of the image data must be the width
// of the texture times 4.

void	Renderer::setTextureImage(int handle, const unsigned *image) {
	HRESULT result;

	// Sanity check handle validity

	if ((handle < 1) || (handle >= nextTextureSlot)) {
		assert(false);
		return;
	}

	// Get shortcut

	TextureCacheEntry *t = &textureCacheList[handle];

	// Make sure we have something allocated

	if (t->d3dTexture == NULL) {
		assert(false);
		return;
	}

	// Lock the texture

	D3DLOCKED_RECT	r;
	result = t->d3dTexture->LockRect(
		0,	// level
		&r,	// resulting info is placed here
		NULL,	// rectangle to lock - we want the whole thing
		0	// flags
	);

	// Check if we can't do it

	if (FAILED(result)) {
		assert(false);
		return;
	}

	// Copy in the data a row at a time

	const unsigned char *srcPtr = (const unsigned char *)image;
	unsigned char *destPtr = (unsigned char *)r.pBits;
	int	srcStride = t->xSize*4;
	int	destStride = r.Pitch;
	for (int y = 0 ; y < t->ySize ; ++y) {

		// Copy the row

		memcpy(destPtr, srcPtr, t->xSize*4);

		// Advance pointers

		srcPtr += srcStride;
		destPtr += destStride;
	}

	// Unlock the surface

	result = t->d3dTexture->UnlockRect(0);
	assert(SUCCEEDED(result));
}

//---------------------------------------------------------------------------
// Renderer::cacheTexture
//
// Loads a texture into the texture cache, given a texture filename.
// The texture is named using the filename.  If a texture with that name
// already exists, the existing handle is returned - the file is not
// reloaded.)
//
// The return value is a handle that you can use to reference the texture.
// Texture handles are always > 0.
//
// Currently, this function never returns failure - if there is a problem,
// it will abort the program.

int	Renderer::cacheTexture(const char *filename) {

	char	text[256];

	// Check if texture already loaded

	int	slot = findTexture(filename);
	if (slot > 0) {
		return slot;
	}

	// Load up the image

	Bitmap bitmap;
	if (!bitmap.load(filename, text)) {
		ABORT("Can't load texture %s.  %s.", filename, text);
	}

	// It must be 32-bit

	if (bitmap.format() != Bitmap::eFormat_8888) {
		ABORT("Can't load texture %s.  Only 32-bit textures supported.", filename);
	}

	// Allocate it

	slot = allocTexture(filename, bitmap.xSize(), bitmap.ySize());

	// Fill in the image

	setTextureImage(slot, (unsigned *)bitmap.rawData());

	// Return handle

	return slot;
}

//---------------------------------------------------------------------------
// Renderer::cacheTexture
//
// Locates a texture in the texture cache and fills in the handle.  The
// texture is loaded from disk if necessary.

void	Renderer::cacheTexture(TextureReference &texture) {

	// Texture must have a valid name

	assert(texture.name[0] != '\0');

	// Check if the handle is already correct.  After the first time
	// the texture is used, this should be the case.

	if ((texture.handle > 0) && (texture.handle < nextTextureSlot)) {

		// Get shortcut to slot

		const TextureCacheEntry *t = &textureCacheList[texture.handle];

		// Make sure the name is correct and the texture exists

		if (
			(stricmp(t->name, texture.name) == 0) && 
			(t->d3dTexture != NULL)
		) {

			// Cache hit.  After textures are cached, this should
			// be the case almost every time

			return;
		}
	}

	// Handle is invalid.  Cache the texture and fill in the handle

	texture.handle = cacheTexture(texture.name);
}

/////////////////////////////////////////////////////////////////////////////
//
// Software vertex rotations
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Renderer::computeOutCode
//
// Compute an outcode for a point in the current reference space.
//
// See Section 16.1.1 for more information on the usefulness of outcodes

int	Renderer::computeOutCode(const Vector3 &p) {

	// Make sure the model-to-clip matrix is computed

	getModelToClipMatrix();

	// Transform the point into clip space.

	float	x =
		p.x * modelToClipMatrix._11 +
		p.y * modelToClipMatrix._21 +
		p.z * modelToClipMatrix._31 +
		modelToClipMatrix._41;

	float	y =
		p.x * modelToClipMatrix._12 +
		p.y * modelToClipMatrix._22 +
		p.z * modelToClipMatrix._32 +
		modelToClipMatrix._42;

	float	z =
		p.x * modelToClipMatrix._13 +
		p.y * modelToClipMatrix._23 +
		p.z * modelToClipMatrix._33 +
		modelToClipMatrix._43;

	float	w =
		p.x * modelToClipMatrix._14 +
		p.y * modelToClipMatrix._24 +
		p.z * modelToClipMatrix._34 +
		modelToClipMatrix._44;

	// Code the point against the six frustum planes.  Remember, we are
	// using a D3D-style clip matrix, where the near clip plane is at z=0

	int	code = 0;
	if (x < w) code |= kOutCodeLeft;
	if (x > w) code |= kOutCodeRight;
	if (y < w) code |= kOutCodeBottom;
	if (y > w) code |= kOutCodeTop;
	if (z < w) code |= kOutCodeNear;
	if (z > w) code |= kOutCodeFar;

	// If fog enabled, then go ahead and code it against the
	// far fog distance.  This can be used to trivially reject
	// objects if the entire object is fogged out

	if (fogEnable) {
		if (z > farFogClipSpaceZ) code |= kOutCodeFog;
	}

	// Return the code

	return code;
}

//---------------------------------------------------------------------------
// Renderer::projectPoint
//
// Compute an outcode for a point in the current reference space,
// and if the point is on-screen, project it to 2D screen space.
//
// The 2D projected point will be filled in if the outcode
// doesn't have any of kOutCodeOffScreenMask set

int	Renderer::projectPoint(const Vector3 &p, Vector3 *result) {

	// Make sure the model-to-clip matrix is computed

	getModelToClipMatrix();

	// Transform the point into clip space.

	float	x =
		p.x * modelToClipMatrix._11 +
		p.y * modelToClipMatrix._21 +
		p.z * modelToClipMatrix._31 +
		modelToClipMatrix._41;

	float	y =
		p.x * modelToClipMatrix._12 +
		p.y * modelToClipMatrix._22 +
		p.z * modelToClipMatrix._32 +
		modelToClipMatrix._42;

	float	z =
		p.x * modelToClipMatrix._13 +
		p.y * modelToClipMatrix._23 +
		p.z * modelToClipMatrix._33 +
		modelToClipMatrix._43;

	float	w =
		p.x * modelToClipMatrix._14 +
		p.y * modelToClipMatrix._24 +
		p.z * modelToClipMatrix._34 +
		modelToClipMatrix._44;

	// Code the point against the six frustum planes.  Remember, we are
	// using a D3D-style clip matrix, where the near clip plane is at z=0

	int	code = 0;
	if (x < w) code |= kOutCodeLeft;
	if (x > w) code |= kOutCodeRight;
	if (y < w) code |= kOutCodeBottom;
	if (y > w) code |= kOutCodeTop;
	if (z < w) code |= kOutCodeNear;
	if (z > w) code |= kOutCodeFar;

	// If fog enabled, then go ahead and code it against the
	// far fog distance.  This can be used to trivially reject
	// objects if the entire object is fogged out

	if (fogEnable) {
		if (z > farFogClipSpaceZ) code |= kOutCodeFog;
	}

	// Check if we can project it

	if ((code & kOutCodeOffScreenMask) == 0) {
		assert(w > 0.0f);

		// Invert w

		float	oow = 1.0f / w;

		// Compute projected point

		result->x = windowCenterX + x*oow*halfWindowSizeX;
		result->y = windowCenterY - y*oow*halfWindowSizeY;
		result->z = z*oow;
	}

	// Return the code

	return code;
}

//---------------------------------------------------------------------------
// Renderer::getModelToCameraMatrix
//
// Returns the 4x3 matrix which transforms points from the current reference
// frame to camera space

const Matrix4x3 &Renderer::getModelToCameraMatrix() {

	// Make sure it's up-to-date

	getModelToClipMatrix();

	// Return it

	return modelToCameraMatrix;
}

//---------------------------------------------------------------------------
// Renderer::getModelToCameraMatrix
//
// Returns the 4x3 matrix which transforms points from the current reference
// frame to world space

const Matrix4x3 &Renderer::getModelToWorldMatrix() {
	return instanceStack[instanceStackPtr].modelToWorldMatrix;
}

/////////////////////////////////////////////////////////////////////////////
//
// class Renderer implementation details
//
/////////////////////////////////////////////////////////////////////////////

// Called when something changes and we need to re-compute the clip
// matrix.

void	Renderer::computeClipMatrix() {

	// Compute actual zoom values to use, based on user desired parms
	// See Section 15.2.4.  Notice that here we are assuming that the
	// physical output device has a 4:3 width:height ratio, like most
	// monitors do.
	//
	// Also note that there is an error in 15.2.4 - it states that
	// the zoom values should be porportional to the physical window
	// size.  Actually, they should be inversely proportional.  Sorry!

	float	xz = zoomX;
	float	yz = zoomY;
	if (xz <= 0.0f) {

		// They want us to auto-compute the horizontal zoom,
		// based on the vertical zoom.  They MUST have
		// specified the vertical zoom.  (I can't auto-compute
		// both.)

		assert(zoomY > 0.0f);

		// Compute horizontal zoom to maintain proper proportions

		xz = yz * (float)windowSizeY / (float)windowSizeX * (3.0f / 4.0f) * (float)screenX / (float)screenY;

	} else if (yz <= 0.0f) {

		// Compute vertical zoom to maintain proper proportions

		yz = xz * (float)windowSizeX / (float)windowSizeY * (4.0f / 3.0f) * (float)screenY / (float)screenX;
	}

	// Fill in the elements.  See Equation 15.5 on page 357

	clipMatrix._11 = xz;
	clipMatrix._12 = 0.0f;
	clipMatrix._13 = 0.0f;
	clipMatrix._14 = 0.0f;

	clipMatrix._21 = 0.0f;
	clipMatrix._22 = yz;
	clipMatrix._23 = 0.0f;
	clipMatrix._24 = 0.0f;

	clipMatrix._31 = 0.0f;
	clipMatrix._32 = 0.0f;
	clipMatrix._33 = farClipPlane / (farClipPlane - nearClipPlane);
	clipMatrix._34 = 1.0f;

	clipMatrix._41 = 0.0f;
	clipMatrix._42 = 0.0f;
	clipMatrix._43 = nearClipPlane * farClipPlane / (nearClipPlane - farClipPlane);
	clipMatrix._44 = 1.0f;

	// Compute far fog clip space z value

	farFogClipSpaceZ = fogFar * clipMatrix._33 + clipMatrix._43;

	// Upload it to D3D, if we are initted

	if (pD3DDevice != NULL) {
		HRESULT result = pD3DDevice->SetTransform(D3DTS_PROJECTION, &clipMatrix);
		assert(SUCCEEDED(result));
	}

	// The model->clip matrix must be recomputed, next time we need it

	needToComputeModelToClipMatrix = true;
}

// Called with the reference frame changes (the model->world matrix)

void	Renderer::updateModelToWorldMatrix() {

	// Upload this to the rendering API, if we have been initted

	if (pD3DDevice != NULL) {

		// Get shortcut to model->world matrix, depending on
		// current nesting level

		const Matrix4x3 *srcMatrix = &instanceStack[instanceStackPtr].modelToWorldMatrix;

		// Convert our 4x3 matrix to D3D-style 4x4 matrix

		D3DMATRIX	m;
		m._11 = srcMatrix->m11;
		m._12 = srcMatrix->m12;
		m._13 = srcMatrix->m13;
		m._14 = 0.0f;

		m._21 = srcMatrix->m21;
		m._22 = srcMatrix->m22;
		m._23 = srcMatrix->m23;
		m._24 = 0.0f;

		m._31 = srcMatrix->m31;
		m._32 = srcMatrix->m32;
		m._33 = srcMatrix->m33;
		m._34 = 0.0f;

		m._41 = srcMatrix->tx;
		m._42 = srcMatrix->ty;
		m._43 = srcMatrix->tz;
		m._44 = 1.0f;

		// Tell D3D about it

		HRESULT result = pD3DDevice->SetTransform(D3DTS_WORLDMATRIX(0), &m);
		assert(SUCCEEDED(result));
	}

	// The model->clip matrix must be recomputed, next time we need it

	needToComputeModelToClipMatrix = true;
}

//---------------------------------------------------------------------------
// Renderer::getModelToClipMatrix
//
// Make sure the software model->clip matrix is up-to-date

void	Renderer::getModelToClipMatrix() {

	// Check if we don't have to compute it

	if (!needToComputeModelToClipMatrix) {
		return;
	}

	// Compute the model->camera matrix by concatenating
	// the model->world matrix with the world->camera matrix

	modelToCameraMatrix = getModelToWorldMatrix() * worldToCameraMatrix;

	// Concatenate this with the clip matrix.

	assert(false); // !FIXME!

	// Matrix is up-to-date

	needToComputeModelToClipMatrix = false;
}

//---------------------------------------------------------------------------
// Renderer::freeAllTextures
//
// Free all textures in the cache and reset to empty state

void Renderer::freeAllTextures() {

	// Make sure and deselect any texture

	if (pD3DDevice != NULL) {
		pD3DDevice->SetTexture(0, NULL);
	}
	currentTextureHandle = 0;

	// Scan textures and free anything allocated

	for (int i = 1 ; i < nextTextureSlot ; ++i) {
		freeTexture(i);
	}

	// Reset all pointers and names

	memset(textureCacheList, 0, sizeof(textureCacheList));

	// Start next allocation at index 1.  (We never use slot 0)

	nextTextureSlot = 1;
}

