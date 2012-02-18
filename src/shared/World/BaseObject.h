#pragma once

// I N C L U D E S /////////////////////////////////////////////
#include <vector>
#include "..\s3dlib\types.h"
#include "..\s3dlib\audio.h"
#include "..\mesh\meshmanager.h"
#include "..\network\messages.h"
#include "particle.h"
#include "..\collision\collision.h"
#include "..\s3dlib\game timer.h"
#include "LightManager.h"

// D E F I N E S ///////////////////////////////////////////////
#define SceneHandle int
class WorldManager;

enum TObjectType  { OT_STATIC, OT_DYNAMIC, OT_POWERUP };

// C L A S S E S ///////////////////////////////////////////////

class BaseObject
{
public:
	// Basic info
	std::string Name; //identification stuff
	int ID;
	TObjectType ObjectType;

	//ownership data
	SceneObject* Parent;
	std::vector<SceneObject*> EquipmentList;
	int EquipedItem;

	// location data
	Position       Location;
	Position       OldLocation;
	Position       NetLocation;
	Vector3D       Velocity;
	Vector3D       Force;

	Real Health;

	bool Collectible;

	AABB Box;
	BoundingSphere Sphere;
	

	bool InTree;
	

	WorldManager* WMPtr;
	MeshManager* MMPtr;
	
	Real LastDrawn; //frame time when the object was last drawn

	//this is the main mesh -- its animations are 
	//children of this
	MeshHandle hMesh;
	

	Light LightSource;



	SceneObject() { hMesh = -1; ID = -1; Stream = NULL; InTree = false; Parent = NULL; Collectible=false; MMPtr = NULL;  }
	virtual ~SceneObject() { Kill(); }

	//Initialization/destruction stuff
	virtual void Init(Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile);
	virtual void Kill();


	//Functions for inventories/sub objects
	virtual void AddEntity( SceneObject* Ent ) {
		Ent->Parent = this;
		EquipmentList.push_back( Ent );
	}
	virtual void RemoveEntity( int index ) {
		EquipmentList.erase( EquipmentList.begin() + index );
	}

	virtual void EquipItem ( int index ) {
		if( index < EquipmentList.size() )
			EquipedItem = index;
	}
	
	//Some movement functions
	virtual void SetLocation( Position &NewLocation ) { Location = NewLocation; }
	virtual void SetVelocity(Real x, Real y, Real z);
	virtual void SetDestination( Position& NewPosition);
	virtual void Move( Real FrameTime, WorldManager* SM );
	virtual void Update( Real FrameTime, WorldManager* SM );
	virtual void AdjustBoundingBox();
	virtual void GetExtents( AABB& ExtBox );
	virtual Real GetHeight ( );

	//Called whenever a message addressed to this object
	//arrives.
	virtual void HandleNetworkMessage( BYTE* Message );

	//If this object has a special feature (ie, it can fire
	//or be pressed or whatnot), it goes in here.
	virtual void Use( int function, Real Param1, Real Param2, Real FrameTime ) {}

	//Called when something hits this
	virtual void Damage( Vector3D& Source, Vector3D& Force, Vector3D& CollisionPoint );

	//Testing for collisions
	virtual bool Collision( SceneObject* sceneobject ); // not implemented!!
	virtual bool Collision( Vector3D& point1, Vector3D& point2, CollisionInfo* Collision );
	virtual bool SphereCollision( Vector3D& point1, Real radius, CollisionInfo* Collision );

	//Graphics functions
	virtual bool CheckVisibility( Screen3D& Screen, bool RecordTime=true );
	virtual void Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer );
	virtual void DrawInfo( Screen3D& Screen );
	virtual void DrawHUD( Screen3D& Screen, MeshManager& MM , FontManager& FM, Camera& Viewer  );

};