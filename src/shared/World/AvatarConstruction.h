// I N C L U D E S ///////////////////////////
#pragma once
#include <ode\ode.h>
#include "..\physics\construction.h"

// D E F I N I T I O N S /////////////////////

// C L A S S E S /////////////////////////////


//represents an avatar object thingy! YAY!
class AvatarConstruction : public Construction
{
protected:
	bool OnGround;
	int  LastJumpTime;
	int  JumpDelay; 
	bool ForceEnable;
	float Speed;

	bool Moving;

public:
	AvatarConstruction( float speed ) : Construction()
	{
		Speed = speed;
		OnGround = false;
		Moving = false;
	}

	std::string GetTypeString();

	bool Construct( char* descriptionFile, DynamicsSolver* solver, Screen3D& Screen, MeshManager& MM, Position& Location, ICollisionHandler* aCollisionHandler=NULL );
	void Deconstruct();

	void OnTurn( Real Force , Real Angle);
	void OnAccelerate( Real Force, Real SideForce=0, Real UpForce=0 );

	
	void SetOnGround( bool yesno ) { OnGround = yesno; }
	bool IsOnGround () { return OnGround; }

	bool Disable();
		
	void Update( Real FrameTime, WorldManager* SM );
	
	void Draw(Screen3D& Screen);

};