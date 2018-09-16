/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Renderer.h - Very simple low-level 3D renderer interface
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __RENDERER_H_INCLUDED__
#define __RENDERER_H_INCLUDED__

#ifndef __VECTOR3_H_INCLUDED__
	#include "vector3.h"
#endif

#ifndef __EULERANGLES_H_INCLUDED__
	#include "EulerAngles.h"
#endif

#ifndef __MATRIX4X3_H_INCLUDED__
	#include "Matrix4x3.h"
#endif

/////////////////////////////////////////////////////////////////////////////
//
// Simple constants, enums, macros
//
/////////////////////////////////////////////////////////////////////////////

// Maximum number of chars in a texture name (including the '\0')

const int kMaxTextureNameChars = 64;

// Source blend functions

enum ESourceBlendMode {
	eSourceBlendModeSrcAlpha,	// Normal source blending (default)
	eSourceBlendModeOne,		// Use source color unmodified
	eSourceBlendModeZero,		// No source contribution
};

// Destination blend functions

enum EDestBlendMode {
	eDestBlendModeInvSrcAlpha,	// Inverse source alpha blend (default)
	eDestBlendModeOne,		// Additive blending
	eDestBlendModeZero,		// Discard current frame buffer pixel, blend with "black"
	eDestBlendModeSrcColor,		// Multiplicative blend (often used for lightmapping)
};

// Culling modes

enum EBackfaceMode {
	eBackfaceModeCCW,	// cull faces with counterclockwise screen-space order (default)
	eBackfaceModeCW,	// cull faces with clockwise screen-space order
	eBackfaceModeDisable	// Render all faces, regardless of screenspace vertex order
};

// Bitfield of options to the clear() function.

const int	kClearFrameBuffer	= 1;  // clear the frame buffer
const int	kClearDepthBuffer	= 2;  // clear the zbuffer
const int	kClearToConstantColor	= 4;  // clear frame buffer to constant color.  (By default, we clear to black)
const int	kClearToFogColor	= 8;  // clear frame buffer to fog color.  (By default, we clear to black)

// Bitfield of vertex outcodes.  See the computeOutCode() function

const int	kOutCodeLeft		= 0x01;
const int	kOutCodeRight		= 0x02;
const int	kOutCodeBottom		= 0x04;
const int	kOutCodeTop		= 0x08;
const int	kOutCodeNear		= 0x10;
const int	kOutCodeFar		= 0x20;
const int	kOutCodeFog		= 0x40;
const int	kOutCodeFrustumMask	= 0x3f; // bits to test if outside the frustom - don't worry about fog
const int	kOutCodeOffScreenMask	= 0x1f; // bits to test if the projected point is off screen - far or fog don't matter

// Symbolic refresh rates that can be used when setting the video mode

const int	kRefreshRateDefault	= -1;
const int	kRefreshRateFastest	= -2;

// Special texture handle that is always reserved for the "white texture,"
// whiet is a texture that is solid white.  This important texture is useful
// in a wide variety of circumstances

const int	kWhiteTexture = 1;

// Macro to construct a color in 3D-form.
//
// This color value is NOT corrected for different pixel formats
// on different hardware formats.  It is for interface purposes
// only, NOT for frame buffer access.  The values are packed as follows:
//
// bits  0.. 7: blue
// bits  8..15: green
// bits 16..23: red
// bits 24..31: alpha
//
// It is also legal to pass a color like this: 0xAARRGGBB
//
// WARNING:
//
// The above rules apply to accessing a color via an integer
// value only, and have NOTHING to do with accessing the bytes
// in memory.

#define MAKE_RGB(r, g, b) ((unsigned int)(((unsigned int)(r) << 16) | ((unsigned int)(g) << 8) | ((unsigned int)(b))))
#define MAKE_ARGB(a, r, g, b) ((unsigned int)(((unsigned int)(a) << 24) | ((unsigned int)(r) << 16) | ((unsigned int)(g) << 8) | ((unsigned int)(b))))

