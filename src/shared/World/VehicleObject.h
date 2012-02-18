// I N C L U D E S /////////////////////////////////////////////
#include "SceneObject.h"
#include "..\S3Dlib\SoundManager.h"

// D E F I N E S ///////////////////////////////////////////////

class WorldManager;


// C L A S S E S ///////////////////////////////////////////////


//Vehicle object! WHEE!!
class VehicleObject : public SceneObject
{
protected:
	ConstructionHandle hConstruct;
	Matrix CameraMatrix, SkyMatrix;

	Real CameraX, CameraY, CameraZ;

	bool LostFocus;

	//SoundHandle EngineSound;
	FMOD::Sound* SndEngine;
	FMOD::Sound* SndHit;

	PlayingSound engineSound;
	
public:
	VehicleObject() { ObjectType = OT_DYNAMIC; ID = -1;  InTree = false; Parent = NULL; Collectible=false; MMPtr = NULL; LostFocus=false; }
	~VehicleObject() { Kill(); }

	//Initialization/destruction stuff
	void Init(Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile);
	void Kill();


	//Some movement functions
	void SetLocation( Position &NewLocation ) { Location = NewLocation; }
	void SetVelocity(Real x, Real y, Real z);
	void SetDestination( Position& NewPosition);
	void Move( Real FrameTime, WorldManager* SM );
	void Update( Real FrameTime, WorldManager* SM );
	void AdjustBoundingVolume();
	void GetExtents( AABB& ExtBox );
	Real GetHeight ( );

	//Called whenever a message addressed to this object
	//arrives.
	void HandleNetworkMessage( BYTE* Message );
	void HandleInput(  Real FrameTime, POINT MouseDelta, UCHAR* KeyBuffer );

	//If this object has a special feature (ie, it can fire
	//or be pressed or whatnot), it goes in here.
	void Use( int function, Real Param1, Real Param2, Real Param3, Real FrameTime ) ;

	void OnCollide( ICollisionHandler* collisionHandler );

	void OnFocusLost();
	
	float GetSpeed();

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