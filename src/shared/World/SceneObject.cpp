#include "SceneObject.h"
#include "World.h"
#include "SceneManager.h"

void SceneObject::Init(Screen3D& Screen,  WorldManager* WM, MeshManager& MM,  SoundManager& AM,  char *ObjectFile)
{
}

void SceneObject::Kill()
{
    DropAll();
}


void SceneObject::AddEntity( SceneObject* Ent ) {
	Ent->Parent = this;
	EquipmentList.push_back( Ent );

	WMPtr->sceneManager->RemoveObjectByPtr( Ent );
}
void SceneObject::RemoveEntity( int index ) {
	EquipmentList.erase( EquipmentList.begin() + index );
}

void SceneObject::RemoveAllEntities() {EquipmentList.clear();	}

void SceneObject::DropAll( ) {
	return;
	for(int i=0; i< (int)EquipmentList.size(); i++ )
		EquipmentList[i]->Parent = NULL;
	EquipmentList.clear();	
}

void SceneObject::EquipItem ( int index ) {
	if( index >= 0 && index < (int)EquipmentList.size() )
		EquipedItem = index;
}

void SceneObject::Damage( const Vector3D& Source, const Vector3D& Force, const Vector3D &CollisionPoint )
{
	//Health -= Force.Mag();
	OnDamage( Source, CollisionPoint, Force.Mag() );

}

void SceneObject::OnDamage ( const Vector3D& source, const Vector3D& collisionPoint, float damageAmount ) 
{
	Health -= damageAmount;
}


void SceneObject::Move( Real FrameTime, WorldManager* SM )
{
	//readjust bounding box
	AdjustBoundingVolume();
}

void SceneObject::HandleNetworkMessage( BYTE* Message )
{
}

void SceneObject::HandleInput(  Real FrameTime, POINT MouseDelta, UCHAR* KeyBuffer )
{
	

}

void SceneObject::OnFocusLost()
{

}

void SceneObject::StealInputFocus( )
{
	if( WMPtr->InputFocus == this )
	{
		WMPtr->InputFocus = &WMPtr->LocalPlayer;
		WMPtr->Viewer.Pos = &WMPtr->LocalPlayer.GetLocation();
		if(WMPtr->Viewer.CS == CS_FOLLOW_MATRIX)
			WMPtr->Viewer.CS =CS_FIRST_PERSON;
		OnFocusLost();
	}
	else
	{
		WMPtr->InputFocus = this;
		WMPtr->Viewer.Pos = &Location;
		
	}

	
}

void SceneObject::Update( Real FrameTime, WorldManager* SM )
{
	Move( FrameTime, SM );
}

void SceneObject::SetVelocity(Real x, Real y, Real z)
{
	
}

void SceneObject::SetDestination( Position& NewPosition)
{

}

void SceneObject::AdjustBoundingVolume()
{
}


void SceneObject::GetExtents( AABB& ExtBox )
{
	ExtBox = Box;

}

Real SceneObject::GetHeight( )
{
	return Box.max.y;
}


bool SceneObject::CheckVisibility( Screen3D& Screen, bool RecordTime )
{
	return false;
}

bool SceneObject::LightCollision ( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision )
{
	return this->Collision( point1, point2, Collision );
}

bool SceneObject::Collision( SceneObject* sceneobject )
{
	return false;
}


bool SceneObject::Collision( const Vector3D& point1, const Vector3D& point2, CollisionInfo* Collision )
{
	return false;

}

bool SceneObject::SphereCollision( const Vector3D& point1, Real radius, CollisionInfo* Collision )
{
	return false;
}


void SceneObject::Draw( Screen3D& Screen, MeshManager& MM, Camera& Viewer, GameTimer& Timer )
{

}

void SceneObject::DrawInfo( Screen3D& Screen )
{
}

void SceneObject::DrawHUD( Screen3D& Screen, MeshManager& MM, FontManager& FM, Camera& Viewer )
{
}

