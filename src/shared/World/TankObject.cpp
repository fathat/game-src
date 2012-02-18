#include "tankobject.h"
#include "tankconstruct.h"
#include "..\ini\ini.h"
#include "..\collision\collision.h"
#include "World.h"
#include "..\CVar\CVar.h"


#include "..\defs\defs.h"

void TankObject::Init(Screen3D& Screen,  WorldManager* WM, MeshManager& MM,  SoundManager& AM,  char *ObjectFile)
{
	//Save some pointers we'll need periodically
	WMPtr = WM;
	MMPtr = &MM;
	
	//A collectible object? 
	Collectible = GetIniValue(ObjectFile, "properties", "collectible");

	//[HACK]This should probably be loaded from teh file, but I'm lazy
	Health = 0;

	//setup the construction
	Construction* c = new TankConstruct;
	c->Construct( ObjectFile, &WM->Solver, Screen, MM, Location, this );
	hConstruct = WM->Solver.RegisterConstruction(c);

	//initialize the main gun
	MainGunParticles.Init( Screen, "..\\base\\art\\particles\\rocket.ini", WM );

	LastShotTime =0;
}


void TankObject::Kill()
{
	//something!
}



void TankObject::Move( Real FrameTime, WorldManager* SM )
{
	
	
}

void TankObject::HandleNetworkMessage( BYTE* Message )
{
}

//Function to respond to OnCollide
void TankObject::OnCollide( ICollisionHandler* otherObject )
{
	Construction* c = WMPtr->Solver.GetConstruction( hConstruct );
	c->Activate();
	//c->Disable();
	//c->DisableAllGeoms();
}

void TankObject::OnFocusLost()
{
	//put player off to side of vehicle
	Vector3D location;

	Construction* c = WMPtr->Solver.GetConstruction( hConstruct );
	c->GetLocation( location );
	WMPtr->LocalPlayer.SetPosition( location.x, location.y+15, location.z );
	WMPtr->LocalPlayer.SetVelocity( 0, 0, 0 );
	

	CVar.WriteLine( "OnFocusLost called" );
}

void TankObject::Update( Real FrameTime, WorldManager* SM )
{
	TankConstruct* c = (TankConstruct* )SM->Solver.GetConstruction( hConstruct );
	const dReal* v = dBodyGetPosition( c->GetSubObject(0)->Body );
	
	
	SetX(v[0] );
	SetY(v[1] );
	SetZ(v[2] );

	//move the avatar with the vehicle such and such

	if(SM->InputFocus == this )
	{	

			

		//set some camera stuff
		Matrix VehicleMatrix, OffsetMatrix,Matrix, GunMatrix, FinalGunMatrix;
		SM->Solver.GetTransform( c->GetSubObject(c->GetTurretIndex()+1)->Body, &VehicleMatrix );
		SM->Solver.GetRotationMatrix( c->GetSubObject(c->GetTurretIndex()+1)->Body, &SkyMatrix );
		OffsetMatrix.Translate(-2, 0.5f, 0 );
		GunMatrix.Translate(0, 0, 8 );
		CameraMatrix = OffsetMatrix* VehicleMatrix ;

		SM->Viewer.Yaw   = 0;
		SM->Viewer.Pitch = 0;
		
		
		
		D3DXVECTOR4 locationVector;
		D3DXVECTOR3 baseVector;
		baseVector.x = 0;
		baseVector.y = 0;
		baseVector.z = 0;

		SM->Viewer.Follow( CameraMatrix, SkyMatrix );
		SM->Viewer.CS = CS_FOLLOW_MATRIX;
		
		D3DXVec3Transform( &locationVector, &baseVector, &CameraMatrix.Mat );

		//set the particle stream location and direction
		D3DXVECTOR3 StreamLocation, StreamDirection;
		D3DXVECTOR4 ResultLocation;
		StreamLocation.x=0; StreamLocation.y=0; StreamLocation.z=0;
		StreamDirection.x=0;StreamDirection.y=0;StreamDirection.z=1;
        
		D3DXVec3Transform( &ResultLocation, &StreamLocation, &VehicleMatrix.Mat );
		MainGunParticles.x = ResultLocation.x;
		MainGunParticles.y = ResultLocation.y;
		MainGunParticles.z = ResultLocation.z;

		D3DXVec3Transform( &ResultLocation, &StreamDirection, &SkyMatrix.Mat );
		MainGunParticles.StreamHeading.x = ResultLocation.x*500;
		MainGunParticles.StreamHeading.y = ResultLocation.y*500;
		MainGunParticles.StreamHeading.z = ResultLocation.z*500;
		MainGunParticles.x += ResultLocation.x * c->GetGunZOffset();
		MainGunParticles.y += ResultLocation.y * c->GetGunZOffset();
		MainGunParticles.z += ResultLocation.z * c->GetGunZOffset();
		MainGunParticles.UseHeading = true;
		

		//get avatar construction and set him right

		Construction *ac = SM->Solver.GetConstruction( SM->LocalPlayer.hConstruction);
		DynamicsObject* d = ac->GetSubObject(0);
		if(d)
		{
				dBodySetPosition( d->Body, v[0], v[1]+10.0f, v[2]  );
		}
	}

	MainGunParticles.Update(FrameTime );
	

}

