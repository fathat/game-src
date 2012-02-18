#include "staticobject.h"
#include "..\ini\ini.h"
#include "..\collision\collision.h"
#include "World.h"
#include "..\physics\Construction.h"
#include "..\physics\Structure.h"


#include "..\defs\defs.h"


StaticObject::StaticObject() : SceneObject()
{ 
	hMesh = -1; 
	ID = -1; 
	InTree = false; 
	Parent = NULL; 
	Collectible=false; 
	MMPtr = NULL; 
	Stream = NULL; 
	StreamOffset = 0; 
	hStructure=-1; 
	ObjectType = OT_STATIC;
	NoCollision = false;
	hImposter = -1;
}

StaticObject::~StaticObject() 
{ 
	Kill(); 
}

void StaticObject::Init(Screen3D& Screen,  WorldManager* WM, MeshManager& MM,  SoundManager& AM,  char *ObjectFile)
{


	//Save some pointers we'll need periodically
	WMPtr = WM;
	MMPtr = &MM;


	//Load in the object's mesh
	char TempBuffer[255];
	GetIniValue(ObjectFile, "graphics", "object", TempBuffer);
	hMesh = MM.GetMeshHandle( TempBuffer, true );

	//hImposter = WMPtr->GetImposterManager()->GetImposterHandle( hMesh );

	//load some properties	
	Collectible = GetIniValue( ObjectFile, "properties", "collectible" );
	NoCollision = GetIniValue( ObjectFile, "properties", "noCollision" );

	//construct the structure from the mesh
	if(!NoCollision)
	{
		Structure* staticStructure = new Structure();
		Vector3D l = Location;
		staticStructure->Init( hMesh, &MM, &WM->Solver, l, Location.Yaw, Location.Pitch, Location.Roll );
		hStructure = WM->Solver.RegisterStructure( staticStructure );
	}
	

	

	//[HACK]This should probably be loaded from the file, but I'm lazy
	Health = GetIniValue(ObjectFile, "properties", "health");


	//Don't worry about the particle stream right now

	GetIniValue(ObjectFile, "graphics", "particle_stream", TempBuffer);
	if(strcmpi(TempBuffer, "nostring") != 0)
	{
		Stream = new ParticleStream;
		Stream->Init(Screen, TempBuffer, WMPtr);
		StreamOffset = GetIniValue( ObjectFile, "graphics", "particle_y_offset");
		Stream->MoveSpawn( Location.x, Location.y+StreamOffset, Location.z);
	}


	//Load in any light emission stuff this object has.
	if( GetIniValue( ObjectFile, "light", "lighted") )
	{
		LightSource.r = GetIniValueFloat( ObjectFile, "light", "red", TempBuffer);
		LightSource.g = GetIniValueFloat( ObjectFile, "light", "green", TempBuffer);
		LightSource.b = GetIniValueFloat( ObjectFile, "light", "blue", TempBuffer);
		LightSource.Radius = GetIniValueFloat( ObjectFile, "light", "radius", TempBuffer);
		LightSource.x = 0;
		LightSource.y = 0;
		LightSource.z = 0;

		WM->LM.RegisterLight( &LightSource );
	}

	//readjust bounding box
	AdjustBoundingVolume();
}


void StaticObject::Kill()
{

}




void StaticObject::Move( Real FrameTime, WorldManager* SM )
{
	if(Stream)
		Stream->MoveSpawn( Location.x, Location.y, Location.z);
	//readjust bounding box
	AdjustBoundingVolume();
}

void StaticObject::HandleNetworkMessage( BYTE* Message )
{
}

void StaticObject::Update( Real FrameTime, WorldManager* SM )
{
	Move( FrameTime, SM );
	if(Stream)
		Stream->Update( FrameTime );
}

void StaticObject::SetVelocity(Real x, Real y, Real z)
{	
}

void StaticObject::SetDestination( Position& NewPosition)
{
}

