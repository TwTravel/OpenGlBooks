/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// MathUtil.h - Declarations for miscellaneous math utilities
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MATHUTIL_H_INCLUDED__
#define __MATHUTIL_H_INCLUDED__

#include <math.h>

// Declare a global constant for pi and a few multiples.

const float kPi = 3.14159265f;
const float k2Pi = kPi * 2.0f;
const float kPiOver2 = kPi / 2.0f;
const float k1OverPi = 1.0f / kPi;
const float k1Over2Pi = 1.0f / k2Pi;
const float kPiOver180 = kPi / 180.0f;
const float k180OverPi = 180.0f / kPi;

// "Wrap" an angle in range -pi...pi by adding the correct multiple
// of 2 pi

extern float wrapPi(float theta);

// "Safe" inverse trig functions

extern float safeAcos(float x);

// Convert between degrees and radians

inline float	degToRad(float deg) { return deg * kPiOver180; }
inline float	radToDeg(float rad) { return rad * k180OverPi; }

// Compute the sin and cosine of an angle.  On some platforms, if we know
// that we need both values, it can be computed faster than computing
// the two values seperately.

inline void sinCos(float *returnSin, float *returnCos, float theta) {

	// For simplicity, we'll just use the normal trig functions.
	// Note that on some platforms we may be able to do better

	*returnSin = sin(theta);
	*returnCos = cos(theta);
}

// Convert between "field of view" and "zoom"  See section 15.2.4.
// The FOV angle is specified in radians.

inline float	fovToZoom(float fov) { return 1.0f / tan(fov * .5f); }
inline float	zoomToFov(float zoom) { return 2.0f * atan(1.0f / zoom); }

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __MATHUTIL_H_INCLUDED__
