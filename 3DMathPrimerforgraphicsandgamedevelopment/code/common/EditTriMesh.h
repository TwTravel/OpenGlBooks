/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// EditTriMesh.h - Declarations for class EditTriMesh
//
// Visit gamemath.com for the latest version of this file.
//
// For more details, see EditTriMesh.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __EDITTRIMESH_H_INCLUDED__
#define __EDITTRIMESH_H_INCLUDED__

#ifndef __VECTOR3_H_INCLUDED__
	#include "Vector3.h"
#endif

class Matrix4x3;
class AABB3;

/////////////////////////////////////////////////////////////////////////////
//
// class EditTriMesh
//
// Store an indexed triangle mesh in a very flexible format that makes
// editing and mesh manipulations easy.  (NOT optimized for rendering,
// collision detection, or anything else.)
//
// This class supports texture mapping coordinates and vertex normals
//
/////////////////////////////////////////////////////////////////////////////

class EditTriMesh {
public:

// Local types

	// class Vertex represents the information we keep track of for
	// one vertex

	class Vertex {
	public:
		Vertex() { setDefaults(); }
		void	setDefaults();

		// 3D vertex position;

		Vector3	p;

		// Vertex-level texture mapping coordinates.  Notice that
		// these may be invalid at various times.  The "real" UVs
		// are in the triangles.  For rendering, we often need UV's
		// at the vertex level.  But for many other optimizations,
		// we may need to weld vertices for faces with different
		// UV's.

		float	u, v;

		// vertex-level surface normal.  Again, this is only
		// valid in certain circumstances

		Vector3	normal;

		// Utility "mark" variable, often handy

		int	mark;
	};

	// class Tri represents the information we keep track of
	// for one triangle

	class Tri {
	public:
		Tri() { setDefaults(); }
		void	setDefaults();

		// Face vertices.

		struct Vert {
			int	index;	// index into the vertex list
			float	u,v;	// mapping coords
		};

		Vert	v[3];

		// Surface normal

		Vector3	normal;

		// Which part does this tri belong to?

		int	part;

		// Index into the material list

		int	material;

		// Utility "mark" variable, often handy

		int	mark;

		// Return true if the triangle is "degenerate" - it uses
		// the same vertex more than once

		bool	isDegenerate() const;

		// Return index of vertex (0..2), or -1 if we
		// don't use that vertex

		int	findVertex(int vertexIndex) const;
	};

	// This is the information we store for a "material"
	// In our case, we're only going to store a simple
	// diffuse texture map.  However, more complex properties
	// are often associated with materials.

	class Material {
	public:
		Material() { setDefaults(); }
		void	setDefaults();

		char	diffuseTextureName[256];

		// Utility "mark" variable, often handy

		int	mark;
	};

	// This is the information we store for a "part"
	// We don't store much except the part name,
	// and a mark variable

	class Part {
	public:
		Part() { setDefaults(); }
		void	setDefaults();

		char	name[256];

		// Utility "mark" variable, often handy

		int	mark;
	};

	// This class contains options used to control
	// optimization

	class OptimizationParameters {
	public:
		OptimizationParameters() { setDefaults(); }
		void	setDefaults();

		// A tolerance value which is used to
		// determine if two vertices are coincident.

		float	coincidentVertexTolerance;

		// Triangle angle tolerance.  Vertices
		// are not welded if the are on an edge
		// and the angle between the nrmals of the
		// triangles on this edge are too
		// far apart.  We store the cosine of this
		// value since that's what's actually used.
		// Use the functions to set it

		float	cosOfEdgeAngleTolerance;
		void	setEdgeAngleToleranceInDegrees(float degrees);
	};

// Standard class object maintenance

	EditTriMesh();
	EditTriMesh(const EditTriMesh &x);
	~EditTriMesh();

	// Operator = makes a copy of the mesh

	EditTriMesh &operator=(const EditTriMesh &src);

// Accessors to the mesh data:

