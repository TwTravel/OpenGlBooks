/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Bitmap.cpp - Simple bitmap loader
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Bitmap.h"
#include "CommonStuff.h"
#include "Renderer.h"

/////////////////////////////////////////////////////////////////////////////
//
// Local stuff
//
/////////////////////////////////////////////////////////////////////////////

// File header for .TGA file

#pragma pack(1)

struct TGAHeader {
	unsigned char	imageIDLength;
	unsigned char	colorMapType;
	unsigned char	imageType;
	unsigned short	colorMapFirstIndex;
	unsigned short	colorMapLength;
	unsigned char	colorMapBitsPerEntry;
	unsigned short	xOrigin, yOrigin;
	unsigned short	width, height;
	unsigned char	bitsPerPixel;
	unsigned char	imageDescriptor;
};

#pragma pack()

/////////////////////////////////////////////////////////////////////////////
//
// class Bitmap member functions
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Bitmap::Bitmap
//
// Constructor - reset interval variables to default, empty state

Bitmap::Bitmap() {
	sizeX = 0;
	sizeY = 0;
	fmt = eFormat_None;
	data = NULL;
}

//---------------------------------------------------------------------------
// Bitmap::~Bitmap
//
// Destructor - make sure resources are freed

Bitmap::~Bitmap() {
	freeMemory();
}

//---------------------------------------------------------------------------
// Bitmap::allocateMemory
//
// Allocate a bitmap in the specified format

void	Bitmap::allocateMemory(int xs, int ys, EFormat format) {

	// Sanity check

	assert(xs > 0);
	assert(ys > 0);

	// First, free up anything already allocated

	freeMemory();

	// Figure out how big one row is, in bytes

	int	rowBytes;
	switch (format) {
		case eFormat_8888:
			rowBytes = xs*4;
			break;

		default:
			assert(false); // bogus pixel format
	}

	// Allocate memory

	data = malloc(ys * rowBytes);
	if (data == NULL) {
		ABORT("Out of memory for bitmap");
	}

	// Remember dimensions

	sizeX = xs;
	sizeY = ys;
	fmt = format;
}

//---------------------------------------------------------------------------
// Bitmap::freeMemory
//
// Allocate a bitmap in the specified format

void	Bitmap::freeMemory() {

	// Free memory, if some was allocated

	if (data != NULL) {
		free(data);
		data = NULL;
	}

	// Reset

	sizeX = 0;
	sizeY = 0;
	fmt = eFormat_None;
}

//---------------------------------------------------------------------------
// Bitmap::getPix
//
// Fetch a pixel at the given coordinates.  The pixel is always returned in
// 32-bit 0xAARRGGBB format, the same as used by the Renderer class
// and MAKE_ARGB macro.

unsigned	Bitmap::getPix(int x, int y) const {

	// Safety check

	if ((x < 0) || (y < 0) || (x >= sizeX) || (y >= sizeY) || (data == NULL)) {
		assert(false);
		return 0;
	}

	// Check format

	unsigned result;
	switch (fmt) {
		case eFormat_8888:
			result = ((unsigned *)data)[y*sizeX + x];
			break;

		default:
			assert(false);
			result = 0;
			break;
	}

	// Return it

	return result;
}

//---------------------------------------------------------------------------
// Bitmap::setPix
//
// Set a pixel at the given coordinates.  The pixel is specified in
// 32-bit 0xAARRGGBB format, the same as used by the Renderer class
// and MAKE_ARGB macro.

void	Bitmap::setPix(int x, int y, unsigned argb) {

	// Safety check

	if ((x < 0) || (y < 0) || (x >= sizeX) || (y >= sizeY) || (data == NULL)) {
		assert(false);
		return;
	}

	// Check format

	switch (fmt) {
		case eFormat_8888:
			((unsigned *)data)[y*sizeX + x] = argb;
			break;

		default:
			assert(false);
			break;
	}
}

//---------------------------------------------------------------------------
// Bitmap::load
//
// Load a bitmap from an image file.  Uses the extension to
// figure out how to load.

