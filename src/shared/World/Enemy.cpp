#include "enemy.h"
#include "world.h"
#include "Weapon.h"
#include "VehicleObject.h"
#include "..\..\Game\SystemObjects.h"

#include "..\defs\defs.h"



void Enemy::Init( Screen3D& Screen, WorldManager* WM, MeshManager& MM, SoundManager& AM,  char* ObjectFile )
{
	Character::Init( Screen, WM, MM, AM, ObjectFile );

	InitialHealth = Health;
	AttackDistance = GetIniValueFloat( ObjectFile, "properties",  "attack_distance" );
	EnemySpeed	   = GetIniValueFloat( ObjectFile, "properties",  "speed" );
	RunDistance    = GetIniValueFloat( ObjectFile, "properties",  "run_distance" );
	HearingDistance    = GetIniValueFloat( ObjectFile, "properties",  "hearing_distance" );

	char buffer[256];
	GetIniValueFloat( ObjectFile, "properties",  "blood", buffer );
	BloodParticles.Init( Screen, buffer, WM );

	Alerted = false; EState = ES_IDLE; ObjectType = OT_DYNAMIC;


	GetIniValue( ObjectFile, "properties",  "attack_yell", buffer );
	AttackSound = DeviceManager::OpenALSound.GetSoundBuffer( buffer );

	DeathTime = 0;

}

void Enemy::CheckForStateTransition(Real FrameTime)
{
	//Set up a vector to face the player.
	Vector3D FollowVector;
	FollowVector.x = WMPtr->LocalPlayer.GetLocation().x-Location.x;
	FollowVector.y = CurrentVelocity.y;
	FollowVector.z = WMPtr->LocalPlayer.GetLocation().z-Location.z;


	if(DeathTime > 90 && Health <= 0)
	{
		ChangeState("idle");
		EState = ES_IDLE;
		DeathTime = 0;
		Health = InitialHealth/2; 
		Alerted = false;

	}

	//figure out if the player is even nearby enough
	Real PlayerDistance = FollowVector.Mag();
	
	if( Alerted && Health > 0  && PlayerDistance <= AttackDistance && EState != ES_SHOOTING )
	{
		ChangeState("shooting");
		EState = ES_SHOOTING;
	}

	//Chase the player if they're far away
	if( PlayerDistance > RunDistance && Alerted && Health > 0 )
	{
		ChangeState("run1");
		EState = ES_RUNNING;
	}

	
}

void Enemy::AvoidObstacles()
{
	SceneObject* object = WMPtr->FindNearestObject( Location, 10, this );
	
	AnimatedObject* c = dynamic_cast<AnimatedObject*> (object);
	VehicleObject* vo = dynamic_cast< VehicleObject* > (object);

	if( (c && c != this) || (vo && vo->GetSpeed() < 20) )
	{
		Vector3D directionVector = Location.GetVector() - object->GetLocation().GetVector() ;

		if( directionVector.Mag() < 10 )
		{
			directionVector.Normalize();
			directionVector.SetLength( 10 );
			Location.SetLocation( object->GetLocation().GetVector() + directionVector ); 
			
			
		}
	}
	

	
	
}

