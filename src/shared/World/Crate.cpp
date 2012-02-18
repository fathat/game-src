#include "crate.h"
#include "boxconstruct.h"
#include "..\ini\ini.h"
#include "..\collision\collision.h"
#include "World.h"


#include "..\defs\defs.h"

void Crate::Init(Screen3D& Screen,  WorldManager* WM, MeshManager& MM,  SoundManager& AM,  char *ObjectFile)
{

	//Load in the object's mesh
	char TempBuffer[255];
	GetIniValue(ObjectFile, "box", "mesh", TempBuffer);
	hMesh = MM.GetMeshHandle( TempBuffer, true );

	//construct the structure from the mesh
	BoxConstruction* boxConstruction = new BoxConstruction();
	Vector3D l = Location;
	boxConstruction->Construct( ObjectFile, &WM->Solver, Screen, MM, Location );
	hConstruct = WM->Solver.RegisterConstruction( boxConstruction );
	

	//Save some pointers we'll need periodically
	WMPtr = WM;
	MMPtr = &MM;
		
	//A collectible object?
	Collectible = GetIniValue(ObjectFile, "properties", "collectible");

	//[HACK]This should probably be loaded from teh file, but I'm lazy
	Health = 2000;
	
	//readjust bounding box
	AdjustBoundingVolume();
}


void Crate::Kill()
{

}




void Crate::Move( Real FrameTime, WorldManager* WMPtr )
{

	//get the box location blah blah such and such
	BoxConstruction* c = (BoxConstruction*)WMPtr->Solver.GetConstruction( hConstruct);
	Vector3D v;
	c->GetLocation(v);
	Location = v;

	//readjust bounding box
	AdjustBoundingVolume();
}

void Crate::HandleNetworkMessage( BYTE* Message )
{
}

void Crate::Update( Real FrameTime, WorldManager* SM )
{
	Move( FrameTime, SM );
	
}

void Crate::SetVelocity(Real x, Real y, Real z)
{	
}

void Crate::SetDestination( Position& NewPosition)
{
}

void Crate::AdjustBoundingVolume()
{
	//readjust bounding sphere
	Mesh* ObjectMesh = MMPtr->Get( hMesh );
	if(!ObjectMesh)
		return;

	
	Sphere = ObjectMesh->MakeBoundingSphere( Location.GetMatrix() );
	//Sphere.location = Location;
	//Sphere.Radius = ObjectMesh->FarthestPoint+4 ;


}


void Crate::GetExtents( AABB& ExtBox )
{
	ExtBox = Box;
}

Real Crate::GetHeight( )
{
	return Box.max.y;
}


bool Crate::CheckVisibility( Screen3D& Screen, bool RecordTime )
{
	return false;
}

bool Crate::Collision( SceneObject* sceneobject )
{
	return false;
}


bool Crate::Collision( Vector3D& point1, Vector3D& point2, CollisionInfo* Collision )
{
	
	return false;

}

bool Crate::SphereCollision( Vector3D& point1, Real radius, CollisionInfo* Collision )
{
	return false;
}


void Crate::Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer )
{
	//construction draws itself		
}

void Crate::OnCollide ( ICollisionHandler* objectHit )
{
	Construction* c = WMPtr->Solver.GetConstruction( hConstruct );
	c->Activate();
}

void Crate::Use( int function, Real Param1, Real Param2, Real Param3, Real FrameTime ) 
{

}

void Crate::DrawInfo( Screen3D& Screen )
{

}

void Crate::DrawHUD( Screen3D& Screen, MeshManager& MM, FontManager& FM, Camera& Viewer )
{

}