bool	Bitmap::load(const char *filename, char *returnErrMsg) {

	// Free up anything already allocated

	freeMemory();

	// Fetch extension.  I wish I could use the _splitpath function,
	// but it's not cross-platform.  I'll parse the thing myself.

	const char *ext = strchr(filename, '.');
	if (ext == NULL) {
		strcpy(returnErrMsg, "Filename has no extension");
		return false;
	}
	for (;;) {
		const char *dot = strchr(ext+1, '.');
		if (dot == NULL) {
			break;
		}
		ext = dot;
	}

	// Check for known extensions

	if (stricmp(ext, ".tga") == 0) {
		return loadTGA(filename, returnErrMsg);
	}
	if (stricmp(ext, ".bmp") == 0) {
		return loadBMP(filename, returnErrMsg);
	}

	// Unknown extension

	sprintf(returnErrMsg, "Unknown/unsupported file extension '%s'", ext);
	return false;
}

//---------------------------------------------------------------------------
// Bitmap::loadTGA
//
// Load image in .TGA format.

bool	Bitmap::loadTGA(const char *filename, char *returnErrMsg) {

	// Open the file

	FILE *f = fopen(filename, "rb");
	if (f == NULL) {
		strcpy(returnErrMsg, "Can't open file.");
failed:

		// Cleanup

		freeMemory();
		if (f != NULL) {
			fclose(f);
		}

		// Report failure

		return false;
	}

	// Read TGA header

	TGAHeader	head;
	if (fread(&head, sizeof(head), 1, f) != 1) {
ioError:
		strcpy(returnErrMsg, "I/O error, or file is corrupt.");
		goto failed;
	}

	// Check format

	if (head.imageType == 2) { // UNCOMPRESSED_TRUECOLOR
		if ((head.bitsPerPixel != 24) && (head.bitsPerPixel != 32)) {
			sprintf(returnErrMsg, "%d-bit truecolor image not supported", (int)head.bitsPerPixel);
			goto failed;
		}
		if (head.colorMapType != 0) {
			strcpy(returnErrMsg, "Truecolor image with colormap not supported");
			goto failed;
		}
	//} else if (head.imageType == 1) { // UNCOMPRESSED_COLORMAPPED
	//	if (
	//		(head.colorMapType != 1) ||
	//		(head.bitsPerPixel != 8) ||
	//		(head.colorMapFirstIndex != 0) ||
	//		(head.colorMapLength != 256) ||
	//		(head.colorMapBitsPerEntry != 24)
	//	) {
	//		strcpy(returnErrMsg, "Invalid colormapped image file format");
	//		return 0;
	//	}
	} else {
		sprintf(returnErrMsg, ".TGA image type %d not supported", (int)head.imageType);
		goto failed;
	}

	// Check origin

	assert(!(head.imageDescriptor & 0x10)); // x origin at the right not supported

	// Allocate image of the correct size

	allocateMemory(head.width, head.height, eFormat_8888);

	// Read the image data, in file order

	int	rowSz = head.bitsPerPixel / 8 * sizeX;
	for (int y = 0 ; y < sizeY ; ++y) {

		// Figure out which row this is in the image.
		// TGA's can be stored "upside down"

		int	dy;
		if (head.imageDescriptor & 0x20) {
			dy = y;
		} else {
			dy = sizeY - y - 1;
		}

		// Get shortcut pointer to first pixel in the row

		unsigned *destPtr = (unsigned *)data + dy*sizeX;

		// Read in the data for this row

		for (int x = 0 ; x < sizeX ; ++x) {
			int b = fgetc(f);
			int g = fgetc(f);
			int r = fgetc(f);
			int a;
			if (head.bitsPerPixel == 24) {
				a = 255;
			} else {
				a = fgetc(f);
			}
			if (b < 0 || g < 0 || r < 0 || a < 0) {
				goto ioError;
			}
			*destPtr = MAKE_ARGB(a, r, g, b);
			++destPtr;
		}
	}

	// OK

	fclose(f);
	return true;
}

//---------------------------------------------------------------------------
// Bitmap::loadBMP
//
// Load image in .BMP format.

bool	Bitmap::loadBMP(const char *filename, char *returnErrMsg) {

	// Free up anything already allocated

	freeMemory();

	// !FIXME!

	assert(false);
	strcpy(returnErrMsg, "BMP support not implemented yet");
	return false;
}
