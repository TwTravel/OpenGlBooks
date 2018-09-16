/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// EulerAngles.h - Declarations for class EulerAngles
//
// Visit gamemath.com for the latest version of this file.
//
// For more details, see EulerAngles.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __EULERANGLES_H_INCLUDED__
#define __EULERANGLES_H_INCLUDED__

// Forward declarations

class Quaternion;
class Matrix4x3;
class RotationMatrix;

//---------------------------------------------------------------------------
// class EulerAngles
//
// This class represents a heading-pitch-bank Euler angle triple.

class EulerAngles {
public:

// Public data

	// Straightforward representation.  Store the three angles, in
	// radians

	float	heading;
	float	pitch;
	float	bank;

// Public operations

	// Default constructor does nothing

	EulerAngles() {}

	// Construct from three values

	EulerAngles(float h, float p, float b) :
		heading(h), pitch(p), bank(b) {}

	// Set to identity triple (all zeros)

	void	identity() { pitch = bank = heading = 0.0f; }

	// Determine "canonical" Euler angle triple

	void	canonize();

	// Convert the quaternion to Euler angle format.  The input quaternion
	// is assumed to perform the rotation from object-to-inertial
	// or inertial-to-object, as indicated.

	void	fromObjectToInertialQuaternion(const Quaternion &q);
	void	fromInertialToObjectQuaternion(const Quaternion &q);

	// Convert the transform matrix to Euler angle format.  The input
	// matrix is assumed to perform the transformation from
	// object-to-world, or world-to-object, as indicated.  The
	// translation portion of the matrix is ignored.  The
	// matrix is assumed to be orthogonal.

	void	fromObjectToWorldMatrix(const Matrix4x3 &m);
	void	fromWorldToObjectMatrix(const Matrix4x3 &m);

	// Convert a rotation matrix to Euler Angle form.

	void	fromRotationMatrix(const RotationMatrix &m);
};

// A global "identity" Euler angle constant

extern const EulerAngles kEulerAnglesIdentity;

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __EULERANGLES_H_INCLUDED__
