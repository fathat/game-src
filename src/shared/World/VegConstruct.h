// I N C L U D E S ///////////////////////////
#pragma once
#include <ode\ode.h>
#include "..\physics\construction.h"

// D E F I N I T I O N S /////////////////////

// C L A S S E S /////////////////////////////
class WorldManager;


//construction for crushable vegetation
class VegConstruction : public Construction
{
protected:
	//MESH!OMGOMGOMG
	MeshHandle hMesh;

	Real Width,Height,Length;

	MeshManager* pMM;

	bool Untouched;

public:

	VegConstruction() : Construction()
	{
		hMesh = NULL;
		Untouched = false;
	}
	std::string GetTypeString();

	bool Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen, MeshManager& MM, Position& Location, ICollisionHandler* collisionHandler=NULL  );
	
	bool CheckForParticleCollision ( Vector3D& p1, Vector3D& p2, CollisionInfo* c, MeshManager* MM );

	void Update( Real FrameTime, WorldManager* WMPtr );

	bool Touched() { return !Untouched; }
		
	void Draw(Screen3D& Screen);

};