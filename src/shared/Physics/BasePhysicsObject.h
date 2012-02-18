#pragma once

#include <string>
#include "..\S3Dlib\Types.h"

//Interface for handling collisions
class ICollisionHandler
{
public:
	virtual void OnCollide( ICollisionHandler* objectHit ) { }
	virtual void OnDamage ( const Vector3D& source, const Vector3D& collisionPoint, float damageAmount ) { } 
};

// Physics object
class BasePhysicsObject : public ICollisionHandler
{
public:
	virtual std::string GetTypeString() { return "BasePhysicsObject"; }

	virtual void Activate() {}
	virtual bool Disable() { return false; }
	virtual bool IsActive() { return false; }
	virtual bool IsGeomActive() { return false; }

};