void Enemy::Think ( Real FrameTime, WorldManager* SM )
{
	CheckForStateTransition(FrameTime);

	if( Health <= 0 )
		DeathTime += FrameTime;

	if(Health <= 0 || Thinking == false)
	{
		Real ty = TargetVelocity.y;
		TargetVelocity.Normalize();
		TargetVelocity.y = ty;

		return;
	}
	
	if(SM->InWater( Location ))
	{
		Health -= SM->WaterDamage*FrameTime;
		Location.y = SM->GetHeightValue( Location, NULL, NULL, NULL, NULL, true );

	}

	//Set up a vector to face the player.
	Vector3D FollowVector;
	FollowVector.x = SM->LocalPlayer.GetLocation().x-Location.x;
	FollowVector.y = CurrentVelocity.y;
	FollowVector.z = SM->LocalPlayer.GetLocation().z-Location.z;

	
	//figure out if the player is even nearby enough
	Real PlayerDistance = FollowVector.Mag();
	
		

	//if(Location.Yaw > RADIAN (360) )
		Location.Yaw = fmod( Location.Yaw, RADIAN(360) );

	//Figure out some angles between the player and monster and something
	//something.
	Real Angle = (float)atan2(FollowVector.x,FollowVector.z);
	Real ViewAngle = Angle - Location.Yaw;

	/*if(PlayerDistance < SightDistance )
	{
		//If the player is in view and close enough we make the enemy "alerted"
		if( fabs( ViewAngle ) < RADIAN(45 ) || fabs( ViewAngle ) > RADIAN(315 ))
			Alerted = true;
	}
	else
	{
		Alerted = false;
	}*/

	if(PlayerDistance > HearingDistance )
	{
		Alerted = false; 
	}
	if( PlayerDistance < HearingDistance && Alerted == false )
	{
		Alerted = true;
	}

	if( PlayerDistance < AttackDistance  && HasYelled == false)
	{
		DeviceManager::OpenALSound.PlaySoundImmediate( AttackSound, Location.AsSoundSource() );
		HasYelled = true ;
	}

	//Face the player if alerted
	if(Alerted )
		Location.Yaw = Angle;

	//Chase the player if they're far away
	if(EState == ES_RUNNING)
	{
		FollowVector.Normalize();
		FollowVector *= EnemySpeed;
		TargetVelocity.x = FollowVector.x;
		TargetVelocity.z = FollowVector.z;
	}
	
	//Otherwise, shoot at the player
	if( EState == ES_SHOOTING )
	{
		if(fabs((float)TargetVelocity.x) > 0 || fabs((float)TargetVelocity.z) > 0)
		{
			Real ty = TargetVelocity.y;
			TargetVelocity.Normalize();
			TargetVelocity.y = ty;
		}

		FollowVector.Normalize();

		//Move the weapon into position

		//[HACK] if it's not a weapon bad things will happenx0r!!11
		Weapon* MyWeapon = (Weapon*)EquipmentList[EquipedItem];
		MyWeapon->SetHand ( false );
		Position WeaponLocation = Location;
		WeaponLocation.y += MeshA.GetBox().max.y*.75f;
		MyWeapon->SetLocation( WeaponLocation );


		Vector3D FireVector;
		FireVector.x = Location.x-SM->LocalPlayer.GetLocation().x;
		FireVector.y = Location.y-(SM->LocalPlayer.GetLocation().y-1);
		FireVector.z = Location.z-SM->LocalPlayer.GetLocation().z;
		FireVector *= -1;
		FireVector.Normalize();
		
		MyWeapon->SetFireVector( FireVector );
		MyWeapon->Update( FrameTime, this->WMPtr );
		
		//actually shoot the weapon
		Use(0, 0, 0,0, FrameTime );
	}
}



//////// Damage ///////////////////////////////
//
// Description:
//
// Notifies enemy of a force acting upon him/her
//
///////////////////////////////////////////////

void Enemy::OnDamage (const Vector3D& source, const Vector3D& collisionPoint, float damageAmount ) 
{
	Health -= damageAmount;
	
	if(Health > 0)
		ChangeState( "injured" );

	BloodParticles.MoveSpawn( collisionPoint.x, collisionPoint.y, collisionPoint.z );
	BloodParticles.SpawnParticles( 100, false );

	
	Alerted = true;
}



//////// Update ///////////////////////////////
//
// Description:
//
// Lets avatar do any neccesary tasks for this
// frame.
//
///////////////////////////////////////////////

void Enemy::Update ( Real FrameTime, WorldManager* SM )
{
	//Update the enemies's position
	Think( FrameTime, SM );
	Move( FrameTime, SM);
	AvoidObstacles();
	AdjustBoundingVolume();

	if( TargetVelocity.Mag() >= 1 )
	{
		MeshA.SetTimeMultiplier( 3 );
	}
	else
	{
		MeshA.SetTimeMultiplier( 1.0f );
	}


	MeshA.Update( FrameTime );
	BloodParticles.Update(FrameTime);

	if(Health<= 0)
	{
		TargetVelocity.x=0;
		TargetVelocity.z=0;

		ChangeState("dead");
	}

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


void Enemy::Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer )
{
	Character::Draw( Screen, MM, Viewer, Timer );
	WMPtr->AddStream( &BloodParticles );
	//BloodParticles.Draw( Screen, Viewer );
}
