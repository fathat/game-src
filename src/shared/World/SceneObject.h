#pragma once

// I N C L U D E S /////////////////////////////////////////////
#include <vector>
#include "..\s3dlib\types.h"
#include "..\s3dlib\audio.h"
#include "..\s3dlib\input.h"
#include "..\s3dlib\game timer.h"
#include "..\network\messages.h"
#include "..\mesh\meshmanager.h"
#include "lightmanager.h"
#include "particle.h"
#include "..\physics\solver.h"
#include "..\physics\basephysicsobject.h"

// D E F I N E S ///////////////////////////////////////////////
#define SceneHandle int

struct M3DAddEntPacket;
struct CollisionInfo;
class WorldManager;

enum TObjectType  { OT_STATIC, OT_DYNAMIC, OT_POWERUP, OT_WEAPON };

#define USE_PRIMARY   0
#define USE_SECONDARY 1
#define USE_THIRD     2
#define USE_FOURTH    3

// C L A S S E S ///////////////////////////////////////////////

class SceneObject : public ICollisionHandler
{
protected:

	friend class WorldManager;
	// Basic info
	char Name[128]; //identification stuff
	int ID;
	TObjectType ObjectType;


	//variable for network stuff
	int LastPacket;

	//ownership data
	SceneObject* Parent;
	std::vector<SceneObject*> EquipmentList;
	int EquipedItem;
	int SecondaryEquipedItem;
	bool Vehicle;
	bool Collectible;
	TextureHandle Icon;

	// location data
	Position       Location;
	AABB Box;
	BoundingSphere Sphere;
	bool InTree;

	//Pointar storage
	WorldManager* WMPtr;
	MeshManager* MMPtr;

	//Health
	Real Health;

	//if the entity AI is enabled or not
	bool Thinking;

	//Datamagig!
	Real LastDrawn; //frame time when the object was last drawn
	Light LightSource;

public:
	//cached data
	Real CachedDistanceToCamera;
public:
	//Accessor thingamajigs
	int  GetID( ) { return ID; }
	void SetID( int nID ) { ID = nID; }
	TObjectType GetType() { return ObjectType; }
	void        SetType( TObjectType t ) { ObjectType = t; }
	char*       GetName() { return Name; }
	void		SetName(char* newname) { strcpy(Name, newname); }
	bool GetCollectible() { return Collectible; }
	void SetCollectible( bool c ) { Collectible = c; }
	Real GetHealth() { return Health; }
	void SetHealth(Real NewHealth ) { Health = NewHealth; }
	SceneObject* GetParent() { return Parent; }
	bool IsOwned () { 
		if(Parent) 
			return true; 
		return false; 
	}
	void SetLastPacket( int nPacket ) { LastPacket = nPacket; }
	int  GetLastPacket() { return LastPacket; }

	bool IsStatic () { return (ObjectType == OT_STATIC); }

	bool IsThinking() { return Thinking; }
	void EnableThinking( bool tf ) { Thinking = tf; }

	TextureHandle GetIcon() { return Icon; }
	void SetIcon( TextureHandle t ) { Icon = t; }

	SceneObject() {  ID = -1; InTree = false; Parent = NULL; Collectible=false; MMPtr = NULL; ZeroMemory(Name, 128); LastPacket=-1; Thinking = true; SecondaryEquipedItem=-1;}
	virtual ~SceneObject() { Kill(); }

	//Initialization/destruction stuff
	virtual void Init(Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile);
	virtual void Kill();


	//Functions for inventories/sub objects
	virtual void AddEntity( SceneObject* Ent ) ;
	virtual void RemoveEntity( int index ) ;

	virtual void RemoveAllEntities() ;

	virtual void DropAll( ) ;

	virtual void EquipItem ( int index ) ;

	int  GetEquippedItemIndex() { return EquipedItem; }
	int  GetItemCount() { return EquipmentList.size(); }
	SceneObject*  GetItem( int index ) { return EquipmentList[index]; }

	
	//Some movement functions
	

	virtual void SetLocation( Position &NewLocation ) { Location = NewLocation; }
	virtual void SetVelocity(Real x, Real y, Real z);
	virtual Real GetVelocityX() { return 0; }
	virtual Real GetVelocityY() { return 0; }
	virtual Real GetVelocityZ() { return 0; }
	virtual void SetDestination( Position& NewPosition);
	virtual void Move( Real FrameTime, WorldManager* SM );
	virtual void MoveByDelta( Real ForwardDelta, Real SideDelta ) {}
	virtual void Update( Real FrameTime, WorldManager* SM );
	virtual void AdjustBoundingVolume();
	virtual void GetExtents( AABB& ExtBox );
	virtual BoundingSphere& GetBoundingSphere( ) { return Sphere; }
	virtual AABB GetBoundingBox() { return Box; }
	virtual AABB GetLocalBoundingBox() { return Box; }
	virtual Real GetHeight ( );
	Position& GetLocation() { return Location; }
	Real GetX() { return Location.x; }
	Real GetY() { return Location.y; }
	Real GetZ() { return Location.z; }
	Real GetYaw() { return Location.Yaw; }
	Real GetPitch() { return Location.Pitch; }
	Real GetRoll() { return Location.Roll; }
	void SetX(Real x) { Location.x = x; }
	void SetY(Real y) { Location.y = y; }
	void SetZ(Real z) { Location.z = z; }
	void SetYaw(Real a) { Location.Yaw = a; }
	void SetPitch(Real a) { Location.Pitch = a; }
	void SetRoll(Real a) { Location.Roll = a; }

	//Called whenever a message addressed to this object
	//arrives.
	virtual void HandleNetworkMessage( BYTE* Message );

	//handle input
	virtual void HandleInput(  Real FrameTime, POINT MouseDelta, UCHAR* KeyBuffer );
	virtual void StealInputFocus( );
	virtual void OnFocusLost();

	//If this object has a special feature (ie, it can fire
	//or be pressed or whatnot), it goes in here.
	virtual void Use( int function, Real Param1, Real Param2, Real Param3, Real FrameTime ) {}

	//Called when something hits this
	virtual void Damage(const Vector3D& Source, const Vector3D& Force, const Vector3D& CollisionPoint );
	void OnDamage (const Vector3D& source, const Vector3D& collisionPoint, float damageAmount ) ;

	//Testing for collisions
	virtual bool LightCollision ( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision );
	virtual bool Collision( SceneObject* sceneobject ); // not implemented!!
	virtual bool Collision( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision );
	virtual bool SphereCollision( const Vector3D& point1, Real radius, CollisionInfo* Collision );

	//Graphics functions
	virtual bool CheckVisibility( Screen3D& Screen, bool RecordTime=true );
	virtual void Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer );
	virtual void DrawInfo( Screen3D& Screen );
	virtual void DrawBounds( Screen3D& Screen ) {}
	virtual void DrawHUD( Screen3D& Screen, MeshManager& MM , FontManager& FM, Camera& Viewer  );

	virtual void ChangeState( std::string NewState ) {}

};