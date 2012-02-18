// I N C L U D E S /////////////////////////////////////////
#pragma once
#include "..\Mesh\meshmanager.h"
#include "..\Mesh\mesh.h"
#include "..\Mesh\AnimatedMesh.h"
#include "..\s3dlib\types.h"
#include "..\s3dlib\font.h"
#include "..\s3dlib\audio.h"
#include "..\s3dlib\soundmanager.h"
#include "particle.h"
#include "world.h"


// D E F I N E S ///////////////////////////////////////////
enum WeaponState { WS_FIRING, WS_IDLE, WS_RELOADING };

//Use defines for the weapons
#define WF_FIRE1  0
#define WF_FIRE2  1
#define WF_RELOAD 2

// C L A S S E S ///////////////////////////////////////////


class Weapon : public SceneObject
{
public:
    int Ammo;
	int ClipAmmo;
	Screen3D* ScreenPtr;

	int MaxClipAmmo; //0 indicates that weapon doesn't use clip but fires constantly
	int MaxAmmo; //-1 means that weapon has unlimited ammo

	Real RateOfFire;
	Real ReloadTime;

	Real ProjectileSpeed;

	AnimatedMesh MeshA;
	AnimatedMesh MeshB;
	//SoundHandle FiringSound;
	//SoundHandle ReloadSound;

	FMOD::Sound* SndFiring;
	FMOD::Sound* SndReload;


	TextureHandle Aimer;
	TextureHandle GlossMap;
	TextureHandle MuzzleFlash;

	float MuzzleFlashFade;

	WeaponState State;
	Real TimeLeft;

	Position WeaponShot;
	Position SecondaryWeaponLocation;

	Font2D* WeaponFont;

	SoundManager* AMPtr;

	ParticleStream Smoke;
	ParticleStream Bullets;
	ParticleStream Explosives;

	
	bool AimUsingRotation; //Uses a yaw and pitch instead of a vector
	bool HitPlayer;        //This weapon can affect the player?
	bool RightHanded;      //Which side is the gun on?
	bool ShowAmmoAmount;   //Show the ammo amount in the bottom right?
	bool DualWeapons;	   //weapon consists of two guns? (like dual berettas)
	bool OddShot;

	bool InstantBullets;
	float DamageOnHit;


public:

	Weapon() ;

	void Init( Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* WeaponFile );
	void SetHand( bool RightHand ) { RightHanded = RightHand; }

	WeaponState GetState() { return State; }
	Real GetTimeLeft() { return TimeLeft; }
	Real GetRateOfFire() { return RateOfFire; }
	void ToggleHUD() { ShowAmmoAmount = !ShowAmmoAmount; }

	

	void SetLocation( Position& NewLocation );
	void SetFireVector( Vector3D& Aim );
	void SetInjurePlayer( bool hitPlayer ) { HitPlayer = hitPlayer; }

	Font2D* GetFont() { return WeaponFont; }

	bool CheckVisibility( Screen3D& Screen, bool RecordTime=true );
	void AdjustBoundingVolume();

	void Use( int function,  Real Param1, Real Param2,  Real Param3, Real FrameTime );

	void Update( Real Frametime, WorldManager* SM );
	void Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer );
	void DrawHUD( Screen3D& Screen, MeshManager& MM , FontManager& FM, Camera& Viewer  );


};