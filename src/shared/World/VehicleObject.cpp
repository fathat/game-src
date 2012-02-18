#include "vehicleobject.h"
#include "Vehicle.h"
#include "..\ini\ini.h"
#include "..\collision\collision.h"
#include "..\cvar\cvar.h"
#include "World.h"
#include "..\physics\dynamicsobject.h"
#include "..\..\Game\systemobjects.h"
#include "enemy.h"


#include "..\defs\defs.h"

void VehicleObject::Init(Screen3D& Screen,  WorldManager* WM, MeshManager& MM,  SoundManager& AM,  char *ObjectFile)
{
	//Save some pointers we'll need periodically
	WMPtr = WM;
	MMPtr = &MM;
	
	//A collectible object? 
	Collectible = GetIniValue(ObjectFile, "properties", "collectible");

	//[HACK]This should probably be loaded from teh file, but I'm lazy
	Health = 0;

	//camera stuff
	char Buffer[512];
	CameraX = GetIniValueFloat( ObjectFile, "vehicle4wheeled", "camerax", Buffer ) ; 
	CameraY = GetIniValueFloat( ObjectFile, "vehicle4wheeled", "cameray", Buffer ) ; 
	CameraZ = GetIniValueFloat( ObjectFile, "vehicle4wheeled", "cameraz", Buffer ) ; 

	//EngineSound = AM.GetSound( "..\\sound\\engine.wav" );
	SndEngine = DeviceManager::OpenALSound.GetSoundBuffer( "..\\sound\\engine.wav" );
	SndHit = DeviceManager::OpenALSound.GetSoundBuffer( "..\\sound\\hithard.wav" );

	//setup the construction
	Construction* c = new Vehicle4Wheeled;
	c->Construct( ObjectFile, &WM->Solver, Screen, MM, Location, this );

	hConstruct = WM->Solver.RegisterConstruction(c);
}


void VehicleObject::Kill()
{
	//something!
}



void VehicleObject::Move( Real FrameTime, WorldManager* SM )
{
	
	
}
void VehicleObject::OnCollide( ICollisionHandler* collisionHandler )
{
	Construction* c = WMPtr->Solver.GetConstruction( hConstruct );
	c->Activate ();
}

void VehicleObject::OnFocusLost()
{

	//put player off to side of vehicle
	Vector3D location;

	Construction* c = WMPtr->Solver.GetConstruction( hConstruct );
	c->GetLocation( location );
	WMPtr->LocalPlayer.SetPosition( location.x, location.y+15, location.z );
	WMPtr->LocalPlayer.SetVelocity( 0, 0, 0 );
	

	LostFocus = true;

	CVar.WriteLine( "OnFocusLost called" );
	
}

void VehicleObject::HandleNetworkMessage( BYTE* Message )
{
}


float VehicleObject::GetSpeed()
{
	Vehicle4Wheeled *c;
	c = (Vehicle4Wheeled *)WMPtr->Solver.GetConstruction(hConstruct );
	return c->GetCurrentSpeed();
}

void VehicleObject::Update( Real FrameTime, WorldManager* SM )
{
	Vehicle4Wheeled *c;
	c = (Vehicle4Wheeled *)WMPtr->Solver.GetConstruction(hConstruct );
	const dReal* v = dBodyGetPosition( c->GetSubObject(4)->Body );
	
	
	SetX(v[0] );
	SetY(v[1] );
	SetZ(v[2] );


	Vector3D cv;
	c->GetVelocity(cv);

	SceneObject*  so = WMPtr->FindNearestObject( Location, 35, this ); 
	AnimatedObject* o = dynamic_cast<AnimatedObject*>( so );
	
	if( o && o != &WMPtr->LocalPlayer && c->GetCurrentSpeed() > 10 )
	{
		if( o->GetHealth() > 0 ) 
		{
			o->Damage( Location.GetVector(), cv*2.0f, o->GetLocation().GetVector() );
			
			//Vector3D backForce = cv/2.0f;
			c->ApplyForce( cv * -3.0f );
			DeviceManager::OpenALSound.PlaySoundImmediate( SndHit, o->GetLocation().AsSoundSource() );
		}

	}


	//move the avatar with the vehicle such and such
	
	engineSound.UpdateLocation( Location.AsSoundSource() );
	float soundRPM = fmod( c->GetCurrentSpeed(), 35.0f );
	float pitch = 0.4f+ soundRPM / 35.0f + (((int)c->GetCurrentSpeed())/35);
	engineSound.SetPitch( pitch /2.0f);
	


	if(SM->InputFocus == this && LostFocus == false )
	{	
	
		//set some camera stuff
		Matrix VehicleMatrix, OffsetMatrix,Matrix;
		SM->Solver.GetTransform( c->GetSubObject(4)->Geom, &VehicleMatrix );
		SM->Solver.GetRotationMatrix( c->GetSubObject(4)->Geom, &SkyMatrix );
		OffsetMatrix.Translate( CameraX, CameraY, CameraZ );
		CameraMatrix = OffsetMatrix* VehicleMatrix ;

		
		
		
		D3DXVECTOR4 locationVector;
		D3DXVECTOR3 baseVector;
		baseVector.x = 0;
		baseVector.y = 0;
		baseVector.z = 0;

		SM->Viewer.Follow( CameraMatrix, SkyMatrix );
		SM->Viewer.CS = CS_FOLLOW_MATRIX;
		
		D3DXVec3Transform( &locationVector, &baseVector, &CameraMatrix.Mat );

		//get avatar construction and set him
		SM->LocalPlayer.SetPosition( v[0], v[1]+8.0f, v[2] );
		/*
	

		Construction *ac = SM->Solver.GetConstruction( SM->LocalPlayer.hConstruction);
		DynamicsObject* d = ac->GetSubObject(0);
		if(d)
		{
			//dBodyDisable( d->Body );
			dBodySetPosition( d->Body, v[0], v[1]+5.0f, v[2]  );
		}*/
	}
	LostFocus = false;

}

