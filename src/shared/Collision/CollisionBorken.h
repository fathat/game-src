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
	std::vector<CollisionMeshGroup> MeshList;
	std::vector<Vector3D> TempVertexList;
	int MeshIndex;

	void FindBoundingSphere( int meshindex );

public:
	CollisionMesh() { NextGroup(); MeshIndex=0; }

	bool AddTriangles( Vector3D* v, int nTriangles );
	void NextGroup();
	void Finalize();

	void CombineWith( CollisionMesh& CmbMesh, CollisionMesh* OutMesh, Real Percent);

    bool Collision( Position &MeshLocation, Vector3D& point1, Vector3D& point2, CollisionInfo* Collision );
	bool SphereCollision( Position &MeshLocation, Vector3D& location, Real radius, CollisionInfo* Collision); 
};


// F U N C T I O N S ////////////////////////////////////////////////
bool RayIntersectsTriangle( const D3DXVECTOR3& orig,
						const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                        D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                        FLOAT* t, FLOAT* u, FLOAT* v );

bool RayIntersectsTriangle ( Vector3D& orig, Vector3D& dir, Vector3D* vertex_list, Real* t, Real* u, Real* v);
bool LineCrossesPlane(  D3DXPLANE& Plane, Vector3D& point1, Vector3D& point2 );
bool LineIntersectsTriangle ( Vector3D* Line, Vector3D* vertex_list, CollisionInfo* col_info );