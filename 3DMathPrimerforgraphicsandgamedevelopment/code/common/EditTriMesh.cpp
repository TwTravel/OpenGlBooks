/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// EditTriMesh.cpp - Implementation of class EditTriMesh
//
// Visit gamemath.com for the latest version of this file.
//
// For more details, see Chapter 13
//
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "MathUtil.h"
#include "EditTriMesh.h"
#include "CommonStuff.h"
#include "Matrix4x3.h"
#include "AABB3.h"

/////////////////////////////////////////////////////////////////////////////
//
// Local utility stuff
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// vertexCompareByMark
//
// Compare two vertices by their mark field.  Used to sort using qsort.

static int vertexCompareByMark(const void *va, const void *vb) {

	// Cast pointers

	const EditTriMesh::Vertex *a = (const EditTriMesh::Vertex *)va;
	const EditTriMesh::Vertex *b = (const EditTriMesh::Vertex *)vb;

	// Return comparison result as per Qsort conventions:
	//
	// <0	a goes "before" b
	// >0	a goes "after" b
	// 0	doesn't matter
	//
	// We want the lower mark to come first

	return a->mark - b->mark;
}

//---------------------------------------------------------------------------
// triCompareByMaterial
//
// Compare two triangles by their material field.  Used to sort using qsort.

static int triCompareByMaterial(const void *va, const void *vb) {

	// Cast pointers

	const EditTriMesh::Tri *a = (const EditTriMesh::Tri *)va;
	const EditTriMesh::Tri *b = (const EditTriMesh::Tri *)vb;

	// Return comparison result as per Qsort conventions:
	//
	// <0	a goes "before" b
	// >0	a goes "after" b
	// 0	doesn't matter
	//
	// We want the lower material to come first

	if (a->material < b->material) return -1;
	if (a->material > b->material) return +1;

	// Same material - use "mark" field, which contained the
	// original index, so we'll have a stable sort

	return a->mark - b->mark;
}

//---------------------------------------------------------------------------
// skipLine
//
// Skip a line of text from a file.  Returns false on failure (EOF or error).