#define GET_A(argb) ((int)(((unsigned int)(argb) >> 24U) & (unsigned int)0xff))
#define GET_R(argb) ((int)(((unsigned int)(argb) >> 16U) & (unsigned int)0xff))
#define GET_G(argb) ((int)(((unsigned int)(argb) >> 8U) & (unsigned int)0xff))
#define GET_B(argb) ((int)((unsigned int)(argb) & (unsigned int)0xff))

/////////////////////////////////////////////////////////////////////////////
//
// Utility structures and classes
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// struct VideoMode
//
// Defines a video mode - the resolution, color bit depth, and refresh rate.
// This struct is used when querying for a list of available modes, and
// also when setting the video mode.

struct VideoMode {
	int	xRes;		// horizontal resolution, in pixels
	int	yRes;		// vertical resolution, in pixels
	int	bitsPerPixel;	// currently only 16, 24, or 32 supported
	int	refreshHz;	// you can use one of kRefreshRateXxx constants when setting the video mode
};

//---------------------------------------------------------------------------
// struct RenderVertex - Untransformed, unlit vertex
// struct RenderVertexL - Untransformed, lit vertex
// struct RenderVertexTL - Transformed and lit vertex
//
// These structures are used to pass vertex data to the renderer.  Depending
// on what you want the renderer to do for you, you use a different
// structure.  For example, if the vertices are in modeling space and
// need to be transformed and projected to screen space, then you would
// use an untransformed vertex type.  If you want the renderer to apply
// lighting calculations, then you would use an unlit vertex type.  If you
// want to specify the vertex color manually, then use a pre-lit vertex type.
//
// See Section 15.7.2 for more information.

struct RenderVertex {
	Vector3	p;	// position
	Vector3	n;	// normal
	float	u,v;	// texture mapping coordinate
};

struct RenderVertexL {
	Vector3		p;	// position
	unsigned	argb;	// prelit diffuse color
	float		u,v;	// texture mapping coordinate
};

// Transformed and lit vertex

struct RenderVertexTL {
	Vector3		p;	// screen space position and z value
	float		oow;	// One Over W.  This is used for perspective projection.  Usually, you can just use 1/z.
	unsigned	argb;	// prelit diffuse color (8 bits per component - 0xAARRGGBB)
	float		u,v;	// texture mapping coordinate
};

//---------------------------------------------------------------------------
// struct RenderTri
//
// A single triangle for rendering.  It's just three indices.
// Notice that the indices are unsigned shorts.  This is for two reasons.
// First, using 16-bit rather than 32-bit indices effectively doubles the
// memory throughput for the index data.  Second, some graphics cards do
// not natively support 32-bit index data.
//
// This does put a limit on the max number of vertices in a single mesh
// at 65536.  This is usually too big of not a problem, since most large
// objects can easily be broken down into multiple meshes - in fact,
// you probably want to divide things up for visibility, etc, anyway.

struct RenderTri {
	unsigned short index[3];
};

//---------------------------------------------------------------------------
// struct TextureReference
//
// Handy class for keeping track of a texture's name and handle.

struct TextureReference {

	// Name of the texture.  Usually this is a filename

	char	name[kMaxTextureNameChars];

	// Texture handle, within the graphics system

	int	handle;
};

/////////////////////////////////////////////////////////////////////////////
//
// class Renderer
//
// Low-level renderer abstraction layer.
//
// See the .cpp file for more comments and opinions.
//
/////////////////////////////////////////////////////////////////////////////

class Renderer {
public:
	Renderer();

	// Access to video mode list

	int		getVideoModeCount();
	const VideoMode &getVideoMode(int index);

	// Init/kill.  Must be called once at program startup/shutdown

	void	init(const VideoMode &mode);
	void	shutdown();

	// Screen resolution

	int	getScreenX() const { return screenX; }
	int	getScreenY() const { return screenY; }

	// Flip video pages and display the back buffer

	void	flipPages();

	// Begin/end the scene

