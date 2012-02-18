#include "animatedobject.h"
#include "..\ini\ini.h"
#include "..\collision\collision.h"
#include "World.h"

#include "..\defs\defs.h"

void AnimatedObject::Init(Screen3D& Screen,  WorldManager* WM, MeshManager& MM,  SoundManager& AM,  char *ObjectFile)
{

	//Load in the object's mesh
	char TempBuffer[255];
	char TempBufferA[255];
	GetIniValue(ObjectFile, "graphics", "animation_file", TempBuffer);
	MeshA.Init( Screen, MM, TempBuffer );

	//Load object's icon
	GetIniValue(ObjectFile, "graphics", "icon", TempBuffer);
	GetIniValue(ObjectFile, "graphics", "icon_a", TempBufferA);
	Icon = Screen.GetTextureHandle( TempBuffer, TempBufferA );
	
	//Save some pointers we'll need periodically
	WMPtr = WM;
	MMPtr = &MM;

	//Location
	Location.x = 0;
	Location.y = 0;
	Location.z = 0;
	UseDestination = false;
	
	//A collectible object?
	Collectible = GetIniValue(ObjectFile, "properties", "collectible");

	
	Health = GetIniValue(ObjectFile, "properties", "health");


	//Load in any light emission stuff this object has.
	if( GetIniValue( ObjectFile, "light", "lighted") )
	{
		LightSource.r = (float)GetIniValueFloat( ObjectFile, "light", "red", TempBuffer);
		LightSource.g = (float)GetIniValueFloat( ObjectFile, "light", "green", TempBuffer);
		LightSource.b = (float)GetIniValueFloat( ObjectFile, "light", "blue", TempBuffer);
		LightSource.Radius = (float)GetIniValueFloat( ObjectFile, "light", "radius", TempBuffer);
		LightSource.x = 0;
		LightSource.y = 0;
		LightSource.z = 0;

		WM->LM.RegisterLight( &LightSource );
	}

	//readjust bounding box
	AdjustBoundingVolume();
}


void AnimatedObject::Kill()
{
	DropAll();
}



void AnimatedObject::Move( Real FrameTime, WorldManager* SM )
{
	//readjust bounding box
	AdjustBoundingVolume();
}

void AnimatedObject::HandleNetworkMessage( BYTE* Message )
{
}

void AnimatedObject::Update( Real FrameTime, WorldManager* SM )
{
	Move( FrameTime, SM );
}

void AnimatedObject::SetVelocity(Real x, Real y, Real z)
{
	TargetVelocity.x = x;
	TargetVelocity.y = y;
	TargetVelocity.z = z;
	CurrentVelocity.x = x;
	CurrentVelocity.y = y;
	CurrentVelocity.z = z;
	UseDestination = false;
}

void AnimatedObject::SetDestination( Position& NewPosition)
{
	Destination = NewPosition;
	UseDestination = true;
}

void AnimatedObject::AdjustBoundingVolume()
{
	//readjust bounding box
	Box.min.x = Location.x + MeshA.GetBox().min.x;
	Box.min.y = Location.y + MeshA.GetBox().min.y ;
	Box.min.z = Location.z + MeshA.GetBox().min.z;
	Box.max.x = Location.x + MeshA.GetBox().max.x;
	Box.max.y = Location.y + MeshA.GetBox().max.y;
	Box.max.z = Location.z + MeshA.GetBox().max.z;

	Sphere.x (Location.x);
	Sphere.y ( Location.y+ MeshA.GetBox().max.y/2);
	Sphere.z ( Location.z);

	Sphere.Radius = MeshA.GetSphereRadius()+4 ;

	
	LightSource.x = Location.x;
	LightSource.y = Location.y;
	LightSource.z = Location.z;

	

}


void AnimatedObject::GetExtents( AABB& ExtBox )
{
	ExtBox = Box;
}

Real AnimatedObject::GetHeight( )
{
	return MeshA.GetBox().max.y;
}


bool AnimatedObject::CheckVisibility( Screen3D& Screen, bool RecordTime )
{
	/*if(RecordTime)
	{
		if( LastDrawn == WMPtr->GTPtr->GetFrameStart() )
			return false;

		LastDrawn = WMPtr->GTPtr->GetFrameStart();
	}*/

	if(MeshA.CheckVisibility( Screen, Location  ))
		return true;

	for(unsigned int i=0; i<EquipmentList.size(); i++)
	{
		if(EquipmentList[i]->CheckVisibility( Screen, RecordTime ) )
			return true;
	}
	
	return false;
}

bool AnimatedObject::Collision( SceneObject* sceneobject )
{
	return false;
}


bool AnimatedObject::Collision( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision )
{
	Vector3D Line[2];
	Line[0] = point1;
	Line[1] = point2;
	
	if( MeshA.CheckForCollision( Location, Line, Collision ) )
	{
		Vector3D LineOfForce = Line[1]-Line[0];
			
		Damage( Line[0],  LineOfForce, Collision->CollisionPoint );
		return true;
	}
	

	return false;

}

bool AnimatedObject::SphereCollision( const Vector3D& point1, Real radius, CollisionInfo* Collision )
{
	return false;
}


void AnimatedObject::Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer )
{
	//set the transformation
	Position DrawLocation;
	DrawLocation = Location;
	DrawLocation.Pitch = 0;

	MeshA.Draw( Screen, DrawLocation.GetMatrix() );
}

void AnimatedObject::DrawInfo( Screen3D& Screen )
{
	
}

void AnimatedObject::DrawHUD( Screen3D& Screen, MeshManager& MM, FontManager& FM, Camera& Viewer )
{

}

