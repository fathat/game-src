#pragma once

// I N C L U D E S //////////////////////////////////////
#include "sceneobject.h"
#include "..\s3dlib\font.h"
#include "..\mesh\animatedmesh.h"
#include "animatedobject.h"
#include "avatar.h"
#include "..\network\messages.h"

// D E F I N E S ////////////////////////////////////////

//forward definition for the weapon
struct Weapon;

// C L A S S E S ////////////////////////////////////////

// Represents the main local player, generally only one of these
// should be in the game. More than one *might* work, but right now
// it's unknown.
class NetworkAvatar : public AnimatedObject
{
protected:
	Position Pos1, Pos2;
	Real LerpDistance;
	Real StartTime;
	Real time; 
	Vector3D LerpVector;
	bool Lerping;
public:
	NetworkAvatar() { 
		EquipedItem = 0;
		Health = 1000;
		ObjectType = OT_DYNAMIC;
	}
		//Initialization/destruction stuff
	void Init(Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile);
	
	void Move( Real FrameTime, WorldManager* SM );
	void Update ( Real FrameTime, WorldManager* SM );
	
	bool Collision( Vector3D& point1, Vector3D& point2, CollisionInfo* Collision );

	void HandleNetworkMessage( BYTE* Message );
	
	void Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer );
	void Use( int function, Real Param1, Real Param2,  Real Param3,Real FrameTime );
};