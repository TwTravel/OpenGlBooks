/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// TriMesh.cpp - Triangle mesh class for real-time use (rendering and
// collision detection)
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdlib.h>

#include "CommonStuff.h"
#include "TriMesh.h"
#include "Renderer.h"
#include "EditTriMesh.h"

/////////////////////////////////////////////////////////////////////////////
//
// class TriMesh member functions
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// TriMesh::TriMesh
//
// Constructor - reset internal variables to default (empty) state

TriMesh::TriMesh() {
	vertexCount = 0;
	vertexList = NULL;
	triCount = 0;
	triList = NULL;
	boundingBox.empty();
}

//---------------------------------------------------------------------------
// TriMesh::~TriMesh
//
// Destructor - make sure resources are freed

TriMesh::~TriMesh() {
	freeMemory();
}

//---------------------------------------------------------------------------
// TriMesh::allocateMemory
//
// Allocate mesh lists

void	TriMesh::allocateMemory(int nVertexCount, int nTriCount) {

	// First, make sure and free any memory already allocated

	freeMemory();

	// !KLUDGE! Since we are using unsigned shorts for indices,
	// we can't handle meshes with more than 65535 vertices

	if (nVertexCount > 65536) {
		ABORT("Can't allocate triangle mesh with more than 655356 vertices");
	}

	// Allocate vertex list

	vertexCount = nVertexCount;
	vertexList = new RenderVertex[vertexCount];

	// Allocate triangle list

	triCount = nTriCount;
	triList = new RenderTri[triCount];
}

//---------------------------------------------------------------------------
// TriMesh::freeMemory
//
// Free up any memory and reset object to default state

void	TriMesh::freeMemory() {

	// Free lists

	delete [] vertexList;
	delete [] triList;

	// Reset variables

	vertexList = NULL;
	triList = NULL;
	vertexCount = 0;
	triCount = 0;
}

//---------------------------------------------------------------------------
// TriMesh::render
//
// Render the mesh using current 3D renderer context

void	TriMesh::render() const {
	gRenderer.renderTriMesh(vertexList, vertexCount, triList, triCount);
}

//---------------------------------------------------------------------------
// TriMesh::computeBoundingBox
//
// Compute axially aligned bounding box from vertex list

void	TriMesh::computeBoundingBox() {

	// Empty bounding box

	boundingBox.empty();

	// Add in vertex locations

	for (int i = 0 ; i < vertexCount ; ++i) {
		boundingBox.add(vertexList[i].p);
	}
}

//---------------------------------------------------------------------------
// TriMesh::fromEditMesh
//
// Convert an EditTriMesh to a TriMesh.  Note that this function may need
// to make many logical changes to the mesh, such as ordering of vertices.
// Vertices may need to be duplictaed to place UV's at the vertex level.
// Unused vertices are discarded and the vertex list order is optimized.
// However, the actual mesh geometry will not be modified as far as number
// of faces, vertex positions, vertex normals, etc.
//
// Also, since TriMesh doesn't have any notion of parts or materials,
// that information is lost.
//
// The input mesh is not modified.

void	TriMesh::fromEditMesh(const EditTriMesh &mesh) {
	int	i;

	// Make a copy of the mesh

	EditTriMesh tempMesh(mesh);

	// Make sure UV's are perperly set at the vertex level

	tempMesh.copyUvsIntoVertices();

	// Optimize the order of the vertices for best cache performance.
	// This also discards unused vertices

	tempMesh.optimizeVertexOrder();

	// Allocate memory

	allocateMemory(tempMesh.vertexCount(), tempMesh.triCount());

	// Make sure we have something

	if (triCount < 1) {
		return;
	}

	// Convert vertices

	for (i = 0 ; i < vertexCount ; ++i) {
		const EditTriMesh::Vertex *s = &tempMesh.vertex(i);
		RenderVertex *d = &vertexList[i];

		d->p = s->p;
		d->n = s->normal;
		d->u = s->u;
		d->v = s->v;
	}

	// Convert faces

	for (i = 0 ; i < triCount ; ++i) {
		const EditTriMesh::Tri *s = &tempMesh.tri(i);
		RenderTri *d = &triList[i];
		d->index[0] = s->v[0].index;
		d->index[1] = s->v[1].index;
		d->index[2] = s->v[2].index;
	}

	// Make sure bounds are computed

	computeBoundingBox();
}

//---------------------------------------------------------------------------
// TriMesh::toEditMesh
//
// Convert a TriMesh to an EditTriMesh.  The output mesh is setup with a
// single default part and a single default material.

void	TriMesh::toEditMesh(EditTriMesh &mesh) const {
	// !FIXME!
	assert(false);
}
