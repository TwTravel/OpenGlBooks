/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// RotationMatrix.h - Declarations for class RotationMatrix
//
// Visit gamemath.com for the latest version of this file.
//
// For more details, see RotationMatrix.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __ROTATIONMATRIX_H_INCLUDED__
#define __ROTATIONMATRIX_H_INCLUDED__

class Vector3;
class EulerAngles;
class Quaternion;

//---------------------------------------------------------------------------
// class RotationMatrix
//
// Implement a simple 3x3 matrix that is used for ROTATION ONLY.  The
// matrix is assumed to be orthogonal.  The direction of transformation
// is specified at the time of transformation.

class RotationMatrix {
public:

// Public data

	// The 9 values of the matrix.  See RotationMatrix.cpp file for
	// the details of the layout

	float	m11, m12, m13;
	float	m21, m22, m23;
	float	m31, m32, m33;

// Public operations

	// Set to identity

	void	identity();

	// Setup the matrix with a specified orientation

	void	setup(const EulerAngles &orientation);

	// Setup the matrix from a quaternion, assuming the
	// quaternion performs the rotation in the
	// specified direction of transformation

	void	fromInertialToObjectQuaternion(const Quaternion &q);
	void	fromObjectToInertialQuaternion(const Quaternion &q);

	// Perform rotations

	Vector3	inertialToObject(const Vector3 &v) const;
	Vector3	objectToInertial(const Vector3 &v) const;
};

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __ROTATIONMATRIX_H_INCLUDED__
