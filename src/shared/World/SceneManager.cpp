#include "SceneManager.h"
#include "world.h"
#include "spheretree.h"

#include "..\defs\defs.h"

#include "weapon.h"
#include "StaticObject.h"
#include "character.h"
#include "enemy.h"
#include "networkobject.h"
#include "vehicleobject.h"
#include "tankobject.h"
#include "crate.h"
#include "tree.h"
#include "powerup.h"

//Constructor/destructor
SceneManager::SceneManager( WorldManager* wm )
{
	currentID = 0;
	this->WMPtr = wm;
	MaxTreeDepth = 2;
	root = new SphereNode();

	

}

SceneManager::~SceneManager()
{
	//OH SNAPPITY SNAP
	delete root;

	//delete all the objects
	while ( objectList.size() )
	{
		SceneObject* so = objectList.begin()->second;
		delete so;
		objectList.erase( objectList.begin() );
	}

	
}

void SceneManager::BuildCollisionTree()
{
	kdtree.Init( WMPtr->GetWidth() * WMPtr->GetSpacing(), WMPtr->GetHeight()*WMPtr->GetSpacing(), 8);

	//add appropriate objects to KD tree
	for( SceneObjectMap::iterator itor = objectList.begin(); itor != objectList.end(); itor++)
	{
		SceneObject* so = (itor->second);
		if(so->GetType() == OT_STATIC )
			kdtree.Add( so );
	}
}

SceneHandle SceneManager::GenerateID()
{
	currentID++;
	return currentID;
}

//Object management functions
SceneHandle SceneManager::AddObject( SceneObject* o )
{
	SceneHandle h = GenerateID();
	objectList[h] = o;
	return h;
}

SceneObject* SceneManager::RemoveObject( SceneHandle s )
{
	SceneObjectMap::iterator itor = objectList.find( s );

	if( itor == objectList.end())
		return NULL;
	else
	{
		SceneObject* so = itor->second;
		objectList.erase( s );
		return so;
	}
}

SceneObject* SceneManager::RemoveObjectByPtr( SceneObject* o )
{
	SceneObjectMap::iterator itor = objectList.begin();
	for ( itor = objectList.begin(); itor != objectList.end(); ++itor )
	{
		SceneObject* so = itor->second;
		if( so == o )
		{
			//delete so;
			objectList.erase( itor->first );
			return so;
		}
	}

	return NULL;
}

SceneManagerIterator SceneManager::Start()
{
	return objectList.begin();
}

SceneManagerIterator SceneManager::End()
{
	return objectList.end();
}


SceneHandle SceneManager::GetObjectByScriptName( const char* name )
{
	for( SceneObjectMap::iterator itor = objectList.begin(); itor != objectList.end(); itor++)
	{
		SceneObject* so = (itor->second);
		if(so)
		{
			if( _stricmp( so->GetName(), name ) == 0)
				return itor->first;
		}
	}

	return -1;
}

SceneObject* SceneManager::FindNearestObject( Vector3D* location, float MaxDistance, SceneObject* objectToIgnore )
{
	SceneObject* nearestObject = NULL; 
	Vector3D soLocation;
	float shortestDistanceSq = MaxDistance*MaxDistance;
	
	for( SceneObjectMap::iterator itor = objectList.begin(); itor != objectList.end(); itor++)
	{
		SceneObject* so = (itor->second);
		if(so) 
		{
			soLocation = so->GetLocation();
		
	        
			float d = (soLocation - *location ).MagSq() ;
			if( so != objectToIgnore && d < shortestDistanceSq )
			{
				nearestObject = so;
				shortestDistanceSq = d;
			}
		}
	}

	return nearestObject;
	
}

void SceneManager::BuildTree()
{
	root->SetupAsRoot( this );
	root->Build( this, NULL, false, 0 );
}