void VehicleObject::SetVelocity(Real x, Real y, Real z)
{	

}

void VehicleObject::SetDestination( Position& NewPosition)
{
}

void VehicleObject::AdjustBoundingVolume()
{
	
}


void VehicleObject::GetExtents( AABB& ExtBox )
{
	ExtBox = Box;
}

Real VehicleObject::GetHeight( )
{
	return Box.max.y;
}


bool VehicleObject::CheckVisibility( Screen3D& Screen, bool RecordTime )
{
	return true;
}

bool VehicleObject::Collision( SceneObject* sceneobject )
{
	return false;
}


bool VehicleObject::Collision( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision )
{
	Matrix M;
	
	Construction *c;
	c = WMPtr->Solver.GetConstruction(hConstruct );
	
	//do some more stuff here
	

	return false;
}

bool VehicleObject::SphereCollision( const Vector3D& point1, Real radius, CollisionInfo* Collision )
{
	return false;
}


void VehicleObject::Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer )
{
	//Solver handles the drawing of this
}

void VehicleObject::Use( int function, Real Param1, Real Param2, Real Param3, Real FrameTime ) 
{
	
	StealInputFocus(); 

	if( WMPtr->InputFocus == this )
	{
		engineSound = DeviceManager::OpenALSound.PlayPersistentSound( SndEngine, Location.AsSoundSource() );
	}
	else
	{
		engineSound.Stop();
	}
	
}


void VehicleObject::HandleInput(  Real FrameTime, POINT MouseDelta, UCHAR* KeyBuffer )
{
	Vehicle4Wheeled *c;
	c = (Vehicle4Wheeled *)WMPtr->Solver.GetConstruction(hConstruct );
	WMPtr->LocalPlayer.SetYaw( WMPtr->Viewer.Yaw );

	if(  HIWORD(GetKeyState('S')) )
		c->Accelerate( -100.0f);
	else if(  HIWORD(GetKeyState('W') ))
		c->Accelerate( 100.0f);
	else
		c->Accelerate( 0);

	if(  HIWORD(GetKeyState('D')) )
		c->Turn( 1600.0f, -.75);
	else if(  HIWORD(GetKeyState('A') ))
		c->Turn( 1600.0f, .75);
	else
		c->Turn( 1600.0f, 0 );


	if(  HIWORD(GetKeyState('R')) )
		c->SetGear( -1 );
	if(  HIWORD(GetKeyState('Q')) )
		c->SetGear( 0 );

	if(  HIWORD(GetKeyState('1')) )
		c->SetGear( 1 );
	if(  HIWORD(GetKeyState('2')) )
		c->SetGear( 2 );
	if(  HIWORD(GetKeyState('3')) )
		c->SetGear( 3 );
	if(  HIWORD(GetKeyState('4')) )
		c->SetGear( 4 );

	if( HIWORD(GetKeyState(VK_LBUTTON)) )
	{
		//WMPtr->AMPtr->Play( EngineSound );
		c->SetClutch( true );
	}
	else
		c->SetClutch( false );

	if( HIWORD( GetKeyState ( VK_SPACE)))
		c->SetHandBrake( true);
	else
		c->SetHandBrake( false );
		
		

	if(  HIWORD(GetKeyState('U')) )
		c->Accelerate( 0, -500, 50);
	
}

void VehicleObject::DrawInfo( Screen3D& Screen )
{
	
}

void VehicleObject::DrawHUD( Screen3D& Screen, MeshManager& MM, FontManager& FM, Camera& Viewer )
{
	//Draw RPM meter and such
	Font2D* f = FM.GetFont( "Impact", 18 );
	 
	Vehicle4Wheeled *c;
	c = (Vehicle4Wheeled *)WMPtr->Solver.GetConstruction(hConstruct );


	/*f->Draw( Screen, 0, Screen.GetHeight() - 48, false, D3DCOLOR_XRGB(0, 64, 255), "RPM: %d", (int)c->GetRPM());
	f->Draw( Screen, 0, Screen.GetHeight() - 72, false, D3DCOLOR_XRGB(0, 64, 255), "Gear: %d", (int)c->GetGear());
	f->Draw( Screen, 0, Screen.GetHeight() - 96, false, D3DCOLOR_XRGB(0, 64, 255), "Speed: %d", (int) c->GetCurrentSpeed() );
	f->Draw( Screen, 0, Screen.GetHeight() - 120, false, D3DCOLOR_XRGB(0, 64, 255), "Brake: %d", (int) c->GetHandBrake() );*/
}

