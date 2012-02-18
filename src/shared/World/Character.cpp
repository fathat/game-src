#include "character.h"
#include "weapon.h"

#include "..\defs\defs.h"

Character::Character()
{
	EquipedItem = 0;
	Health = 500;

	TargetVelocity.x = 0;
	TargetVelocity.y = 0;
	TargetVelocity.z = 0;
	Location.x = 0;
	Location.y = 0;
	Location.z = 0;

	UseDestination = false;

	ObjectType = OT_DYNAMIC;
}

Character::~Character()
{

}


//////// Init /////////////////////////////////
//
// Description:
//
// Initializes data to acceptable starting
// values.
//
///////////////////////////////////////////////

void Character::Init( Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile )
{
	//load in the animated mesh
	char TempBuffer[255];
	GetIniValue(ObjectFile, "graphics", "animation_file", TempBuffer);
	MeshA.Init( Screen, MM, TempBuffer );

	//Store some pointery things
	WMPtr = WM;
	MMPtr = &MM;


	Health = GetIniValue(ObjectFile, "properties", "health");
	//Load in the character's primary weapon
	GetIniValue(ObjectFile, "weapon", "weapon", TempBuffer);
	if( strcmpi(TempBuffer, "nostring") != 0)
	{

		//Create the weapon
		Weapon* DefaultWeapon = new Weapon;
		DefaultWeapon->Init( Screen, WM, MM, AM, TempBuffer );

		//Add it to the world
		WMPtr->Add( DefaultWeapon );

		//also add it locally
		this->AddEntity(DefaultWeapon );
	}

	//load in the side arm
	GetIniValue(ObjectFile, "weapon", "sidearm", TempBuffer);
	if( strcmpi(TempBuffer, "nostring") != 0)
	{
		Weapon* SideWeapon = new Weapon;
		SideWeapon->Init( Screen, WM, MM, AM, TempBuffer );

		WMPtr->Add( SideWeapon );

		this->AddEntity(SideWeapon );
	}

	CurrentState = "idle";

	//CharacterScript = GetSSLKernel()->TaskMan.BuildScript( "..\\scripts\\enemy.shil", NULL );

/*
	Value V;
	V.SetNum( ID );
	GetSSLKernel()->TaskMan.AddFunctionParameter( CharacterScript, V );
	GetSSLKernel()->TaskMan.CallAppFunction( CharacterScript, "SetCharacterHandle", 1 );
	GetSSLKernel()->TaskMan.RunActiveScripts( 100);*/
}


void Character::Use( int function, Real Param1, Real Param2, Real Param3, Real FrameTime )
{
	if(EquipmentList.size() == 0)
		return;


	if( function == 0 )
	{
		ChangeState("shooting");
		if(EquipmentList[EquipedItem])
			EquipmentList[EquipedItem]->Use( WF_FIRE1, NULL, NULL, NULL, WMPtr->GTPtr->GetFrameTime()  );
	}

	else if( function == 1 )
	{
		if(EquipmentList[EquipedItem])
			EquipmentList[EquipedItem]->Use( WF_FIRE1, NULL, NULL, NULL, WMPtr->GTPtr->GetFrameTime()  );
	}

	//Reloads weapon
	else if( function == 2 )
	{
		if(EquipmentList[EquipedItem])
			EquipmentList[EquipedItem]->Use( WF_RELOAD, NULL, NULL, NULL, WMPtr->GTPtr->GetFrameTime()   );
	}
	
}




//////// Damage ///////////////////////////////
//
// Description:
//
// Notifies player of a force acting upon him/her
//
///////////////////////////////////////////////

void Character::OnDamage (const Vector3D& source, const Vector3D& collisionPoint, float damageAmount ) 
{
	Health -= damageAmount*10;
	ChangeState( "injured" );
}


//////// Move /////////////////////////////////
//
// Description:
//
// Adjusts vectors and moves players by them. 
//
///////////////////////////////////////////////

void Character::Move( Real FrameTime, WorldManager* SM )
{

	//declarations
	Real Slope;
	Vector3D SlopeNormal;
	D3DXPLANE TestPlane;

	
	//Move the object down from gravity.
	TargetVelocity.y -= (GRAVITY/2) * FrameTime;

	if(TargetVelocity.y < -GRAVITY)
		TargetVelocity.y = -GRAVITY;

	
	//find the objects current altitude.
	Real GetHeightValue = SM->GetHeightValue(Location);
	

	//Move the object based on the velocity	
	if(fabs(TargetVelocity.x) > 1 || fabs(TargetVelocity.z) > 1)
	{
		Location.x += TargetVelocity.x * FrameTime;
		Location.z += TargetVelocity.z * FrameTime;
	}

	if(fabs(TargetVelocity.y) > .2f)
		Location.y += TargetVelocity.y * FrameTime;
	   
	Real NewHeight = SM->GetHeightValue(Location, &Slope, &SlopeNormal, &TestPlane);

	//adjust the objects height if it's falling through
	//the ground
	if(Location.y< NewHeight)
	{

		if(Slope > .8f)
		{
			Real UpSpeed = (NewHeight-Location.y)*12;

			Location.y+= UpSpeed*FrameTime;

			if(Location.y > NewHeight)
				Location.y = NewHeight;
			TargetVelocity.y = 0;
		}

		else
		{
		
			//Adjust the objects height if it's falling. Take into
			//account slope and "slide" the object if neccesary.

			//first, create a ray to test with
			D3DXVECTOR3 p1, p2, result;

			p1.x = Location.x;
			p1.y = Location.y;
			p1.z = Location.z;

			p2.x = Location.x + SlopeNormal.x * 30;
			p2.y = Location.y + SlopeNormal.y * 30;
			p2.z = Location.z + SlopeNormal.z * 30;

			//Next, find where our ray intersects the plane
			D3DXPlaneIntersectLine(&result, &TestPlane, &p1, &p2);

			//Now, place the player at that intersection point 
			Location.x = result.x;
			Location.y = result.y;
			Location.z = result.z;

			TargetVelocity.y += FRICTION/1.2f * FrameTime;
		}

	
	}

}