void StaticObject::AdjustBoundingVolume()
{
	//readjust bounding box
	Mesh* ObjectMesh = MMPtr->Get( hMesh );
	if(!ObjectMesh)
		return;


	if(Stream)
		Stream->MoveSpawn(Location.x, Location.y+StreamOffset, Location.z);

	LightSource.x = Location.x;
	LightSource.y = Location.y;
	LightSource.z = Location.z;

	Box.min.x = Location.x + ObjectMesh->Box.min.x;
	Box.min.y = Location.y + ObjectMesh->Box.min.y ;
	Box.min.z = Location.z + ObjectMesh->Box.min.z;
	Box.max.x = Location.x + ObjectMesh->Box.max.x;
	Box.max.y = Location.y + ObjectMesh->Box.max.y;
	Box.max.z = Location.z + ObjectMesh->Box.max.z;

	Sphere = ObjectMesh->MakeBoundingSphere( Location.GetMatrix() );

	if(Stream && Stream->FarthestPoint > Sphere.Radius )
		Sphere.Radius=Stream->FarthestPoint;	

}


void StaticObject::GetExtents( AABB& ExtBox )
{
	ExtBox = Box;
}

Real StaticObject::GetHeight( )
{
	return Box.max.y;
}


bool StaticObject::CheckVisibility( Screen3D& Screen, bool RecordTime )
{
	/*if(RecordTime)
	{
		if( LastDrawn == WMPtr->GTPtr->GetFrameStart() )
			return false;

		LastDrawn = WMPtr->GTPtr->GetFrameStart();
	}*/

	Mesh* ObjectMesh = MMPtr->Get( hMesh );
	if(ObjectMesh)
	{
		if(ObjectMesh->CheckVisibility( Screen, Location  ))
			return true;
	}

	if(Stream)
	{
		if(Stream->CheckVisibility( Screen ))
			return true;
	}


	return false;
}

bool StaticObject::Collision( SceneObject* sceneobject )
{
	return false;
}



bool StaticObject::Collision( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision )
{
	if(NoCollision)
		return false;
		
	//query the structure for the collision
	Structure* structure = WMPtr->Solver.GetStructure( hStructure );

	if( structure == NULL )
		return false;

	if ( structure->Geom == NULL )
		return false;
	bool hit = false;


	bool isactive = structure->IsGeomActive();
	if( !isactive )
		structure->Activate();

	if( structure->CheckForCollision( point1,point2, Collision ) )
		hit = true;
			

	if( !isactive)
		structure->Disable();
	

	return hit;

}

bool StaticObject::SphereCollision( const Vector3D& point1, Real radius, CollisionInfo* Collision )
{
	return false;
}


void StaticObject::Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer )
{

	//set the transformation
	Screen.SetTransform( Location.GetMatrix() );

	Screen.SetRenderState( D3DRS_ALPHAREF, 128 );
	Screen.SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	Screen.SetRenderState( D3DRS_FOGENABLE,  TRUE );
	Mesh* pMesh = MM.Get(hMesh);
	ImposterType* it = WMPtr->GetImposterManager()->GetImposter( hImposter) ;

	//draw the object
	
	/*Vector3D a( Location.x, Location.y+pMesh->Box.min.y, Location.z );
	Vector3D b( Location.x, Location.y+pMesh->Box.max.y, Location.z );
	Vector3D sa, sb;
	Screen.GetScreenCoordinates( &sa, a );
	Screen.GetScreenCoordinates( &sb, b );

	if( it &&  (sb.y-sa.y) < 16 )
	{
		
		if( it )
			it->WriteVertices( Screen, MM, &Viewer, &Location ) ;
	}
	else
	{*/
		if( pMesh )
			pMesh->Draw(Screen, (Position*)&Viewer, &Location);	
	//}

	

	if(Stream)
		WMPtr->AddStream( Stream );
	
	//DrawInfo( Screen );

}

void StaticObject::Use( int function, Real Param1, Real Param2, Real Param3, Real FrameTime ) 
{
	if (function == 0)
	{
		if(Stream)
			Stream->ToggleActivity();
	}

}

void StaticObject::DrawInfo( Screen3D& Screen )
{
	Matrix IdentityMatrix;
	Screen.SetTransform( IdentityMatrix);
	
	Vector3D ScreenCoordinates;
	Screen.GetScreenCoordinates(  &ScreenCoordinates, (Vector3D&)Location );
	Font2D* DrawFont = Screen.FM.GetFont( "tahoma", 8);
	DrawFont->Draw( Screen, ScreenCoordinates.x, ScreenCoordinates.y, true, D3DCOLOR_XRGB(0, 230, 0), Name);
}

void StaticObject::DrawHUD( Screen3D& Screen, MeshManager& MM, FontManager& FM, Camera& Viewer )
{
	return;
}