	void	beginScene();
	void	endScene();

	// Save/restore video mode.  This is called when our app moves
	// in and out of the foreground, and should not be called directly

	void	videoSave();
	void	videoRestore();

// Camera specification

	// Camera position/orientation

	void			setCamera(const Vector3 &pos, const EulerAngles &orient);
	const Vector3		&getCameraPos() const { return cameraPos; }
	const EulerAngles	&getCameraOrient() const { return cameraOrient; }

	// Set zoom.  A zero zoom value means "compute it for me"

	void	setZoom(float xZoom, float yZoom = 0.0f);

	// Near/far clipping planes

	void	setNearFarClippingPlanes(float n, float f);
	float	getNearClippingPlane() const { return nearClipPlane; }
	float	getFarClippingPlane() const { return farClipPlane; }

// Window definition

	// The 2D output window

	void	setWindow(int x1, int y1, int xSize, int ySize);
	void	setFullScreenWindow();
	void	getWindow(int *x1, int *y1, int *xSize, int *ySize);

// Reference frame

	// Nested reference frame stack

	void	instance(const Vector3 &pos, const EulerAngles &orient);
	void	instancePop();

	// Low-level texture cache maintenance functions using names and handles directly

	void	resetTextureCache();
	int	findTexture(const char *name);
	int	allocTexture(const char *name, int xSize, int ySize);
	void	freeTexture(int handle);
	void	setTextureImage(int handle, const unsigned *image);
	int	cacheTexture(const char *filename);

	// Slightly simpler texture cache access through the TextureReference class.
	// Note that TextureReference structs cannot be used for anonymous textures.

	void	cacheTexture(TextureReference &texture);

// Rendering context management functions

	// Zbuffer mode

	void	setDepthBufferMode(bool readEnabled, bool writeEnabled);
	void	setDepthBufferMode() { setDepthBufferMode(true, true); } // no parms - set defaults
	bool	getDepthBufferRead() const { return depthBufferRead; }
	bool	getDepthBufferWrite() const { return depthBufferWrite; }

	// Alpha blending control

	void			setBlendEnable(bool blend = false);
	bool			getBlendEnable() const { return blendEnable; }
	void			setSourceBlendMode(ESourceBlendMode mode = eSourceBlendModeSrcAlpha);
	ESourceBlendMode	getSourceBlendMode() const { return sourceBlendMode; }
	void			setDestBlendMode(EDestBlendMode mode = eDestBlendModeInvSrcAlpha);
	EDestBlendMode		getDestBlendMode() const { return destBlendMode; }

	// Global constant color (used for 2D primitives) and
	// global constant opacity

	void		setRGB(unsigned int rgb); // alpha is ignored
	unsigned	getARGB() { return constantARGB; } // returns global opacity in the alpha channel
	void		setARGB(unsigned argb); // sets color and opacity in one call
	void		setOpacity(float a = 1.0F);
	float		getOpacity() const { return constantOpacity; }

	// Fog.  The alpha portion of the fog color is ignored, only the RGB portion
	// is relevent

	void		setFogEnable(bool flag = false);
	bool		getFogEnable() { return fogEnable; }
	void		setFogColor(unsigned rgb);
	unsigned 	getFogColor() const { return fogColor; }
	void		setFogDistance(float nearFog, float farFog);
	float		getFogNear() const { return fogNear; }
	float		getFogFar() const { return fogFar; }

	// Lighting context.  We will assume one single directional light, with ambient

	void		setAmbientLightColor(unsigned rgb);
	unsigned	getAmbientLightColor() const { return ambientLightColor; }

	void		setDirectionalLightVector(const Vector3 &v);
	const Vector3	&getDirectionalLightVector() const { return directionalLightVector; }

	void		setDirectionalLightColor(unsigned rgb);
	unsigned	getDirectionalLightColor() const { return directionalLightColor; }

	// Master switch for lighting enable.  If lighting is disabled,
	// everything is rendered unlit ("fully bright")

