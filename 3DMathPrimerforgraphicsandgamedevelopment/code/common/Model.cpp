/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Model.cpp - Model used for rendering
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "CommonStuff.h"
#include "Model.h"
#include "Renderer.h"
#include "TriMesh.h"
#include "EditTriMesh.h"

/////////////////////////////////////////////////////////////////////////////
//
// class Model member functions
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// Model::Model
//
// Constructor - reset internal variables to default, empty state

Model::Model() {
	partCount = 0;
	partMeshList = NULL;
	partTextureList = NULL;
}

//---------------------------------------------------------------------------
// Model::~Model
//
// Destructor - make sure resources are freed

Model::~Model() {
	freeMemory();
}

//---------------------------------------------------------------------------
// Model::allocateMemory
//
// Allocate list of parts.  The parts are not initialized with any geometry

void	Model::allocateMemory(int nPartCount) {

	// First, whack anything already allocated

	freeMemory();

	// Check if not allocating anything

	if (nPartCount < 1) {
		return;
	}

	// Allocate lists

	partMeshList = new TriMesh[nPartCount];
	partTextureList = new TextureReference[nPartCount];

	// Clear out texture list

	memset(partTextureList, 0, sizeof(partTextureList[0]) * nPartCount);

	// Remember number of parts

	partCount = nPartCount;
}

//---------------------------------------------------------------------------
// Model::freeMemory
//
// Free any resources and reset variables to empty state.

void	Model::freeMemory() {

	// Free arrays

	delete [] partMeshList;
	partMeshList = NULL;
	delete [] partTextureList;
	partTextureList = NULL;

	// Reset count

	partCount = 0;
}

//---------------------------------------------------------------------------
// Model::getPartMesh
//
// Accessor - return pointer to the part mesh, by index.

TriMesh	*Model::getPartMesh(int index) {

	// Sanity check

	assert(index >= 0);
	assert(index < partCount);
	assert(partMeshList != NULL);

	// Return it

	return &partMeshList[index];
}

//---------------------------------------------------------------------------
// Model::getPartTexture
//
// Accessor - return pointer to the texture reference

TextureReference *Model::getPartTexture(int index) {

	// Sanity check

	assert(index >= 0);
	assert(index < partCount);
	assert(partTextureList != NULL);

	// Return it

	return &partTextureList[index];
}

//---------------------------------------------------------------------------
// Model::setPartTextureName
//
// Accessor - set texture name for a part

void	Model::setPartTextureName(int index, const char *name) {

	// Sanity check

	assert(index >= 0);
	assert(index < partCount);
	assert(partTextureList != NULL);

	// Copy in name

	strcpy(partTextureList[index].name, name);
}

//---------------------------------------------------------------------------
// Model::cache
//
// Cache textures.  For best performance, always cache your textures
// before rendering

void	Model::cache() const {

	// Cache all textures

	for (int i = 0 ; i < partCount ; ++i) {
		gRenderer.cacheTexture(partTextureList[i]);
	}
}

//---------------------------------------------------------------------------
// Model::render
//
// Render the parts of the model using the curent 3D context.

void	Model::render() const {

	// Render all the parts

	for (int i = 0 ; i < partCount ; ++i) {
		renderPart(i);
	}
}

//---------------------------------------------------------------------------
// Model::renderPart
//
// Render a single part of the model using the curent 3D context.

void	Model::renderPart(int index) const {

	// Sanity check

	assert(index >= 0);
	assert(index < partCount);
	assert(partMeshList != NULL);
	assert(partTextureList != NULL);

	// Select the texture

	gRenderer.selectTexture(partTextureList[index]);

	// Render the part

	partMeshList[index].render();
}

//---------------------------------------------------------------------------
// Model::fromEditMesh
//
// Convert an EditTriMesh to a Model.  Note that this function may need
// to make many logical changes to the mesh, such as number of actual
// parts, ordering of vertices, materials, faces, etc.  Faces may need
// to be detached across part boundaries.  Vertices may need to be duplictaed
// to place UV's at the vertex level.  However, the actual mesh geometry will
// not be modified as far as number of faces, vertex positions,
// vertex normals, etc.
//
// The input mesh is not modified, except for possibly the mark fields.

void	Model::fromEditMesh(EditTriMesh &mesh) {
	int	i;

	// Free up anything already allocated

	freeMemory();

	// Make sure something exists in the destination mesh

	if (mesh.partCount() < 1) {
		return;
	}

	// Extract the part meshes

	EditTriMesh	*partMeshes = new EditTriMesh[mesh.partCount()];
	mesh.extractParts(partMeshes);

	// Figure out how many parts we'll need.  Remember,
	// each of our parts must have a single material,
	// so we must duplicate parts for multiple materials.

	int	numParts = 0;
	for (i = 0 ; i < mesh.partCount() ; ++i) {
		numParts += partMeshes[i].materialCount();
	}

	// Allocate

	allocateMemory(numParts);

	// Convert each part

	int	destPartIndex = 0;
	for (i = 0 ; i < mesh.partCount() ; ++i) {
		EditTriMesh *srcMesh = &partMeshes[i];
		for (int j = 0 ; j < srcMesh->materialCount() ; ++j) {

			// Get a mesh consisting of the faces
			// in this part that use this material

			EditTriMesh	onePartOneMaterial;
			srcMesh->extractOnePartOneMaterial(0, j, &onePartOneMaterial);

			// Sanity check the output mesh

			assert(onePartOneMaterial.vertexCount() > 0);
			assert(onePartOneMaterial.triCount() > 0);
			assert(onePartOneMaterial.partCount() == 1);
			assert(onePartOneMaterial.materialCount() == 1);

			// Convert the mesh to a trimesh

			getPartMesh(destPartIndex)->fromEditMesh(onePartOneMaterial);

			// Convert the material

			setPartTextureName(destPartIndex, onePartOneMaterial.material(0).diffuseTextureName);

			// !FIXME! Need to implement part names!

			// Next destination part, please

			++destPartIndex;
		}
	}
	assert(destPartIndex == getPartCount());

	// Free uindividual part meshes

	delete [] partMeshes;
}

//---------------------------------------------------------------------------
// Model::toEditMesh
//
// Convert a Model to an EditTriMesh

void	Model::toEditMesh(EditTriMesh &mesh) const {
	// !FIXME!
	assert(false);
}

//---------------------------------------------------------------------------
// Model::toEditMesh
//
// Convert a Model to an EditTriMesh

void	Model::importS3d(const char *s3dFilename) {

	char	text[256];

	// Load up the S3D into an EditTriMesh

	EditTriMesh editMesh;
	if (!editMesh.importS3d(s3dFilename, text)) {
		ABORT("Can't load %s.  %s.", s3dFilename, text);
	}

	// Optimize it for rendering

	editMesh.optimizeForRendering();

	// Convert it to renderable Model format

	fromEditMesh(editMesh);
}
