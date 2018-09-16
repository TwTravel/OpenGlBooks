/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Matrix4x3.cpp - Implementation of class Matrix4x3
//
// Visit gamemath.com for the latest version of this file.
//
// For more details see section 11.5.
//
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <math.h>

#include "Vector3.h"
#include "EulerAngles.h"
#include "Quaternion.h"
#include "RotationMatrix.h"
#include "Matrix4x3.h"
#include "MathUtil.h"

/////////////////////////////////////////////////////////////////////////////
//
// Notes:
//
// See Chapter 11 for more information on class design decisions.
//
//---------------------------------------------------------------------------
//
// MATRIX ORGANIZATION
//
// The purpose of this class is so that a user might perform transformations
// without fiddling with plus or minus signs or transposing the matrix
// until the output "looks right."  But of course, the specifics of the
// internal representation is important.  Not only for the implementation
// in this file to be correct, but occasionally direct access to the
// matrix variables is necessary, or beneficial for optimization.  Thus,
// we document our matrix conventions here.
//
// We use row vectors, so multiplying by our matrix looks like this:
//
//               | m11 m12 m13 |
//     [ x y z ] | m21 m22 m23 | = [ x' y' z' ]
//               | m31 m32 m33 |
//               | tx  ty  tz  |
//
// Strict adherance to linear algebra rules dictates that this
// multiplication is actually undefined.  To circumvent this, we can
// consider the input and output vectors as having an assumed fourth
// coordinate of 1.  Also, since we cannot technically invert a 4x3 matrix
// according to linear algebra rules, we will also assume a rightmost
// column of [ 0 0 0 1 ].  This is shown below:
//
//                 | m11 m12 m13 0 |
//     [ x y z 1 ] | m21 m22 m23 0 | = [ x' y' z' 1 ]
//                 | m31 m32 m33 0 |
//                 | tx  ty  tz  1 |
//
// In case you have forgotten your linear algebra rules for multiplying
// matrices (which are described in section 7.1.6 and 7.1.7), see the
// definition of operator* for the expanded computations.
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Matrix4x3 class members
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Matrix4x3::identity
//
// Set the matrix to identity

