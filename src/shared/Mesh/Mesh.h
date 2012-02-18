#pragma once
// I N C L U D E S ///////////////////////////////////

#include "..\s3dlib\screen.h"
#include "..\s3dlib\types.h"
#include "..\s3dlib\vertexbuffer.h"
#include "..\Collision\Collision.h"
#include "cfeff.h"
#include <ode/ode.h>

// C L A S S E S /////////////////////////////////////

struct MeshMaterial
{
	//constructors
	MeshMaterial (); 
	~MeshMaterial ();

	//color stuff
	D3DCOLORVALUE AmbientRGBA;
	D3DCOLORVALUE DiffuseRGBA;
	D3DCOLORVALUE SpecularRGBA;
	Real Transparency;
	Real Shininess; //how much specular lighting there is.

	//texture stuff
	TextureHandle Image;

	D3DMATERIAL9 AsDirect3DMaterial()
	{
		D3DMATERIAL9 D3DMat;
		ZeroMemory( &D3DMat, sizeof(D3DMATERIAL9) );
		D3DMat.Diffuse  = DiffuseRGBA;
		D3DMat.Ambient  = AmbientRGBA;
		D3DMat.Specular = SpecularRGBA;

		return D3DMat;
	}

	void SetOnScreen( Screen3D* screen )
	{
		screen->SetMaterial( &AsDirect3DMaterial() );
		screen->SetTexture(  Image, 0  );
	}
};


//Storage structure for a group of polygons and such

struct MeshGroup
{
	VertexBuffer VB; //list of vertex buffers for each "group" in the mesh
	IndexBuffer  IB;
	BoundingSphere boundingSphere;
	unsigned int* IndexList;
	int  nIndices;
	dReal* VertexList;
	int  nVertices;

	int materialIndex;

	float radius;

	ID3DXMesh* boundingMesh;
	
	MeshGroup(Screen3D& screen, cf_mesh& mesh, CollisionMesh* colmesh, AABB* bbox, FILE* file, bool isStatic) ;

	~MeshGroup() {
	
		if(IndexList)
			delete [] IndexList;
		if( VertexList)
			delete [] VertexList;
	}
};

//////// Mesh /////////////////////////////////
//
// Description:
//
// Class that represents a single mesh.
//
// Known Problems:
//
// - None
//
// TODO:
//
// - Re-add combining with other objects.
// - Fix = operator
//
///////////////////////////////////////////////

class Mesh
{
public:

	// Polygon data
	std::vector<MeshGroup*> subMeshes; //vertices and indices used for collision detection

	CollisionMesh ColMesh;
	
	MeshMaterial* Material;
	int nMaterials;

	// Filename of the mesh loaded
	std::string filename;

	// Dimensions
	Real FarthestPoint; //used to create a bounding sphere
	AABB Box;

	bool StaticMesh;
	
	
public:
	Mesh ();
	~Mesh();

	//Create/Destroy
	bool Create  ( char* MeshFile, Screen3D& Screen, bool Static = true ) ;
	void Destroy ( );

	//accessors
	const char* GetFilename() { return filename.c_str(); }
	int GetSubmeshCount() { return subMeshes.size(); }
	MeshGroup* GetSubmesh(int n) { return subMeshes[n]; }
		
	//Real functions
	bool CheckVisibility( Screen3D& Screen, Position& MeshLocation);
	BoundingSphere MakeBoundingSphere( Matrix& transform );
	bool CheckForCollision( Position& MeshLocation, Vector3D* Line, CollisionInfo* c);
	bool CheckForCollision( Matrix& MeshLocation, Vector3D* Line, CollisionInfo* c);
	bool CheckForSphereCollision( Position& MeshLocation, Vector3D& point, Real radius, CollisionInfo* c);
	void Draw ( Screen3D& Screen, Position* camera=NULL, Position* Location=NULL, bool IgnoreAlpha = false );
	void DrawBounds( Screen3D & Screen );


};