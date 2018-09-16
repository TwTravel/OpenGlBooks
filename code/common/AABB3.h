/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// AABB3.h - Declarations for class AABB3
//
// Visit gamemath.com for the latest version of this file.
//
// For more details, see AABB3.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __AABB3_H_INCLUDED__
#define __AABB3_H_INCLUDED__

#ifndef __VECTOR3_H_INCLUDED__
	#include "Vector3.h"
#endif

class Matrix4x3;

//---------------------------------------------------------------------------
// class AABB3
//
// Implement a 3D axially aligned bounding box

class AABB3 {
public:

// Public data

	// Min and max values.  Pretty simple.

	Vector3	min;
	Vector3	max;

// Query for dimentions

	Vector3	size() const { return max - min; }
	float	xSize() { return max.x - min.x; }
	float	ySize() { return max.y - min.y; }
	float	zSize() { return max.z - min.z; }
	Vector3	center() const { return (min + max) * .5f; }

	// Fetch one of the eight corner points.  See the
	// .cpp for numbering conventions

	Vector3	corner(int i) const;

// Box operations

	// "Empty" the box, by setting the values to really
	// large/small numbers

	void	empty();

	// Add a point to the box

	void	add(const Vector3 &p);

	// Add an AABB to the box

	void	add(const AABB3 &box);

	// Transform the box and compute the new AABB

	void	setToTransformedBox(const AABB3 &box, const Matrix4x3 &m);

// Containment/intersection tests

	// Return true if the box is enmpty

	bool	isEmpty() const;

	// Return true if the box contains a point

	bool	contains(const Vector3 &p) const;

	// Return the closest point on this box to another point

	Vector3	closestPointTo(const Vector3 &p) const;

	// Return true if we intersect a sphere

	bool	intersectsSphere(const Vector3 &center, float radius) const;

	// Parametric intersection with a ray.  Returns >1 if no intresection

	float	rayIntersect(const Vector3 &rayOrg, const Vector3 &rayDelta,
		Vector3 *returnNormal = 0) const;

	// Classify box as being on one side or the other of a plane

	int	classifyPlane(const Vector3 &n, float d) const;

	// Dynamic intersection with plane

	float	intersectPlane(const Vector3 &n, float planeD,
		const Vector3 &dir) const;
};

// Check if two AABBs intersect, and return true if so.  Optionally return
// the AABB of their intersection if an intersection is detected

bool	intersectAABBs(const AABB3 &box1, const AABB3 &box2,
	AABB3 *boxIntersect = 0);

// Return parametric point in time when a moving AABB collides
// with a stationary AABB.  Returns > 1 if no intersection

float	intersectMovingAABB(
	const AABB3 &stationaryBox,
	const AABB3 &movingBox,
	const Vector3 &d
);

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __AABB3_H_INCLUDED__