void	Matrix4x3::identity() {
	m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
	m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
	m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
	tx  = 0.0f; ty  = 0.0f; tz  = 1.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::zeroTranslation
//
// Zero the 4th row of the matrix, which contains the translation portion.

void	Matrix4x3::zeroTranslation() {
	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setTranslation
//
// Sets the translation portion of the matrix in vector form

void	Matrix4x3::setTranslation(const Vector3 &d) {
	tx = d.x; ty = d.y; tz = d.z;
}

//---------------------------------------------------------------------------
// Matrix4x3::setTranslation
//
// Sets the translation portion of the matrix in vector form

void	Matrix4x3::setupTranslation(const Vector3 &d) {

	// Set the linear transformation portion to identity

	m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
	m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
	m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;

	// Set the translation portion

	tx = d.x; ty = d.y; tz = d.z;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupLocalToParent
//
// Setup the matrix to perform a local -> parent transformation, given
// the position and orientation of the local reference frame within the
// parent reference frame.
//
// A very common use of this will be to construct a object -> world matrix.
// As an example, the transformation in this case is straightforward.  We
// first rotate from object space into inertial space, then we translate
// into world space.
//
// We allow the orientation to be specified using either euler angles,
// or a RotationMatrix

void	Matrix4x3::setupLocalToParent(const Vector3 &pos, const EulerAngles &orient) {

	// Create a rotation matrix.

	RotationMatrix orientMatrix;
	orientMatrix.setup(orient);

	// Setup the 4x3 matrix.  Note: if we were really concerned with
	// speed, we could create the matrix directly into these variables,
	// without using the temporary RotationMatrix object.  This would
	// save us a function call and a few copy operations.

	setupLocalToParent(pos, orientMatrix);
}

void	Matrix4x3::setupLocalToParent(const Vector3 &pos, const RotationMatrix &orient) {

	// Copy the rotation portion of the matrix.  According to
	// the comments in RotationMatrix.cpp, the rotation matrix
	// is "normally" an inertial->object matrix, which is
	// parent->local.  We want a local->parent rotation, so we
	// must transpose while copying

	m11 = orient.m11; m12 = orient.m21; m13 = orient.m31;
	m21 = orient.m12; m22 = orient.m22; m23 = orient.m32;
	m31 = orient.m13; m32 = orient.m23; m33 = orient.m33;

	// Now set the translation portion.  Translation happens "after"
	// the 3x3 portion, so we can simply copy the position
	// field directly

	tx = pos.x; ty = pos.y; tz = pos.z;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupParentToLocal
//
// Setup the matrix to perform a parent -> local transformation, given
// the position and orientation of the local reference frame within the
// parent reference frame.
//
// A very common use of this will be to construct a world -> object matrix.
// To perform this transformation, we would normally FIRST transform
// from world to inertial space, and then rotate from inertial space into
// object space.  However, out 4x3 matrix always translates last.  So
// we think about creating two matrices T and R, and then concatonating
// M = TR.
//
// We allow the orientation to be specified using either euler angles,
// or a RotationMatrix

void	Matrix4x3::setupParentToLocal(const Vector3 &pos, const EulerAngles &orient) {

	// Create a rotation matrix.

	RotationMatrix orientMatrix;
	orientMatrix.setup(orient);

	// Setup the 4x3 matrix.

	setupParentToLocal(pos, orientMatrix);
}

void	Matrix4x3::setupParentToLocal(const Vector3 &pos, const RotationMatrix &orient) {

	// Copy the rotation portion of the matrix.  We can copy the
	// elements directly (without transposing) according
	// to the layout as commented in RotationMatrix.cpp

	m11 = orient.m11; m12 = orient.m12; m13 = orient.m13;
	m21 = orient.m21; m22 = orient.m22; m23 = orient.m23;
	m31 = orient.m31; m32 = orient.m32; m33 = orient.m33;

	// Now set the translation portion.  Normally, we would
	// translate by the negative of the position to translate
	// from world to inertial space.  However, we must correct
	// for the fact that the rotation occurs "first."  So we
	// must rotate the translation portion.  This is the same
	// as create a translation matrix T to translate by -pos,
	// and a rotation matrix R, and then creating the matrix
	// as the concatenation of TR

	tx = -(pos.x*m11 + pos.y*m21 + pos.z*m31);
	ty = -(pos.x*m12 + pos.y*m22 + pos.z*m32);
	tz = -(pos.x*m13 + pos.y*m23 + pos.z*m33);
}

//---------------------------------------------------------------------------
// Matrix4x3::setupRotate
//
// Setup the matrix to perform a rotation about a cardinal axis
//
// The axis of rotation is specified using a 1-based index:
//
//	1 => rotate about the x-axis
//	2 => rotate about the y-axis
//	3 => rotate about the z-axis
//
// theta is the amount of rotation, in radians.  The left-hand rule is
// used to define "positive" rotation.
//
// The translation portion is reset.
//
// See 8.2.2 for more info.

void	Matrix4x3::setupRotate(int axis, float theta) {

	// Get sin and cosine of rotation angle

	float	s, c;
	sinCos(&s, &c, theta);

	// Check which axis they are rotating about

	switch (axis) {

		case 1: // Rotate about the x-axis

			m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
			m21 = 0.0f; m22 = c;    m23 = s;
			m31 = 0.0f; m32 = -s;   m33 = c;
			break;

		case 2: // Rotate about the y-axis

			m11 = c;    m12 = 0.0f; m13 = -s;
			m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
			m31 = s;    m32 = 0.0f; m33 = c;
			break;

		case 3: // Rotate about the z-axis

			m11 = c;    m12 = s;    m13 = 0.0f;
			m21 = -s;   m22 = c;    m23 = 0.0f;
			m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
			break;

		default:

			// bogus axis index

			assert(false);
	}

	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupRotate
//
// Setup the matrix to perform a rotation about an arbitrary axis.
// The axis of rotation must pass through the origin.
//
// axis defines the axis of rotation, and must be a unit vector.
//
// theta is the amount of rotation, in radians.  The left-hand rule is
// used to define "positive" rotation.
//
// The translation portion is reset.
//
// See 8.2.3 for more info.

void	Matrix4x3::setupRotate(const Vector3 &axis, float theta) {

	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis

	assert(fabs(axis*axis - 1.0f) < .01f);

	// Get sin and cosine of rotation angle

	float	s, c;
	sinCos(&s, &c, theta);

	// Compute 1 - cos(theta) and some common subexpressions

	float	a = 1.0f - c;
	float	ax = a * axis.x;
	float	ay = a * axis.y;
	float	az = a * axis.z;

	// Set the matrix elements.  There is still a little more
	// opportunity for optimization due to the many common
	// subexpressions.  We'll let the compiler handle that...

	m11 = ax*axis.x + c;
	m12 = ax*axis.y + axis.z*s;
	m13 = ax*axis.z - axis.y*s;

	m21 = ay*axis.x - axis.z*s;
	m22 = ay*axis.y + c;
	m23 = ay*axis.z + axis.x*s;

	m31 = az*axis.x + axis.y*s;
	m32 = az*axis.y - axis.x*s;
	m33 = az*axis.z + c;

	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::fromQuaternion
//
// Setup the matrix to perform a rotation, given the angular displacement
// in quaternion form.
//
// The translation portion is reset.
//
// See 10.6.3 for more info.

void	Matrix4x3::fromQuaternion(const Quaternion &q) {

	// Compute a few values to optimize common subexpressions

	float	ww = 2.0f * q.w;
	float	xx = 2.0f * q.x;
	float	yy = 2.0f * q.y;
	float	zz = 2.0f * q.z;

	// Set the matrix elements.  There is still a little more
	// opportunity for optimization due to the many common
	// subexpressions.  We'll let the compiler handle that...

	m11 = 1.0f - yy*q.y - zz*q.z;
	m12 = xx*q.y + ww*q.z;
	m13 = xx*q.z - ww*q.x;

	m21 = xx*q.y - ww*q.z;
	m22 = 1.0f - xx*q.x - zz*q.z;
	m23 = yy*q.z + ww*q.x;

	m31 = xx*q.z + ww*q.y;
	m32 = yy*q.z - ww*q.x;
	m33 = 1.0f - xx*q.x - yy*q.y;

	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupScale
//
// Setup the matrix to perform scale on each axis.  For uniform scale by k,
// use a vector of the form Vector3(k,k,k)
//
// The translation portion is reset.
//
// See 8.3.1 for more info.

void	Matrix4x3::setupScale(const Vector3 &s) {

	// Set the matrix elements.  Pretty straightforward

	m11 = s.x;  m12 = 0.0f; m13 = 0.0f;
	m21 = 0.0f; m22 = s.y;  m23 = 0.0f;
	m31 = 0.0f; m32 = 0.0f; m33 = s.z;

	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupScaleAlongAxis
//
// Setup the matrix to perform scale along an arbitrary axis.
//
// The axis is specified using a unit vector.
//
// The translation portion is reset.
//
// See 8.3.2 for more info.

void	Matrix4x3::setupScaleAlongAxis(const Vector3 &axis, float k) {

	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis

	assert(fabs(axis*axis - 1.0f) < .01f);

	// Compute k-1 and some common subexpressions

	float	a = k - 1.0f;
	float	ax = a * axis.x;
	float	ay = a * axis.y;
	float	az = a * axis.z;

	// Fill in the matrix elements.  We'll do the common
	// subexpression optimization ourselves here, since diagonally
	// opposite matrix elements are equal

	m11 = ax*axis.x + 1.0f;
	m22 = ay*axis.y + 1.0f;
	m32 = az*axis.z + 1.0f;

	m12 = m21 = ax*axis.y;
	m13 = m31 = ax*axis.z;
	m23 = m32 = ay*axis.z;

	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupShear
//
// Setup the matrix to perform a shear
//
// The type of shear is specified by the 1-based "axis" index.  The effect
// of transforming a point by the matrix is described by the pseudocode
// below:
//
//	axis == 1  =>  y += s*x, z += t*x
//	axis == 2  =>  x += s*y, z += t*y
//	axis == 3  =>  x += s*z, y += t*z
//
// The translation portion is reset.
//
// See 8.6 for more info.

void	Matrix4x3::setupShear(int axis, float s, float t) {

	// Check which type of shear they want

	switch (axis) {

		case 1: // Shear y and z using x

			m11 = 1.0f; m12 = s;    m13 = t;
			m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
			m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
			break;

		case 2: // Shear x and z using y

			m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
			m21 = s;    m22 = 1.0f; m23 = t;
			m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
			break;

		case 3: // Shear x and y using z

			m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
			m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
			m31 = s;    m32 = t;    m33 = 1.0f;
			break;

		default:

			// bogus axis index

			assert(false);
	}

	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupProject
//
// Setup the matrix to perform a projection onto a plane passing
// through the origin.  The plane is perpendicular to the
// unit vector n.
//
// See 8.4.2 for more info.

void	Matrix4x3::setupProject(const Vector3 &n) {

	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis

	assert(fabs(n*n - 1.0f) < .01f);

	// Fill in the matrix elements.  We'll do the common
	// subexpression optimization ourselves here, since diagonally
	// opposite matrix elements are equal

	m11 = 1.0f - n.x*n.x;
	m22 = 1.0f - n.y*n.y;
	m33 = 1.0f - n.z*n.z;

	m12 = m21 = -n.x*n.y;
	m13 = m31 = -n.x*n.z;
	m23 = m32 = -n.y*n.z;

	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Matrix4x3::setupReflect
//
// Setup the matrix to perform a reflection about a plane parallel
// to a cardinal plane.
//
// axis is a 1-based index which specifies the plane to project about:
//
//	1 => reflect about the plane x=k
//	2 => reflect about the plane y=k
//	3 => reflect about the plane z=k
//
// The translation is set appropriately, since translation must occur if
// k != 0
//
// See 8.5 for more info.

void	Matrix4x3::setupReflect(int axis, float k) {

	// Check which plane they want to reflect about

	switch (axis) {

		case 1: // Reflect about the plane x=k

			m11 = -1.0f; m12 =  0.0f; m13 =  0.0f;
			m21 =  0.0f; m22 =  1.0f; m23 =  0.0f;
			m31 =  0.0f; m32 =  0.0f; m33 =  1.0f;

			tx = 2.0f * k;
			ty = 0.0f;
			tz = 0.0f;

			break;

		case 2: // Reflect about the plane y=k

			m11 =  1.0f; m12 =  0.0f; m13 =  0.0f;
			m21 =  0.0f; m22 = -1.0f; m23 =  0.0f;
			m31 =  0.0f; m32 =  0.0f; m33 =  1.0f;

			tx = 0.0f;
			ty = 2.0f * k;
			tz = 0.0f;

			break;

		case 3: // Reflect about the plane z=k

			m11 =  1.0f; m12 =  0.0f; m13 =  0.0f;
			m21 =  0.0f; m22 =  1.0f; m23 =  0.0f;
			m31 =  0.0f; m32 =  0.0f; m33 = -1.0f;

			tx = 0.0f;
			ty = 0.0f;
			tz = 2.0f * k;

			break;

		default:

			// bogus axis index

			assert(false);
	}

}

//---------------------------------------------------------------------------
// Matrix4x3::setupReflect
//
// Setup the matrix to perform a reflection about an arbitrary plane
// through the origin.  The unit vector n is perpendicular to the plane.
//
// The translation portion is reset.
//
// See 8.5 for more info.

void	Matrix4x3::setupReflect(const Vector3 &n) {

	// Quick sanity check to make sure they passed in a unit vector
	// to specify the axis

	assert(fabs(n*n - 1.0f) < .01f);

	// Compute common subexpressions

	float	ax = -2.0f * n.x;
	float	ay = -2.0f * n.y;
	float	az = -2.0f * n.z;

	// Fill in the matrix elements.  We'll do the common
	// subexpression optimization ourselves here, since diagonally
	// opposite matrix elements are equal

	m11 = 1.0f + ax*n.x;
	m22 = 1.0f + ay*n.y;
	m32 = 1.0f + az*n.z;

	m12 = m21 = ax*n.y;
	m13 = m31 = ax*n.z;
	m23 = m32 = ay*n.z;

	// Reset the translation portion

	tx = ty = tz = 0.0f;
}

//---------------------------------------------------------------------------
// Vector * Matrix4x3
//
// Transform the point.  This makes using the vector class look like it
// does with linear algebra notation on paper.
//
// We also provide a *= operator, as per C convention.
//
// See 7.1.7


Vector3	operator*(const Vector3 &p, const Matrix4x3 &m) {

	// Grind through the linear algebra.

	return Vector3(
		p.x*m.m11 + p.y*m.m21 + p.z*m.m31 + m.tx,
		p.x*m.m12 + p.y*m.m22 + p.z*m.m32 + m.ty,
		p.x*m.m13 + p.y*m.m23 + p.z*m.m33 + m.tz
	);
}

Vector3 &operator*=(Vector3 &p, const Matrix4x3 &m) {
	p = p * m;
	return p;
}

//---------------------------------------------------------------------------
// Matrix4x3 * Matrix4x3
//
// Matrix concatenation.  This makes using the vector class look like it
// does with linear algebra notation on paper.
//
// We also provide a *= operator, as per C convention.
//
// See 7.1.6

Matrix4x3 operator*(const Matrix4x3 &a, const Matrix4x3 &b) {

	Matrix4x3 r;

	// Compute the upper 3x3 (linear transformation) portion

	r.m11 = a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31;
	r.m12 = a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32;
	r.m13 = a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33;

	r.m21 = a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31;
	r.m22 = a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32;
	r.m23 = a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33;

	r.m31 = a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31;
	r.m32 = a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32;
	r.m33 = a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33;

	// Compute the translation portion

	r.tx = a.tx*b.m11 + a.ty*b.m21 + a.tz*b.m31 + b.tx;
	r.ty = a.tx*b.m12 + a.ty*b.m22 + a.tz*b.m32 + b.ty;
	r.tz = a.tx*b.m13 + a.ty*b.m23 + a.tz*b.m33 + b.tz;

	// Return it.  Ouch - involves a copy constructor call.  If speed
	// is critical, we may need a seperate function which places the
	// result where we want it...

	return r;
}

Matrix4x3 &operator*=(Matrix4x3 &a, const Matrix4x3 &b) {
	a = a * b;
	return a;
}

//---------------------------------------------------------------------------
// determinant
//
// Compute the determinant of the 3x3 portion of the matrix.
//
// See 9.1.1 for more info.

float	determinant(const Matrix4x3 &m) {
	return
		  m.m11 * (m.m22*m.m33 - m.m23*m.m32)
		+ m.m12 * (m.m23*m.m31 - m.m21*m.m33)
		+ m.m13 * (m.m21*m.m32 - m.m22*m.m31);
}

//---------------------------------------------------------------------------
// inverse
//
// Compute the inverse of a matrix.  We use the classical adjoint divided
// by the determinant method.
//
// See 9.2.1 for more info.

Matrix4x3 inverse(const Matrix4x3 &m) {

	// Compute the determinant

	float	det = determinant(m);

	// If we're singular, then the determinant is zero and there's
	// no inverse

	assert(fabs(det) > 0.000001f);

	// Compute one over the determinant, so we divide once and
	// can *multiply* per element

	float	oneOverDet = 1.0f / det;

	// Compute the 3x3 portion of the inverse, by
	// dividing the adjoint by the determinant

	Matrix4x3	r;

	r.m11 = (m.m22*m.m33 - m.m23*m.m32) * oneOverDet;
	r.m12 = (m.m13*m.m32 - m.m12*m.m33) * oneOverDet;
	r.m13 = (m.m12*m.m23 - m.m13*m.m22) * oneOverDet;

	r.m21 = (m.m23*m.m31 - m.m21*m.m33) * oneOverDet;
	r.m22 = (m.m11*m.m33 - m.m13*m.m31) * oneOverDet;
	r.m23 = (m.m13*m.m21 - m.m11*m.m23) * oneOverDet;

	r.m31 = (m.m21*m.m32 - m.m22*m.m31) * oneOverDet;
	r.m32 = (m.m12*m.m31 - m.m11*m.m32) * oneOverDet;
	r.m33 = (m.m11*m.m22 - m.m12*m.m21) * oneOverDet;

	// Compute the translation portion of the inverse

	r.tx = -(m.tx*r.m11 + m.ty*r.m21 + m.tz*r.m31);
	r.ty = -(m.tx*r.m12 + m.ty*r.m22 + m.tz*r.m32);
	r.tz = -(m.tx*r.m13 + m.ty*r.m23 + m.tz*r.m33);

	// Return it.  Ouch - involves a copy constructor call.  If speed
	// is critical, we may need a seperate function which places the
	// result where we want it...

	return r;
}

//---------------------------------------------------------------------------
// getTranslation
//
// Return the translation row of the matrix in vector form

Vector3	getTranslation(const Matrix4x3 &m) {
	return Vector3(m.tx, m.ty, m.tz);
}

//---------------------------------------------------------------------------
// getPositionFromParentToLocalMatrix
//
// Extract the position of an object given a parent -> local transformation
// matrix (such as a world -> object matrix)
//
// We assume that the matrix represents a rigid transformation.  (No scale,
// skew, or mirroring)

Vector3	getPositionFromParentToLocalMatrix(const Matrix4x3 &m) {

	// Multiply negative translation value by the
	// transpose of the 3x3 portion.  By using the transpose,
	// we assume that the matrix is orthogonal.  (This function
	// doesn't really make sense for non-rigid transformations...)

	return Vector3(
		-(m.tx*m.m11 + m.ty*m.m12 + m.tz*m.m13),
		-(m.tx*m.m21 + m.ty*m.m22 + m.tz*m.m23),
		-(m.tx*m.m31 + m.ty*m.m32 + m.tz*m.m33)
	);
}

//---------------------------------------------------------------------------
// getPositionFromLocalToParentMatrix
//
// Extract the position of an object given a local -> parent transformation
// matrix (such as an object -> world matrix)

Vector3	getPositionFromLocalToParentMatrix(const Matrix4x3 &m) {

	// Position is simply the translation portion

	return Vector3(m.tx, m.ty, m.tz);
}
