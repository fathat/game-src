// I N C L U D E S ///////////////////////////
#pragma once
#include <ode\ode.h>
#include "..\S3DLib\Screen.h"
#include "..\Mesh\Mesh.h"
#include "..\Mesh\MeshManager.h"
#include "..\S3DLib\types.h"
#include "..\World\sceneobject.h"
#include "BasePhysicsObject.h"
#include "DynamicsObject.h"
#include "SurfaceDesc.h"
// D E F I N I T I O N S /////////////////////
class DynamicsSolver;
class WorldManager;
struct CollisionInfo;
class MeshManager;

// C L A S S E S /////////////////////////////


//represents an object or device made up of a number
//of physics objects and joints and such
class Construction : public BasePhysicsObject 
{
protected:
	friend class DynamicsSolver;
	DynamicsSolver* mySolver;

	DynamicsObject* ObjectList;
	int nObjects;

	dJointID* JointList;
	int nJoints;

	dSpaceID GeomGroup;

	LPD3DXMESH DrawMesh;

	Real LinearDisableEpsilon;
	Real AngularDisableEpsilon;

	ICollisionHandler* CollisionHandler;

	bool Active;

public:

	std::string GetTypeString();

	//constructors
	Construction();
	virtual ~Construction();

	virtual bool Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen, MeshManager& MM, Position& Location, ICollisionHandler* ch );
	virtual void Deconstruct();

	virtual void ApplyForce( Vector3D force );

	virtual void Turn( Real Force , Real Angle) ;
	virtual void Accelerate( Real Force, Real SideForce=0, Real UpForce=0 );

	virtual void OnTurn( Real Force , Real Angle);
	virtual void OnAccelerate( Real Force, Real SideForce=0, Real UpForce=0 );
	
	virtual void GetLocation(Vector3D& v);
	virtual void GetVelocity(Vector3D& v);
	virtual DynamicsObject* GetSubObject( int i );

	virtual void SetLocation ( Real x, Real y, Real z );
	virtual void SetVelocity ( Real x, Real y, Real z );

	virtual void EnableAllGeoms();
	virtual void DisableAllGeoms();

	void Activate();
	bool Disable();
	bool IsActive();
	bool IsGeomActive();

	ICollisionHandler* GetCollisionHandler() { return CollisionHandler; }

	void OnCollide( ICollisionHandler* objectHit ) ;
	void OnDamage ( const Vector3D& source, const Vector3D& collisionPoint, float damageAmount );

	virtual bool CheckForParticleCollision ( Vector3D& p1, Vector3D& p2, CollisionInfo* c, MeshManager* MM );

	virtual void GetMatrix( Matrix* M );
	
	virtual void MainUpdate( Real FrameTime, WorldManager* SM );
	virtual void Update( Real FrameTime, WorldManager* SM );
	
	virtual void Draw(Screen3D& Screen);

};