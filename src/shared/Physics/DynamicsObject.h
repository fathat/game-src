// I N C L U D E S ////////////////////////////////////
#pragma once
#include <ode\ode.h>
#include "BasePhysicsObject.h"
#include "SurfaceDesc.h"

// F O R W A R D   D E C L A R A T I O N S ////////////
class DynamicsSolver;


// C L A S S E S //////////////////////////////////////

class DynamicsObject : public ICollisionHandler
{
public:
	dBodyID Body;
	dGeomID Geom;
	dMass Mass;
	bool Setup;
	bool isAvatar;
	bool HasGeom;
	

	DynamicsSurfaceDesc SurfaceDesc;
	BasePhysicsObject* Owner;
public:
	DynamicsObject();
	~DynamicsObject();

	void CreateBody(DynamicsSolver* solver);
	void SetData();

	void OnCollide(ICollisionHandler* objectHit );
	void OnDamage ( const Vector3D& source, const Vector3D& collisionPoint, float damageAmount );

	void Destroy();
};
