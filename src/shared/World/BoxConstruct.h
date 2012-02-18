// I N C L U D E S ///////////////////////////
#pragma once
#include <ode\ode.h>
#include "..\physics\construction.h"

// D E F I N I T I O N S /////////////////////

// C L A S S E S /////////////////////////////


//represents an avatar object thingy! YAY!
class BoxConstruction : public Construction
{
protected:

	MeshHandle hMesh;

	Real Width,Height,Length;

	MeshManager* pMM;

public:

	BoxConstruction() : Construction()
	{
		pMM = NULL;
	}
	
	std::string GetTypeString();

	bool Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen, MeshManager& MM, Position& Location, ICollisionHandler* collisionHandler=NULL  );
	
	bool CheckForParticleCollision ( Vector3D& p1, Vector3D& p2, CollisionInfo* c, MeshManager* MM );
		
	void Draw(Screen3D& Screen);

};