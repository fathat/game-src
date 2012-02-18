// I N C L U D E S ///////////////////////////
#pragma once
#include <ode\ode.h>
#include "..\S3DLib\Screen.h"
#include "..\Mesh\Mesh.h"
#include "..\Mesh\MeshManager.h"
#include "..\S3DLib\types.h"
#include "..\World\sceneobject.h"
#include "BasePhysicsObject.h"
#include "construction.h"

// D E F I N I T I O N S /////////////////////
class DynamicsSolver;
class WorldManager;
class DynamicsObject;
struct CollisionInfo;
class MeshManager;


// C L A S S E S /////////////////////////////

//a fixed object with a mesh for the geom
class Structure : public BasePhysicsObject 
{
public:
	MeshHandle structureMesh;
	dTriMeshDataID meshData;
	dGeomID Geom;
	bool Registered;
	bool Initialized;

	bool NoDisable; //makes the geom automatically enable itself. Mostly
					  //usefull for terrain which shouldn't subject to auto-disabling
					  //at the moment since it has some funny location issues (each node think's
					  //it's at 0,0,0)

	Real Radius;

	dReal* SavedVertices;
	dTriIndex* SavedIndices;
	int nSavedVertices;
	int nSavedIndices;

	DynamicsSurfaceDesc SurfaceDesc;

	bool collisionHappened;

public:
	Structure() { Registered = false; Initialized = false;
		SavedVertices = NULL;
		SavedIndices  = NULL;
		collisionHappened = false;
		NoDisable = false;
	}
	~Structure() { 
		Shutdown(); 
	}

	std::string GetTypeString();

	bool Init(MeshHandle mh, MeshManager* MM, DynamicsSolver* solver, Vector3D& Location, float yaw, float pitch, float roll );
	bool Init(CollisionMesh& ColMesh,DynamicsSolver* solver, Vector3D& Location, float yaw, float pitch, float roll );
	bool Init(Vector3D* Vertices, int nVertices, int* Indices, int nIndices,DynamicsSolver* solver, Vector3D& Location, float yaw, float pitch, float roll );
	void Shutdown();

	bool CheckForCollision(const Vector3D& a, const Vector3D& b, CollisionInfo * info );
};