static bool	skipLine(FILE *f) {
	for (;;) {
		int c = fgetc(f);
		if (c < 0) {
			return false;
		}
		if (c == '\n') {
			return true;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// EditTriMesh helper class members
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// EditTriMesh::Vertex::setDefaults
//
// Reset vertex to a default state

void EditTriMesh::Vertex::setDefaults() {

	// Reset everything

	memset(this, 0, sizeof(this));
}

//---------------------------------------------------------------------------
// EditTriMesh::Tri::setDefaults
//
// Reset triangle to a default state

void EditTriMesh::Tri::setDefaults() {

	// Reset everything

	memset(this, 0, sizeof(this));
}

//---------------------------------------------------------------------------
// EditTriMesh::Tri::isDegenerate
//
// Return true if we are degenerate (any two vertex indices are the same)

bool EditTriMesh::Tri::isDegenerate() const {
	return
		(v[0].index == v[1].index) ||
		(v[1].index == v[2].index) ||
		(v[0].index == v[2].index);
}

//---------------------------------------------------------------------------
// EditTriMesh::Tri::findVertex
//
// Check if we use the vertex (by index into the master vertex list).  Return
// the first face vertex index (0..2) if we reference it, or -1 otherwise

int EditTriMesh::Tri::findVertex(int vertexIndex) const {

	// Search vertices.  Let's unroll the loop here...

	if (v[0].index == vertexIndex) return 0;
	if (v[1].index == vertexIndex) return 1;
	if (v[2].index == vertexIndex) return 2;

	// Not found.

	return -1;
}

//---------------------------------------------------------------------------
// EditTriMesh::Material::setDefaults
//
// Reset material to a default state

void EditTriMesh::Material::setDefaults() {

	// Reset everything

	memset(this, 0, sizeof(this));
}

//---------------------------------------------------------------------------
// EditTriMesh::Part::setDefaults
//
// Reset part to a default state

void EditTriMesh::Part::setDefaults() {

	// Reset everything

	memset(this, 0, sizeof(this));
}

//---------------------------------------------------------------------------
// EditTriMesh::OptimizationParameters::setDefaults
//
// Set options to reasonable default values.  (*I* think they are reasonable
// you may not...)

void	EditTriMesh::OptimizationParameters::setDefaults() {

	// Weld vertices within 1/8 of an inch.  (We use 1 unit = 1ft)

	coincidentVertexTolerance = 1.0f / 12.0f / 8.0f;

	// Weld vertices across edge if the edge is 80 degrees or more.
	// If more (for example, the edges of a cube) then let's keep
	// the edges detached

	setEdgeAngleToleranceInDegrees(80.0f);
}

//---------------------------------------------------------------------------
// EditTriMesh::OptimizationParameters::setEdgeAngleToleranceInDegrees
//
// Set tolerance angle value used to determine if two vertices can be
// welded.  If they share a very "sharp" edge we may not wish to weld them,
// since it destroys the lighting discontinuity that should be present at
// this geometric discontinuity.
//
// Pass in a really large number (> 180 degrees) to effectively
// weld all evrtices, regardless of angle tolerance

void	EditTriMesh::OptimizationParameters::setEdgeAngleToleranceInDegrees(float degrees) {

	// Check for a really big angle

	if (degrees >= 180.0f) {

		// Slam cosine to very small number

		cosOfEdgeAngleTolerance = -999.0f;
	} else {

		// Compute the cosine

		cosOfEdgeAngleTolerance = cos(degToRad(degrees));
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// EditTriMesh members - Standard class object maintenance
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// EditTriMesh::EditTriMesh
//
// Default constructor - reset object to default, empty state.

EditTriMesh::EditTriMesh() {
	construct();
}

//---------------------------------------------------------------------------
// EditTriMesh::EditTriMesh
//
// Copy constructor - make a copy of a mesh.

EditTriMesh::EditTriMesh(const EditTriMesh &x) {

	// Initialize to empty state

	construct();

	// Use operator= to do the real work and make a copy

	*this = x;
}

//---------------------------------------------------------------------------
// EditTriMesh::construct
//
// Do the real work of object construction

void	EditTriMesh::construct() {

	// Reset the lists

	vAlloc = 0;
	vCount = 0;
	vList = NULL;

	tAlloc = 0;
	tCount = 0;
	tList = NULL;

	mCount = 0;
	mList = NULL;

	pCount = 0;
	pList = NULL;
}

//---------------------------------------------------------------------------
// EditTriMesh::~EditTriMesh
//
// Destructor - make sure resources are freed

EditTriMesh::~EditTriMesh() {
	empty();
}

//---------------------------------------------------------------------------
// EditTriMesh::operator=
//
// Assignment operator - make a copy of the mesh

EditTriMesh &EditTriMesh::operator=(const EditTriMesh &src) {
	int	i;

	// Start by freeing up what we already have

	empty();

	// Copy materials and parts first.  We copy these stupidly,
	// since the lists probably won't be very big

	setMaterialCount(src.materialCount());
	for (i = 0 ; i < materialCount() ; ++i) {
		material(i) = src.material(i);
	}

	setPartCount(src.partCount());
	for (i = 0 ; i < partCount() ; ++i) {
		part(i) = src.part(i);
	}

	// Make sure vertex list isn't empty

	if (src.vertexCount() > 0) {

		// Compute size in bytes

		int	bytes = src.vertexCount() * sizeof(*vList);

		// Allocate it.  We don't use setVertexCount(), since
		// that initializes the list, which we don't need

		vList = (Vertex *)::malloc(bytes);
		if (vList == NULL) {
			ABORT("Out of memory");
		}
		vCount = src.vertexCount();
		vAlloc = vCount;

		// Blast copy it

		memcpy(vList, src.vList, bytes);
	}

	// Make sure face list isn't empty

	if (src.triCount() > 0) {

		// Compute size in bytes

		int	bytes = src.triCount() * sizeof(*tList);

		// Allocate it.  We don't use setVertexCount(), since
		// that initializes the list, which we don't need

		tList = (Tri *)::malloc(bytes);
		if (tList == NULL) {
			ABORT("Out of memory");
		}
		tCount = src.triCount();
		tAlloc = tCount;

		// Blast copy it

		memcpy(tList, src.tList, bytes);
	}

	// Return reference to l-value, as per C convention

	return *this;
}

/////////////////////////////////////////////////////////////////////////////
//
// EditTriMesh members - Accessors to the mesh data
//
// All of these functions act like an array operator, returning a reference
// to the element.  They provide array bounds checking, and can therefore
// catch a number of common bugs.  We have const and non-const versions,
// so you can only modify a non-const mesh.
//
/////////////////////////////////////////////////////////////////////////////

EditTriMesh::Vertex &EditTriMesh::vertex(int vertexIndex) {
	assert(vertexIndex >= 0);
	assert(vertexIndex < vCount);
	return vList[vertexIndex];
}

const EditTriMesh::Vertex &EditTriMesh::vertex(int vertexIndex) const {
	assert(vertexIndex >= 0);
	assert(vertexIndex < vCount);
	return vList[vertexIndex];
}

EditTriMesh::Tri &EditTriMesh::tri(int triIndex) {
	assert(triIndex >= 0);
	assert(triIndex < tCount);
	return tList[triIndex];
}

const EditTriMesh::Tri &EditTriMesh::tri(int triIndex) const {
	assert(triIndex >= 0);
	assert(triIndex < tCount);
	return tList[triIndex];
}

EditTriMesh::Material &EditTriMesh::material(int materialIndex) {
	assert(materialIndex >= 0);
	assert(materialIndex < mCount);
	return mList[materialIndex];
}

const EditTriMesh::Material &EditTriMesh::material(int materialIndex) const {
	assert(materialIndex >= 0);
	assert(materialIndex < mCount);
	return mList[materialIndex];
}

EditTriMesh::Part &EditTriMesh::part(int partIndex) {
	assert(partIndex >= 0);
	assert(partIndex < pCount);
	return pList[partIndex];
}

const EditTriMesh::Part &EditTriMesh::part(int partIndex) const {
	assert(partIndex >= 0);
	assert(partIndex < pCount);
	return pList[partIndex];
}

/////////////////////////////////////////////////////////////////////////////
//
// EditTriMesh members - Basic mesh operations
//
// All of these functions act like an array operator, returning a reference
// to the element.  They provide array bounds checking, and can therefore
// catch a number of common bugs.  We have const and non-const versions,
// so you can only modify a non-const mesh.
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// EditTriMesh::empty
//
// Reset the mesh to empty state

void	EditTriMesh::empty() {

	// Free vertices

	if (vList != NULL) {
		::free(vList);
		vList = NULL;
	}
	vAlloc = 0;
	vCount = 0;

	// Free triangles

	if (tList != NULL) {
		::free(tList);
		tList = NULL;
	}
	tAlloc = 0;
	tCount = 0;

	// Free materials

	if (mList != NULL) {
		::free(mList);
		mList = NULL;
	}
	mCount = 0;

	// Free parts

	if (pList != NULL) {
		::free(pList);
		pList = NULL;
	}
	pCount = 0;
}

//---------------------------------------------------------------------------
// EditTriMesh::setVertexCount
//
// Set the vertex count.  If the list is grown, the new vertices at the end
// are initialized with default values.  If the list is shrunk, any invalid
// faces are deleted.

void	EditTriMesh::setVertexCount(int vc) {
	assert(vc >= 0);

	// Make sure we had enough allocated coming in

	assert(vCount <= vAlloc);

	// Check if growing or shrinking the list

	if (vc > vCount) {

		// Check if we need to allocate more

		if (vc > vAlloc) {

			// We need to grow the list.  Figure out the
			// new count.  We don't want to constantly be
			// allocating memory every time a single vertex
			// is added, but yet we don't want to allocate
			// too much memory and be wasteful.  The system
			// shown below seems to be a good compromise.

			vAlloc = vc * 4 / 3 + 10;
			vList = (Vertex *)::realloc(vList, vAlloc * sizeof(*vList));

			// Check for out of memory.  You may need more
			// robust error handling...

			if (vList == NULL) {
				ABORT("Out of memory");
			}
		}

		// Initilaize the new vertices

		while (vCount < vc) {
			vList[vCount].setDefaults();
			++vCount;
		}

	} else if (vc < vCount) {

		// Shrinking the list.  Go through
		// and mark invalid faces for deletion

		for (int i = 0 ; i < triCount() ; ++i) {
			Tri *t = &tri(i);
			if (
				(t->v[0].index >= vc) ||
				(t->v[1].index >= vc) ||
				(t->v[2].index >= vc)
			) {

				// Mark it for deletion

				t->mark = 1;

			} else {

				// It's OK

				t->mark = 0;
			}
		}

		// Delete the marked triangles

		deleteMarkedTris(1);

		// Set the new count.  Any extra memory is
		// wasted for now...

		vCount = vc;
	}

}

//---------------------------------------------------------------------------
// EditTriMesh::setTriCount
//
// Set the triangle count.  If the list is grown, the new triangles at the
// end are initialized with default values.

void	EditTriMesh::setTriCount(int tc) {
	assert(tc >= 0);

	// Make sure we had enough allocated coming in

	assert(tCount <= tAlloc);

	// Check if we are growing the list

	if (tc > tCount) {

		// Check if we need to allocate more

		if (tc > tAlloc) {

			// We need to grow the list.  Figure out the
			// new count.  We don't want to constantly be
			// allocating memory every time a single tri
			// is added, but yet we don't want to allocate
			// too much memory and be wasteful.  The system
			// shown below seems to be a good compromise.

			tAlloc = tc * 4 / 3 + 10;
			tList = (Tri *)::realloc(tList, tAlloc * sizeof(*tList));

			// Check for out of memory.  You may need more
			// robust error handling...

			if (tList == NULL) {
				ABORT("Out of memory");
			}
		}

		// Initilaize the new triangles

		while (tCount < tc) {
			tList[tCount].setDefaults();
			++tCount;
		}
	} else {

		// Set the new count.  Any extra memory is
		// wasted for now...

		tCount = tc;
	}
}

//---------------------------------------------------------------------------
// EditTriMesh::setMaterialCount
//
// Set the material count.  If the list is grown, the new materials at the end
// are initialized with default values.  If the list is shrunk, any invalid
// faces are deleted.

void	EditTriMesh::setMaterialCount(int mc) {
	assert(mc >= 0);

	// Check if growing or shrinking the list

	if (mc > mCount) {

		// Grow the list.  For materials, we don't have any fancy
		// allocation like we do for the vertices and triangles.

		mList = (Material *)::realloc(mList, mc * sizeof(*mList));

		// Check for out of memory.  You may need more
		// robust error handling...

		if (mList == NULL) {
			ABORT("Out of memory");
		}

		// Initilaize the new materials

		while (mCount < mc) {
			mList[mCount].setDefaults();
			++mCount;
		}

	} else if (mc < mCount) {

		// Shrinking the list.  Go through
		// and mark invalid faces for deletion

		for (int i = 0 ; i < triCount() ; ++i) {
			Tri *t = &tri(i);
			if (t->material >= mc) {

				// Mark it for deletion

				t->mark = 1;

			} else {

				// It's OK

				t->mark = 0;
			}
		}

		// Delete the marked triangles

		deleteMarkedTris(1);

		// Set the new count.  For now, no need to
		// shrink the list.  We'll just waste it.

		mCount = mc;
	}

}

//---------------------------------------------------------------------------
// EditTriMesh::setPartCount
//
// Set the part count.  If the list is grown, the new parts at the end
// are initialized with default values.  If the list is shrunk, any invalid
// faces are deleted.

void	EditTriMesh::setPartCount(int pc) {
	assert(pc >= 0);

	// Check if growing or shrinking the list

	if (pc > pCount) {

		// Grow the list.  For parts, we don't have any fancy
		// allocation like we do for the vertices and triangles.

		pList = (Part *)::realloc(pList, pc * sizeof(*pList));

		// Check for out of memory.  You may need more
		// robust error handling...

		if (pList == NULL) {
			ABORT("Out of memory");
		}

		// Initilaize the new parts

		while (pCount < pc) {
			pList[pCount].setDefaults();
			++pCount;
		}

	} else if (pc < pCount) {

		// Shrinking the list.  Go through
		// and mark invalid faces for deletion

		for (int i = 0 ; i < triCount() ; ++i) {
			Tri *t = &tri(i);
			if (t->part >= pc) {

				// Mark it for deletion

				t->mark = 1;

			} else {

				// It's OK

				t->mark = 0;
			}
		}

		// Delete the marked triangles

		deleteMarkedTris(1);

		// Set the new count.  For now, no need to
		// shrink the list.  We'll just waste it.

		pCount = pc;
	}

}

//---------------------------------------------------------------------------
// EditTriMesh::addTri
//
// Add a new, default triangle.  The index of the new item is returned

int	EditTriMesh::addTri() {

	// Fetch index of the new one we will add

	int	r = tCount;

	// Check if we have to allocate

	if (tCount >= tAlloc) {

		// Need to actually allocate memory - use the other function
		// to do the hard work

		setTriCount(tCount+1);

	} else {

		// No need for heavy duty work - we can
		// do it quickly here

		++tCount;
		tList[r].setDefaults();
	}

	// Return index of new triangle

	return r;
}

//---------------------------------------------------------------------------
// EditTriMesh::addTri
//
// Add triangle to the end of the list.  The index of the new item is returned

int	EditTriMesh::addTri(const Tri &t) {

	// Fetch index of the new one we will add

	int	r = tCount;

	// Check if we have to allocate

	if (tCount >= tAlloc) {

		// Need to actually allocate memory - use the other function
		// to do the hard work

		setTriCount(tCount+1);

	} else {

		// No need for heavy duty work - we can
		// do it quickly here

		++tCount;

		// No need to default - we are about to assign it
	}

	// Fill it in

	tList[r] = t;

	// Return index of new triangle

	return r;
}

//---------------------------------------------------------------------------
// EditTriMesh::addVertex
//
// Add a new, default vertex.  The index of the new item is returned

int	EditTriMesh::addVertex() {

	// Fetch index of the new one we will add

	int	r = vCount;

	// Check if we have to allocate

	if (vCount >= vAlloc) {

		// Need to actually allocate memory - use the other function
		// to do the hard work

		setVertexCount(vCount+1);

	} else {

		// No need for heavy duty work - we can
		// do it quickly here

		++vCount;
		vList[r].setDefaults();
	}

	// Return index of new vertex

	return r;
}

//---------------------------------------------------------------------------
// EditTriMesh::addvertex
//
// Add vertex to the end of the list.  The index of the new item is returned

int	EditTriMesh::addVertex(const Vertex &v) {

	// Fetch index of the new one we will add

	int	r = vCount;

	// Check if we have to allocate

	if (vCount >= vAlloc) {

		// Need to actually allocate memory - use the other function
		// to do the hard work

		setVertexCount(vCount+1);

	} else {

		// No need for heavy duty work - we can
		// do it quickly here

		++vCount;

		// No need to default - we are about to assign it
	}

	// Fill it in

	vList[r] = v;

	// Return index of new vertex

	return r;
}

//---------------------------------------------------------------------------
// EditTriMesh::dupVertex
//
// Add a duplicate of a vertex to the end of the list.
//
// Notice that we do NOT make the mistake of coding this simply as:
//
//	return addVertex(vertex(srcVertexIndex))
//
// Because the vertices may shift in memory as the lists are reallocated,
// and our reference will be invalid.

int	EditTriMesh::dupVertex(int srcVertexIndex) {

	// Fetch index of the new one we will add

	int	r = vCount;

	// Check if we have to allocate

	if (vCount >= vAlloc) {

		// Need to actually allocate memory - use the other function
		// to do the hard work

		setVertexCount(vCount+1);

	} else {

		// No need for heavy duty work - we can
		// do it quickly here

		++vCount;

		// No need to default - we are about to assign it
	}

	// Make the copy

	vList[r] = vList[srcVertexIndex];

	// Return index of new vertex

	return r;
}

//---------------------------------------------------------------------------
// EditTriMesh::addMaterial
//
// Add a material to the end of the list.  Not optimized

int	EditTriMesh::addMaterial(const Material &m) {

	// Grow list

	int	r = materialCount();
	setMaterialCount(r+1);

	// Fill it in

	mList[r] = m;

	// Return index of the new material

	return r;
}

//---------------------------------------------------------------------------
// EditTriMesh::addPart
//
// Add a part to the end of the list.  Not optimized

int	EditTriMesh::addPart(const Part &p) {

	// Grow list

	int	r = partCount();
	setPartCount(r+1);

	// Fill it in

	pList[r] = p;

	// Return index of the new part

	return r;
}

//---------------------------------------------------------------------------
// EditTriMesh::markAllVertices
//
// Mark all vertices with the given value

void	EditTriMesh::markAllVertices(int mark) {
	for (int i = 0 ; i < vertexCount() ; ++i) {
		vertex(i).mark = mark;
	}
}

//---------------------------------------------------------------------------
// EditTriMesh::markAllTris
//
// Mark all triangles with the given value

void	EditTriMesh::markAllTris(int mark) {
	for (int i = 0 ; i < triCount() ; ++i) {
		tri(i).mark = mark;
	}
}

//---------------------------------------------------------------------------
// EditTriMesh::markAllMaterials
//
// Mark all materials with the given value

void	EditTriMesh::markAllMaterials(int mark) {
	for (int i = 0 ; i < materialCount() ; ++i) {
		material(i).mark = mark;
	}
}

//---------------------------------------------------------------------------
// EditTriMesh::markAllParts
//
// Mark all parts with the given value

void	EditTriMesh::markAllParts(int mark) {
	for (int i = 0 ; i < partCount() ; ++i) {
		part(i).mark = mark;
	}
}

//---------------------------------------------------------------------------
// EditTriMesh::deleteVertex
//
// Deletes one vertex from the vertex list.  This will fixup vertex
// indices in the triangles, and also delete triangles that referenced
// that vertex

void	EditTriMesh::deleteVertex(int vertexIndex) {

	// Check index.  Warn in debug build, don't crash release

	if ((vertexIndex < 0) || (vertexIndex >= vertexCount())) {
		assert(false);
		return;
	}

	// Scan triangle list and fixup vertex indices

	for (int i = 0 ; i < triCount() ; ++i) {
		Tri *t = &tri(i);

		// Assume it won't get deleted

		t->mark = 0;

		// Fixup vertex indices on this face

		for (int j = 0 ; j < 3 ; ++j) {

			// Do we need to delete it?

			if (t->v[j].index == vertexIndex) {
				t->mark = 1;
				break;
			}

			// Fixup vertex index?

			if (t->v[j].index > vertexIndex) {
				--t->v[j].index;
			}
		}
	}

	// Delete the vertex from the vertex array

	--vCount;
	memmove(&vList[vertexIndex], &vList[vertexIndex+1], (vCount - vertexIndex) * sizeof(*vList));

	// Delete the triangles that used it

	deleteMarkedTris(1);
}

//---------------------------------------------------------------------------
// EditTriMesh::deleteTri
//
// Deletes one triangle from the triangle list.

void	EditTriMesh::deleteTri(int triIndex) {

	// Check index.  Warn in debug build, don't crash release

	if ((triIndex < 0) || (triIndex >= triCount())) {
		assert(false);
		return;
	}

	// Delete it

	--tCount;
	memmove(&tList[triIndex], &tList[triIndex+1], (tCount - triIndex) * sizeof(*tList));
}

//---------------------------------------------------------------------------
// EditTriMesh::deleteMaterial
//
// Deletes one material from the material list.  Material indices in
// the triangles are fixed up and any triangles that used that material
// are deleted

void	EditTriMesh::deleteMaterial(int materialIndex) {

	// Check index.  Warn in debug build, don't crash release

	if ((materialIndex < 0) || (materialIndex >= materialCount())) {
		assert(false);
		return;
	}

	// Scan triangle list and fixup vertex indices

	for (int i = 0 ; i < triCount() ; ++i) {
		Tri *t = &tri(i);

		// Do we need to delete it?

		if (t->material == materialIndex) {
			t->mark = 1;
		} else {
			t->mark = 0;
			if (t->material > materialIndex) {
				--t->material;
			}
		}
	}

	// Delete it

	--mCount;
	memmove(&mList[materialIndex], &mList[materialIndex+1], (mCount - materialIndex) * sizeof(*mList));

	// Delete the triangles that used it

	deleteMarkedTris(1);
}

//---------------------------------------------------------------------------
// EditTriMesh::deletePart
//
// Deletes one part from the part list.  Part indices in the triangles are
// fixed up and any triangles from that part are deleted

void	EditTriMesh::deletePart(int partIndex) {

	// Check index.  Warn in debug build, don't crash release

	if ((partIndex < 0) || (partIndex >= partCount())) {
		assert(false);
		return;
	}

	// Scan triangle list and fixup vertex indices

	for (int i = 0 ; i < triCount() ; ++i) {
		Tri *t = &tri(i);

		// Do we need to delete it?

		if (t->part == partIndex) {
			t->mark = 1;
		} else {
			t->mark = 0;
			if (t->part > partIndex) {
				--t->part;
			}
		}
	}

	// Delete it

	--pCount;
	memmove(&pList[partIndex], &pList[partIndex+1], (pCount - partIndex) * sizeof(*pList));

	// Delete the triangles that used it

	deleteMarkedTris(1);
}

//---------------------------------------------------------------------------
// EditTriMesh::deleteUnusedMaterials
//
// Scan list of materials and delete any that are not used by any triangles
//
// This method may seem a little more complicated, but it operates
// in linear time with respect to the number of triangles.
// Other methods will run in quadratic time or worse.

void	EditTriMesh::deleteUnusedMaterials() {

	int i;

	// Assume all materials will be unused

	markAllMaterials(0);

	// Scan triangle list and mark referenced materials

	for (i = 0 ; i < triCount() ; ++i) {
		material(tri(i).material).mark = 1;
	}

	// OK, figure out how many materials there will be,
	// and where they will go int he new material list,
	// after the unused ones are removed

	int	newMaterialCount = 0;
	for (i = 0 ; i < materialCount() ; ++i) {
		Material *m = &material(i);

		// Was it used?

		if (m->mark == 0) {

			// No - mark it to be whacked

			m->mark = -1;

		} else {

			// Yes - it will occupy the next slot in the
			// new material list

			m->mark = newMaterialCount;
			++newMaterialCount;
		}
	}

	// Check if nothing got deleted, then don't bother with the
	// rest of this

	if (newMaterialCount == materialCount()) {
		return;
	}

	// Fixup indices in the face list

	for (i = 0 ; i < triCount() ; ++i) {
		Tri *t = &tri(i);
		t->material = material(t->material).mark;
	}

	// Remove the empty spaces from the material list

	int	destMaterialIndex = 0;
	for (i = 0 ; i < materialCount() ; ++i) {
		const Material *m = &material(i);

		// This one staying?

		if (m->mark != -1) {
			assert(m->mark == destMaterialIndex);
			if (i != destMaterialIndex) {
				material(destMaterialIndex) = *m;
			}
			++destMaterialIndex;
		}
	}
	assert(destMaterialIndex == newMaterialCount);

	// Set the new count.  We don't call the function to
	// do this, since it will scan for triangles that use the
	// whacked entries.  We already took care of that.

	mCount = newMaterialCount;
}

//---------------------------------------------------------------------------
// EditTriMesh::deleteEmptyParts
//
// Scan list of parts and delete any that do not contain any triangles
//
// This method may seem a little more complicated, but it operates
// in linear time with respect to the number of triangles.
// Other methods will run in quadratic time or worse.

void	EditTriMesh::deleteEmptyParts() {

	int i;

	// Assume all parts will be empty

	markAllParts(0);

	// Scan triangle list and mark referenced parts

	for (i = 0 ; i < triCount() ; ++i) {
		part(tri(i).part).mark = 1;
	}

	// OK, figure out how many parts there will be,
	// and where they will go int he new part list,
	// after the unused ones are removed

	int	newPartCount = 0;
	for (i = 0 ; i < partCount() ; ++i) {
		Part *p = &part(i);

		// Was it used?

		if (p->mark == 0) {

			// No - mark it to be whacked

			p->mark = -1;

		} else {

			// Yes - it will occupy the next slot in the
			// new part list

			p->mark = newPartCount;
			++newPartCount;
		}
	}

	// Check if nothing got deleted, then don't bother with the
	// rest of this

	if (newPartCount == partCount()) {
		return;
	}

	// Fixup indices in the face list

	for (i = 0 ; i < triCount() ; ++i) {
		Tri *t = &tri(i);
		t->part = part(t->part).mark;
	}

	// Remove the empty spaces from the part list

	int	destPartIndex = 0;
	for (i = 0 ; i < partCount() ; ++i) {
		const Part *p = &part(i);

		// This one staying?

		if (p->mark != -1) {
			assert(p->mark == destPartIndex);
			if (i != destPartIndex) {
				part(destPartIndex) = *p;
			}
			++destPartIndex;
		}
	}
	assert(destPartIndex == newPartCount);

	// Set the new count.  We don't call the function to
	// do this, since it will scan for triangles that use the
	// whacked entries.  We already took care of that.

	pCount = newPartCount;
}

//---------------------------------------------------------------------------
// EditTriMesh::deleteMarkedTris
//
// Scan triangle list, deleting triangles with the given mark

void	EditTriMesh::deleteMarkedTris(int mark) {

	// Scan triangle list, and move triangles forward to
	// suck up the "holes" left by deleted triangles

	int	destTriIndex = 0;
	for (int i = 0 ; i < triCount() ; ++i) {
		const Tri *t = &tri(i);

		// Is it staying?

		if (t->mark != mark) {
			if (destTriIndex != i) {
				tri(destTriIndex) = *t;
			}
			++destTriIndex;
		}
	}

	// Set new triangle count

	setTriCount(destTriIndex);
}

//---------------------------------------------------------------------------
// EditTriMesh::deleteDegenerateTris
//
// Scan triangle list and remove "degenerate" triangles.  See
// isDegenerate() for the defininition of "degenerate" in this case.

void	EditTriMesh::deleteDegenerateTris() {

	// Scan triangle list, marking the bad ones

	for (int i = 0 ; i < triCount() ; ++i) {
		Tri *t = &tri(i);

		// Is it bogus?

		if (t->isDegenerate()) {

			// Mark it to be whacked

			t->mark = 1;

		} else {

			// Keep it

			t->mark = 0;
		}
	}

	// Delete the bad triangles that we found

	deleteMarkedTris(1);
}

//---------------------------------------------------------------------------
// EditTriMesh::detachAllFaces
//
// Detach all the faces from one another.  This creates a new vertex list,
// with each vertex only used by one triangle.  Simultaneously, unused
// vertices are removed.

void	EditTriMesh::detachAllFaces() {

	// Check if we don't have any faces, then bail now.
	// This saves us a crash with a spurrious "out of memory"

	if (triCount() < 1) {
		return;
	}

	// Figure out how many triangles we'll have

	int	newVertexCount = triCount() * 3;

	// Allocate a new vertex list

	Vertex *newVertexList = (Vertex *)::malloc(newVertexCount * sizeof(Vertex));

	// Check for out of memory.  You may need more
	// robust error handling...

	if (newVertexList == NULL) {
		ABORT("Out of memory");
	}

	// Scan the triangle list and fill it in

	for (int i = 0 ; i < triCount() ; ++i) {
		Tri *t = &tri(i);

		// Process the three vertices on this face

		for (int j = 0 ; j < 3 ; ++j) {

			// Get source and destination vertex indices

			int	sIndex = t->v[j].index;
			int	dIndex = i*3 + j;
			Vertex	*dPtr = &newVertexList[dIndex];

			// Copy the vertex

			*dPtr = vertex(sIndex);

			// Go ahead and fill in UV and normal now.  It can't hurt

			dPtr->u = t->v[j].u;
			dPtr->v = t->v[j].v;
			dPtr->normal = t->normal;

			// Set new vertex index

			t->v[j].index = dIndex;
		}
	}

	// Free the old vertex list

	::free(vList);

	// Install the new one

	vList = newVertexList;
	vCount = newVertexCount;
	vAlloc = newVertexCount;
}

//---------------------------------------------------------------------------
// EditTriMesh::transformVertices
//
// Transform all the vertices.  We could transform the surface normals,
// but they may not even be valid, anyway.  If you need them, compute them.

void	EditTriMesh::transformVertices(const Matrix4x3 &m) {
	for (int i = 0 ; i < vertexCount() ; ++i) {
		vertex(i).p *= m;
	}
}

//---------------------------------------------------------------------------
// EditTriMesh::extractParts
//
// Extract each part into a seperate mesh.  Each resulting mesh will
// have exactly one part

void	EditTriMesh::extractParts(EditTriMesh *meshes) {

	// !SPEED! This function will run in O(partCount * triCount).
	// We could optimize it somewhat by having the triangles sorted by
	// part.  However, any real optimization would be considerably
	// more complicated.  Let's just keep it simple.

	// Scan through each part

	for (int partIndex = 0 ; partIndex < partCount() ; ++partIndex) {

		// Get shortcut to destination mesh

		EditTriMesh *dMesh = &meshes[partIndex];

		// Mark all vertices and materials, assuming they will
		// not be used by this part

		markAllVertices(-1);
		markAllMaterials(-1);

		// Setup the destination part mesh with a single part

		dMesh->empty();
		dMesh->setPartCount(1);
		dMesh->part(0) = part(partIndex);

		// Convert face list, simultaneously building material and
		// vertex list

		for (int faceIndex = 0 ; faceIndex < triCount() ; ++faceIndex) {

			// Fetch shortcut, make sure it belongs to this
			// part

			Tri *tPtr = &tri(faceIndex);
			if (tPtr->part != partIndex) {
				continue;
			}

			// Make a copy

			Tri t = *tPtr;

			// Remap material index

			Material *m = &material(t.material);
			if (m->mark < 0) {
				m->mark = dMesh->addMaterial(*m);
			}
			t.material = m->mark;

			// Remap vertices

			for (int j = 0 ; j < 3 ; ++j) {
				Vertex *v = &vertex(t.v[j].index);
				if (v->mark < 0) {
					v->mark = dMesh->addVertex(*v);
				}
				t.v[j].index = v->mark;
			}

			// Add the face

			t.part = 0;
			dMesh->addTri(t);
		}
	}
}

void	EditTriMesh::extractOnePartOneMaterial(int partIndex, int materialIndex, EditTriMesh *result) {

	// Mark all vertices, assuming they will not be used

	markAllVertices(-1);

	// Setup the destination mesh with a single part and material

	result->empty();
	result->setPartCount(1);
	result->part(0) = part(partIndex);
	result->setMaterialCount(1);
	result->material(0) = material(materialIndex);

	// Convert face list, simultaneously building vertex list

	for (int faceIndex = 0 ; faceIndex < triCount() ; ++faceIndex) {

		// Fetch shortcut, make sure it belongs to this
		// part and uses this material

		Tri *tPtr = &tri(faceIndex);
		if (tPtr->part != partIndex) {
			continue;
		}
		if (tPtr->material != materialIndex) {
			continue;
		}

		// Make a copy

		Tri t = *tPtr;

		// Remap vertices

		for (int j = 0 ; j < 3 ; ++j) {
			Vertex *v = &vertex(t.v[j].index);
			if (v->mark < 0) {
				v->mark = result->addVertex(*v);
			}
			t.v[j].index = v->mark;
		}

		// Add the face

		t.part = 0;
		t.material = 0;
		result->addTri(t);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// EditTriMesh members - Computations
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// EditTriMesh::computeOneTriNormal
//
// Compute a single triangle normal.

void	EditTriMesh::computeOneTriNormal(int triIndex) {
	computeOneTriNormal(tri(triIndex));
}

void	EditTriMesh::computeOneTriNormal(Tri &t) {

	// Fetch shortcuts to vertices

	const Vector3 &v1 = vertex(t.v[0].index).p;
	const Vector3 &v2 = vertex(t.v[1].index).p;
	const Vector3 &v3 = vertex(t.v[2].index).p;

	// Compute clockwise edge vectors.  We use the edge vector
	// indexing that agrees with Section 12.6.

	Vector3 e1 = v3 - v2;
	Vector3 e2 = v1 - v3;

	// Cross product to compute surface normal

	t.normal = crossProduct(e1, e2);

	// Normalize it

	t.normal.normalize();
}

//---------------------------------------------------------------------------
// EditTriMesh::computeTriNormals
//
// Compute all the triangle normals

void	EditTriMesh::computeTriNormals() {
	for (int i = 0 ; i < triCount() ; ++i) {
		computeOneTriNormal(tri(i));
	}
}

//---------------------------------------------------------------------------
// EditTriMesh::computeTriNormals
//
// Compute vertex level surface normals.  This automatically computes the
// triangle level surface normals

void	EditTriMesh::computeVertexNormals() {

	int	i;

	// First, make sure triangle level surface normals are up-to-date

	computeTriNormals();

	// Zero out vertex normals

	for (i = 0 ; i < vertexCount() ; ++i) {
		vertex(i).normal.zero();
	}

	// Sum in the triangle normals into the vertex normals
	// that are used by the triangle

	for (i = 0 ; i < triCount() ; ++i) {
		const Tri *t = &tri(i);
		for (int j = 0 ; j < 3 ; ++j) {
			vertex(t->v[j].index).normal += t->normal;
		}
	}

	// Now "average" the vertex surface normals, by normalizing them

	for (i = 0 ; i < vertexCount() ; ++i) {
		vertex(i).normal.normalize();
	}
}

//---------------------------------------------------------------------------
// EditTriMesh::computeBounds
//
// Compute the bounding box of the mesh

AABB3	EditTriMesh::computeBounds() const {

	// Generate the bounding box of the vertices

	AABB3	box;
	box.empty();
	for (int i = 0 ; i < vertexCount() ; ++i) {
		box.add(vertex(i).p);
	}

	// Return it

	return box;
}

/////////////////////////////////////////////////////////////////////////////
//
// EditTriMesh members - Optimization
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// EditTriMesh::optimizeVertexOrder
//
// Re-order the vertex list, in the order that they are used by the faces.
// This can improve cache performace and vertex caching by increasing the
// locality of reference.
//
// If removeUnusedVertices is true, then any unused vertices are discarded.
// Otherwise, they are retained at the end of the vertex list.  Normally
// you will want to discard them, which is why we default the paramater to
// true.

void	EditTriMesh::optimizeVertexOrder(bool removeUnusedVertices) {

	int	i;

	// Mark all vertices with a very high mark, which assumes
	// that they will not be used

	for (i = 0 ; i < vertexCount() ; ++i) {
		vertex(i).mark = vertexCount();
	}

	// Scan the face list, and figure out where the vertices
	// will end up in the new, ordered list.  At the same time,
	// we remap the indices in the triangles according to this
	// new ordering.

	int	usedVertexCount = 0;
	for (i = 0 ; i < triCount() ; ++i) {
		Tri *t = &tri(i);

		// Process each of the three vertices on this triangle

		for (int j = 0 ; j < 3 ; ++j) {

			// Get shortcut to the vertex used

			Vertex *v = &vertex(t->v[j].index);

			// Has it been used already?

			if (v->mark == vertexCount()) {

				// We're the first triangle to use
				// this one.  Assign the vertex to
				// the next slot in the new vertex
				// list

				v->mark = usedVertexCount;
				++usedVertexCount;
			}

			// Remap the vertex index

			t->v[j].index = v->mark;
		}
	}

	// Re-sort the vertex list.  This puts the used vertices
	// in order where they go, and moves all the unused vertices
	// to the end (in no particular order, since qsort is not
	// a stable sort)

	qsort(vList, vertexCount(), sizeof(Vertex), vertexCompareByMark);

	// Did they want to discard the unused guys?

	if (removeUnusedVertices) {

		// Yep - chop off the unused vertices by slamming
		// the vertex count.  We don't call the function to
		// set the vertex count here, since it will scan
		// the triangle list for any triangle that use those
		// vertices.  But we already know that all of the
		// vertices we are deleting are unused

		vCount = usedVertexCount;
	}
}

//---------------------------------------------------------------------------
// EditTriMesh::sortTrisByMaterial
//
// Sort triangles by material.  This is VERY important for effecient
// rendering

void	EditTriMesh::sortTrisByMaterial() {

	// Put the current index into the "mark" field so we can
	// have a stable sort

	for (int i = 0 ; i < triCount() ; ++i) {
		tri(i).mark = i;
	}

	// Use qsort

	qsort(tList, triCount(), sizeof(Tri), triCompareByMaterial);
}

//---------------------------------------------------------------------------
// EditTriMesh::weldVertices
//
// Weld coincident vertices.  For the moment, this disregards UVs and welds
// all vertices that are within geometric tolerance

void	EditTriMesh::weldVertices(const OptimizationParameters &opt) {

	// !FIXME!

}

//---------------------------------------------------------------------------
// EditTriMesh::copyUvsIntoVertices
//
// Ensure that the vertex UVs are correct, possibly duplicating
// vertices if necessary

void	EditTriMesh::copyUvsIntoVertices() {

	// Mark all vertices indicating thet their UV's are invalid

	markAllVertices(0);

	// Scan the faces, and shove in the UV's into the vertices

	for (int triIndex = 0 ; triIndex < triCount() ; ++triIndex) {
		Tri *triPtr = &tri(triIndex);
		for (int i = 0 ; i < 3 ; ++i) {

			// Locate vertex

			int	vIndex = triPtr->v[i].index;
			Vertex *vPtr = &vertex(vIndex);

			// Have we filled in the UVs for this vertex yet?

			if (vPtr->mark == 0) {

				// Nope.  Shove them in

				vPtr->u = triPtr->v[i].u;
				vPtr->v = triPtr->v[i].v;

				// Mark UV's as valid, and keep going

				vPtr->mark = 1;
				continue;
			}

			// UV's have already been filled in by another face.
			// Did that face have the same UV's as me?

			if (
				(vPtr->u == triPtr->v[i].u) &&
				(vPtr->v == triPtr->v[i].v)
			) {

				// Yep - no need to change anything

				continue;
			}

			// OK, we can't use this vertex - somebody else already has
			// it "claimed" with different UV's.  First, we'll search
			// for another vertex with the same position.  Yikes - 
			// this requires a linear search through the vertex list.
			// Luckily, this should not happen the majority of the time.

			bool	foundOne = false;
			for (int newIndex = 0 ; newIndex < vertexCount() ; ++newIndex) {
				Vertex *newPtr = &vertex(newIndex);

				// Is the position and normal correct?

				if (
					(newPtr->p != vPtr->p) ||
					(newPtr->normal != vPtr->normal)
				) {
					continue;
				}

				// OK, this vertex is geometrically correct.
				// Has anybody filled in the UV's yet?

				if (newPtr->mark == 0) {

					// We can claim this one.

					newPtr->mark = 1;
					newPtr->u = triPtr->v[i].u;
					newPtr->v = triPtr->v[i].v;

					// Remap vertex index

					triPtr->v[i].index = newIndex;

					// No need to keep looking

					foundOne = true;
					break;
				}

				// Already claimed by somebody else, so we can't change
				// them.  but are they correct, already anyway?

				if (
					(newPtr->u == triPtr->v[i].u) &&
					(newPtr->v == triPtr->v[i].v)
				) {

					// Yep - no need to change anything.  Just remap the
					// vertex index 

					triPtr->v[i].index = newIndex;

					// No need to keep looking

					foundOne = true;
					break;
				}

				// No good - keep looking
			}

			// Did we find a vertex?

			if (!foundOne) {

				// Nope, we'll have to create a new one

				Vertex newVertex = *vPtr;
				newVertex.mark = 1;
				newVertex.u = triPtr->v[i].u;
				newVertex.v = triPtr->v[i].v;
				triPtr->v[i].index = addVertex(newVertex);
			}
		}
	}
}

// Do all of the optimizations and prepare the model
// for fast rendering under *most* rendering systems,
// with proper lighting.

void	EditTriMesh::optimizeForRendering() {
	computeVertexNormals();
}

/////////////////////////////////////////////////////////////////////////////
//
// EditTriMesh members - Import/Export S3D format
//
// For more on the S3D file format, and free tools for using the
// S3D format with popular rendering packages, visit
// gamemath.com
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// EditTriMesh::importS3d
//
// Load up an S3D file.  Returns true on success.  If failure, returns
// false and puts an error message into returnErrMsg

bool	EditTriMesh::importS3d(const char *filename, char *returnErrMsg) {
	int	i;

	// Try to open up the file

	FILE *f = fopen(filename, "rt");
	if (f == NULL) {
		strcpy(returnErrMsg, "Can't open file");
failed:
		empty();
		if (f != NULL) {
			fclose(f);
		}
		return false;
	}

	// Read and check version

	if (!skipLine(f)) {
corrupt:
		strcpy(returnErrMsg, "File is corrupt");
		goto failed;
	}
	int	version;
	if (fscanf(f, "%d\n", &version) != 1) {
		goto corrupt;
	}
	if (version != 103) {
		sprintf(returnErrMsg, "File is version %d - only version 103 supported", version);
		goto failed;
	}

	// Read header

	if (!skipLine(f)) {
		goto corrupt;
	}
	int	numTextures, numTris, numVerts, numParts, numFrames, numLights, numCameras;
	if (fscanf(f, "%d , %d , %d , %d , %d , %d , %d\n", &numTextures, &numTris, &numVerts, &numParts, &numFrames, &numLights, &numCameras) != 7) {
		goto corrupt;
	}

	// Allocate lists

	setMaterialCount(numTextures);
	setTriCount(numTris);
	setVertexCount(numVerts);
	setPartCount(numParts);

	// Read part list.  the only number we care about
	// is the triangle count, which we'll temporarily
	// stach into the mark field

	if (!skipLine(f)) {
		goto corrupt;
	}
	int	firstVert = 0, firstTri = 0;
	for (i = 0 ; i < numParts ; ++i) {
		Part *p = &part(i);
		int	partFirstVert, partNumVerts, partFirstTri, partNumTris;
		if (fscanf(f, "%d , %d , %d , %d , \"%[^\"]\"\n", &partFirstVert, &partNumVerts, &partFirstTri, &partNumTris, p->name) != 5) {
			sprintf(returnErrMsg, "Corrupt at part %d", i);
			goto failed;
		}
		if (firstVert != partFirstVert || firstTri != partFirstTri) {
			sprintf(returnErrMsg, "Part vertex/tri mismatch detected at part %d", i);
			goto failed;
		}
		p->mark = partNumTris;
		firstVert += partNumVerts;
		firstTri += partNumTris;
	}
	if (firstVert != numVerts || firstTri != numTris) {
		strcpy(returnErrMsg, "Part vertex/tri mismatch detected at end of part list");
		goto failed;
	}

	// Read textures.

	if (!skipLine(f)) {
		goto corrupt;
	}
	for (i = 0 ; i < numTextures ; ++i) {
		Material	*m = &material(i);

		// Fetch line of text

		if (fgets(m->diffuseTextureName, sizeof(m->diffuseTextureName), f) != m->diffuseTextureName) {
			sprintf(returnErrMsg, "Corrupt reading texture %d", i);
			goto failed;
		}

		// Styrip off newline, which fgets leaves.
		// Wouldn't it have been nice if the stdio
		// functions would just have a function to read a line
		// WITHOUT the newline character.  What a pain...

		char *nl = strchr(m->diffuseTextureName, '\n');
		if (nl != NULL) {
			*nl = '\0';
		}
	}

	// Read triangles a part at a time

	if (!skipLine(f)) {
		goto corrupt;
	}
	int	whiteTextureIndex = -1;
	int	destTriIndex = 0;
	for (int partIndex = 0 ; partIndex < numParts ; ++partIndex) {

		// Read all triangles in this part

		for (int i = 0 ; i < part(partIndex).mark ; ++i) {

			// get shortcut to destination triangle

			Tri *t = &tri(destTriIndex);

			// Slam part number

			t->part = partIndex;

			// Parse values from file

			if (fscanf(f, "%d , %d , %f , %f , %d , %f , %f , %d , %f , %f\n",
				&t->material,
				&t->v[0].index, &t->v[0].u, &t->v[0].v,
				&t->v[1].index, &t->v[1].u, &t->v[1].v,
				&t->v[2].index, &t->v[2].u, &t->v[2].v
			) != 10) {
				sprintf(returnErrMsg, "Corrupt reading triangle %d (%d of part %d)", destTriIndex, i, partIndex);
				goto failed;
			}

			// Check for untextured triangle

			if (t->material < 0) {
				if (whiteTextureIndex < 0) {
					Material whiteMaterial;
					strcpy(whiteMaterial.diffuseTextureName, "White");
					whiteTextureIndex = addMaterial(whiteMaterial);
				}
				t->material = whiteTextureIndex;
			}

			// Scale UV's to 0...1 range

			t->v[0].u /= 256.0f;
			t->v[0].v /= 256.0f;
			t->v[1].u /= 256.0f;
			t->v[1].v /= 256.0f;
			t->v[2].u /= 256.0f;
			t->v[2].v /= 256.0f;

			// Next triangle, please

			++destTriIndex;
		}
	}
	assert(destTriIndex == triCount());

	// Read vertices

	if (!skipLine(f)) {
		goto corrupt;
	}
	for (i = 0 ; i < numVerts ; ++i) {
		Vertex *v = &vertex(i);
		if (fscanf(f, "%f , %f , %f\n", &v->p.x, &v->p.y, &v->p.z) != 3) {
			sprintf(returnErrMsg, "Corrupt reading vertex %d", i);
			goto failed;
		}
	}

	// OK, we don't need anything from the rest of the file.  Close file.

	fclose(f);
	f = NULL;

	// Check for structural errors in the mesh

	if (!validityCheck(returnErrMsg)) {
		goto failed;
	}

	// OK!

	return true;
}

void	EditTriMesh::exportS3d(const char *filename) {
}

/////////////////////////////////////////////////////////////////////////////
//
// EditTriMesh members - Debugging
//
/////////////////////////////////////////////////////////////////////////////

void	EditTriMesh::validityCheck() {
	char	errMsg[256];
	if (!validityCheck(errMsg)) {
		ABORT("EditTriMesh failed validity check:\n%s", errMsg);
	}
}

bool	EditTriMesh::validityCheck(char *returnErrMsg) {
	return true;
}
