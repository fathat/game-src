// I N C L U D E S //////////////////////////////////////////////////
#pragma once
#include <vector>
#include "..\s3dlib\screen.h"


// S T R U C T U R E S //////////////////////////////////////////////
struct CollisionInfo
{
	Vector3D CollisionPoint;
	Vector3D SuggestedNewLocation;
	Vector3D PlaneNormal;
	Real DistanceToPlane;
	Vector3D Force;
	bool Stuck;

	CollisionInfo() { Stuck = false; }
};


struct CollisionMeshGroup
{
	//Sphere data
	Real x, y, z;
	Real Radius;
	

	//final vertex data
	Vector3D* VertexList;
	int nVertices;

	CollisionMeshGroup() {
		VertexList = NULL;
		nVertices = 0;
	}
	~CollisionMeshGroup() {
	
	}
};

// C L A S S E S ////////////////////////////////////////////////////

class CollisionMesh
{
public:
	//finalized mesh data
	CollisionMeshGroup*   MeshList;
	int nMeshes;

	//stuff for building the collision mesh
	std::vector<CollisionMeshGroup> MeshListTemp;
	std::vector<Vector3D> TempVertexList;
	int MeshIndex;

	void FindBoundingSphere( int meshindex );

public:
	CollisionMesh() { NextGroup(); MeshIndex=0; MeshList = NULL; nMeshes=0; }
	~CollisionMesh() { if(MeshList) delete [] MeshList; }

	bool AddTriangles( Vector3D* v, int nTriangles );
	void NextGroup();
	void Finalize();

	void CombineWith( CollisionMesh& CmbMesh, CollisionMesh* OutMesh, Real Percent);

    bool Collision( Position &MeshLocation, Vector3D& p1, Vector3D& p2, CollisionInfo* Collision );
	bool SphereCollision( Position &MeshLocation, Vector3D& location, Real radius, CollisionInfo* Collision); 
};
