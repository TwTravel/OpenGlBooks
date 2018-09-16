/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Font.h - Simple font interface that renders using the 3D hardware
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __FONT_H_INCLUDED__
#define __FONT_H_INCLUDED__

#ifndef __RENDERER_H_INCLUDED__
#include "Renderer.h"
#endif

/////////////////////////////////////////////////////////////////////////////
//
// class Font
//
// This is a simple virtual class which renders via the 3D interface.
//
/////////////////////////////////////////////////////////////////////////////

class Font {
public:

	// Info we keep track of for a single character

	struct CharInfo {

		// Width of the character

		float	xSize;

		// UV coordinates in the bitmap

		float	u1, v1;
		float	u2, v2;

		// Vertical extent of the character.  These are relative
		// to the baseline, with positive y pointing DOWN

		float	y0;	// usually negative
		float	y1;	// 0, if the character sits on the baseline
	};

	// List of characters.  Everything's public.

	CharInfo	charTable[256];

	// The texture data, which we always keep around.  Everything's
	// public, mostly because we're lazy.  Doing major operations
	// directly is not a good idea.  If an accessor or helper function
	// exists to do what you want to do. you should probably use it.

	int		textureSizeX;
	int		textureSizeY;
	unsigned	*textureData;

	// Reference to the texture in the renderer.  

	TextureReference	texture;

	void	allocSoftwareTexture(int xSize, int ySize);
	void	freeTexture();
	void	selectTexture();
	void	uploadTexture();

};
