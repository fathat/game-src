#pragma once

// I N C L U D E S //////////////////////////////////////
#include "sceneobject.h"
#include "..\s3dlib\font.h"
#include "..\mesh\animatedmesh.h"
#include "animatedobject.h"

// D E F I N E S ////////////////////////////////////////

//Names for various actions the avatar can take
#define AU_USE    0
#define AU_RELOAD 1
#define AU_JUMP   2
#define AU_CROUCH 3
#define AU_SWITCHWEAPON 4
#define AU_USE2   5

enum AvatarState { AV_IDLE, AV_RUNNING, AV_STRAFERIGHT, AV_STRAFELEFT };

//forward definition for the weapon
struct Weapon;

// C L A S S E S ////////////////////////////////////////

// Represents the main local player, generally only one of these
// should be in the game. More than one *might* work, but right now
// it's unknown.
class Avatar : public AnimatedObject
{
public:
	//description of what avatar is doing -- also used for animation
	char Action;                //what's the character's current action?
	Real PercentActionComplete; //how far along is the action?

	Position SpawnPoint;

	float Money;
	float MoneyLimit;

	AvatarState State;

	Real MaxSpeed;
	Real Acceleration;

	Real ViewBob;
	Real MaxViewBob;
	Real ViewBobDirection;
	Real ViewBobSpeed;
	
	bool Swimming;
	bool Crouching;
	bool OnPlatform;
	bool StrafeLeft;
	bool StrafeRight;
	
	ConstructionHandle hConstruction;

	float InventoryTime;

	Vector3D MoveVector;

	float RespawnTime;

	
	
public:

	Avatar();
	virtual ~Avatar();
	

	void Init( Screen3D& Screen,  WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile );

	void Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer );
	void Move( Real FrameTime, WorldManager* SM );
	void Update ( Real FrameTime, WorldManager* SM );

	void AddEntity(SceneObject* ent );

	void MoveByDelta( Real ForwardDelta, Real SideDelta );

	void SetPosition( Real x, Real y, Real z );
	void SetVelocity( Real x, Real y, Real z );

	float GetMoney() { return Money; }

	void OnCollide( ICollisionHandler* collisionHandler );

	Vector3D GetVelocity();
	
	bool Collision( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision );

	void Use( int function, Real Param1, Real Param2, Real Param3, Real FrameTime );
	void CheckForWeapons( WorldManager* WM  );

	void HandleInput(  Real FrameTime, POINT MouseDelta, UCHAR* KeyBuffer );


	void DrawHUD( Screen3D& Screen, MeshManager& MM, FontManager& FM, Camera& Viewer );
	

	void OnDamage ( const Vector3D& source, const Vector3D& collisionPoint, float damageAmount ) ;

	

	
};