#include "NetworkObject.h"
#include "weapon.h"


//////// Init /////////////////////////////////
//
// Description:
//
// Initializes data to acceptable starting
// values.
//
///////////////////////////////////////////////

void NetworkAvatar::Init( Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile )
{
	//load in the animated mesh
	char TempBuffer[255];
	GetIniValue(ObjectFile, "graphics", "animation_file", TempBuffer);
	MeshA.Init( Screen, MM, TempBuffer );


	//Store some pointery things
	WMPtr = WM;
	MMPtr = &MM;

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

	//load in the third weapon
	GetIniValue(ObjectFile, "weapon", "third", TempBuffer);
	if( strcmpi(TempBuffer, "nostring") != 0)
	{
		Weapon* SideWeapon = new Weapon;
		SideWeapon->Init( Screen, WM, MM, AM, TempBuffer );

		WMPtr->Add( SideWeapon );

		this->AddEntity(SideWeapon );
	}

	//load in the fourth weapon
	GetIniValue(ObjectFile, "weapon", "fourth", TempBuffer);
	if( strcmpi(TempBuffer, "nostring") != 0)
	{
		Weapon* SideWeapon = new Weapon;
		SideWeapon->Init( Screen, WM, MM, AM, TempBuffer );

		WMPtr->Add( SideWeapon );

		this->AddEntity(SideWeapon );
	}

	//load in the fifth weapon
	GetIniValue(ObjectFile, "weapon", "fifth", TempBuffer);
	if( strcmpi(TempBuffer, "nostring") != 0)
	{
		Weapon* SideWeapon = new Weapon;
		SideWeapon->Init( Screen, WM, MM, AM, TempBuffer );

		WMPtr->Add( SideWeapon );

		this->AddEntity(SideWeapon );
	}



}


void NetworkAvatar::Use( int function, Real Param1, Real Param2, Real Param3, Real FrameTime )
{
	if( function == AU_USE )
	{
		if(EquipmentList[EquipedItem])
			EquipmentList[EquipedItem]->Use( WF_FIRE1, NULL, NULL, NULL, WMPtr->GTPtr->GetFrameTime()  );
	}
	else if( function == AU_RELOAD )
	{
		if(EquipmentList[EquipedItem])
			EquipmentList[EquipedItem]->Use( WF_RELOAD, NULL, NULL, NULL, WMPtr->GTPtr->GetFrameTime()   );
	}

}




void NetworkAvatar::HandleNetworkMessage ( BYTE* Message )
{
	M3DPacketHeader	*Packet;
	Packet = (M3DPacketHeader*)Message;
	
	

	if( Packet->MessageType == MSG_MOVEENTITY )
	{
		

		//Get the current position and the target position
		M3DMoveEnt* MovPacket = (M3DMoveEnt*) Message;
		Vector3D v;
		Pos1 = Location;
		Pos2.x     = MovPacket->x;
		Pos2.y     = MovPacket->y;
		Pos2.z	   = MovPacket->z;
		Pos2.Yaw   = MovPacket->Yaw;
		Pos2.Pitch = MovPacket->Pitch;
		LerpVector.x = (Pos2.x - Pos1.x)  ;
		LerpVector.y = (Pos2.y - Pos1.y)  ;
		LerpVector.z = (Pos2.z - Pos1.z)  ;
		LerpDistance = LerpVector.Mag();
		StartTime = WMPtr->GTPtr->GetTime();
		time = 0;

		//set the movement such and such
		CurrentVelocity.x = MovPacket->vx;
		CurrentVelocity.y = MovPacket->vy;
		CurrentVelocity.z = MovPacket->vz;


						
		Lerping = true;
		
		if(LerpDistance > 50)
		{
			SetX( MovPacket->x );
			SetY( MovPacket->y );
			SetZ( MovPacket->z );
			Pos1 = Location;
		}

		LerpVector.x += MovPacket->vx ;
		LerpVector.y += MovPacket->vy ;
		LerpVector.z += MovPacket->vz;
		SetYaw ( MovPacket->Yaw );
		
				
		AdjustBoundingVolume();
	}
}



//////// Move /////////////////////////////////
//
// Description:
//
// Adjusts vectors and moves players by them. 
//
///////////////////////////////////////////////

void NetworkAvatar::Move( Real FrameTime, WorldManager* SM )
{
	//figure out how much time has elapsed in seconds
	double frametime = SM->GTPtr->GetFrameTime();
	time += frametime;

	if(time-StartTime > .1)
		time = .1;

	if(Lerping)
	{
		Location.x = Pos1.x + LerpVector.x*time;
		Location.y = Pos1.y + LerpVector.y*time;
		Location.z = Pos1.z + LerpVector.z*time;
	}

	Real LandHeight = SM->GetHeightValue( Location );
	if(Location.y < LandHeight)
		Location.y = LandHeight;
}



bool NetworkAvatar::Collision( Vector3D& point1, Vector3D& point2, CollisionInfo* Collision )
{
	//Declarations
	Vector3D Line[2];
	Line[0] = point1;
	Line[1] = point2;
	
	//Check for the collision
	if( MeshA.CheckForCollision( Location, Line, Collision ) )
	{
		//Figure out the force line
		Vector3D LineOfForce = Line[1]-Line[0];
		Damage( Line[0],  LineOfForce, Collision->CollisionPoint );

		return true;
	}

	return false;
}



//////// Update ///////////////////////////////
//
// Description:
//
// Lets avatar do any neccesary tasks for this
// frame.
//
///////////////////////////////////////////////

void NetworkAvatar::Update ( Real FrameTime, WorldManager* SM )
{
	//Update the player's position
	Move( FrameTime, SM);
	AdjustBoundingVolume();
	MeshA.Update( FrameTime );

	//Update the weapons
	if(EquipmentList[EquipedItem])
	{
		Position ItemLocation = Location;
		ItemLocation.y += MeshA.GetBox().max.y*.75f;
		EquipmentList[EquipedItem]->SetLocation(ItemLocation);
		EquipmentList[EquipedItem]->Update( FrameTime, SM );
	}
}



//////// Draw /////////////////////////////////
//
// Description:
//
// Draws the avatar, animations and such included.
//
///////////////////////////////////////////////

void NetworkAvatar::Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer )
{
	MeshA.Draw( Screen, Location.GetMatrix() );

	for(int i=0; i<EquipmentList.size(); i++)
	{
		if(EquipmentList[i])
			EquipmentList[i]->Draw( Screen, MM, Viewer, Timer );

	}

}

