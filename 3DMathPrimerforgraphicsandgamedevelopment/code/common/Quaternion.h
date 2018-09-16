/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Quaternion.h - Declarations for class Quaternion
//
// Visit gamemath.com for the latest version of this file.
//
// For more details, see Quaternion.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __QUATERNION_H_INCLUDED__
#define __QUATERNION_H_INCLUDED__

class Vector3;
class EulerAngles;

//---------------------------------------------------------------------------
// class Quaternion
//
// Implement a quaternion, for purposes of representing an angular
// displacement (orientation) in 3D.

class Quaternion {
public:

// Public data

	// The 4 values of the quaternion.  Normally, it will not
	// be necessary to manipulate these directly.  However,
	// we leave them public, since prohibiting direct access
	// makes some operations, such as file I/O, unnecessarily
	// complicated.

	float	w, x, y, z;

// Public operations

	// Set to identity

	void	identity() { w = 1.0f; x = y = z = 0.0f; }

	// Setup the quaternion to a specific rotation

	void	setToRotateAboutX(float theta);
	void	setToRotateAboutY(float theta);
	void	setToRotateAboutZ(float theta);
	void	setToRotateAboutAxis(const Vector3 &axis, float theta);

	// Setup to perform object<->inertial rotations,
	// given orientation in Euler angle format

	void	setToRotateObjectToInertial(const EulerAngles &orientation);
	void	setToRotateInertialToObject(const EulerAngles &orientation);

	// Cross product

	Quaternion operator *(const Quaternion &a) const;

	// Multiplication with assignment, as per C++ convention

	Quaternion &operator *=(const Quaternion &a);

	// Normalize the quaternion.

	void	normalize();

	// Extract and return the rotation angle and axis.

	float	getRotationAngle() const;
	Vector3	getRotationAxis() const;
};

// A global "identity" quaternion constant

extern const Quaternion kQuaternionIdentity;

// Quaternion dot product.

extern float dotProduct(const Quaternion &a, const Quaternion &b);

// Spherical linear interpolation

extern Quaternion slerp(const Quaternion &p, const Quaternion &q, float t);

// Quaternion conjugation

extern Quaternion conjugate(const Quaternion &q);

// Quaternion exponentiation

extern Quaternion pow(const Quaternion &q, float exponent);

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __QUATERNION_H_INCLUDED__
