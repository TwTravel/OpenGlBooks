/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Bitmap.h - Simple bitmap loader
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __BITMAP_H_INCLUDED__
#define __BITMAP_H_INCLUDED__

/////////////////////////////////////////////////////////////////////////////
//
// class Bitmap - a simple class to hold a bitmap image
//
/////////////////////////////////////////////////////////////////////////////

class Bitmap {
public:

	// An enum to describe the format of the image

	enum EFormat {
		eFormat_None,	// dummy placeholder value
		eFormat_8888,	// 32-bit ARGB

		// !KLUDGE! FOr now, this is all we'll support.

	};

	// Constructor/destructor

	Bitmap();
	~Bitmap();

	// Memory management

	void	allocateMemory(int xs, int ys, EFormat format);
	void	freeMemory();

	// Accessors

	int	xSize() const { return sizeX; }
	int	ySize() const { return sizeY; }
	EFormat	format() const { return fmt; }
	void	*rawData() const { return data; }

	// Access to the pixel data.  Colors are specified using
	// the standard 0xAARRGGBB format used by class Renderer

	unsigned	getPix(int x, int y) const;
	void		setPix(int x, int y, unsigned argb);

	// Load a bitmap from an image file.  Uses the extension to
	// figure out how to load.

	bool	load(const char *filename, char *returnErrMsg);

	// Load a specific file format.

	bool	loadTGA(const char *filename, char *returnErrMsg);
	bool	loadBMP(const char *filename, char *returnErrMsg);

private:

	int	sizeX;
	int	sizeY;
	EFormat	fmt;
	void	*data;

};

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __BITMAP_H_INCLUDED__