	void	setLightEnable(bool flag = true);
	bool	getLightEnable() const { return lightEnable; }

	// Culling

	void		setBackfaceMode(EBackfaceMode mode = eBackfaceModeCCW);
	EBackfaceMode	getBackfaceMode() const { return backfaceMode; }

	// Set current diffuse texture using handle

	void	selectTexture(int handle);
	int	getCurrentTexture() const { return currentTextureHandle; }

	// Slightly more conveinent interface that uses texture reference

	void	selectTexture(TextureReference &texture);

	// Texture clamping

	void	setTextureClamp(bool flag = false);
	bool	getTextureClamp() const { return textureClamp; }

// Rendering primitives

	// Clear the frame buffer and/or depth buffer.  This clears
	// the current 2D window, not the entire screen
	// The options are formed from the kClearXxxx constants above

	void	clear(int options = kClearFrameBuffer | kClearDepthBuffer);

	// Render a triangle mesh

	void	renderTriMesh(const RenderVertex *vertexList, int vertexCount, const RenderTri *triList, int triCount);
	void	renderTriMesh(const RenderVertexL *vertexList, int vertexCount, const RenderTri *triList, int triCount);
	void	renderTriMesh(const RenderVertexTL *vertexList, int vertexCount, const RenderTri *triList, int triCount);

	// 2D primitives.  These use the current constant color

	void	dot(int x, int y);
	void	line(int x1, int y1, int x2, int y2);
	void	boxFill(int x1, int y1, int x2, int y2);

// Software vertex transformations functions.  These are primarily useful for
// performing visibility or other manual manipulations.

	// Access to various matrices

	const Matrix4x3 &getWorldToCameraMatrix() { return worldToCameraMatrix; }
	const Matrix4x3 &getModelToCameraMatrix();
	const Matrix4x3 &getModelToWorldMatrix();

	// Return a vertex outcode given a point in the current reference space.
	// Returns a bitfield componsef of the kOutCodeXXX constants

	int	computeOutCode(const Vector3 &p);

	// Compute outcode and project point onto screen space,
	// if possible.  If the outcode contains any of
	// kOutCodeOffScreenMask, then the vertex was not projected.

	int	projectPoint(const Vector3 &p, Vector3 *result);

private:

// Internal state variables

	// Full screen resolution

	int	screenX;
	int	screenY;

	// Camera specification

	Vector3		cameraPos;
	EulerAngles	cameraOrient;
	float		zoomX;
	float		zoomY;

	// Near/far clipping planes

	float	nearClipPlane;
	float	farClipPlane;

	// The 2D output window

	int	windowX1;
	int	windowY1;
	int	windowX2;
	int	windowY2;
	int	windowSizeX;
	int	windowSizeY;

	// Zbuffer mode

	bool	depthBufferRead;
	bool	depthBufferWrite;

	// Alpha blending

	bool			blendEnable;
	ESourceBlendMode	sourceBlendMode;
	EDestBlendMode		destBlendMode;

	// Global constant color and opacity.

	unsigned	constantARGB;
	float		constantOpacity;

	// Fog

	bool		fogEnable;
	unsigned 	fogColor;
	float		fogNear;
	float		fogFar;

	// Lighting context.

	bool		lightEnable;
	unsigned	ambientLightColor;
	Vector3		directionalLightVector;
	unsigned	directionalLightColor;

	// Culling

	EBackfaceMode	backfaceMode;

	// Currently selected texture

	int	currentTextureHandle;

	// Texture clamp

	bool	textureClamp;

	// Current world->camera matrix.  This will always be a rigid body
	// transform - it does not contain zoom or aspect ratio correction.

	Matrix4x3	worldToCameraMatrix;

// Implementation details

	void	updateModelToWorldMatrix();
	void	computeClipMatrix();
	void	getModelToClipMatrix();
	void	freeAllTextures();
};

// The global class object

extern Renderer	gRenderer;

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __RENDERER_H_INCLUDED__

