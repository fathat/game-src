#pragma once

// I N C L U D E S //////////////////////////////////////
#include "..\os\ssl.h"
#include "sceneobject.h"
#include "..\s3dlib\font.h"
#include "..\mesh\animatedmesh.h"
#include "animatedobject.h"


// D E F I N E S ////////////////////////////////////////


//forward definition for the weapon
struct Weapon;

// C L A S S E S ////////////////////////////////////////

// Represents a generic character thingamajig in the game.
// Can be friend or foe since behavior is controlled
// by a script
class Character : public AnimatedObject
{
public:
	std::string CurrentState;
	StateHandle CharacterScript;

	

public:
	Character();
	virtual ~Character();

	void Init( Screen3D& Screen,  WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile );

	void Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer );
	void Move( Real FrameTime, WorldManager* SM );
	void Update ( Real FrameTime, WorldManager* SM );

	void MoveByDelta( Real ForwardDelta, Real SideDelta );

	void AdjustBoundingVolume();

	void OnDamage ( const Vector3D& source, const Vector3D& collisionPoint, float damageAmount ) ;
	
	bool Collision( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision );

	void Use( int function, Real Param1, Real Param2, Real Param3, Real FrameTime );

	//Not inherited
	void ChangeState( std::string NewState );

	
};