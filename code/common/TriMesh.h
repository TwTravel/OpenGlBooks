/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// TriMesh.h - Triangle mesh class for real-time use (rendering and
// collision detection)
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __TRIMESH_H_INCLUDED__
#define __TRIMESH_H_INCLUDED__

#ifndef __AABB3_H_INCLUDED__
	#include "AABB3.h"
#endif

struct RenderVertex;
struct RenderTri;
class EditTriMesh;

/////////////////////////////////////////////////////////////////////////////
//
// TriMesh
//
// Triangle mesh class that can be rendered and collided with
//
/////////////////////////////////////////////////////////////////////////////

class TriMesh {
public:
	TriMesh();
	~TriMesh();

	// Memory allocation

	void	allocateMemory(int nVertexCount, int nTriCount);
	void	freeMemory();

	// Mesh accessesors

	int		getVertexCount() const { return vertexCount; }
	RenderVertex	*getVertexList() const { return vertexList; }
	int		getTriCount() const { return triCount; }
	RenderTri	*getTriList() const { return triList; }

	// Rendering.  This will use the current 3D context.

	void	render() const;

	// Bounding box

	void		computeBoundingBox();
	const AABB3	&getBoundingBox() const { return boundingBox; }

	// Conversion to/from an "edit" mesh.  Note that this class
	// doesn't know anything about parts or materials, so the
	// conversion is not an exact translation.

	void	fromEditMesh(const EditTriMesh &mesh);
	void	toEditMesh(EditTriMesh &mesh) const;

protected:

	// Mesh data

	int		vertexCount;
	RenderVertex	*vertexList;
	int		triCount;
	RenderTri	*triList;

	// Axially aligned bounding box.  You must call computeBoundingBox()
	// to update this if you modify the vertex list directly

	AABB3	boundingBox;
};

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __TRIMESH_H_INCLUDED__