	int	vertexCount() const { return vCount; }
	int	triCount() const { return tCount; }
	int	materialCount() const { return mCount; }
	int	partCount() const { return pCount; }

	Vertex		&vertex(int vertexIndex);
	const Vertex	&vertex(int vertexIndex) const;

	Tri		&tri(int triIndex);
	const Tri	&tri(int triIndex) const;

	Material	&material(int materialIndex);
	const Material	&material(int materialIndex) const;

	Part		&part(int partIndex);
	const Part	&part(int partIndex) const;

// Basic mesh operations

	// Reset the mesh to empty state

	void	empty();

	// Set list counts.  If the lists are grown, the new
	// entries will be properly defaulted.  If the lists
	// are shrunk, no check is made to ensure that a valid
	// mesh remains.

	void	setVertexCount(int vc);
	void	setTriCount(int tc);
	void	setMaterialCount(int mc);
	void	setPartCount(int pc);

	// Add a triangle/vertex/material.  The index of the newly
	// added item is returned

	int	addTri();
	int	addTri(const Tri &t);
	int	addVertex();
	int	addVertex(const Vertex &v);
	int	dupVertex(int srcVertexIndex);
	int	addMaterial(const Material &m);
	int	addPart(const Part &p);

	// Handy functions to reset all marks at once

	void	markAllVertices(int mark);
	void	markAllTris(int mark);
	void	markAllMaterials(int mark);
	void	markAllParts(int mark);

	// Deletion.

	void	deleteVertex(int vertexIndex);
	void	deleteTri(int triIndex);
	void	deleteMarkedTris(int mark);
	void	deleteDegenerateTris();
	void	deleteMaterial(int materialIndex);
	void	deleteUnusedMaterials();
	void	deletePart(int partIndex);
	void	deleteEmptyParts();

	// Extract parts

	void	extractParts(EditTriMesh *meshes);
	void	extractOnePartOneMaterial(int partIndex, int materialIndex, EditTriMesh *result);

	// Detach all the faces from one another.  This
	// creates a new vertex list, with each vertex
	// only used by one triangle.  Simultaneously,
	// unused vertices are removed.

	void	detachAllFaces();

	// Transform all the vertices

	void	transformVertices(const Matrix4x3 &m);

// Computations

	// Compute triangle-level surface normals

	void	computeOneTriNormal(int triIndex);
	void	computeOneTriNormal(Tri &t);
	void	computeTriNormals();

	// Compute vertex level surface normals.  This
	// automatically computes the triangle level
	// surface normals

	void	computeVertexNormals();

	// Compute the size of the mesh

	AABB3	computeBounds() const;

// Optimization

	// Re-order the vertex list, in the order that they
	// are used by the faces.  This can improve cache
	// performace and vertex caching by increasing the
	// locality of reference.  This function can also remove
	// unused vertices simultaneously

	void	optimizeVertexOrder(bool removeUnusedVertices = true);

	// Sort triangles by material.  This is VERY important
	// for effecient rendering

	void	sortTrisByMaterial();

	// Weld coincident vertices

	void	weldVertices(const OptimizationParameters &opt);

	// Ensure that the vertex UVs are correct, possibly
	// duplicating vertices if necessary

	void	copyUvsIntoVertices();

	// Do all of the optimizations and prepare the model
	// for fast rendering under *most* rendering systems,
	// with proper lighting.

	void	optimizeForRendering();

// Import/Export S3D format

	bool	importS3d(const char *filename, char *returnErrMsg);
	void	exportS3d(const char *filename);

// Debugging

	void	validityCheck();
	bool	validityCheck(char *returnErrMsg);

// Private representation

private:

	// The mesh lists

	int		vAlloc;
	int		vCount;
	Vertex		*vList;

	int		tAlloc;
	int		tCount;
	Tri		*tList;

	int		mCount;
	Material	*mList;

	int		pCount;
	Part		*pList;

// Implementation details:

	void	construct();
};

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __EDITTRIMESH_H_INCLUDED__