//Creates a new scene object based on the type string given.
//Here are the supported types:
// "avatar"
// "character"
// "enemy"
// "weapon"
// "tank"
// "vehicle4wheeled"
// "crate"
// "random_tree"
// "static_object"
SceneObject* SceneManager::MakeObject( const char* type )
{
	SceneObject* returnObject = NULL;
	if (strcmpi( type, "character" ) == 0 )
	{
		returnObject = new Character();
	}
	else if (strcmpi( type, "enemy" ) == 0 )
	{
		returnObject = new Enemy();
	}
	else if( strcmpi(type, "weapon") == 0 )
	{
		returnObject = new Weapon();
	}

	else if( strcmpi(type, "vehicle4wheeled") == 0 )
	{
		returnObject = new VehicleObject();
	}

	else if( strcmpi(type, "tank") == 0 )
	{
		returnObject = new TankObject();
	}


	else if( strcmpi(type, "crate") == 0 )
	{
		returnObject = new Crate();
	}


	else if( strcmpi(type, "random_tree") == 0 )
	{
		returnObject = new ProceduralTree();
	}

	else if ( strcmpi( type, "powerup" ) == 0 )
	{
		returnObject = new Powerup();
	}

	else
	{
		returnObject = new StaticObject();
	}

	return returnObject;

}

//Collision stuff
bool SceneManager::CheckForCollision( Vector3D* p1, Vector3D* p2, CollisionInfo* Collision, bool CheckPlayer, bool LightCheck )
{
	/*SceneObjectMap::iterator itor;

	for( itor = objectList.begin(); itor != objectList.end(); itor++)
	{
		SceneObject* so = (itor->second);
		if( so->Collision( *p1, *p2, Collision ) )
			return true;
	}
	return false;*/
	return root->Collide( this, p1, p2, Collision );
}

void SceneManager::UpdateAll( float frametime )
{
	for( SceneManagerIterator i= objectList.begin(); i!=objectList.end(); i++)
	{
		SceneObject* so = i->second;
		if(so)
			so->Update( frametime, WMPtr );
	}
}

//Stuff for drawing
void SceneManager::DrawAll()
{
	SceneObjectMap::iterator itor;

	WMPtr->ScreenPtr->SetRenderState( D3DRS_ALPHAREF, 128 );
	WMPtr->ScreenPtr->SetRenderState(D3DRS_FOGENABLE, TRUE);

	for( itor = objectList.begin(); itor != objectList.end(); itor++)
	{
		WMPtr->ScreenPtr->D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		SceneObject* so = (itor->second);
		if( so && so->CheckVisibility( *WMPtr->ScreenPtr ) )
			so->Draw( *WMPtr->ScreenPtr, *WMPtr->MMPtr, WMPtr->Viewer, *WMPtr->GTPtr );
	}

	WMPtr->ScreenPtr->SetRenderState( D3DRS_ALPHAREF, 32 );
}

bool SceneManager::BasicCheckForCollision(  Vector3D& a,  Vector3D& b, Vector3D& hitPosition, Vector3D& normal, SceneObject** objectHit, SceneObject* excludedObject)
{
	SceneObjectMap::iterator itor;
	
	for( itor = objectList.begin(); itor != objectList.end(); itor++)
	{
		SceneObject* so = (itor->second);
		if( so && !so->IsStatic() && so != excludedObject )
		{
			CollisionInfo ci;
			if( so->Collision( a, b, &ci ) )
			{
				hitPosition = ci.CollisionPoint;
				normal      = ci.PlaneNormal;
				*objectHit  = so;
				return true;
			}
		}
			
	}

	CollisionInfo collisionData;
	if( excludedObject != &(WMPtr->LocalPlayer) && WMPtr->LocalPlayer.Collision( a, b, &collisionData ) )
	{
		hitPosition = collisionData.CollisionPoint;
		normal      = collisionData.PlaneNormal;
		*objectHit  = &(WMPtr->LocalPlayer);
		return true;
	}
	return false;
}