#pragma once

// I N C L U D E S /////////////////////////////////////////////
#include "SceneObject.h"
#include "..\mesh\AnimatedMesh.h"

// D E F I N E S ///////////////////////////////////////////////
class WorldManager;

// C L A S S E S ///////////////////////////////////////////////

class AnimatedObject : public SceneObject
{
protected:
	AnimatedMesh MeshA;	

	Real MaxSpeed;
	Real Acceleration;

	Vector3D TargetVelocity;
	Vector3D CurrentVelocity;

	Vector3D Destination;
	bool UseDestination; //Use the set destination or ignore it?

public:
	AnimatedObject() { ID = -1;InTree = false; Parent = NULL; Collectible=false; MMPtr = NULL; MaxSpeed=30;  Acceleration=30;}
	~AnimatedObject() { Kill(); }

	//Initialization/destruction stuff
	void Init(Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile);
	void Kill();


	//Some movement functions
	void SetLocation( Position &NewLocation ) { Location = NewLocation; }
	void SetVelocity(Real x, Real y, Real z);
	void SetDestination( Position& NewPosition);
	Real GetVelocityX() { return TargetVelocity.x; }
	Real GetVelocityY() { return TargetVelocity.y; }
	Real GetVelocityZ() { return TargetVelocity.z; }
	void Move( Real FrameTime, WorldManager* SM );
	void Update( Real FrameTime, WorldManager* SM );
	void AdjustBoundingVolume();
	void GetExtents( AABB& ExtBox );
	Real GetHeight ( );
	AABB GetLocalBoundingBox() { return MeshA.GetBox(); }

	//Called whenever a message addressed to this object
	//arrives.
	void HandleNetworkMessage( BYTE* Message );

	//If this object has a special feature (ie, it can fire
	//or be pressed or whatnot), it goes in here.
	void Use( int function, Real Param1, Real Param2, Real Param3, Real FrameTime ) {}


	//Testing for collisions
	bool Collision( SceneObject* sceneobject ); // not implemented!!
	bool Collision( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision );
	bool SphereCollision( const Vector3D& point1, Real radius, CollisionInfo* Collision );

	//Graphics functions
	bool CheckVisibility( Screen3D& Screen, bool RecordTime=true );
	void Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer );
	void DrawInfo( Screen3D& Screen );
	void DrawHUD( Screen3D& Screen, MeshManager& MM , FontManager& FM, Camera& Viewer  );

};