void TankObject::SetVelocity(Real x, Real y, Real z)
{	

}

void TankObject::SetDestination( Position& NewPosition)
{
}

void TankObject::AdjustBoundingVolume()
{
	
}


void TankObject::GetExtents( AABB& ExtBox )
{
	ExtBox = Box;
}

Real TankObject::GetHeight( )
{
	return Box.max.y;
}


bool TankObject::CheckVisibility( Screen3D& Screen, bool RecordTime )
{
	return true;
}

bool TankObject::Collision( SceneObject* sceneobject )
{
	return false;
}


bool TankObject::Collision( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision )
{
	Matrix M;
	
	Construction *c;
	c = WMPtr->Solver.GetConstruction(hConstruct );
	
	//do some more stuff here
	

	return false;
}

bool TankObject::SphereCollision( const Vector3D& point1, Real radius, CollisionInfo* Collision )
{
	return false;
}


void TankObject::Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer )
{
	//Solver handles the drawing of the construction

	WMPtr->AddStream( &MainGunParticles);
	

}

void TankObject::Use( int function, Real Param1, Real Param2, Real Param3, Real FrameTime ) 
{
	StealInputFocus(); 
}

void TankObject::HandleInput(  Real FrameTime, POINT MouseDelta, UCHAR* KeyBuffer )
{
	TankConstruct *c;
	c = (TankConstruct *)WMPtr->Solver.GetConstruction(hConstruct );
	WMPtr->LocalPlayer.SetYaw( WMPtr->Viewer.Yaw );

	Real SideForce=0.0f, Throttle=0.0f;

	if(  HIWORD(GetKeyState('S')) )
		Throttle = -100.0f;
	else if(  HIWORD(GetKeyState('W') ))
		Throttle =  100.0f;
	

	if(  HIWORD(GetKeyState('D')) )
		SideForce = 100.0f;
	else if(  HIWORD(GetKeyState('A') ))
		SideForce = -100.0f;

	if( HIWORD(GetKeyState(VK_LBUTTON)))
	{
		if(timeGetTime() - LastShotTime > 1500 )
		{
			MainGunParticles.SpawnParticles( 1, false );
			LastShotTime = timeGetTime();

			//add recoil
			dBodyAddRelForce( c->GetSubObject(c->GetTurretIndex()+1)->Body, 0, 0, -25000 );
		}
	}
	

	c->Turn( MouseDelta.x, MouseDelta.y );

	c->Accelerate( Throttle, SideForce );
	
	
}

void TankObject::DrawInfo( Screen3D& Screen )
{
	
}

void TankObject::DrawHUD( Screen3D& Screen, MeshManager& MM, FontManager& FM, Camera& Viewer )
{
	//Draw RPM meter and such
	Font2D* f = FM.GetFont( "Impact", 18 );
	
	TankConstruct *c;
	c = (TankConstruct *)WMPtr->Solver.GetConstruction(hConstruct );
	
	f->Draw( Screen, 0, Screen.GetHeight() - 96, false, D3DCOLOR_XRGB(0, 128, 255), "Speed: %d", (int) c->GetCurrentSpeed() );
}

