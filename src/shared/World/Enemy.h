#pragma once

// I N C L U D E S //////////////////////////////////////
#include "..\s3dlib\soundmanager.h"
#include "character.h"


// D E F I N E S ////////////////////////////////////////

//forward definition for the weapon
struct Weapon;

enum EnemyState { ES_IDLE, ES_RUNNING, ES_SHOOTING };

// C L A S S E S ////////////////////////////////////////

// Represents a enemy that kills the player and some other things
// and such. BLAH BLAH BLAH
class Enemy : public Character
{
public:
	EnemyState EState;
	bool Alerted;

	bool HasYelled;

	float AttackDistance;
	float RunDistance;
	float EnemySpeed;
	float HearingDistance;
	float SightDistance;

	float DeathTime ;

	float InitialHealth;

	ParticleStream BloodParticles;

	FMOD::Sound* AttackSound;
	
public:
	Enemy() { Alerted = false; EState = ES_IDLE; ObjectType = OT_DYNAMIC; AttackSound = NULL; HasYelled = false;}

	void Init( Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile );

	void OnDamage ( const Vector3D& source, const Vector3D& collisionPoint, float damageAmount ) ;

	void AvoidObstacles();

	void CheckForStateTransition(Real FrameTime);
	void Think ( Real FrameTime, WorldManager* SM );
	void Update ( Real FrameTime, WorldManager* SM );

	void Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer );
	
};