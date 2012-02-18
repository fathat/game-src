// I N C L U D E S //////////////////////////////////////////////////
#pragma once
#include <vector>
#include "..\s3dlib\screen.h"
#include <ode\ode.h>


// S T R U C T U R E S //////////////////////////////////////////////
struct CollisionInfo
{
	Vector3D CollisionPoint;
	Vector3D SuggestedNewLocation;
	Vector3D PlaneNormal;
	Real DistanceToPlane;
	Real Mass; //<-- for particles
	Vector3D Force;
	bool Stuck;

	CollisionInfo() { Stuck = false; Mass = 0;}
};

struct OdeVertex
{
	dReal x, y, z;
};

struct CMTriangle
{
	OdeVertex v1;
	OdeVertex v2;
	OdeVertex v3;
};

// C L A S S E S ////////////////////////////////////////////////////


class CollisionMesh
{
protected:
	//Mesh data
	dTriMeshDataID meshData;
	dGeomID Geom;

	//stuff for building the collision mesh
	std::vector<CMTriangle> TriangleList;
		
public:
	CollisionMesh() { NextGroup(); meshData = NULL; Geom = NULL; }
	~CollisionMesh();

	bool AddTriangles( Vector3D* v, int nTriangles );
	void NextGroup();
	void Finalize();

	void CombineWith( CollisionMesh& CmbMesh, CollisionMesh* OutMesh, Real Percent);

    bool Collision( Position &MeshLocation, Vector3D& p1, Vector3D& p2, CollisionInfo* Collision );
	bool Collision( Matrix& MeshLocation, Vector3D& p1, Vector3D& p2, CollisionInfo* Collision );
	bool SphereCollision( Position &MeshLocation, Vector3D& location, Real radius, CollisionInfo* Collision); 
};