bool Character::Collision( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision )
{
	//Declarations
	Vector3D Line[2];
	Line[0] = point1;
	Line[1] = point2;
	
	//Check for the collision
	if( MeshA.CheckForCollision( Location, Line, Collision ) )
	{
		return true;
	}

	return false;
}

void Character::MoveByDelta ( Real ForwardDelta, Real SideDelta)
{
	if(Health <= 0)
	{
		EquipItem(9);
		return;
	}

	Position NewLocation = Location;
	
	if(ForwardDelta != 0)
	{
		NewLocation.x += ( -ForwardDelta ) * (float)(*m_cos)(Location.Yaw + RADIAN(90));
		NewLocation.z += ( ForwardDelta ) * (float)(*m_sin)(Location.Yaw + RADIAN(90));
	}

	if(SideDelta != 0)
	{
		NewLocation.x += ( SideDelta ) * (float)(*m_cos)(Location.Yaw );
		NewLocation.z += ( -SideDelta ) * (float)(*m_sin)(Location.Yaw );
	}

	if(ForwardDelta !=0 || SideDelta != 0)
	{
		TargetVelocity.x = NewLocation.x - Location.x;
		TargetVelocity.z = NewLocation.z - Location.z;
	}


}


void Character::ChangeState( std::string NewState )
{
	//If this state is different from what we're doing right
	//now, then change to it
	if(NewState.compare( CurrentState))
	{
		MeshA.Change( (char*)NewState.c_str(), .1f );
		CurrentState = NewState;
	}
}


//////// Update ///////////////////////////////
//
// Description:
//
// Lets avatar do any neccesary tasks for this
// frame.
//
///////////////////////////////////////////////

void Character::Update ( Real FrameTime, WorldManager* SM )
{
	//Update the player's position
	Move( FrameTime, SM);
	AdjustBoundingVolume();
	MeshA.Update( FrameTime );

	if(Health<= 0)
		ChangeState("dead");

	//Update the weapons
	if(EquipmentList.size())
	{
		if(EquipmentList[EquipedItem])
		{
			Position ItemLocation = Location;
			ItemLocation.y += MeshA.GetBox().max.y*.75f;
			EquipmentList[EquipedItem]->SetLocation(ItemLocation);
			EquipmentList[EquipedItem]->Update( FrameTime, SM );
		}
	}
}


void Character::AdjustBoundingVolume()
{
	//readjust bounding box
	Box.min.x = Location.x + MeshA.GetBox().min.x;
	Box.min.y = Location.y + MeshA.GetBox().min.y ;
	Box.min.z = Location.z + MeshA.GetBox().min.z;
	Box.max.x = Location.x + MeshA.GetBox().max.x;
	Box.max.y = Location.y + MeshA.GetBox().max.y;
	Box.max.z = Location.z + MeshA.GetBox().max.z;

	Sphere.x( Location.x);
	Sphere.y( Location.y+ MeshA.GetBox().max.y/2);
	Sphere.z( Location.z);

	Sphere.Radius = MeshA.GetSphereRadius()+4 ;
	
	LightSource.x = Location.x;
	LightSource.y = Location.y;
	LightSource.z = Location.z;

	//Update the weapons
	
	if(EquipmentList.size())
	{
		if(EquipmentList[EquipedItem])
		{
			//[HACK] This will crash if the guy has something other
			//       than a weapon. Needs type check fix.
			Weapon* w = (Weapon*)EquipmentList[EquipedItem];
            if(w->GetBoundingBox().min.x < Box.min.x )
				Box.min.x = w->GetBoundingBox().min.x;
			if(w->GetBoundingBox().min.y < Box.min.y )
				Box.min.y = w->GetBoundingBox().min.y;
			if(w->GetBoundingBox().min.z < Box.min.z )
				Box.min.z = w->GetBoundingBox().min.z;
			if(w->GetBoundingBox().max.x > Box.max.x )
				Box.max.x = w->GetBoundingBox().max.x;
			if(w->GetBoundingBox().max.y > Box.max.y )
				Box.max.y = w->GetBoundingBox().max.y;
			if(w->GetBoundingBox().max.z > Box.max.z )
				Box.max.z = w->GetBoundingBox().max.z;
				
			if(Sphere.Radius < w->GetBoundingSphere().Radius )
				Sphere.Radius = w->GetBoundingSphere().Radius;
		}
	}

	

}


//////// Draw /////////////////////////////////
//
// Description:
//
// Draws the avatar, animations and such included.
//
///////////////////////////////////////////////

void Character::Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer )
{
	//set the transformation
	Position DrawLocation;
	DrawLocation = Location;
	DrawLocation.Pitch = 0;
	
	MeshA.Draw( Screen, DrawLocation.GetMatrix() );


	for(int i=0; i<EquipmentList.size(); i++)
	{
		if(EquipmentList[i])
			EquipmentList[i]->Draw( Screen, MM, Viewer, Timer );

	}

}

